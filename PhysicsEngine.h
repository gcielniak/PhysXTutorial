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

	static const PxVec3 default_color(.8f,.8f,.8f);

	///Abstract Actor class
	///Inherit from this class to create your own actors
	class Actor
	{
	protected:
		PxTransform pose;
		PxActor* actor;
		std::vector<PxVec3> colors;

	public:
		///Constructor
		Actor(const PxTransform& _pose)
			: pose(_pose), actor(0)
		{
		}

		PxActor* Actor::Get()
		{
			return actor;
		}

		void Color(PxVec3 new_color, PxU32 shape_indx=0)
		{
			PxShape* shape = GetShape(shape_indx);
			if (shape)
			{
				colors[shape_indx] = new_color;
				shape->userData = &colors[shape_indx];
			}
		}

		const PxVec3* Color(PxU32 shape_indx=0)
		{
			if (shape_indx < colors.size())
				return &colors[shape_indx];
			else 
				return 0;			
		}

		PxShape* GetShape(PxU32 shape_indx=0)
		{
			if (actor->isRigidActor())
			{
				std::vector<PxShape*> shapes(((PxRigidActor*)actor)->getNbShapes());
				if (shape_indx < ((PxRigidActor*)actor)->getShapes((PxShape**)&shapes.front(),shapes.size()))
					return shapes[shape_indx];
			}

			return 0;
		}

		virtual void AddShape(const PxGeometry& geometry, PxReal density) {}
	};

	class DynamicActor : public Actor
	{
	public:
		DynamicActor(const PxTransform& pose) :
			Actor(pose)
		{
		}

		void AddShape(const PxGeometry& geometry, PxReal density)
		{
			if (!actor)
				actor = (PxActor*)GetPhysics()->createRigidDynamic(pose);
			PxShape* shape = ((PxRigidDynamic*)actor)->createShape(geometry,*GetDefaultMaterial());
			PxRigidBodyExt::setMassAndUpdateInertia(*(PxRigidDynamic*)actor, density);
			colors.push_back(default_color);
			shape->userData = &colors.back();
		}
	};

	class StaticActor : public Actor
	{
	public:
		StaticActor(const PxTransform& pose) :
			Actor(pose)
		{
		}

		void AddShape(const PxGeometry& geometry, PxReal density=0.f)
		{
			if (!actor)
				actor = (PxActor*)GetPhysics()->createRigidStatic(pose);
			PxShape* shape = ((PxRigidStatic*)actor)->createShape(geometry,*GetDefaultMaterial());
			colors.push_back(default_color);
			shape->userData = &colors.back();
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

		void HighlightOn(PxRigidDynamic* actor);

		void HighlightOff(PxRigidDynamic* actor);

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
		std::vector<PxActor*> GetAllActors();
	};
}

