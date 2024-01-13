#ifndef HINAPE_JET_SIM_JETCOLLIDER_H
#define HINAPE_JET_SIM_JETCOLLIDER_H

#include <SIM/SIM_ColliderPoint.h>
#include <SIM/SIM_ColliderInfo.h>
#include <SIM/SIM_ColliderBFA.h>
#include <SIM/SIM_SDFCollision.h>

#include <PRM/PRM_Name.h>

#include "jet/jet.h"

#define JET_COLLIDER_DATANAME "JetCollider"

class SIM_JETCollider : public SIM_Collider
{
public:
	GETSET_DATA_FUNCS_S("soppath", SOPPath)

protected:
	SIM_JETCollider(const SIM_DataFactory *factory) : SIM_Collider(factory) {}
	~SIM_JETCollider() override = default;
	bool collideObjectsSubclass(SIM_Engine &engine, SIM_Object &object, SIM_Object &affector, const SIM_Time &starttime, const SIM_Time &endtime, SIM_ImpactApplyType impactapplytype, int impactflags) const override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *GetDescription();

	SIM_Guide *createGuideObjectSubclass() const override;
	void buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const override;
	static PRM_Name showGuideGeometry;

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_JETCollider, SIM_Collider, "JET Collider", GetDescription());

public:
	jet::Surface3Ptr InnerSurfacePtr;
};

#endif //HINAPE_JET_SIM_JETCOLLIDER_H
