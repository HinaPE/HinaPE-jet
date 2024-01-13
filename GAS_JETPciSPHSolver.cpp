#include "GAS_JETPciSPHSolver.h"

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

#include <SOP/SOP_Node.h>

#include <SIM_JETParticleData.h>
#include <SIM_JETCollider.h>
#include <GAS_JETVolumeEmitter.h>

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

#include "hdk_interface/util_geometry.h"

PRM_Name GAS_JETPciSPHSolver::showGuideGeometry("show_guide_geometry", "Show Guide Geometry");

const SIM_DopDescription *GAS_JETPciSPHSolver::getDopDescription()
{
	static PRM_Name theGeometryName(GAS_NAME_GEOMETRY, "Geometry");
	static PRM_Default theGeometryNameDefault(0, "Geometry");

	static PRM_Name targetDensity("target_density", "Target Density");
	static PRM_Default targetDensityDefault(1000.f);

	static PRM_Name targetSpacing("target_spacing", "Target Spacing");
	static PRM_Default targetSpacingDefault(0.02f);

	static PRM_Name relativeKernelRadius("relative_kernel_radius", "Relative Kernel Radius");
	static PRM_Default relativeKernelRadiusDefault(1.8f);

	static PRM_Name boundingBoxSize("bounding_box_size", "BoundingBox Size");
	static std::array<PRM_Default, 3> boundingBoxSizeDefault{2, 2, 2};

	static PRM_Name boundingBoxCenter("bounding_box_center", "BoundingBox Center");
	static std::array<PRM_Default, 3> boundingBoxCenterDefault{0, 0, 0};

	static std::array<PRM_Template, 7> PRMS{
			PRM_Template(PRM_STRING, 1, &theGeometryName, &theGeometryNameDefault),
			PRM_Template(PRM_FLT, 1, &targetDensity, &targetDensityDefault),
			PRM_Template(PRM_FLT, 1, &targetSpacing, &targetSpacingDefault),
			PRM_Template(PRM_FLT, 1, &relativeKernelRadius, &relativeKernelRadiusDefault),
			PRM_Template(PRM_FLT_J, 3, &boundingBoxSize, boundingBoxSizeDefault.data()),
			PRM_Template(PRM_FLT_J, 3, &boundingBoxCenter, boundingBoxCenterDefault.data()),
			PRM_Template()
	};

	static std::array<PRM_Template, 2> PRMS_GUIDE{
			PRM_Template(PRM_TOGGLE, 1, &showGuideGeometry, PRMzeroDefaults),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_pcisph_solver",
								   "JET PciSPH Solver",
								   JET_PCISPHSOLVER_DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setGuideTemplates(PRMS_GUIDE.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_JETPciSPHSolver::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
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

void GAS_JETPciSPHSolver::initializeSubclass()
{
	SIM_Data::initializeSubclass();

	// Init Solver
	setPseudoViscosityCoefficient(0);
	setIsUsingFixedSubTimeSteps(false);
}

void GAS_JETPciSPHSolver::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);

	const GAS_JETPciSPHSolver *src = SIM_DATA_CASTCONST(source, GAS_JETPciSPHSolver);
	static_cast<jet::PciSphSolver3 &>(*this) = static_cast<const jet::PciSphSolver3 &>(*src); // [important] Notice this usage!
}

SIM_Guide *GAS_JETPciSPHSolver::createGuideObjectSubclass() const
{
	return new SIM_GuideShared(this, true);
}

void GAS_JETPciSPHSolver::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	if (!options.hasOption(showGuideGeometry.getToken()) || !options.getOptionB(showGuideGeometry.getToken()))
		return;

	// TODO: setup Guide Geometry
}

bool GAS_JETPciSPHSolver::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	// Set Particle Data
	{
		const jet::ParticleSystemData3Ptr jet_data_ptr = ExtractJetParticleData(obj, error_msg);
		if (!jet_data_ptr) return false;
		setParticleSystemData(jet_data_ptr);
	}

	// Set Colliders
	{
		const jet::Collider3Ptr jet_colliders = ExtractColliders(obj, error_msg);
		if (!jet_colliders) return false;
		setCollider(jet_colliders);
	}

	// Set Emitter
	{
		const jet::ParticleEmitter3Ptr jet_emitter = ExtractEmitter(obj, error_msg);
		if (!jet_emitter) return false;
		setEmitter(jet_emitter);
	}

	// External Force
	{
		jet::Vector3D GravityFinal = ExtractGravity(obj, error_msg);
		setGravity(GravityFinal);
	}

	// Solve
	{
		jet::Frame current(engine.getSimulationFrame(time), timestep.operator fpreal64());
		std::cout << "Frame: " << current.index << " Delta Time: " << current.timeIntervalInSeconds << std::endl;
		update(current);
	}

	// Write Back Result to GDP
	if (!SyncGeometry(obj, error_msg))
		return false;

	return true;
}

