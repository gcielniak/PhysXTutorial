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
	debugger::comm::PvdConnection* vd_connection = 0;
	PxPhysics* physics = 0;
	PxCooking* cooking = 0;

	///PhysX functions
	void PxInit()
	{
		//foundation
		if (!foundation)
			foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

		if(!foundation)
			throw new Exception("PhysicsEngine::PxInit, Could not create the PhysX SDK foundation.");

		//physics
		if (!physics)
			physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());

		if(!physics)
			throw new Exception("PhysicsEngine::PxInit, Could not initialise the PhysX SDK.");

		if (!cooking)
			cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(PxTolerancesScale()));

		if(!cooking)
			throw new Exception("PhysicsEngine::PxInit, Could not initialise the cooking component.");

		//visual debugger
		if (!vd_connection)
			vd_connection = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), 
			"localhost", 5425, 100, PxVisualDebuggerExt::getAllConnectionFlags());

		//create a deafult material
		CreateMaterial();
	}

	void PxRelease()
	{
		if (vd_connection)
			vd_connection->release();
		if (cooking)
			cooking->release();
		if (physics)
			physics->release();
		if (foundation)
			foundation->release();
	}

	PxPhysics* GetPhysics() 
	{ 
		return physics; 
	}

	PxCooking* GetCooking()
	{
		return cooking;
	}

	PxMaterial* GetMaterial(PxU32 index)
	{
		std::vector<PxMaterial*> materials(physics->getNbMaterials());
		if (index < physics->getMaterials((PxMaterial**)&materials.front(),materials.size()))
			return materials[index];
		else
			return 0;
	}

	PxMaterial* CreateMaterial(PxReal sf, PxReal df, PxReal cr) 
	{
		return physics->createMaterial(sf, df, cr);
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

		pause = false;

		selected_actor = 0;

		SelectNextActor();
	}

	void Scene::Update(PxReal dt)
	{
		if (pause)
			return;

		CustomUpdate();

		px_scene->simulate(dt);
		px_scene->fetchResults(true);
	}

	void Scene::Add(Actor& actor)
	{
		px_scene->addActor(*actor.Get());
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

	void Scene::Pause(bool value)
	{
		pause = value;
	}

	bool Scene::Pause() 
	{ 
		return pause;
	}

	PxRigidDynamic* Scene::GetSelectedActor()
	{
		return selected_actor;
	}

	void Scene::SelectNextActor()
	{
		std::vector<PxRigidDynamic*> actors(px_scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC));
		if (px_scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC, (PxActor**)&actors.front(), actors.size()))
		{
			if (selected_actor)
			{
				for (unsigned int i = 0; i < actors.size(); i++)
					if (selected_actor == actors[i])
					{
						HighlightOff(selected_actor);
						//select the next actor
						selected_actor = actors[(i+1)%actors.size()];
						break;
					}
			}
			else
			{
				selected_actor = actors[0];
			}
			HighlightOn(selected_actor);
		}
		else
			selected_actor = 0;
	}

	std::vector<PxActor*> Scene::GetAllActors()
	{
		physx::PxActorTypeSelectionFlags selection_flag = PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC | 
			PxActorTypeSelectionFlag::eCLOTH;
		std::vector<PxActor*> actors(px_scene->getNbActors(selection_flag));
		px_scene->getActors(selection_flag, (PxActor**)&actors.front(), actors.size());
		return actors;
	}

	void Scene::HighlightOn(PxRigidDynamic* actor)
	{
		//store the original colour and adjust brightness of the selected actor
		std::vector<PxShape*> shapes(actor->getNbShapes());
		actor->getShapes((PxShape**)&shapes.front(),shapes.size());

		sactor_color_orig.clear();

		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			PxVec3* color = ((UserData*)shapes[i]->userData)->color;
			sactor_color_orig.push_back(*color);
			*color += PxVec3(.2f,.2f,.2f);
		}
	}

	void Scene::HighlightOff(PxRigidDynamic* actor)
	{
		//restore the original color
		std::vector<PxShape*> shapes(actor->getNbShapes());
		actor->getShapes((PxShape**)&shapes.front(),shapes.size());

		for (unsigned int i = 0; i < shapes.size(); i++)
			*((UserData*)shapes[i]->userData)->color = sactor_color_orig[i];
	}
}
