#include "GAS_TESTAddARandomParticleSolver.h"

const SIM_DopDescription *GAS_TESTAddARandomParticleSolver::getDopDescription()
{
	return nullptr;
}

bool GAS_TESTAddARandomParticleSolver::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	return false;
}

void GAS_TESTAddARandomParticleSolver::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_TESTAddARandomParticleSolver::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

SIM_Guide *GAS_TESTAddARandomParticleSolver::createGuideObjectSubclass() const
{
	return SIM_Data::createGuideObjectSubclass();
}

void GAS_TESTAddARandomParticleSolver::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	SIM_Data::buildGuideGeometrySubclass(root, options, gdh, xform, t);
}

bool GAS_TESTAddARandomParticleSolver::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	return true;
}
