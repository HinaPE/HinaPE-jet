#ifndef HINAPE_JET_GAS_TESTADDARANDOMPARTICLESOLVER_H
#define HINAPE_JET_GAS_TESTADDARANDOMPARTICLESOLVER_H

#include <GAS/GAS_SubSolver.h>
#include <PRM/PRM_Default.h>

#include "jet/jet.h"

#define JET_TESTADDARANDOMPARTICLESOLVER_DATANAME "JETTESTAddARandomParticleSolver"

class GAS_TESTAddARandomParticleSolver : public GAS_SubSolver
{
protected:
	GAS_TESTAddARandomParticleSolver(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_TESTAddARandomParticleSolver() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

	SIM_Guide *createGuideObjectSubclass() const override;
	void buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const override;
	static PRM_Name showGuideGeometry;

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_TESTAddARandomParticleSolver,
					GAS_SubSolver,
					"JET Test Add A Random Particle SubSolver",
					getDopDescription());

public:
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg);
};

#endif //HINAPE_JET_GAS_TESTADDARANDOMPARTICLESOLVER_H
