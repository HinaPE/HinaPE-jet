#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "SIM_JETSPHSolver.h"

void initializeSIM(void *)
{
	IMPLEMENT_DATAFACTORY(SIM_JETSPHSolver);
}
