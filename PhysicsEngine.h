#pragma once

#include <vector>
#include "PxPhysicsAPI.h"
#include "Exception.h"

namespace PhysicsEngine
{
	using namespace physx;

	///Initialise PhysX framework
	void PxInit();
	
	///Release PhysX resources
	void PxRelease();

	///Get the PxPhysics object
	PxPhysics* GetPhysics();

	///Get the cooking object
	PxCooking* GetCooking();

	///Get default material
	PxMaterial* GetDefaultMaterial();

	///Create a material
	PxMaterial* CreateMaterial(PxReal sf=0.f, PxReal df=0.f, PxReal cr=0.f);

	///Abstract Actor class
	///Inherit from this class to create your own actors
	class Actor
	{
	protected:
		PxTransform pose;
		PxVec3 color;
		PxActor* actor;

		///a pure virtual function that creates the actor following your implementation
		virtual void Create()=0;

	public:
		///Constructor
		Actor(const PxTransform& pose, const PxVec3& color);
		
		///Pointer to PxActor
		PxActor* PxActor();

		PxRigidActor* PxRigidActor() { return (physx::PxRigidActor*)PxActor(); }

		///Get color
		const PxVec3& Color()
		{
			return color;
		}
		
		//Set color
		void Color(PxVec3 new_color)
		{
			color = new_color;
		}
	};

	///Generic scene class
	class Scene
	{
	protected:
		//a PhysX scene object
		PxScene* px_scene;
		//pause simulation
		bool pause;
		//selected dynamic actor on the scene
		PxRigidDynamic* selected_actor;
		//original and modified colour of the selected actor
		PxVec3 sactor_color, sactor_color_orig;

	public:
		///Init the scene
		void Init();

		///User defined initialisation
		virtual void CustomInit() {}

		///Perform a single simulation step
		void Update(PxReal dt);

		///User defined update step
		virtual void CustomUpdate() {}

		///Add actors
		void Add(Actor& actor);
		
		void Add(Actor* actor) { Add(*actor); };

		///Get the PxScene object
		PxScene* Get();

		///Reset the scene
		void Reset();

		///Set pause
		void Pause(bool value);

		///Get pause
		bool Pause();

		///Get the selected dynamic actor on the scene
		PxRigidDynamic* GetSelectedActor();

		///Switch to the next dynamic actor
		void SelectNextActor();

		///a list with all actors
		std::vector<PxRigidActor*> GetAllActors();

		///a list with all cloth actors
		std::vector<PxActor*> GetAllCloths();
	};
}

