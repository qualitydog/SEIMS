j:
cd J:\code\SEIMS\preprocess
python import_parameters.py -ini zts_25m_longterm_omp_sf_win.ini
python import_bmp_scenario.py -ini zts_25m_longterm_omp_sf_win.ini

call J:\code\seimsVS\Debug\seims_omp J:\code\SEIMS\model_data\zts\model_zts_25m_longterm 6 0 127.0.0.1 27017 0
pause