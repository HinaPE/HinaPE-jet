#include "SIM_JETParticleData.h"

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

const SIM_DopDescription *SIM_JETParticleData::GetDescription()
{
	static PRM_Name DataType[] =
			{
					PRM_Name("0", "SPHData"),
					PRM_Name("1", "PciSPHData"),
					PRM_Name(0)
			};

	static PRM_ChoiceList particleDataType((PRM_ChoiceListType) (PRM_CHOICELIST_SINGLE), &(DataType[0]));
	static PRM_Name particleDataTypeName("particleDataType", "Particle Data Type");

	static PRM_Name targetDensity("target_density", "Target Density");
	static PRM_Default targetDensityDefault(1000.f);

	static PRM_Name targetSpacing("target_spacing", "Target Spacing");
	static PRM_Default targetSpacingDefault(0.02f);

	static PRM_Name relativeKernelRadius("relative_kernel_radius", "Relative Kernel Radius");
	static PRM_Default relativeKernelRadiusDefault(1.8f);

	static std::array<PRM_Template, 5> PRMS{
			PRM_Template(PRM_INT, 1, &particleDataTypeName, 0, &particleDataType),
			PRM_Template(PRM_FLT, 1, &targetDensity, &targetDensityDefault),
			PRM_Template(PRM_FLT, 1, &targetSpacing, &targetSpacingDefault),
			PRM_Template(PRM_FLT, 1, &relativeKernelRadius, &relativeKernelRadiusDefault),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_particle_data",
								   "Jet Particle Data",
								   JET_PARTICLEDATA_DATANAME,
								   classname(),
								   PRMS.data());
	return &DESC;
}

void SIM_JETParticleData::initializeSubclass()
{
	BaseClass::initializeSubclass();

	float target_density = getTargetDensity();
	float target_spacing = getTargetSpacing();
	float relative_kernel_radius = getRelativeKernelRadius();

	auto data = std::make_shared<jet::SphSystemData3>();
	data->setTargetDensity(target_density);
	data->setTargetSpacing(target_spacing);
	data->setRelativeKernelRadius(relative_kernel_radius);
	this->InnerDataPtr.reset();
	this->InnerDataPtr = data;
}

void SIM_JETParticleData::makeEqualSubclass(const SIM_Data *source)
{
	BaseClass::makeEqualSubclass(source);

	const SIM_JETParticleData *src = SIM_DATA_CASTCONST(source, SIM_JETParticleData);
	this->InnerDataPtr = src->InnerDataPtr;
}
