#include "utils_jet.h"
//
//jet::ParticleSystemData3Ptr ExtractJetParticleData(SIM_Object *obj, UT_WorkBuffer &error_msg)
//{
//	SIM_JETParticleData *particle_data = SIM_DATA_GET(*obj, SIM_JETParticleData::DATANAME, SIM_JETParticleData);
//	if (!particle_data)
//	{
//		error_msg.appendSprintf("Object: %s: No SIM_JETParticleData\n", obj->getName().toStdString().c_str());
//		return nullptr;
//	}
//
//	if (!particle_data->InnerDataPtr)
//	{
//		error_msg.appendSprintf("Object: %s: No SIM_JETParticleData::InnerDataPtr\n", obj->getName().toStdString().c_str());
//		return nullptr;
//	}
//
//	return particle_data->InnerDataPtr;
//}
//
//jet::Vector3D ExtractJetGravity(SIM_Object *obj, UT_WorkBuffer &error_msg)
//{
//	jet::Vector3D GravityFinal;
//
//	SIM_ConstDataArray gravities;
//	obj->filterConstSubData(gravities, 0, SIM_DataFilterByType("SIM_ForceGravity"), SIM_FORCES_DATANAME, SIM_DataFilterNone());
//	for (exint i = 0; i < gravities.entries(); ++i)
//	{
//		const SIM_ForceGravity *force = SIM_DATA_CASTCONST(gravities(i), SIM_ForceGravity);
//		if (!force)
//			continue;
//		UT_Vector3 outForce, outTorque;
//		force->getForce(*obj, UT_Vector3(), UT_Vector3(), UT_Vector3(), 1.0f, outForce, outTorque); // TODO: assume mass is 1.0f
//
//		GravityFinal += {outForce.x(), outForce.y(), outForce.z()};
//	}
//
//	return GravityFinal;
//}
//
//jet::Collider3Ptr ExtractJetColliders(SIM_Object *obj, UT_WorkBuffer &error_msg)
//{
//	std::vector<jet::Surface3Ptr> surfaces;
//
//	SIM_ObjectArray affectors;
//	obj->getAffectors(affectors, "SIM_RelationshipCollide"); // We Only Need Collide Relationship
//	exint num_affectors = affectors.entries();
//	for (exint i = 0; i < num_affectors; ++i)
//	{
//		SIM_Object *affector = affectors(i);
//		if (!affector->getName().equal(obj->getName()))
//		{
//			// Get Sim Collider
//			SIM_ColliderLabel *collider_label = SIM_DATA_GET(*affector, SIM_COLLIDERS_DATANAME, SIM_ColliderLabel);
//			if (!collider_label)
//			{
//				error_msg.appendSprintf("Object: %s: No SIM_ColliderLabel\n", affector->getName().toStdString().c_str());
//				continue;
//			}
//
//			// Get Jet Collider
//			SIM_JETCollider *collider = SIM_DATA_GET(*collider_label, JET_COLLIDER_DATANAME, SIM_JETCollider);
//			if (!collider)
//			{
//				error_msg.appendSprintf("Object: %s: No SIM_JETCollider\n", affector->getName().toStdString().c_str());
//				continue;
//			}
//
//			// Check Collider Label
//			const UT_StringHolder label_affector = collider_label->getColliderLabel(); // "None", "Default", "Volume"... etc.
//			if (label_affector.equal(SIM_COLLIDERLABEL_NONE))
//			{
//
//			} else if (label_affector.equal(SIM_COLLIDERLABEL_DEFAULT))
//			{
//
//			} else if (label_affector.equal(SIM_COLLIDERLABEL_SELF))
//			{
//
//			} else if (label_affector.equal(SIM_COLLIDERLABEL_PARTICLE))
//			{
//
//			} else if (label_affector.equal(SIM_COLLIDERLABEL_WIRE))
//			{
//
//			} else if (label_affector.equal(SIM_COLLIDERLABEL_CLOTH))
//			{
//
//			} else if (label_affector.equal(SIM_COLLIDERLABEL_VOLUME))
//			{
//
//			} else if (label_affector.equal(SIM_COLLIDERLABEL_THINPLATE))
//			{
//
//			} else if (label_affector.equal(SIM_COLLIDERLABEL_PACKEDOBJECT))
//			{
//
//			}
//
//			surfaces.emplace_back(collider->InnerSurfacePtr);
//		}
//	}
//
////	auto surface_set = jet::ImplicitSurfaceSet3::builder()
////			.withExplicitSurfaces(surfaces)
////			.makeShared();
////	jet::RigidBodyCollider3Ptr colliders = jet::RigidBodyCollider3::builder().withSurface(surface_set).makeShared();
////	return colliders;
//
//	return jet::RigidBodyCollider3::builder().makeShared();
//}
//
//jet::ParticleEmitter3Ptr ExtractJetEmitter(SIM_Object *obj, UT_WorkBuffer &error_msg)
//{
//	return jet::ParticleEmitter3Ptr();
//}
