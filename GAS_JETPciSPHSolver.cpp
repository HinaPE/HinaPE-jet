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

#include <GEO/GEO_PrimPart.h>

#include <SOP/SOP_Node.h>

#include <SIM_JETParticleData.h>
#include <SIM_JETCollider.h>
#include <GAS_JETVolumeEmitter.h>

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

#include "hdk_interface/util_geometry.h"
#include "utils_jet.h"

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
	jet::Logging::mute();
//
//	jet::BoundingBox3D domain(jet::Vector3D(), jet::Vector3D(1, 2, 1));
//
//	// Set Particle Data
//	{
//		const jet::ParticleSystemData3Ptr jet_data_ptr = ExtractJetParticleData(obj, error_msg);
//		if (!jet_data_ptr) return false;
//		setParticleSystemData(jet_data_ptr);
//	}
//
//	{
//		jet::ParticleSystemData3Ptr jet_data_ptr = ExtractJetParticleData(obj, error_msg);
//
//		// 假设这个solver是发射粒子用的（不一定必须是JET的emitter）
//		for (int i = 0; i < jet_data_ptr->positions().size(); ++i)
//		{
//			jet::Vector3D pos = jet_data_ptr->positions()[i];
//		}
//	}
//
//	// Set Colliders
//	{
////		const jet::Collider3Ptr jet_colliders = ExtractColliders(obj, error_msg);
////		if (!jet_colliders) return false;
//		auto box = jet::Box3::builder()
//				.withIsNormalFlipped(true)
//				.withBoundingBox(domain)
//				.makeShared();
//
//		auto collider = jet::RigidBodyCollider3::builder().withSurface(box).makeShared();
//		setCollider(collider);
//	}
//
//	// Set Emitter
//	{
////		const jet::ParticleEmitter3Ptr jet_emitter = ExtractEmitter(obj, error_msg);
////		if (!jet_emitter) return false;
//		jet::BoundingBox3D sourceBound(domain);
//		sourceBound.expand(-getTargetSpacing());
//
//		auto plane = jet::Plane3::builder()
//				.withNormal({0, 1, 0})
//				.withPoint({0, 0.25 * domain.height(), 0})
//				.makeShared();
//
//		auto sphere = jet::Sphere3::builder()
//				.withCenter(domain.midPoint())
//				.withRadius(0.15 * domain.width())
//				.makeShared();
//
//		auto surfaceSet = jet::ImplicitSurfaceSet3::builder()
//				.withExplicitSurfaces({plane, sphere})
//				.makeShared();
//
//		auto emitter = jet::VolumeParticleEmitter3::builder()
//				.withImplicitSurface(surfaceSet)
//				.withSpacing(getTargetSpacing())
//				.withMaxRegion(sourceBound)
//				.withIsOneShot(true)
//				.makeShared();
//		setEmitter(emitter);
//	}
//
//	// External Force
//	{
//		jet::Vector3D GravityFinal = ExtractJetGravity(obj, error_msg);
//		setGravity(GravityFinal);
//	}
//
//	// Solve
//	{
//		jet::Frame current(engine.getSimulationFrame(time), timestep.operator fpreal64());
//		std::cout << "Frame: " << current.index << " Delta Time: " << current.timeIntervalInSeconds << std::endl;
//		update(current);
//		std::cout << "Completed " << std::endl;
//	}
//
////	// Write Back Result to GDP
//	if (!SyncGeometry(obj, error_msg))
//		return false;

	return true;
}

bool GAS_JETPciSPHSolver::SyncGeometry(SIM_Object *obj, UT_WorkBuffer &error_msg)
{
	SIM_GeometryCopy *geo = nullptr;
	geo = SIM_DATA_GET(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy);
	if (!geo)
	{
		geo = SIM_DATA_CREATE(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy,
							  SIM_DATA_RETURN_EXISTING | SIM_DATA_ADOPT_EXISTING_ON_DELETE);
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		gdp.clearAndDestroy();
	}

	if (!geo)
		return false;

	SIM_GeometryAutoWriteLock lock(geo);
	GU_Detail &gdp = lock.getGdp();
	gdp.clearAndDestroy();

	for (const auto &pos: particleSystemData()->positions())
	{
		GA_Offset offset = gdp.appendPoint();
		gdp.setPos3(offset, UT_Vector3(pos.x, pos.y, pos.z));
	}

	return true;
}
