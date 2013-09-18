#pragma once

#include <iostream>
#include "Exception.h"
#include "PxPhysicsAPI.h"

using namespace physx;
using namespace std;

namespace PhysicsEngine2
{
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;

	PxFoundation* foundation = 0;
	PVD::PvdConnection* vd_connection = 0;
	PxPhysics* physics = 0;
	PxMaterial* default_material = 0;

	void InitPhysX()
	{
		//foundation
		if (!foundation)
			foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

		if(!foundation)
			throw new Exception("PhysicsEngine2::Init, Could not create the PhysX SDK foundation.");

		//physics
		if (!physics)
			physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());

		if(!physics)
			throw new Exception("PhysicsEngine2::Init, Could not initialise the PhysX SDK.");

		//visual debugger
		if (!vd_connection)
			vd_connection = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), 
			"localhost", 5425, 100, PxVisualDebuggerExt::getAllConnectionFlags());

		if (!default_material)
			default_material = physics->createMaterial(0.0f, 0.0f, 0.5f);
	}

	void ReleasePhysX()
	{
		if (default_material)
			default_material->release();
		if (vd_connection)
			vd_connection->release();
		if (physics)
			physics->release();
		if (foundation)
			foundation->release();
	}

	class Material
	{
	protected:
		std::string name;
		PxMaterial* material;

	public:
		Material(PxReal staticFriction=0.0f, PxReal dynamicFriction=0.0f, PxReal restitution=0.0f)
		{
			material = physics->createMaterial(staticFriction, dynamicFriction, restitution);
		}
	};

	class Actor
	{
	protected:
		PxActor* actor;
		std::string name;
		PxVec3 color;
		friend class Scene;
	};

	class Scene
	{
	protected:
		PxScene* scene;

	public:
		Scene()
		{
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
				throw new Exception("PhysicsEngine::Scene::Init, Could not initialise the scene.");

			//default gravity
			scene->setGravity(PxVec3(0.0f, -9.81f, 0.0f));
		}

		~Scene() { scene->release(); }

		void SimulationStep(PxReal dt)
		{
			scene->simulate(dt);
			scene->fetchResults(true);
		}

		void Add(Actor& actor)
		{
			scene->addActor(*actor.actor);
		}
	};

	class Sphere : public Actor
	{
	protected:
		PxRigidDynamic* sphere;

	public:
		Sphere() { Sphere(PxVec3(0.0f, 0.0f, 0.0f)); }

		Sphere(PxVec3 position) 
		{
			sphere = physics->createRigidDynamic(PxTransform(position));
			sphere->createShape(PxSphereGeometry(1.0f), *default_material);
			PxRigidBodyExt::setMassAndUpdateInertia(*sphere, 1.0f);
			actor = sphere;
		}

		PxRigidDynamic* Get() { return sphere; }
	};

	class Plane : public Actor
	{
	protected:
		PxRigidStatic* plane;

	public:
		Plane(PxVec3 normal, PxReal distance=0.0f)
		{
			plane = PxCreatePlane(*physics, PxPlane(normal, distance), *default_material);
			actor = plane;
		}
	};

	class PlaneXZ : public Plane
	{
	public:
		PlaneXZ(PxReal distance=0.0f) : Plane(PxVec3(0.0, 1.0, 0.0), distance) { }
	};
}

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

	virtual void InitScene() {}

	virtual void UpdateScene() {}
};

PxDefaultErrorCallback PhysicsEngine::gDefaultErrorCallback = PxDefaultErrorCallback();
PxDefaultAllocator PhysicsEngine::gDefaultAllocatorCallback = PxDefaultAllocator();
