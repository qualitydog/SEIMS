/*!
 * \brief Plant management operation class
 * \author Liang-Jun Zhu
 * \date June 2016
 */
#ifndef SEIMS_PLANT_MANAGEMENT_OPERATION_H
#define SEIMS_PLANT_MANAGEMENT_OPERATION_H

#include "BMPText.h"
#include "utilities.h"

using namespace std;
namespace MainBMP {
/*!
 * \ingroup MainBMP
 * \brief Base namespace for Plant management related operations
 * \author Liang-Jun Zhu
 */
namespace PlantManagement {
/*!
 * \class PlantManagementOperation
 * \ingroup PlantManagement
 * \brief Base class of plant management operation
 */
class PlantManagementOperation {
public:
    /*!
     * \brief Constructor
     * \param[in] mgtCode 1 to 16
     * \param[in] usebaseHU true or false
     * \param[in] husc Fraction of heat units (base or plant)
     * \param[in] year Rotation year, e.g., 1,2,...
     * \param[in] month
     * \param[in] day
     * \param[in] location Indexes of field to practice the operation (string)
     */
    PlantManagementOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day,
                             float *parameters);

    //! virtual Destructor, there are no allocated memory to release!
    virtual ~PlantManagementOperation() = default;

    //! Output
    virtual void dump(ostream *fs) = 0;

    bool UseBaseHUSC() { return m_useBaseHUSC; }

    float GetHUFraction() { return m_frHU; }

    int GetMonth() { return m_month; }

    int GetDay() { return m_day; }

    int GetOperationCode() { return m_mgtOp; }

protected:
    /// use base hu or plant accumulated hu
    bool m_useBaseHUSC;
    /// husc
    float m_frHU;
    /// year
    int m_year;
    /// month
    int m_month;
    /// day
    int m_day;
    /// management operation code
    int m_mgtOp;
    /// parameters, mgt1~mgt10
    float *m_parameters;
};

/*!
 * \class PlantOperation
 * \ingroup PlantManagement
 * \brief Plant management operation
 */
class PlantOperation : public PlantManagementOperation {
public:
    PlantOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day, float *parameters);

    virtual ~PlantOperation() = default;

    int PlantID() { return m_plantID; }

    float CurYearMaturity() { return m_curYrMat; }

    float HeatUnits() { return m_heatUnits; }

    float LAIInit() { return m_laiInit; }

    float BIOInit() { return m_bioInit; }

    float HITarg() { return m_hiTarg; }

    float BIOTarg() { return m_bioTarg; }

    float CNOP() { return m_CNOP; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_plantID;
    float m_curYrMat;
    float m_heatUnits;
    float m_laiInit;
    float m_bioInit;
    float m_hiTarg;
    float m_bioTarg;
    float m_CNOP;
};

/*!
 * \class IrrigationOperation
 * \ingroup PlantManagement
 * \brief Irrigation operation
 */
class IrrigationOperation : public PlantManagementOperation {
public:
    IrrigationOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day, float *parameters);

    virtual ~IrrigationOperation() = default;

    int IRRSource() { return m_irrSrc; }

    int IRRNo() { return m_irrNo; }

    float IRRApplyDepth() { return m_irrAmt; }

    float IRRSalt() { return m_irrSalt; }

    float IRREfficiency() { return m_irrEfm; }

    float IRRSQfrac() { return m_irrSq; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_irrSrc;
    float m_irrAmt;
    float m_irrSalt;
    float m_irrEfm;
    float m_irrSq;
    int m_irrNo;
};

/*!
 * \class FertilizerOperation
 * \ingroup PlantManagement
 * \brief Fertilizer operation
 */
class FertilizerOperation : public PlantManagementOperation {
public:
    FertilizerOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day, float *parameters);

    virtual ~FertilizerOperation() = default;

    int FertilizerID() { return m_fertID; }

    float FertilizerKg_per_ha() { return m_frtKgHa; }

