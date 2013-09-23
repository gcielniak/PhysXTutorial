#pragma once

#include "PxPhysicsAPI.h"
#include "Exception.h"
#include "UserData.h"

namespace PhysicsEngine
{
	using namespace physx;

	///Initialise PhysX framework
	void PxInit();
	
	///Release PhysX resources
	void PxRelease();

	///Get the PxPhysics object
	PxPhysics* GetPhysics();

	///Get default material
	PxMaterial* GetDefaultMaterial();

	///Create a material
	PxMaterial* CreateMaterial(PxReal sf, PxReal df, PxReal cr);

	///Abstract actor class
	///Inherit from this class to create your own actors
	class Actor
	{
	protected:
		//data storing additional info about the actor
		UserData user_data;
		//initial pose
		PxTransform initial_pose;

	public:
		///a constructor
		Actor(PxTransform pose, PxVec3 color, std::string name) ;
		
		///pure virtual function that creates the actor
		virtual PxActor* Create()=0;
	};

	///Generic scene class
	class Scene
	{
	protected:
		//PhysX scene objects
		PxScene* px_scene;
		//pause simulation
		bool pause; 

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

		///Get the PxScene object
		PxScene* Get();

		///Reset the scene
		void Reset();

		///Set pause
		void Pause(bool value);

		///Get pause state
		bool Pause();
	};
}
