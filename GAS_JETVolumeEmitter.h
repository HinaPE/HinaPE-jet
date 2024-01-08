#ifndef HINAPE_JET_GAS_JETVOLUMEEMITTER_H
#define HINAPE_JET_GAS_JETVOLUMEEMITTER_H

#include <GAS/GAS_SubSolver.h>
#include <GAS/GAS_PBDSolve.h>

class GAS_JETVolumeEmitter : public GAS_SubSolver
{
	GETSET_DATA_FUNCS_S("soppath", SOPPath)
	GETSET_DATA_FUNCS_B("show_guide_geometry", ShowGuideGeometry);

protected:
	GAS_JETVolumeEmitter(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_JETVolumeEmitter() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	static const SIM_DopDescription	*getDopDescription();

	SIM_Guide *createGuideObjectSubclass() const override;
	void buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const override;

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_JETVolumeEmitter,
					GAS_SubSolver,
					"JET Volume Emitter SubSolver",
					getDopDescription());
};

#endif //HINAPE_JET_GAS_JETVOLUMEEMITTER_H
