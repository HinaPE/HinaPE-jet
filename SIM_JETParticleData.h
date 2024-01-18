#ifndef HINAPE_JET_SIM_JETPARTICLEDATA_H
#define HINAPE_JET_SIM_JETPARTICLEDATA_H

#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Utils.h>

#include <UT/UT_WorkBuffer.h>

#include "jet/jet.h"

class SIM_JETParticleData : public SIM_Data, public SIM_OptionsUser, public jet::ParticleSystemData3
{
public:
	static const char *DATANAME;

protected:
	SIM_JETParticleData(const SIM_DataFactory *factory) : SIM_Data(factory), SIM_OptionsUser(this) {}
	~SIM_JETParticleData() override = default;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *GetDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_JETParticleData, SIM_Data, "JET Particle Data", GetDescription());

public:
	virtual size_t AddJETParticle(GA_Offset IN_offset, const UT_Vector3 &new_position, const UT_Vector3 &new_velocity = UT_Vector3(0.), const UT_Vector3 &new_force = UT_Vector3(0.));

	virtual bool UpdateToGeometrySheet(SIM_Object *obj, UT_WorkBuffer &error_msg);
	virtual bool UpdateFromGeometrySheet(SIM_Object *obj, UT_WorkBuffer &error_msg);
	static size_t scalar_index_geo_offset;
	static size_t scalar_index_particle_state; // added/dirty/deleted... etc.
	bool dirty = false;

	enum ParticleState
	{
		PARTICLE_ADDED = 0, // default value
		PARTICLE_DELETED = 1,
		PARTICLE_CLEAN = 2,
		PARTICLE_DIRTY = 3,
	};
};

#endif //HINAPE_JET_SIM_JETPARTICLEDATA_H
