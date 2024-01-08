#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "SIM_JETSubSolverTest.h"
#include "GAS_JETVolumeEmitter.h"

void initializeSIM(void *)
{
	IMPLEMENT_DATAFACTORY(SIM_JETSubSolverTest);
	IMPLEMENT_DATAFACTORY(GAS_JETVolumeEmitter);
}
