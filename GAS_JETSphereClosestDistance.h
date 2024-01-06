#ifndef HINAPE_JET_GAS_JETSPHERECLOSESTDISTANCE_H
#define HINAPE_JET_GAS_JETSPHERECLOSESTDISTANCE_H

#include <GAS/GAS_SubSolver.h>
#include <GAS/GAS_GeometryToSDF.h>
#include <GAS/GAS_Utils.h>
#include <GAS/GAS_Advect.h>
#include <GAS/GAS_SPH.h>

class GAS_JETSphereClosestDistance : public GAS_SubSolver
{
protected:
	GAS_JETSphereClosestDistance(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_JETSphereClosestDistance() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	static const SIM_DopDescription *getDopDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_JETSphereClosestDistance,
					GAS_SubSolver,
					"JET SphereClosest Distance SubSolver",
					getDopDescription());
};

class SIM_JETSphere : public SIM_Data, public SIM_OptionsUser
{
public:
	GETSET_DATA_FUNCS_V3("jet_sphere_center", JETSphereCenter)
	GETSET_DATA_FUNCS_F("jet_sphere_radius", JETSphereRadius)

	SIM_JETSphere(const SIM_DataFactory *factory) : SIM_Data(factory), SIM_OptionsUser(this) {}
	~SIM_JETSphere() = default;
	static const SIM_DopDescription* GetDescription();
DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_JETSphere, SIM_Data, "JET Sphere", GetDescription());
};

#endif //HINAPE_JET_GAS_JETSPHERECLOSESTDISTANCE_H
