#include <iostream>
#include "PxPhysicsAPI.h"

using namespace std;
using namespace physx;

static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;

int main()
{
	//Init PhysX
	//foundation
	PxFoundation *mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

	if(!mFoundation)
	{
		cerr << "PxCreateFoundation failed!" << endl;
		return 1;
	}

	//physics
	PxPhysics *mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale());

	if(!mPhysics)
	{
		cerr << "PxCreatePhysics failed!" << endl;
		return 1;
	}

	//scene
	PxScene* mScene;
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	if(!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if(!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	mScene = mPhysics->createScene(sceneDesc);

	if (!mScene)
	{
		cerr << "createScene failed!" << endl;
		return 1;
	}

	//materials
	PxMaterial* mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.1f);    //static friction, dynamic friction, restitution

	//sphere
	PxRigidDynamic* mSphereActor = mPhysics->createRigidDynamic(PxTransform(PxVec3(0,10,0)));
	PxShape* mSphereShape = mSphereActor->createShape(PxSphereGeometry(1.0), *mMaterial);
	PxRigidBodyExt::updateMassAndInertia(*mSphereActor, 10.);
	mScene->addActor(*mSphereActor);

	//plane
	PxRigidStatic* plane = PxCreatePlane(*mPhysics, PxPlane(PxVec3(0,1,0), 0), *mMaterial);
	mScene->addActor(*plane);


	cerr << "OK!" << endl;

	for (int i = 0; i < 100; i++)
	{
		mScene->simulate(1./60.);
		mScene->fetchResults(true);

		PxVec3 position = mSphereActor->getGlobalPose().p;
		cerr << position.x << " " << position.y << " " << position.z << endl;
	}

	//release resources
	mPhysics->release();
	mFoundation->release();

	return 0;
}