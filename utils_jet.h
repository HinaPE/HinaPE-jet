#ifndef HINAPE_JET_UTILS_JET_H
#define HINAPE_JET_UTILS_JET_H

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

#include "jet/jet.h"

template<typename T>
static T* GetJetSIMDataFromObject(SIM_Object *obj, UT_WorkBuffer &error_msg)
{
	T *particle_data = SIM_DATA_GET(*obj, T::DATANAME, T);
	if (!particle_data)
	{
		error_msg.appendSprintf("Object: %s: No %s\n", obj->getName().toStdString().c_str(), T::DATANAME);
		return nullptr;
	}
	return particle_data;
}

#endif //HINAPE_JET_UTILS_JET_H
