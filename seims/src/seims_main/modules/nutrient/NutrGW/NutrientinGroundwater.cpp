#include "seims.h"
#include "NutrientinGroundwater.h"

using namespace std;

NutrientinGroundwater::NutrientinGroundwater() :
//input
    m_TimeStep(-1), m_nCells(-1), m_cellWidth(-1), m_subbasinID(-1), m_gwno3Con(nullptr), m_gwSolPCon(nullptr), m_gw_q(nullptr),
    m_nSubbasins(-1), m_subbasin(nullptr), m_subbasinsInfo(nullptr), m_gwStor(nullptr),
    m_perco_no3_gw(nullptr), m_perco_solp_gw(nullptr), m_soilLayers(nullptr), m_nSoilLayers(-1), m_sol_no3(nullptr),
    m_gwNO3(nullptr), m_gwSolP(nullptr), m_gw0(NODATA_VALUE),
    //output
    m_solpGwToCh(nullptr), m_no3GwToCh(nullptr) {

}

NutrientinGroundwater::~NutrientinGroundwater() {
    if (nullptr != m_no3GwToCh) Release1DArray(m_no3GwToCh);
    if (nullptr != m_solpGwToCh) Release1DArray(m_solpGwToCh);
    // m_gwNO3 and m_gwSolP will be released in ~clsReaches(). By lj, 2017-12-26
}

void NutrientinGroundwater::SetSubbasins(clsSubbasins *subbasins) {
    if (nullptr == m_subbasinsInfo) {
        m_subbasinsInfo = subbasins;
        // m_nSubbasins = m_subbasinsInfo->GetSubbasinNumber(); // Set in SetValue()
        m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
    }
}

bool NutrientinGroundwater::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_NUTRGW, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
        return false;
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) {
            m_nCells = n;
        } else {
            //StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
                m_nCells << ".\n";
            throw ModelException(MID_NUTRGW, "CheckInputSize", oss.str());
        }
    }
    return true;
}

bool NutrientinGroundwater::CheckInputData() {
    CHECK_POSITIVE(MID_NUTRGW, m_nCells);
    CHECK_POSITIVE(MID_NUTRGW, m_TimeStep);
    CHECK_POSITIVE(MID_NUTRGW, m_cellWidth);
    CHECK_POSITIVE(MID_NUTRGW, m_gw0);
    CHECK_POINTER(MID_NUTRGW, m_gw_q);
    CHECK_POINTER(MID_NUTRGW, m_gwStor);
    CHECK_POINTER(MID_NUTRGW, m_perco_no3_gw);
    CHECK_POINTER(MID_NUTRGW, m_perco_solp_gw);
    CHECK_POINTER(MID_NUTRGW, m_soilLayers);
    CHECK_POINTER(MID_NUTRGW, m_sol_no3);
    return true;
}

