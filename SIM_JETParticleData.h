#ifndef HINAPE_JET_SIM_JETPARTICLEDATA_H
#define HINAPE_JET_SIM_JETPARTICLEDATA_H

#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Utils.h>

#include "jet/jet.h"

#define JETParticleData_Name "JetParticleData"

class SIM_JETParticleData : public SIM_Data, public SIM_OptionsUser
{
public:
	GETSET_DATA_FUNCS_F("target_density", TargetDensity)
	GETSET_DATA_FUNCS_F("target_spacing", TargetSpacing)
	GETSET_DATA_FUNCS_F("relative_kernel_radius", RelativeKernelRadius)

protected:
	SIM_JETParticleData(const SIM_DataFactory *factory) : SIM_Data(factory), SIM_OptionsUser(this) {}
	~SIM_JETParticleData() override = default;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription* GetDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_JETParticleData, SIM_Data, "JET Particle Data", GetDescription());

public:
	jet::ParticleSystemData3Ptr InnerDataPtr;
};

#endif //HINAPE_JET_SIM_JETPARTICLEDATA_H
