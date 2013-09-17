#pragma once

#include <iostream>
#include "Exception.h"
#include "PxPhysicsAPI.h"

using namespace physx;
using namespace std;

class PhysicsEngine
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

protected:
	PxFoundation* foundation;
	PVD::PvdConnection* vd_connection;
	PxPhysics* physics;
	PxScene* scene;
	PxMaterial* default_material;

public:
	~PhysicsEngine()
	{
		cerr << "PhysicsEngine::~PhysicsEngine" << endl;
		if (scene)
			scene->release();
		if (vd_connection)
			vd_connection->release();
		if (physics)
			physics->release();
		if (foundation)
			foundation->release();
	}

	void Init()
	{
		//foundation
		foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

		if(!foundation)
			throw new Exception("PhysicsEngine::Init, Could not create the foundation.");

		//physics
		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());

		if(!physics)
			throw new Exception("PhysicsEngine::Init, Could not initialise the PhysX SDK.");

		//visual debugger
		vd_connection = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 100, 
			PxVisualDebuggerExt::getAllConnectionFlags());

		//default material
		default_material = physics->createMaterial(0.0, 0.0, 0.5);

		//scene
		PxSceneDesc sceneDesc(physics->getTolerancesScale());

		if(!sceneDesc.cpuDispatcher)
		{
			PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
			sceneDesc.cpuDispatcher = mCpuDispatcher;
		}

		if(!sceneDesc.filterShader)
			sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		scene = physics->createScene(sceneDesc);

		if (!scene)
			throw new Exception("PhysicsEngine::Init, Could not initialise the scene.");

		//default gravity
		scene->setGravity(PxVec3(0.0f, -9.81f, 0.0f));
	}

	void SimulationStep(double dt)
	{
		scene->simulate(PxReal(dt));
		scene->fetchResults(true);
	}
};

PxDefaultErrorCallback PhysicsEngine::gDefaultErrorCallback = PxDefaultErrorCallback();
PxDefaultAllocator PhysicsEngine::gDefaultAllocatorCallback = PxDefaultAllocator();