    float FertilizerSurfaceFrac() { return m_frtSurface; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_fertID;
    float m_frtKgHa;
    /// fraction of fertilizer which is applied to the top 10 mm of soil (the remaining
    /// fraction is applied to first soil layer defined by user)
    float m_frtSurface;
};

/*!
 * \class PesticideOperation
 * \ingroup PlantManagement
 * \brief Pesticide operation
 */
class PesticideOperation : public PlantManagementOperation {
public:
    PesticideOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day, float *parameters);

    virtual ~PesticideOperation() = default;

    int PesticideID() { return m_pestID; }

    float PesticideKg() { return m_pstKg; }

    float PesticideDepth() { return m_pstDep; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_pestID;
    float m_pstKg;
    float m_pstDep;
};

/*!
 * \class HarvestKillOperation
 * \ingroup PlantManagement
 * \brief HarvestKill operation
 */
class HarvestKillOperation : public PlantManagementOperation {
public:
    HarvestKillOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day,
                         float *parameters);

    virtual ~HarvestKillOperation() = default;

    float CNOP() { return m_CNOP; }

    float HarvestIndexOverride() { return m_hiOvr; }

    float StoverFracRemoved() { return m_fracHarvk; }

    //! Output
    virtual void dump(ostream *fs);

private:
    float m_CNOP;
    float m_hiOvr;
    float m_fracHarvk;
};

/*!
 * \class TillageOperation
 * \ingroup PlantManagement
 * \brief Tillage operation
 */
class TillageOperation : public PlantManagementOperation {
public:
    TillageOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day, float *parameters);

    virtual ~TillageOperation() = default;

    float CNOP() { return m_CNOP; }

    int TillageID() { return m_tillID; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_tillID;
    float m_CNOP;
};

/*!
 * \class HarvestOnlyOperation
 * \ingroup PlantManagement
 * \brief HarvestOnly operation
 */
class HarvestOnlyOperation : public PlantManagementOperation {
public:
    HarvestOnlyOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day,
                         float *parameters);

    virtual ~HarvestOnlyOperation() = default;

    float HarvestEfficiency() { return m_harvEff; }

    float HarvestIndexResidue() { return m_hiRsd; }

    float HarvestIndexBiomass() { return m_hiBms; }

    //! Output
    virtual void dump(ostream *fs);

private:
    float m_harvEff;
    float m_hiRsd;
    float m_hiBms;
};

/*!
 * \class KillOperation
 * \ingroup PlantManagement
 * \brief Kill operation
 */
class KillOperation : public PlantManagementOperation {
public:
    KillOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day, float *parameters);

    virtual ~KillOperation() = default;

    //! Output
    virtual void dump(ostream *fs);
};

/*!
 * \class GrazingOperation
 * \ingroup PlantManagement
 * \brief Grazing operation
 */
class GrazingOperation : public PlantManagementOperation {
public:
    GrazingOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day, float *parameters);

    virtual ~GrazingOperation() = default;

    int GrazingDays() { return m_grzDays; }

    int ManureID() { return m_manureID; }

    float BiomassConsumed() { return m_bioEat; }

    float BiomassTrampled() { return m_bioTrmp; }

    float ManureDeposited() { return m_manureKg; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_grzDays;
    int m_manureID;
    float m_bioEat;
    float m_bioTrmp;
    float m_manureKg;
};

/*!
 * \class AutoIrrigationOperation
 * \ingroup PlantManagement
 * \brief Auto irrigation operation
 */
class AutoIrrigationOperation : public PlantManagementOperation {
public:
    AutoIrrigationOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day,
                            float *parameters);

    virtual ~AutoIrrigationOperation() = default;

    int WaterStrsIdent() { return m_wstrsID; }

    int AutoIrrSrcCode() { return m_irrSrc; }

    int AutoIrrSrcLocs() { return m_irrNoa; }

    float AutoWtrStrsThrsd() { return m_autoWstrs; }

    float IrrigationEfficiency() { return m_irrEff; }

    float IrrigationWaterApplied() { return m_irrMx; }

    float SurfaceRunoffRatio() { return m_irrAsq; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_wstrsID;
    int m_irrSrc;
    float m_autoWstrs;
    int m_irrNoa;
    float m_irrEff;
    float m_irrMx;
    float m_irrAsq;
};

