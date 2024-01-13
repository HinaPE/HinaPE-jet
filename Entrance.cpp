#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "SIM_JETParticleData.h"
#include "SIM_JETEmitterSource.h"
#include "SIM_JETCollider.h"

#include "GAS_JETVolumeEmitter.h"
#include "GAS_JETPciSPHSolver.h"

void initializeSIM(void *)
{
	IMPLEMENT_DATAFACTORY(SIM_JETParticleData);
	IMPLEMENT_DATAFACTORY(SIM_JETEmitterSource);
	IMPLEMENT_DATAFACTORY(SIM_JETCollider);

	IMPLEMENT_DATAFACTORY(GAS_JETVolumeEmitter);
	IMPLEMENT_DATAFACTORY(GAS_JETPciSPHSolver);
}
