#! /usr/bin/env python
# -*- coding: utf-8 -*-
"""Import hydroClimate sites information and variables
    @author   : Liangjun Zhu
    @changelog: 16-12-07  lj - rewrite for version 2.0
                17-07-04  lj - reorganize according to pylint and google style
                17-07-05  lj - integrate hydro_find_sites.py, i.e. SITELIST in workflow database
"""
from osgeo.ogr import Open as ogr_Open
from pygeoc.utils import StringClass
from pymongo import ASCENDING

from text import StationFields, DBTableNames, VariableDesc, DataType, FieldNames
from utility import read_data_items_from_txt, DEFAULT_NODATA


class SiteInfo(object):
    """base class of HydroClimate site information."""

    def __init__(self, sid=0, name='', lat=DEFAULT_NODATA, lon=DEFAULT_NODATA,
                 local_x=DEFAULT_NODATA, local_y=DEFAULT_NODATA, alti=DEFAULT_NODATA):
        """Initialize a SiteInfo object."""
        self.StationID = sid  # integer
        self.Name = name  # station name, string
        self.lat = lat  # latitude, float degree
        self.lon = lon  # longitude, float degree
        self.LocalX = local_x  # X coordinate in projection, float
        self.LocalY = local_y  # Y coordinate in projection, float
        self.alti = alti  # altitude, as ORIGIN: unit 0.1 meter

    def lon_lat(self):
        """Return geographic coordinates."""
        return self.lon, self.lat

    def local_xy(self):
        """Return projected coordinates."""
        return self.LocalX, self.LocalY


