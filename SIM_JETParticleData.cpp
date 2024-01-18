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

#include "jet/jet.h"

size_t SIM_JETParticleData::scalar_index_geo_offset = -1;
size_t SIM_JETParticleData::scalar_index_density = -1;
size_t SIM_JETParticleData::scalar_index_pressure = -1;

UT_StringHolder JetIndexAttributeName("JetIdx");
UT_StringHolder DensityAttributeName("Dens");
UT_StringHolder PressureAttributeName("Pres");

const char *SIM_JETParticleData::DATANAME = "JetParticleData";

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
								   DATANAME,
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

	setTargetDensity(target_density);
	setTargetSpacing(target_spacing);
	setRelativeKernelRadius(relative_kernel_radius);

	scalar_index_geo_offset = jet::ParticleSystemData3::addScalarData(); // Mapping Jet Particle Index and HDK gdp
	scalar_index_density = jet::ParticleSystemData3::addScalarData(); //
	scalar_index_pressure = jet::ParticleSystemData3::addScalarData(); //
}

void SIM_JETParticleData::makeEqualSubclass(const SIM_Data *source)
{
	BaseClass::makeEqualSubclass(source);

	const SIM_JETParticleData *src = SIM_DATA_CASTCONST(source, SIM_JETParticleData);
	static_cast<jet::ParticleSystemData3 &>(*this) = static_cast<const jet::ParticleSystemData3 &>(*src); // [important] Notice this usage!
}

bool SIM_JETParticleData::UpdateToGeometrySheet(SIM_Object *obj, UT_WorkBuffer &error_msg)
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_GeometryCopy *geo = SIM_DATA_GET(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy); // we need write access
	SIM_GeometryAutoWriteLock lock(geo);
	lock.getGdp();

	auto particle_size = jet::ParticleSystemData3::numberOfParticles();

	for (int pos_idx = 0; pos_idx < particle_size; ++pos_idx)
	{
		// async positions
		auto pos_array = jet::ParticleSystemData3::positions();
		auto pos_array_size = pos_array.size();

		if (particle_size != pos_array_size)
		{
			error_msg.appendSprintf("particle_size != pos_array_size, From %s\n", DATANAME);
			return false;
		}

		GA_Offset particle_offset = jet::ParticleSystemData3::scalarDataAt(scalar_index_geo_offset).at(pos_idx);
	}


	return true;
}

bool SIM_JETParticleData::UpdateFromGeometrySheet(SIM_Object *obj, UT_WorkBuffer &error_msg)
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	return true;
}
