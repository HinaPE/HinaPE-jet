#include "SIM_JETCollider.h"

#include <SIM/SIM_Engine.h>
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

#include <GU/GU_PolyWire.h>

#include <SOP/SOP_Node.h>

#include "hdk_interface/util_geometry.h"

PRM_Name SIM_JETCollider::showGuideGeometry("show_guide_geometry", "Show Guide Geometry");

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

	static std::array<PRM_Template, 2> PRMS_GUIDE{
			PRM_Template(PRM_TOGGLE, 1, &showGuideGeometry, PRMoneDefaults),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_collider",
								   "Jet Collider",
								   JET_COLLIDER_DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setGuideTemplates(PRMS_GUIDE.data());
	DESC.setDefaultUniqueDataName(true);
	return &DESC;
}

void SIM_JETCollider::initializeSubclass()
{
	SIM_Data::initializeSubclass();

	InnerSurfacePtr = nullptr;
}

void SIM_JETCollider::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);

	const SIM_JETCollider *src = SIM_DATA_CASTCONST(source, SIM_JETCollider);
	InnerSurfacePtr = src->InnerSurfacePtr;
}

SIM_Guide *SIM_JETCollider::createGuideObjectSubclass() const
{
	return new SIM_GuideShared(this, true);
}

void SIM_JETCollider::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	if (!options.hasOption(showGuideGeometry.getToken()) || !options.getOptionB(showGuideGeometry.getToken()))
		return;

	const UT_StringHolder &SOPPath = getSOPPath();
	SOP_Node *sop = getSOPNode(SOPPath, true);

	if (!sop)
		return;

	OP_Context context(t);
	GU_DetailHandle gdh_sop = sop->getCookedGeoHandle(context);
	const GU_Detail *gdp_sop = sop->getCookedGeo(context);

	if (!gdh.isNull())
	{
		GU_DetailHandleAutoWriteLock gdl(gdh);
		GU_Detail *gdp = gdl.getGdp();
		gdp->clear();
		gdp->duplicate(*gdp_sop);

//		ConvertPolyMeshToWireframe<fpreal>(*gdp);
	}
}
