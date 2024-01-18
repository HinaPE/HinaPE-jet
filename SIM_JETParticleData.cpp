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
size_t SIM_JETParticleData::scalar_index_particle_state = -1;

UT_StringHolder JetIndexMapAttributeName("JetIdxMap");
UT_StringHolder JetParticleDataStateAttributeName("State");

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
	scalar_index_particle_state = jet::ParticleSystemData3::addScalarData(PARTICLE_ADDED); // ParticleState
}

void SIM_JETParticleData::makeEqualSubclass(const SIM_Data *source)
{
	BaseClass::makeEqualSubclass(source);

	const SIM_JETParticleData *src = SIM_DATA_CASTCONST(source, SIM_JETParticleData);
	static_cast<jet::ParticleSystemData3 &>(*this) = static_cast<const jet::ParticleSystemData3 &>(*src); // [important] Notice this usage!
}

size_t SIM_JETParticleData::AddJETParticle(GA_Offset IN_offset, const UT_Vector3 &new_position, const UT_Vector3 &new_velocity, const UT_Vector3 &new_force)
{
	addParticle(
			jet::Vector3D{new_position.x(), new_position.y(), new_position.z()},
			jet::Vector3D{new_velocity.x(), new_velocity.y(), new_velocity.z()},
			jet::Vector3D{new_force.x(), new_force.y(), new_force.z()}
	);

	size_t new_particle_index = numberOfParticles() - 1;

	// update our unique scalar / vector fields
	auto offset_map_array = jet::ParticleSystemData3::scalarDataAt(scalar_index_geo_offset);
	auto particle_state_array = jet::ParticleSystemData3::scalarDataAt(scalar_index_particle_state);

	offset_map_array.at(new_particle_index) = IN_offset;
	particle_state_array.at(new_particle_index) = PARTICLE_ADDED;

	dirty = true;
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
	GA_RWHandleI jet_idx_map_handle = gdp.findPointAttribute(JetIndexMapAttributeName);
	GA_RWHandleI data_state_handle = gdp.findPointAttribute(JetParticleDataStateAttributeName);

	const auto particle_size = jet::ParticleSystemData3::numberOfParticles();
	auto offset_map_array = jet::ParticleSystemData3::scalarDataAt(scalar_index_geo_offset);
	auto particle_state_array = jet::ParticleSystemData3::scalarDataAt(scalar_index_particle_state);
	const auto pos_array = jet::ParticleSystemData3::positions();
	const auto vel_array = jet::ParticleSystemData3::velocities();
	const auto force_array = jet::ParticleSystemData3::forces();
	const auto mass = jet::ParticleSystemData3::mass();

	if (
			particle_size != offset_map_array.size() &&
			particle_size != particle_state_array.size() &&
			particle_size != pos_array.size() &&
			particle_size != vel_array.size() &&
			particle_size != force_array.size()
			)
	{
		error_msg.appendSprintf("Size Error, From %s\n", DATANAME);
		return false;
	}

	{
		// mark geo data dirty to find all deleted particles
		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				data_state_handle.set(pt_off, PARTICLE_DIRTY);
			}
	}

	for (int pt_idx = 0; pt_idx < particle_size; ++pt_idx)
	{
		GA_Offset particle_offset = offset_map_array.at(pt_idx);
		const bool particle_state = particle_state_array.at(pt_idx);
		const auto pos = pos_array.at(pt_idx);
		const auto vel = vel_array.at(pt_idx);
		const auto force = force_array.at(pt_idx);

		if (particle_state == PARTICLE_ADDED)
		{
			// create a new particle, and update the previous particle_offset
			particle_offset = gdp.appendPoint();
			offset_map_array.at(pt_idx) = particle_offset;
			particle_state_array.at(pt_idx) = PARTICLE_CLEAN;
		}
		jet_idx_map_handle.set(particle_offset, pt_idx); // mapping [GDP Particle] to [Jet Particle]

		pos_handle.set(particle_offset, UT_Vector3D{pos.x, pos.y, pos.z});
		vel_handle.set(particle_offset, UT_Vector3D{vel.x, vel.y, vel.z});
		force_handle.set(particle_offset, UT_Vector3D{force.x, force.y, force.z});
		mass_handle.set(particle_offset, mass);
		data_state_handle.set(particle_offset, PARTICLE_CLEAN);
	}

	{
		// delete all unrelated data
		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				ParticleState state = (ParticleState) data_state_handle.get(pt_off);
				if (state == PARTICLE_DIRTY) // it means this particle is not updated
					gdp.destroyPointOffset(pt_off);
			}
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
	const GA_ROHandleV3 pos_handle = gdp.getP();
	const GA_ROHandleV3 vel_handle = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_VELOCITY));
	const GA_ROHandleV3 force_handle = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_MASS));
	const GA_ROHandleF mass_handle = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_MASS));
	GA_RWHandleI jet_idx_map_handle = gdp.findPointAttribute(JetIndexMapAttributeName);
	GA_RWHandleI data_state_handle = gdp.findPointAttribute(JetParticleDataStateAttributeName);

	// If particles is added/deleted inside Houdini Software, we need to consider the default value that GDP will assign
	// to. We may easily get that, if a new particle is created, the [JetParticleDataStateAttributeName] would be [0],
	// which is also [PARTICLE_ADDED], so it means that [0] represents this particle is new added
	auto particle_size = jet::ParticleSystemData3::numberOfParticles();
	auto offset_map_array = jet::ParticleSystemData3::scalarDataAt(scalar_index_geo_offset);
	auto particle_state_array = jet::ParticleSystemData3::scalarDataAt(scalar_index_particle_state);
	auto pos_array = jet::ParticleSystemData3::positions();
	auto vel_array = jet::ParticleSystemData3::velocities();
	auto force_array = jet::ParticleSystemData3::forces();

	// mark geo data dirty to find all deleted particles
	for (auto &state: particle_state_array)
	{
		state = PARTICLE_DIRTY;
	}

	{
		// sync all GDP points
		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				const UT_Vector3 pos = pos_handle.get(pt_off);
				const UT_Vector3 vel = vel_handle.get(pt_off);
				const UT_Vector3 force = force_handle.get(pt_off);
				const fpreal mass = mass_handle.get(pt_off); // new, we deal with every particle as the same particles
				const ParticleState state = (ParticleState) data_state_handle.get(pt_off);
				int jet_idx = jet_idx_map_handle.get(pt_off);

				if (state == PARTICLE_ADDED)
				{
					jet_idx = AddJETParticle(pt_off, pos, vel, force);
					jet_idx_map_handle.set(pt_off, jet_idx);
					data_state_handle.set(pt_off, PARTICLE_CLEAN);
				}
				offset_map_array.at(jet_idx) = pt_off; // mapping [Jet Particle] to [GDP Particle]

				pos_array.at(jet_idx) = jet::Vector3D{pos.x(), pos.y(), pos.z()};
				vel_array.at(jet_idx) = jet::Vector3D{vel.x(), vel.y(), vel.z()};
				force_array.at(jet_idx) = jet::Vector3D{force.x(), force.y(), force.z()};
			}
	}

	{
		// In Fact, Jet NEVER delete any existing particles.
		// so, currently we don't need to deal with DELETING Behaviour
	}

	dirty = false;

	return true;
}
