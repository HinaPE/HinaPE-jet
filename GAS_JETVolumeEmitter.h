#ifndef HINAPE_JET_GAS_JETVOLUMEEMITTER_H
#define HINAPE_JET_GAS_JETVOLUMEEMITTER_H

#include <GAS/GAS_SubSolver.h>

class SIM_JETParticleData;

class GAS_JETVolumeEmitter : public GAS_SubSolver
{
public:
	GETSET_DATA_FUNCS_S("soppath", SOPPath)
	GETSET_DATA_FUNCS_F("target_spacing", TargetSpacing)
	GETSET_DATA_FUNCS_V3("initial_velocity", InitialVelocity)
	GETSET_DATA_FUNCS_V3("linear_velocity", LinearVelocity)
	GETSET_DATA_FUNCS_V3("angular_velocity", AngularVelocity)
	GETSET_DATA_FUNCS_F("max_number_of_particles", MaxNumberOfParticles)
	GETSET_DATA_FUNCS_F("jitter", Jitter)
	GETSET_DATA_FUNCS_B("is_one_shot", IsOneShot)
	GETSET_DATA_FUNCS_B("allow_overlapping", AllowOverlapping)
	GETSET_DATA_FUNCS_F("random_seed", RandomSeed)

protected:
	GAS_JETVolumeEmitter(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_JETVolumeEmitter() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	static const SIM_DopDescription	*getDopDescription();

	SIM_Guide *createGuideObjectSubclass() const override;
	void buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const override;
	static PRM_Name showGuideGeometry;

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_JETVolumeEmitter,
					GAS_SubSolver,
					"JET Volume Emitter SubSolver",
					getDopDescription());

public:
	SIM_JETParticleData* FetchJetParticleData(SIM_Object *obj);
};

#endif //HINAPE_JET_GAS_JETVOLUMEEMITTER_H
