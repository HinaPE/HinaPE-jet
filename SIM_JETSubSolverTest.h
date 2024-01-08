#ifndef HINAPE_JET_SIM_JETSUBSOLVERTEST_H
#define HINAPE_JET_SIM_JETSUBSOLVERTEST_H

#include <GAS/GAS_SubSolver.h>
#include <GAS/GAS_GeometryToSDF.h>
#include <GAS/GAS_Utils.h>
#include <GAS/GAS_Advect.h>
#include <GAS/GAS_RepeatSolver.h>

class SIM_JETSubSolverTest : public GAS_SubSolver
{
protected:
	SIM_JETSubSolverTest(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~SIM_JETSubSolverTest() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	static const SIM_DopDescription	*getDopDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_JETSubSolverTest,
					GAS_SubSolver,
					"JET TEST SubSolver",
					getDopDescription());
};

#endif //HINAPE_JET_SIM_JETSUBSOLVERTEST_H
