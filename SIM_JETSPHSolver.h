#ifndef HINAPE_JET_SIM_JETSPHSOLVER_H
#define HINAPE_JET_SIM_JETSPHSOLVER_H

#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Utils.h>

class SIM_JETSPHSolver : public SIM_SingleSolver, public SIM_OptionsUser
{
private:
	SIM_JETSPHSolver(const SIM_DataFactory *factory) : SIM_SingleSolver(factory), SIM_OptionsUser(this) {}
	~SIM_JETSPHSolver() override = default;
	SIM_Result solveSingleObjectSubclass(SIM_Engine &engine, SIM_Object &object, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep, bool newobject) override;
	static const SIM_DopDescription* GetDescription();

	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_JETSPHSolver, SIM_SingleSolver, "JET SPH Solver Description", GetDescription());
};

#endif //HINAPE_JET_SIM_JETSPHSOLVER_H
