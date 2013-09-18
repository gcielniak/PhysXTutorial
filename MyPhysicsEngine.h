#pragma once

#include "PhysicsEngine.h"

class MyPhysicsEngine : public PhysicsEngine
{
	PxRigidDynamic* sphere;
	PxRigidStatic* plane;

public:
	PxRigidDynamic* AddSphere(PxVec3 position, PxReal radius=1.0, PxReal mass=1.0)
	{		
		//sphere
		PxRigidDynamic* sphere = physics->createRigidDynamic(PxTransform(position));
		sphere->createShape(PxSphereGeometry(radius), *default_material);
		PxRigidBodyExt::setMassAndUpdateInertia(*sphere, mass);
		scene->addActor(*sphere);
		return sphere;
	}

	PxRigidStatic* AddStaticPlane(PxVec3 normal, PxReal distance=0.0f)
	{
		PxRigidStatic* plane = PxCreatePlane(*physics, PxPlane(normal, distance), *default_material);
		scene->addActor(*plane);
		return plane;
	}

	PxRigidStatic* AddStaticPlaneXZ(PxReal distance=0.0f)
	{
		return AddStaticPlane(PxVec3(0.0, 1.0, 0.0), distance);
	}

	PxRigidDynamic* AddBox(PxVec3 position, PxReal hx=1.0f, PxReal hy=1.0f, PxReal hz=1.0f, PxReal mass=1.0f)
	{
		PxRigidDynamic* box = physics->createRigidDynamic(PxTransform(position));
		box->createShape(PxBoxGeometry(hx, hy, hz), *default_material);
		PxRigidBodyExt::setMassAndUpdateInertia(*box, mass);
		scene->addActor(*box);
		return box;
	}

	virtual void InitScene()
	{
		//Init scene
		sphere = AddSphere(PxVec3(0.0f, 10.0f, 0.0f));
		plane = AddStaticPlaneXZ();
	}

	virtual void UpdateScene()
	{
		sphere->setGlobalPose(PxTransform(sphere->getGlobalPose().p + PxVec3(0.0f,0.0f,0.01f)));
	}
};