#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "GAS_JETVolumeEmitter.h"
#include "GAS_JETPciSPHSolver.h"
#include "SIM_JETParticleData.h"

void initializeSIM(void *)
{
	IMPLEMENT_DATAFACTORY(SIM_JETParticleData);
	IMPLEMENT_DATAFACTORY(GAS_JETVolumeEmitter);
	IMPLEMENT_DATAFACTORY(GAS_JETPciSPHSolver);
}
