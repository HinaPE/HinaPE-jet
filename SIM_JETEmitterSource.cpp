#include "SIM_JETEmitterSource.h"

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

const SIM_DopDescription *SIM_JETEmitterSource::GetDescription()
{
	static PRM_Name sopPathName("soppath", "SOP Path");

	static PRM_Name targetDensity("target_density", "Target Density");
	static PRM_Default targetDensityDefault(1000.f);

	static PRM_Name targetSpacing("target_spacing", "Target Spacing");
	static PRM_Default targetSpacingDefault(0.02f);

	static PRM_Name relativeKernelRadius("relative_kernel_radius", "Relative Kernel Radius");
	static PRM_Default relativeKernelRadiusDefault(1.8f);

	static std::array<PRM_Template, 5> PRMS{
			PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &sopPathName,
						 0, 0, 0, 0, &PRM_SpareData::sopPath),
			PRM_Template(PRM_FLT, 1, &targetDensity, &targetDensityDefault),
			PRM_Template(PRM_FLT, 1, &targetSpacing, &targetSpacingDefault),
			PRM_Template(PRM_FLT, 1, &relativeKernelRadius, &relativeKernelRadiusDefault),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_emitter_source",
								   "Jet Emitter Source",
								   JET_EMITTERSOURCE_DATANAME,
								   classname(),
								   PRMS.data());
	return &DESC;
}

void SIM_JETEmitterSource::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void SIM_JETEmitterSource::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);

	const SIM_JETEmitterSource *src = SIM_DATA_CASTCONST(source, SIM_JETEmitterSource);
	InnerSurfacePtr = src->InnerSurfacePtr;
}
