#include "SIM_JETSPHSolver.h"

#include <SIM/SIM_Engine.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_PositionSimple.h>
#include <SIM/SIM_GeometryCopy.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>

SIM_Solver::SIM_Result SIM_JETSPHSolver::solveSingleObjectSubclass(SIM_Engine &engine, SIM_Object &object, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep, bool newobject)
{
	static bool NeedReBuild = true;

	if (NeedReBuild || newobject)
	{
		NeedReBuild = false;
		return SIM_SOLVER_SUCCESS;
	}

	return SIM_Solver::SIM_SOLVER_SUCCESS;
}

const SIM_DopDescription *SIM_JETSPHSolver::GetDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_sph_solver",
								   "JET SPH Solver",
								   "JETSPHSolver",
								   classname(),
								   PRMS.data());
	return &DESC;
}
