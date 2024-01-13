#include "SIM_JETCollider.h"

#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_Position.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_GuideShared.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>
#include <PRM/PRM_Utils.h>
#include <PRM/PRM_SpareData.h>
#include <PRM/PRM_ChoiceList.h>

bool SIM_JETCollider::collideObjectsSubclass(SIM_Engine &engine, SIM_Object &object, SIM_Object &affector, const SIM_Time &starttime, const SIM_Time &endtime, SIM_Collider::SIM_ImpactApplyType impactapplytype, int impactflags) const
{
	return SIM_Collider::collideObjectsSubclass(engine, object, affector, starttime, endtime, impactapplytype, impactflags);
}

const SIM_DopDescription *SIM_JETCollider::GetDescription()
{
	static PRM_Name sopPathName("soppath", "SOP Path");

	static std::array<PRM_Template, 2> PRMS{
			PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &sopPathName,
						 0, 0, 0, 0, &PRM_SpareData::sopPath),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_collider",
								   "Jet Collider",
								   JET_COLLIDER_DATANAME,
								   classname(),
								   PRMS.data());
	return &DESC;
}

void SIM_JETCollider::initializeSubclass()
{
	SIM_Data::initializeSubclass();

	InnerSurfacePtr = jet::TriangleMesh3::builder()
			.makeShared();
}

void SIM_JETCollider::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);

	const SIM_JETCollider *src = SIM_DATA_CASTCONST(source, SIM_JETCollider);
	InnerSurfacePtr = src->InnerSurfacePtr;
}

