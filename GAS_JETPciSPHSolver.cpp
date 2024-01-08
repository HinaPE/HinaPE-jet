#include "GAS_JETPciSPHSolver.h"

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

#include <SIM_JETParticleData.h>

PRM_Name GAS_JETPciSPHSolver::showGuideGeometry("show_guide_geometry", "Show Guide Geometry");

GAS_JETPciSPHSolver::GAS_JETPciSPHSolver(const SIM_DataFactory *factory) : BaseClass(factory), jet::PciSphSolver3(1000.f, 0.02f, 1.8f) {}

bool GAS_JETPciSPHSolver::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	LoadParticleData(obj);

//	SIM_GeometryCopy *geometry = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);

	SaveParticleData(obj);

	return true;
}

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

	static std::array<PRM_Template, 5> PRMS{
			PRM_Template(PRM_STRING, 1, &theGeometryName, &theGeometryNameDefault),
			PRM_Template(PRM_FLT, 1, &targetDensity, &targetDensityDefault),
			PRM_Template(PRM_FLT, 1, &targetSpacing, &targetSpacingDefault),
			PRM_Template(PRM_FLT, 1, &relativeKernelRadius, &relativeKernelRadiusDefault),
			PRM_Template()
	};

	static std::array<PRM_Template, 2> PRMS_GUIDE{
			PRM_Template(PRM_TOGGLE, 1, &showGuideGeometry, PRMzeroDefaults),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "jet_pcisph_solver",
								   "JET PciSPH Solver",
								   "JETPciSPHSolver",
								   classname(),
								   PRMS.data());
	DESC.setGuideTemplates(PRMS_GUIDE.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

SIM_Guide *GAS_JETPciSPHSolver::createGuideObjectSubclass() const
{
	return new SIM_GuideShared(this, true);
}

void GAS_JETPciSPHSolver::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	if (!options.hasOption(showGuideGeometry.getToken()) || !options.getOptionB(showGuideGeometry.getToken()))
		return;

//	const UT_StringHolder &SOPPath = getSOPPath();
//	SOP_Node *sop = getSOPNode(SOPPath, true);
//	OP_Context context(t);
//	GU_DetailHandle gdh_emitter = sop->getCookedGeoHandle(context);
//	const GU_Detail *gdp_emitter = sop->getCookedGeo(context);
//
//	if (!gdh.isNull())
//	{
//		GU_DetailHandleAutoWriteLock gdl(gdh);
//		GU_Detail *gdp = gdl.getGdp();
//		gdp->clear();
//		gdp->duplicate(*gdp_emitter);
//	}
}

void GAS_JETPciSPHSolver::LoadParticleData(SIM_Object *obj)
{
	SIM_JETParticleData *ParticleData = SIM_DATA_GET(*obj, JETParticleData_Name, SIM_JETParticleData);
	setParticleSystemData(ParticleData->InnerDataPtr);

	// TODO: sync geometry
}
void GAS_JETPciSPHSolver::SaveParticleData(SIM_Object *obj)
{
	SIM_JETParticleData *ParticleData = SIM_DATA_GET(*obj, JETParticleData_Name, SIM_JETParticleData);
	const jet::ParticleSystemData3Ptr &data = particleSystemData();

	// TODO: sync geometry
}
