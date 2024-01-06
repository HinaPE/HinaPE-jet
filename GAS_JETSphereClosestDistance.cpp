#include "GAS_JETSphereClosestDistance.h"

#include <SIM/SIM_Engine.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Utils.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_PositionSimple.h>
#include <SIM/SIM_GeometryCopy.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>

#include "jet/jet.h"

bool GAS_JETSphereClosestDistance::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	SIM_JETSphere *sphere;
	sphere = SIM_DATA_GET(*obj, "JETSphere", SIM_JETSphere);

	if (!sphere)
		return false;

	UT_Vector3 s_center = sphere->getJETSphereCenter();
	fpreal32 s_radius = sphere->getJETSphereRadius();

	using namespace jet;
	auto sphere_jet = Sphere3::builder()
			.withCenter({1, 1, 1})
			.withRadius(1).makeShared();
	auto result =  sphere_jet->closestDistance({0, 0, 0});

	return true;
}

const SIM_DopDescription *GAS_JETSphereClosestDistance::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_sphere_closest_distance_subsolver",
								   "JET Sphere Closest Distance SubSolver",
								   "JETSphereClosestDistanceSubSolver",
								   classname(),
								   PRMS.data());
	setGasDescription(DESC);
	return &DESC;
}

const SIM_DopDescription *SIM_JETSphere::GetDescription()
{
	static PRM_Name jet_sphere_center("jet_sphere_center", "JET Sphere Center");
	static std::array<PRM_Default, 3> jet_sphere_center_default = {0, 0, 0};

	static PRM_Name jet_sphere_radius("jet_sphere_radius", "JET Sphere Radius");
	static PRM_Default jet_sphere_radius_default = 1;

	static std::array<PRM_Template, 3> PRMS{
			PRM_Template(PRM_FLT_J, 3, &jet_sphere_center, jet_sphere_center_default.data()),
			PRM_Template(PRM_FLT_J, 1, &jet_sphere_radius, &jet_sphere_radius_default),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_sphere",
								   "JET Sphere",
								   "JETSphere",
								   classname(),
								   PRMS.data());
	return &DESC;
}
