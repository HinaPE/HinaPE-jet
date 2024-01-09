#ifndef HINAPE_JET_SIM_JETCOLLIDER_H
#define HINAPE_JET_SIM_JETCOLLIDER_H

#include <SIM/SIM_ColliderPoint.h>
#include <SIM/SIM_ColliderInfo.h>
#include <SIM/SIM_ColliderBFA.h>


// TODO: implement JET Collision Here
class SIM_JETCollider : public SIM_Collider
{
protected:
	SIM_JETCollider(const SIM_DataFactory *factory) : SIM_Collider(factory) {}
	~SIM_JETCollider() override = default;
	bool collideObjectsSubclass(SIM_Engine &engine, SIM_Object &object, SIM_Object &affector, const SIM_Time &starttime, const SIM_Time &endtime, SIM_ImpactApplyType impactapplytype, int impactflags) const override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription* GetDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_JETCollider, SIM_Collider, "JET Collider", GetDescription());
};

#endif //HINAPE_JET_SIM_JETCOLLIDER_H
