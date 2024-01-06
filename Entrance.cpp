#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "SIM_JETSPHSolver.h"
#include "SIM_JETSubSolverTest.h"
#include "GAS_JETSphereClosestDistance.h"

void initializeSIM(void *)
{
	IMPLEMENT_DATAFACTORY(SIM_JETSPHSolver);
	IMPLEMENT_DATAFACTORY(SIM_JETSubSolverTest);
	IMPLEMENT_DATAFACTORY(GAS_JETSphereClosestDistance);
	IMPLEMENT_DATAFACTORY(SIM_JETSphere);
}
