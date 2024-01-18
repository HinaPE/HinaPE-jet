#include "GAS_JETSyncData.h"

#include <SIM/SIM_Engine.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_GuideShared.h>
#include <SIM/SIM_ColliderLabel.h>
#include <SIM/SIM_ForceGravity.h>
#include <SIM/SIM_Time.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>
#include <PRM/PRM_Utils.h>
#include <PRM/PRM_SpareData.h>

#include <GEO/GEO_PrimPart.h>

#include <SOP/SOP_Node.h>

#include <SIM_JETParticleData.h>
#include <SIM_JETCollider.h>
#include <GAS_JETVolumeEmitter.h>

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

#include "utils_jet.h"

const SIM_DopDescription *GAS_JETSyncData::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_sync_data",
								   "JET Sync Data",
								   JET_SYNCDATA_DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_JETSyncData::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg))
	{
		if (error_msg.isstring())
			std::cerr << "Error: " << error_msg.buffer() << std::endl;
		return false;
	}

	return true;
}

void GAS_JETSyncData::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_JETSyncData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

SIM_Guide *GAS_JETSyncData::createGuideObjectSubclass() const
{
	return SIM_Data::createGuideObjectSubclass();
}

void GAS_JETSyncData::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	SIM_Data::buildGuideGeometrySubclass(root, options, gdh, xform, t);
}

bool GAS_JETSyncData::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	SIM_JETParticleData *jet_sim_data = GetJetSIMDataFromObject<SIM_JETParticleData>(obj, error_msg);
	if (!jet_sim_data) return false;
	jet::ParticleSystemData3Ptr jet_particle_data = jet_sim_data->InnerDataPtr;
	if (!jet_particle_data) return false;



	return true;
}