/*!
 * \class AutoFertilizerOperation
 * \ingroup PlantManagement
 * \brief Auto Fertilizer operation
 */
class AutoFertilizerOperation : public PlantManagementOperation {
public:
    AutoFertilizerOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day,
                            float *parameters);

    virtual ~AutoFertilizerOperation() = default;

    int FertilizerID() { return m_afertID; }

    int NitrogenMethod() { return m_NStress; }

    float NitrogenStrsFactor() { return m_autoNStrs; }

    float MaxMineralN() { return m_autoNAPP; }

    float MaxMineralNYearly() { return m_autoNYR; }

    float FertEfficiency() { return m_autoEff; }

    float SurfaceFracApplied() { return m_afrtSurface; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_afertID;
    int m_NStress;
    float m_autoNStrs;
    float m_autoNAPP;
    float m_autoNYR;
    float m_autoEff;
    float m_afrtSurface;
};

/*!
 * \class ReleaseImpoundOperation
 * \ingroup PlantManagement
 * \brief ReleaseImpound operation
 */
class ReleaseImpoundOperation : public PlantManagementOperation {
public:
    ReleaseImpoundOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day,
                            float *parameters);

    virtual ~ReleaseImpoundOperation() = default;

    int ImpoundTriger() { return m_impTrig; }

    float MaxPondDepth() { return m_maxPondDepth; }

    float MaxFitDepth() { return m_maxFitDepth; }

    float MinFitDepth() { return m_minFitDepth; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_impTrig;
    float m_maxPondDepth;  ///< Maximum ponding depth, mm
    float m_maxFitDepth;  ///< Maximum fitting depth, mm
    float m_minFitDepth;  ///< Minimum fitting depth, mm
};

/*!
 * \class ContinuousFertilizerOperation
 * \ingroup PlantManagement
 * \brief Continuous Fertilizer operation
 */
class ContinuousFertilizerOperation : public PlantManagementOperation {
public:
    ContinuousFertilizerOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day,
                                  float *parameters);

    virtual ~ContinuousFertilizerOperation() = default;

    int FertilizerID() { return m_cfertID; }

    int ApplyFrequency() { return m_ifrtFreq; }

    int FertilizerDays() { return m_fertDays; }

    float FertilizerKg() { return m_cfrtKg; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_cfertID;
    int m_fertDays;
    float m_cfrtKg;
    int m_ifrtFreq;
};

/*!
 * \class ContinuousPesticideOperation
 * \ingroup PlantManagement
 * \brief Continuous Pesticide operation
 */
class ContinuousPesticideOperation : public PlantManagementOperation {
public:
    ContinuousPesticideOperation(int mgtOp, bool usebaseHU, float husc, int year, int month, int day,
                                 float *parameters);

    virtual ~ContinuousPesticideOperation() = default;

    int PesticideID() { return m_ipstID; }

    int PesticideFrequency() { return m_pstFreq; }

    float PesticideKg() { return m_cpstKg; }

    int PesticideDays() { return m_pstDays; }

    //! Output
    virtual void dump(ostream *fs);

private:
    int m_ipstID;
    int m_pstDays;
    float m_cpstKg;
    int m_pstFreq;
};

/*!
 * \class BurningOperation
 * \ingroup PlantManagement
 * \brief Burning operation
 */
class BurningOperation : public PlantManagementOperation {
public:
    BurningOperation(int mgtOp, bool useBaseHU, float husc, int year, int month, int day, float *parameters);

    virtual ~BurningOperation() = default;

    float FractionLeft() { return m_burnFrlb; }

    //! Output
    virtual void dump(ostream *fs);

private:
    float m_burnFrlb;
};

} /* PlantManagement */
} /* MainBMP */

#endif /* SEIMS_PLANT_MANAGEMENT_OPERATION_H */