bool GAS_JETPciSPHSolver::SyncGeometry(SIM_Object *obj, UT_WorkBuffer &error_msg)
{

	return true;
}

const jet::ParticleSystemData3Ptr GAS_JETPciSPHSolver::ExtractJetParticleData(SIM_Object *obj, UT_WorkBuffer &error_msg)
{
	SIM_JETParticleData *particle_data = SIM_DATA_GET(*obj, JET_PARTICLEDATA_DATANAME, SIM_JETParticleData);
	if (!particle_data)
	{
		error_msg.appendSprintf("Object: %s: No SIM_JETParticleData\n", obj->getName().toStdString().c_str());
		return nullptr;
	}

	if (!particle_data->InnerDataPtr)
	{
		error_msg.appendSprintf("Object: %s: No SIM_JETParticleData::InnerDataPtr\n", obj->getName().toStdString().c_str());
		return nullptr;
	}

	return particle_data->InnerDataPtr;
}

const jet::Vector3D GAS_JETPciSPHSolver::ExtractGravity(SIM_Object *obj, UT_WorkBuffer &error_msg)
{
	jet::Vector3D GravityFinal;

	//Find and Apply Gravity
	SIM_ConstDataArray gravities;
	obj->filterConstSubData(gravities, 0, SIM_DataFilterByType("SIM_ForceGravity"), SIM_FORCES_DATANAME, SIM_DataFilterNone());
	for (exint i = 0; i < gravities.entries(); ++i)
	{
		const SIM_ForceGravity *force = SIM_DATA_CASTCONST(gravities(i), SIM_ForceGravity);
		if (!force)
			continue;
		UT_Vector3 outForce, outTorque;
		force->getForce(*obj, UT_Vector3(), UT_Vector3(), UT_Vector3(), 1.0f, outForce, outTorque); // TODO: assume mass is 1.0f

		GravityFinal += {outForce.x(), outForce.y(), outForce.z()};
	}

	return GravityFinal;
}

const jet::Collider3Ptr GAS_JETPciSPHSolver::ExtractColliders(SIM_Object *obj, UT_WorkBuffer &error_msg)
{
	std::vector<jet::Surface3Ptr> surfaces;

	SIM_ObjectArray affectors;
	obj->getAffectors(affectors, "SIM_RelationshipCollide"); // We Only Need Collide Relationship
	exint num_affectors = affectors.entries();
	for (exint i = 0; i < num_affectors; ++i)
	{
		SIM_Object *affector = affectors(i);
		if (!affector->getName().equal(obj->getName()))
		{
			// Get Sim Collider
			SIM_ColliderLabel *collider_label = SIM_DATA_GET(*affector, SIM_COLLIDERS_DATANAME, SIM_ColliderLabel);
			if (!collider_label)
			{
				error_msg.appendSprintf("Object: %s: No SIM_ColliderLabel\n", affector->getName().toStdString().c_str());
				continue;
			}

			// Get Jet Collider
			SIM_JETCollider *collider = SIM_DATA_GET(*collider_label, JET_COLLIDER_DATANAME, SIM_JETCollider);
			if (!collider)
			{
				error_msg.appendSprintf("Object: %s: No SIM_JETCollider\n", affector->getName().toStdString().c_str());
				continue;
			}

			// Check Collider Label
			const UT_StringHolder label_affector = collider_label->getColliderLabel(); // "None", "Default", "Volume"... etc.
			if (label_affector.equal(SIM_COLLIDERLABEL_NONE))
			{

			} else if (label_affector.equal(SIM_COLLIDERLABEL_DEFAULT))
			{

			} else if (label_affector.equal(SIM_COLLIDERLABEL_SELF))
			{

			} else if (label_affector.equal(SIM_COLLIDERLABEL_PARTICLE))
			{

			} else if (label_affector.equal(SIM_COLLIDERLABEL_WIRE))
			{

			} else if (label_affector.equal(SIM_COLLIDERLABEL_CLOTH))
			{

			} else if (label_affector.equal(SIM_COLLIDERLABEL_VOLUME))
			{

			} else if (label_affector.equal(SIM_COLLIDERLABEL_THINPLATE))
			{

			} else if (label_affector.equal(SIM_COLLIDERLABEL_PACKEDOBJECT))
			{

			}

			surfaces.emplace_back(collider->InnerSurfacePtr);
		}
	}

//	auto surface_set = jet::ImplicitSurfaceSet3::builder()
//			.withExplicitSurfaces(surfaces)
//			.makeShared();
//	jet::RigidBodyCollider3Ptr colliders = jet::RigidBodyCollider3::builder().withSurface(surface_set).makeShared();
//	return colliders;

	return jet::RigidBodyCollider3::builder().makeShared();
}

const jet::ParticleEmitter3Ptr GAS_JETPciSPHSolver::ExtractEmitter(SIM_Object *obj, UT_WorkBuffer &error_msg)
{
	return jet::ParticleEmitter3Ptr();
}
