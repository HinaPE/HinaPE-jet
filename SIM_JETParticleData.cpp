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
size_t SIM_JETParticleData::scalar_index_is_new = -1;

UT_StringHolder JetIndexAttributeName("JetIdx");

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
	scalar_index_is_new = jet::ParticleSystemData3::addScalarData(true); // is_new flag: 1 for new, 0 for old
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

	if (!geo)
	{
		error_msg.appendSprintf("Geometry Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_GeometryAutoWriteLock lock(geo);
	GU_Detail &gdp = lock.getGdp();

	// for built-in type, we'd better follow the existing attribute name
	GA_RWHandleV3 pos_handle = gdp.getP();
	GA_RWHandleV3 vel_handle = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_VELOCITY));
	GA_RWHandleV3 force_handle = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_MASS));
	GA_RWHandleF mass_handle = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_MASS));
	GA_RWHandleI jet_idx_handle = gdp.findPointAttribute(JetIndexAttributeName);

	auto particle_size = jet::ParticleSystemData3::numberOfParticles();
	auto offset_map_array = jet::ParticleSystemData3::scalarDataAt(scalar_index_geo_offset);
	auto index_is_new_array = jet::ParticleSystemData3::scalarDataAt(scalar_index_is_new);
	auto pos_array = jet::ParticleSystemData3::positions();
	auto vel_array = jet::ParticleSystemData3::velocities();
	auto force_array = jet::ParticleSystemData3::forces();
	auto mass = jet::ParticleSystemData3::mass();

	if (
			particle_size != offset_map_array.size() &&
			particle_size != index_is_new_array.size() &&
			particle_size != pos_array.size() &&
			particle_size != vel_array.size() &&
			particle_size != force_array.size()
			)
	{
		error_msg.appendSprintf("Size Error, From %s\n", DATANAME);
		return false;
	}

	for (int pt_idx = 0; pt_idx < particle_size; ++pt_idx)
	{
		GA_Offset particle_offset = offset_map_array.at(pt_idx);
		bool index_is_new = index_is_new_array.at(pt_idx);
		auto pos = pos_array.at(pt_idx);
		auto vel = vel_array.at(pt_idx);
		auto force = force_array.at(pt_idx);

		if (index_is_new)
		{
			// create a new particle, and update the previous particle_offset
			particle_offset = gdp.appendPoint();
			index_is_new_array.at(pt_idx) = false;
		}

		pos_handle.set(particle_offset, UT_Vector3D{pos.x, pos.y, pos.z});
		vel_handle.set(particle_offset, UT_Vector3D{vel.x, vel.y, vel.z});
		force_handle.set(particle_offset, UT_Vector3D{force.x, force.y, force.z});
		mass_handle.set(particle_offset, mass);
		jet_idx_handle.set(particle_offset, pt_idx);
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

	SIM_GeometryCopy *geo = SIM_DATA_GET(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy); // we need write access

	if (!geo)
	{
		error_msg.appendSprintf("Geometry Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_GeometryAutoWriteLock lock(geo);
	GU_Detail &gdp = lock.getGdp();

	// for built-in type, we'd better follow the existing attribute name
	GA_RWHandleV3 pos_handle = gdp.getP();
	GA_RWHandleV3 vel_handle = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_VELOCITY));
	GA_RWHandleV3 force_handle = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_MASS));
	GA_RWHandleF mass_handle = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_MASS));
	GA_RWHandleI jet_idx_handle = gdp.findPointAttribute(JetIndexAttributeName);

	return true;
}
