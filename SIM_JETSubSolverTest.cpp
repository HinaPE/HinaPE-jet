#include "SIM_JETSubSolverTest.h"

#include <SIM/SIM_Engine.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Utils.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_PositionSimple.h>
#include "jet/jet.h"
bool SIM_JETSubSolverTest::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	jet::Vector3D t(1,2,3);
	std::cout << t.min() << std::endl;
	return true;
}

#include <SIM/SIM_GeometryCopy.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>

const SIM_DopDescription *SIM_JETSubSolverTest::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_test_subsolver",
								   "JET Test SubSolver",
								   "JETTestSubSolver",
								   classname(),
								   PRMS.data());
	setGasDescription(DESC);
	return &DESC;
}
