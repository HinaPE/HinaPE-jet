#include "GAS_JETVolumeEmitter.h"

#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_GuideShared.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>
#include <PRM/PRM_Utils.h>
#include <PRM/PRM_SpareData.h>

#include <SOP/SOP_Node.h>

#include "jet/jet.h"

bool GAS_JETVolumeEmitter::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	SIM_GeometryCopy *geometry = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);

	return true;
}

const SIM_DopDescription *GAS_JETVolumeEmitter::getDopDescription()
{
	static PRM_Name showGuideGeometry("show_guide_geometry", "Show Guide Geometry");
	static PRM_Default showGuideGeometryDefault(false);

	static PRM_Name theGeometryName(GAS_NAME_GEOMETRY, "Geometry");
	static PRM_Default theGeometryNameDefault(0, "Geometry");

	static PRM_Name sopPathName("soppath", "SOP Path");

	static std::array<PRM_Template, 4> PRMS{
			PRM_Template(PRM_STRING, 1, &theGeometryName, &theGeometryNameDefault),
			PRM_Template(PRM_TOGGLE, 1, &showGuideGeometry, &showGuideGeometryDefault),
			PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &sopPathName,
						 0, 0, 0, 0, &PRM_SpareData::sopPath),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_volume_emitter",
								   "JET Volume Emitter",
								   "JETVolumeEmitter",
								   classname(),
								   PRMS.data());
	setGasDescription(DESC);
	return &DESC;
}

SIM_Guide *GAS_JETVolumeEmitter::createGuideObjectSubclass() const
{
	return new SIM_GuideShared(this, true);
}

void GAS_JETVolumeEmitter::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	if (!getShowGuideGeometry())
		return;

	const UT_StringHolder &SOPPath = getSOPPath();
	SOP_Node *sop = getSOPNode(SOPPath, true);
	OP_Context context(t);
	GU_DetailHandle gdh_emitter = sop->getCookedGeoHandle(context);
	const GU_Detail* gdp_emitter = sop->getCookedGeo(context);

	if (!gdh.isNull())
	{
		GU_DetailHandleAutoWriteLock gdl(gdh);
		GU_Detail *gdp = gdl.getGdp();
		gdp->clear();
		gdp->duplicate(*gdp_emitter);
	}
}
