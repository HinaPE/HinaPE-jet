#ifndef HINAPE_JET_GAS_JETSYNCDATA_H
#define HINAPE_JET_GAS_JETSYNCDATA_H

#include <GAS/GAS_SubSolver.h>
#include <PRM/PRM_Default.h>

#include "jet/jet.h"

#define JET_SYNCDATA_DATANAME "JETSyncData"

class GAS_JETSyncData : public GAS_SubSolver
{
protected:
	GAS_JETSyncData(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_JETSyncData() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

	SIM_Guide *createGuideObjectSubclass() const override;
	void buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const override;
	static PRM_Name showGuideGeometry;

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_JETSyncData,
					GAS_SubSolver,
					"JET Sync Data SubSolver",
					getDopDescription());

public:
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg);
};

#endif //HINAPE_JET_GAS_JETSYNCDATA_H
