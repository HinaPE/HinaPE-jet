#ifndef HINAPE_JET_GAS_JETPCISPHSOLVER_H
#define HINAPE_JET_GAS_JETPCISPHSOLVER_H

#include <GAS/GAS_SubSolver.h>
#include <PRM/PRM_Default.h>

#include "jet/jet.h"

class SIM_JETCollider;
class SIM_JETParticleData;

/**
 * Animation
 * - update(frame)
 * 		- onUpdate(frame)
 * 			- initialize()
 * 				- onInitialize()
 * 					- updateCollider(dt)
 * 					- updateEmitter(dt)
 * 			- advanceTimeStep(dt)
 * 				- onAdvanceTimeStep(dt)
 * 					- beginAdvanceTimeStep(dt)
 * 						- clear force
 * 						- updateCollider(dt)
 * 						- updateEmitter(dt)
 * 						- allocate buffers
 * 						- onBeginAdvanceTimeStep(dt)
 * 							- buildNeighborLists()
 * 							- updateDensities()
 * 					- accumulateForces(dt)
 * 						- accumulateNonPressureForces(dt)
 * 							- accumulateExternalForces()
 * 							- accumulateViscosityForce()
 * 						- accumulatePressureForce(dt)
 * 							- computePressure()
 * 							- accumulatePressureForce(x, d, p, f)
 * 					- timeIntegration(dt)
 * 					- resolveCollision()
 * 					- endAdvanceTimeStep(dt)
 * 						- update data
 * 						- onEndAdvanceTimeStep(dt)
 * 							- computePseudoViscosity(dt)
 * 							- record max density
 */

class GAS_JETPciSPHSolver : public GAS_SubSolver, public jet::PciSphSolver3
{
public:
	GETSET_DATA_FUNCS_F("target_density", TargetDensity)
	GETSET_DATA_FUNCS_F("target_spacing", TargetSpacing)
	GETSET_DATA_FUNCS_F("relative_kernel_radius", RelativeKernelRadius)
	GETSET_DATA_FUNCS_V3("bounding_box_size", BoundingBoxSize);
	GETSET_DATA_FUNCS_V3("bounding_box_center", BoundingBoxCenter);

protected:
	GAS_JETPciSPHSolver(const SIM_DataFactory *factory) : BaseClass(factory), jet::PciSphSolver3(1000.f, 0.02f, 1.8f) {}
	~GAS_JETPciSPHSolver() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

	SIM_Guide *createGuideObjectSubclass() const override;
	void buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const override;
	static PRM_Name showGuideGeometry;

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_JETPciSPHSolver,
					GAS_SubSolver,
					"JET PciSPH SubSolver",
					getDopDescription());

private:
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg);

	const jet::ParticleSystemData3Ptr ExtractJetParticleData(SIM_Object *obj, UT_WorkBuffer &error_msg);
	const jet::Vector3D ExtractGravity(SIM_Object *obj, UT_WorkBuffer &error_msg);
	const jet::Collider3Ptr ExtractColliders(SIM_Object *obj, UT_WorkBuffer &error_msg);
	const jet::ParticleEmitter3Ptr ExtractEmitter(SIM_Object *obj, UT_WorkBuffer &error_msg);
};

#endif //HINAPE_JET_GAS_JETPCISPHSOLVER_H