class ImportHydroClimateSites(object):
    """Import hydro-climate sites information.
       1. Find meteorology and precipitation sites in study area, and save at SITELIST
          of the workflow database
       2. Import geographic information of each sites to Hydro-Climate database.
    """

    @staticmethod
    def sites_table(hydro_clim_db, site_file, site_type):
        """Import HydroClimate sites table"""
        sites_loc = dict()
        site_data_items = read_data_items_from_txt(site_file)
        site_flds = site_data_items[0]
        for i in range(1, len(site_data_items)):
            dic = dict()
            for j in range(len(site_data_items[i])):
                if StringClass.string_match(site_flds[j], StationFields.id):
                    dic[StationFields.id] = int(site_data_items[i][j])
                elif StringClass.string_match(site_flds[j], StationFields.name):
                    # unicode(site_data_items[i][j], 'gb2312')
                    dic[StationFields.name] = site_data_items[i][j]
                elif StringClass.string_match(site_flds[j], StationFields.x):
                    dic[StationFields.x] = float(site_data_items[i][j])
                elif StringClass.string_match(site_flds[j], StationFields.y):
                    dic[StationFields.y] = float(site_data_items[i][j])
                elif StringClass.string_match(site_flds[j], StationFields.lon):
                    dic[StationFields.lon] = float(site_data_items[i][j])
                elif StringClass.string_match(site_flds[j], StationFields.lat):
                    dic[StationFields.lat] = float(site_data_items[i][j])
                elif StringClass.string_match(site_flds[j], StationFields.elev):
                    dic[StationFields.elev] = float(site_data_items[i][j])
                elif StringClass.string_match(site_flds[j], StationFields.outlet):
                    dic[StationFields.outlet] = float(site_data_items[i][j])
            dic[StationFields.type] = site_type
            curfilter = {StationFields.id: dic[StationFields.id],
                         StationFields.type: dic[StationFields.type]}
            hydro_clim_db[DBTableNames.sites].find_one_and_replace(curfilter, dic, upsert=True)

            if dic[StationFields.id] not in sites_loc.keys():
                sites_loc[dic[StationFields.id]] = SiteInfo(dic[StationFields.id],
                                                            dic[StationFields.name],
                                                            dic[StationFields.lat],
                                                            dic[StationFields.lon],
                                                            dic[StationFields.x],
                                                            dic[StationFields.y],
                                                            dic[StationFields.elev])
        hydro_clim_db[DBTableNames.sites].create_index([(StationFields.id, ASCENDING),
                                                        (StationFields.type, ASCENDING)])
        return sites_loc

    @staticmethod
    def variable_table(db, var_file):
        """Import variables table"""
        var_data_items = read_data_items_from_txt(var_file)
        var_flds = var_data_items[0]
        for i in range(1, len(var_data_items)):
            dic = {}
            for j in range(len(var_data_items[i])):
                if StringClass.string_match(var_flds[j], VariableDesc.type):
                    dic[VariableDesc.type] = var_data_items[i][j]
                elif StringClass.string_match(var_flds[j], VariableDesc.unit):
                    dic[VariableDesc.unit] = var_data_items[i][j]
            # If this item existed already, then update it, otherwise insert one.
            curfilter = {VariableDesc.type: dic[VariableDesc.type]}
            db[DBTableNames.var_desc].find_one_and_replace(curfilter, dic, upsert=True)

    @staticmethod
    def ogrwkt2shapely(input_shape, id_field):
        """Return shape objects list and ids list"""
        # CAUTION, IMPORTANT
        # Because shapely is dependent on sqlite, and the version is not consistent
        #    with GDAL executable (e.g., located in C:\GDAL_x64\bin), thus the shapely
        #    must be locally imported here.
        from shapely.wkt import loads as shapely_loads
        shapely_objects = []
        id_list = []
        # print input_shape
        shp = ogr_Open(input_shape)
        if shp is None:
            raise RuntimeError('The input ESRI Shapefile: %s is not existed or has '
                               'no read permission!' % input_shape)
        lyr = shp.GetLayer()
        for n in range(0, lyr.GetFeatureCount()):
            feat = lyr.GetFeature(n)
            # This function may print Failed `CDLL(/opt/local/lib/libgeos_c.dylib)` in macOS
            # Don't worry about that!
            wkt_feat = shapely_loads(feat.geometry().ExportToWkt())
            shapely_objects.append(wkt_feat)
            if isinstance(id_field, unicode):
                id_field = id_field.encode()
            id_index = feat.GetFieldIndex(id_field)
            id_list.append(feat.GetField(id_index))
        return shapely_objects, id_list

    @staticmethod
    def find_sites(maindb, clim_dbname, subbsn_file, subbsn_field_id,
                   thissen_file_list, thissen_field_id, site_type_list,
                   is_storm=False, is_cluster=False):
        """Find meteorology and precipitation sites in study area"""
        mode = 'DAILY'
        if is_storm:
            mode = 'STORM'
        subbasin_list, subbasin_id_list = ImportHydroClimateSites.ogrwkt2shapely(subbsn_file,
                                                                                 subbsn_field_id)
        n_subbasins = len(subbasin_list)

        # site_dic = dict()
        for i, subbasin in enumerate(subbasin_list):
            cur_id = subbasin_id_list[i]
            # if not for cluster and basin.shp is used, force the SUBBASINID to 0. by LJ.
            if not is_cluster and n_subbasins == 1:
                cur_id = 0
            dic = dict()
            dic[FieldNames.subbasin_id] = cur_id
            dic[FieldNames.db] = clim_dbname
            dic[FieldNames.mode] = mode
            cur_fileter = {FieldNames.subbasin_id: cur_id,
                           FieldNames.db: clim_dbname,
                           FieldNames.mode: mode}

            for meteo_id, thiessen_file in enumerate(thissen_file_list):
                # print thiessen_file
                site_type = site_type_list[meteo_id]
                thiessen_list, thiessen_id_list = ImportHydroClimateSites.ogrwkt2shapely(
                        thiessen_file, thissen_field_id)
                site_list = []
                for poly_id, thiessen in enumerate(thiessen_list):
                    if subbasin.intersects(thiessen):
                        site_list.append(thiessen_id_list[poly_id])
                site_list.sort()
                slist = [str(item) for item in site_list]
                site_list_str = ','.join(slist)

                site_field = '%s%s' % (DBTableNames.main_sitelist, site_type)
                dic[site_field] = site_list_str
            maindb[DBTableNames.main_sitelist].find_one_and_replace(cur_fileter, dic, upsert=True)

        maindb[DBTableNames.main_sitelist].create_index([(FieldNames.subbasin_id, ASCENDING),
                                                         (FieldNames.mode, ASCENDING)])
        # print 'Meteorology sites table was generated.'

    @staticmethod
    def workflow(cfg, main_db, clim_db):
        """Workflow"""
        # 1. Find meteorology and precipitation sites in study area
        thiessen_file_list = [cfg.meteo_sites_thiessen, cfg.prec_sites_thiessen]
        type_list = [DataType.m, DataType.p]

        if not cfg.cluster:  # the entire basin
            ImportHydroClimateSites.find_sites(main_db, cfg.climate_db, cfg.vecs.bsn,
                                               FieldNames.basin, thiessen_file_list,
                                               cfg.thiessen_field, type_list, cfg.storm_mode)
        ImportHydroClimateSites.find_sites(main_db, cfg.climate_db, cfg.vecs.subbsn,
                                           FieldNames.subbasin_id, thiessen_file_list,
                                           cfg.thiessen_field, type_list,
                                           cfg.storm_mode, cfg.cluster)

        # 2. Import geographic information of each sites to Hydro-Climate database
        c_list = clim_db.collection_names()
        tables = [DBTableNames.sites, DBTableNames.var_desc]
        for tb in tables:
            if not StringClass.string_in_list(tb, c_list):
                clim_db.create_collection(tb)
        ImportHydroClimateSites.variable_table(clim_db, cfg.hydro_climate_vars)
        site_m_loc = ImportHydroClimateSites.sites_table(clim_db, cfg.Meteo_sites, DataType.m)
        site_p_loc = ImportHydroClimateSites.sites_table(clim_db, cfg.prec_sites, DataType.p)
        # print (site_m_loc, site_p_loc)
        return site_m_loc, site_p_loc


def main():
    """TEST CODE"""
    from config import parse_ini_configuration
    from db_mongodb import ConnectMongoDB
    seims_cfg = parse_ini_configuration()
    client = ConnectMongoDB(seims_cfg.hostname, seims_cfg.port)
    conn = client.get_conn()
    main_db = conn[seims_cfg.spatial_db]
    hydroclim_db = conn[seims_cfg.climate_db]

    ImportHydroClimateSites.workflow(seims_cfg, main_db, hydroclim_db)

    client.close()


if __name__ == "__main__":
    main()