void NutrientinGroundwater::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, Tag_TimeStep)) { m_TimeStep = int(value); }
    else if (StringMatch(sk, Tag_CellWidth)) { m_cellWidth = value; }
    else if (StringMatch(sk, VAR_GW0)) { m_gw0 = value; }
    else if (StringMatch(sk, VAR_SUBBSNID_NUM)) { m_nSubbasins = value; }
    else if (StringMatch(sk, Tag_SubbasinId)) { m_subbasinID = value; }
    else {
        throw ModelException(MID_NUTRGW, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

void NutrientinGroundwater::Set1DData(const char *key, int n, float *data) {
    string sk(key);
    if (StringMatch(sk, VAR_SUBBSN)) {
        CheckInputSize(key, n);
        m_subbasin = data;
    }
    else if (StringMatch(sk, VAR_SBQG)) { m_gw_q = data; }
    else if (StringMatch(sk, VAR_SBGS)) { m_gwStor = data; }
    else if (StringMatch(sk, VAR_PERCO_N_GW)) { m_perco_no3_gw = data; }
    else if (StringMatch(sk, VAR_PERCO_P_GW)) { m_perco_solp_gw = data; }
    else if (StringMatch(sk, VAR_SOILLAYERS)) {
        CheckInputSize(key, n);
        m_soilLayers = data;
    } else {
        throw ModelException(MID_NUTRGW, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void NutrientinGroundwater::Set2DData(const char *key, int nRows, int nCols, float **data) {
    CheckInputSize(key, nRows);
    string sk(key);
    m_nSoilLayers = nCols;
    if (StringMatch(sk, VAR_SOL_NO3)) { m_sol_no3 = data; }
    else if (StringMatch(sk, VAR_SOL_SOLP)) { m_sol_solp = data; }
    else {
        throw ModelException(MID_NUTRGW, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void NutrientinGroundwater::SetReaches(clsReaches *reaches) {
    if (nullptr != reaches) {
        // m_nSubbasins = reaches->GetReachNumber();
        if (nullptr == m_gwno3Con) { reaches->GetReachesSingleProperty(REACH_GWNO3, &m_gwno3Con); }
        if (nullptr == m_gwSolPCon) { reaches->GetReachesSingleProperty(REACH_GWSOLP, &m_gwSolPCon); }
    } else {
        throw ModelException(MID_NUTRGW, "SetReaches", "The reaches input can not to be NULL.");
    }
}

void NutrientinGroundwater::initialOutputs() {
    CHECK_POSITIVE(MID_NUTRGW, m_nCells);
    // allocate the output variables
    if (nullptr == m_no3GwToCh) {
        Initialize1DArray(m_nSubbasins + 1, m_no3GwToCh, 0.f);
        Initialize1DArray(m_nSubbasins + 1, m_solpGwToCh, 0.f);
    }
    if (nullptr == m_gwNO3) {    /// initial nutrient amount stored in groundwater
        m_gwNO3 = new float[m_nSubbasins + 1];
        m_gwSolP = new float[m_nSubbasins + 1];
        for (auto it = m_subbasinIDs.begin(); it != m_subbasinIDs.end(); it++) {
            Subbasin *subbasin = m_subbasinsInfo->GetSubbasinByID(*it);
            float subArea = subbasin->getCellCount() * m_cellWidth * m_cellWidth;    //m2
            m_gwNO3[*it] = m_gw0 * m_gwno3Con[*it] * subArea / 1000000.f; /// mm * mg/L * m2 = 10^-6 kg
            m_gwSolP[*it] = m_gw0 * m_gwSolPCon[*it] * subArea / 1000000.f;
        }
    }
}

int NutrientinGroundwater::Execute() {
    CheckInputData();
    initialOutputs();
    for (auto it = m_subbasinIDs.begin(); it != m_subbasinIDs.end(); it++) {
        int id = *it;
        Subbasin *subbasin = m_subbasinsInfo->GetSubbasinByID(id);
        int nCells = subbasin->getCellCount();
        float subArea = nCells * m_cellWidth * m_cellWidth;    // m^2
        float revap = subbasin->getEG();
        /// 1. firstly, restore the groundwater storage during current day
        ///    since the m_gwStor has involved percolation water, just need add revap and runoff water
        float gwqVol = m_gw_q[id] * m_TimeStep;    // m^3, water volume flow out
        float reVapVol = revap * subArea / 1000.f; // m^3
        float tmpGwStorage = m_gwStor[id] * subArea / 1000.f + gwqVol + reVapVol;
        /// 2. secondly, update nutrient concentration
        m_gwNO3[id] += m_perco_no3_gw[id]; /// nutrient amount, kg
        m_gwSolP[id] += m_perco_solp_gw[id];
        m_gwno3Con[id] = m_gwNO3[id] / tmpGwStorage * 1000.f; // kg / m^3 * 1000. = mg/L
        m_gwSolPCon[id] = m_gwSolP[id] / tmpGwStorage * 1000.f;
        /// 3. thirdly, calculate nutrient in groundwater runoff
        //cout<<"subID: "<<id<<", gwQ: "<<m_gw_q[id] << ", ";
        m_no3GwToCh[id] = m_gwno3Con[id] * gwqVol / 1000.f;    // g/m3 * m3 / 1000 = kg
        m_solpGwToCh[id] = m_gwSolPCon[id] * gwqVol / 1000.f;
        //cout<<"subID: "<<id<<", gwno3Con: "<<m_gwno3Con[id] << ", no3gwToCh: "<<m_no3gwToCh[id] << ", ";
        /// 4. fourthly, calculate nutrient loss loss through revep and update no3 in the bottom soil layer
        float no3ToSoil = revap / 1000.f * m_gwno3Con[id] * 10.f;    // kg/ha  (m*10*g/m3=kg/ha)
        float solpToSoil = revap / 1000.f * m_gwSolPCon[id] * 10.f;
        float no3ToSoil_kg = no3ToSoil * subArea / 10000.f; /// kg/ha * m^2 / 10000.f = kg
        float solpToSoil_kg = solpToSoil * subArea / 10000.f;
        int *cells = subbasin->getCells();
        int index = 0;
        for (int i = 0; i < nCells; i++) {
            index = cells[i];
            m_sol_no3[index][(int) m_soilLayers[index] - 1] += no3ToSoil;
            m_sol_solp[index][(int) m_soilLayers[index] - 1] += solpToSoil;
        }
        /// finally, update nutrient amount
        m_gwNO3[id] -= (m_no3GwToCh[id] + no3ToSoil_kg);
        m_gwSolP[id] -= (m_solpGwToCh[id] + solpToSoil_kg);
    }
    return 0;
}

void NutrientinGroundwater::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    *n = m_nSubbasins + 1;
    if (StringMatch(sk, VAR_NO3GW_TOCH)) {
        *data = m_no3GwToCh;
    } else if (StringMatch(sk, VAR_MINPGW_TOCH)) {
        *data = m_solpGwToCh;
    } else if (StringMatch(sk, VAR_GWNO3_CONC)) {
        *data = m_gwno3Con;
    } else if (StringMatch(sk, VAR_GWSOLP_CONC)) {
        *data = m_gwSolPCon;
    } else if (StringMatch(sk, VAR_GWNO3)) {
        *data = m_gwNO3;
    } else if (StringMatch(sk, VAR_GWSOLP)) {
        *data = m_gwSolP;
    } else {
        throw ModelException(MID_NUTRGW, "Get1DData", "Parameter " + sk + " does not exist.");
    }
}
