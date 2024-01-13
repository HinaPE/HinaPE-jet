#ifndef HINAPE_JET_SIM_JETEMITTERSOURCE_H
#define HINAPE_JET_SIM_JETEMITTERSOURCE_H

#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Utils.h>

#include "jet/jet.h"

class SIM_JETEmitterSource : public SIM_Data, public SIM_OptionsUser
{
public:
	GETSET_DATA_FUNCS_F("target_density", TargetDensity)
	GETSET_DATA_FUNCS_F("target_spacing", TargetSpacing)
	GETSET_DATA_FUNCS_F("relative_kernel_radius", RelativeKernelRadius)

protected:
	SIM_JETEmitterSource(const SIM_DataFactory *factory) : SIM_Data(factory), SIM_OptionsUser(this) {}
	~SIM_JETEmitterSource() override = default;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription* GetDescription();

	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_JETEmitterSource, SIM_Data, "JET Emitter Source", GetDescription());

public:
	jet::Surface3Ptr InnerSurfacePtr;
};

#endif //HINAPE_JET_SIM_JETEMITTERSOURCE_H
