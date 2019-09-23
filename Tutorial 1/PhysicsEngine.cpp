#include "PhysicsEngine.h"

namespace PhysicsEngine
{
	using namespace physx;
	using namespace std;

	//default error and allocator callbacks
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;

	//PhysX objects
	PxFoundation* foundation = 0;
	PxPhysics* physics = 0;
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
	debugger::comm::PvdConnection* pvd = 0;
#else
	PxPvd*  pvd = 0;
#endif

	///PhysX functions
	void PxInit()
	{
		//foundation
		if (!foundation) {
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
			foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
#else
			foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
#endif
		}

		if(!foundation)
			throw new Exception("PhysicsEngine::PxInit, Could not create the PhysX SDK foundation.");

		//visual debugger
		if (!pvd) {
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
			pvd = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 100,
				PxVisualDebuggerExt::getAllConnectionFlags());
#else
			pvd = PxCreatePvd(*foundation);
			PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
			pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif
		}

		//physics
		if (!physics)
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
			physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());
#else
			physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
#endif

		if(!physics)
			throw new Exception("PhysicsEngine::PxInit, Could not initialise the PhysX SDK.");

		//create a deafult material
		CreateMaterial();
	}

	void PxRelease()
	{
		if (physics)
			physics->release();
		if (pvd)
			pvd->release();
		if (foundation)
			foundation->release();
	}

	PxPhysics* GetPhysics() 
	{ 
		return physics; 
	}

	PxMaterial* GetMaterial(PxU32 index)
	{
		std::vector<PxMaterial*> materials(physics->getNbMaterials());
		if (index < physics->getMaterials((PxMaterial**)&materials.front(), (PxU32)materials.size()))
			return materials[index];
		else
			return 0;
	}

	PxMaterial* CreateMaterial(PxReal sf, PxReal df, PxReal cr) 
	{
		return physics->createMaterial(sf, df, cr);
	}

	///Actor methods

	PxActor* Actor::Get()
	{
		return actor;
	}

	PxShape* Actor::GetShape(PxU32 index)
	{
		std::vector<PxShape*> shapes(((PxRigidActor*)actor)->getNbShapes());
		if (index < ((PxRigidActor*)actor)->getShapes((PxShape**)&shapes.front(), (PxU32)shapes.size()))
			return shapes[index];
		else
			return 0;
	}

	DynamicActor::DynamicActor(const PxTransform& pose) : Actor()
	{
		actor = (PxActor*)GetPhysics()->createRigidDynamic(pose);
	}

	void DynamicActor::CreateShape(const PxGeometry& geometry, PxReal density)
	{
		PxShape* shape = ((PxRigidDynamic*)actor)->createShape(geometry,*GetMaterial());
		PxRigidBodyExt::updateMassAndInertia(*(PxRigidDynamic*)actor, density);
	}

	StaticActor::StaticActor(const PxTransform& pose)
	{
		actor = (PxActor*)GetPhysics()->createRigidStatic(pose);
	}

	void StaticActor::CreateShape(const PxGeometry& geometry, PxReal density)
	{
		PxShape* shape = ((PxRigidStatic*)actor)->createShape(geometry,*GetMaterial());
	}

	///Scene methods
	void Scene::Init()
	{
		//scene
		PxSceneDesc sceneDesc(GetPhysics()->getTolerancesScale());

		if(!sceneDesc.cpuDispatcher)
		{
			PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
			sceneDesc.cpuDispatcher = mCpuDispatcher;
		}

		if(!sceneDesc.filterShader)
			sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		px_scene = GetPhysics()->createScene(sceneDesc);

		if (!px_scene)
			throw new Exception("PhysicsEngine::Scene::Init, Could not initialise the scene.");

		//default gravity
		px_scene->setGravity(PxVec3(0.0f, -9.81f, 0.0f));

		CustomInit();
	}

	void Scene::Update(PxReal dt)
	{
		CustomUpdate();

		px_scene->simulate(dt);
		px_scene->fetchResults(true);
	}

	void Scene::Add(Actor* actor)
	{
		px_scene->addActor(*actor->Get());
	}

	PxScene* Scene::Get() 
	{ 
		return px_scene; 
	}

	void Scene::Reset()
	{
		px_scene->release();
		Init();
	}
}
