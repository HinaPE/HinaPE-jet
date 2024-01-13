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

#include "SIM_JETParticleData.h"
#include "jet/jet.h"

PRM_Name GAS_JETVolumeEmitter::showGuideGeometry("show_guide_geometry", "Show Guide Geometry");

bool GAS_JETVolumeEmitter::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	SIM_GeometryCopy *geometry = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	SIM_JETParticleData *ParticleData = FetchJetParticleData(obj);

	{
		// Sync Simulation Result
		SIM_GeometryAutoWriteLock lock(geometry);
		GU_Detail &gdp = lock.getGdp();

//		gdp.getOffset
	}

	return true;
}

const SIM_DopDescription *GAS_JETVolumeEmitter::getDopDescription()
{
	static PRM_Name theGeometryName(GAS_NAME_GEOMETRY, "Geometry");
	static PRM_Default theGeometryNameDefault(0, "Geometry");

	static PRM_Name sopPathName("soppath", "SOP Path");

	static PRM_Name targetSpacing("target_spacing", "Target Spacing");
	static PRM_Default targetSpacingDefault(0.02);

	static PRM_Name initialVelocity("initial_velocity", "Initial Velocity");
	static std::array<PRM_Default, 3> initialVelocityDefault = {0, 0, 0};

	static PRM_Name linearVelocity("linear_velocity", "Linear Velocity");
	static std::array<PRM_Default, 3> linearVelocityDefault = {0, 0, 0};

	static PRM_Name angularVelocity("angular_velocity", "Angular Velocity");
	static std::array<PRM_Default, 3> angularVelocityDefault = {0, 0, 0};

	static PRM_Name maxNumberOfParticles("max_number_of_particles", "Max Number Of Particles");
	static PRM_Default maxNumberOfParticlesDefault(10e7);

	static PRM_Name jitter("jitter", "Jitter");
	static PRM_Default jitterDefault(0);

	static PRM_Name isOneShot("is_one_shot", "Is One Shot");
	static PRM_Default isOneShotDefault(true);

	static PRM_Name allowOverlapping("allow_overlapping", "Allow Overlapping");
	static PRM_Default allowOverlappingDefault(false);

	static PRM_Name randomSeed("random_seed", "RandomSeed");
	static PRM_Default randomSeedDefault(0);

	static std::array<PRM_Template, 11> PRMS{
			PRM_Template(PRM_STRING, 1, &theGeometryName, &theGeometryNameDefault),
			PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &sopPathName,
						 0, 0, 0, 0, &PRM_SpareData::sopPath),
			PRM_Template(PRM_FLT, 1, &targetSpacing, &targetSpacingDefault),
			PRM_Template(PRM_FLT, 3, &initialVelocity, initialVelocityDefault.data()),
			PRM_Template(PRM_FLT, 3, &linearVelocity, linearVelocityDefault.data()),
			PRM_Template(PRM_FLT, 3, &angularVelocity, angularVelocityDefault.data()),
			PRM_Template(PRM_FLT, 1, &maxNumberOfParticles, &maxNumberOfParticlesDefault),
			PRM_Template(PRM_TOGGLE, 1, &isOneShot, &isOneShotDefault),
			PRM_Template(PRM_TOGGLE, 1, &allowOverlapping, &allowOverlappingDefault),
			PRM_Template(PRM_FLT, 1, &randomSeed, &randomSeedDefault),
			PRM_Template()
	};

	static std::array<PRM_Template, 2> PRMS_GUIDE{
			PRM_Template(PRM_TOGGLE, 1, &showGuideGeometry, PRMzeroDefaults),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_volume_emitter",
								   "JET Volume Emitter",
								   "JETVolumeEmitter",
								   classname(),
								   PRMS.data());
	DESC.setGuideTemplates(PRMS_GUIDE.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

SIM_Guide *GAS_JETVolumeEmitter::createGuideObjectSubclass() const
{
	return new SIM_GuideShared(this, true);
}

void GAS_JETVolumeEmitter::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	if (!options.hasOption(showGuideGeometry.getToken()) || !options.getOptionB(showGuideGeometry.getToken()))
		return;

	const UT_StringHolder &SOPPath = getSOPPath();
	SOP_Node *sop = getSOPNode(SOPPath, true);

	if (!sop)
	{
		return;
	}

	OP_Context context(t);
	GU_DetailHandle gdh_emitter = sop->getCookedGeoHandle(context);
	const GU_Detail *gdp_emitter = sop->getCookedGeo(context);

	if (!gdh.isNull())
	{
		GU_DetailHandleAutoWriteLock gdl(gdh);
		GU_Detail *gdp = gdl.getGdp();
		gdp->clear();
		gdp->duplicate(*gdp_emitter);
	}
}

SIM_JETParticleData *GAS_JETVolumeEmitter::FetchJetParticleData(SIM_Object *obj)
{
	return SIM_DATA_GET(*obj, JET_PARTICLEDATA_DATANAME, SIM_JETParticleData);
}