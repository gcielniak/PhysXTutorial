#pragma once

#include <vector>
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

	///Get the cooking object
	PxCooking* GetCooking();

	///Get the specified material
	PxMaterial* GetMaterial(PxU32 index=0);

	///Create a new material
	PxMaterial* CreateMaterial(PxReal sf=.0f, PxReal df=.0f, PxReal cr=.0f);

	static const PxVec3 default_color(.8f,.8f,.8f);

	///Abstract Actor class
	///Inherit from this class to create your own actors
	class Actor
	{
	protected:
		PxActor* actor;
		std::vector<PxVec3> colors;

	public:
		///Constructor
		Actor()
			: actor(0)
		{
		}

		PxActor* Actor::Get()
		{
			return actor;
		}

		void Color(PxVec3 new_color, PxU32 shape_index=-1)
		{
			//change color of all shapes
			if (shape_index == -1)
			{
				for (unsigned int i = 0; i < colors.size(); i++)
					colors[i] = new_color;
			}
			//or only the selected one
			else if (shape_index < colors.size())
			{
				colors[shape_index] = new_color;
			}
		}

		const PxVec3* Color(PxU32 shape_indx=0)
		{
			if (shape_indx < colors.size())
				return &colors[shape_indx];
			else 
				return 0;			
		}

		void Material(PxMaterial* new_material, PxU32 shape_index=-1)
		{
			PxShape* shape;
			PxU32 shape_nr = ((PxRigidActor*)actor)->getNbShapes();

			//change material for all shapes
			if (shape_index == -1)
			{
				for (PxU32 i = 0; i < shape_nr; i++)
				{
					shape = GetShape(i);
					//change all materials belonging to the same shape
					//this is probably not correct
					std::vector<PxMaterial*> materials(shape->getNbMaterials());
					shape->getMaterials(&materials.front(),materials.size());
					for (unsigned int j = 0; j < materials.size(); j++)
						materials[j] = new_material;
				}
			}
			//or only for the selected one
			else if (shape_index < shape_nr)
			{
				shape = GetShape(shape_index);
				//change all materials belonging to the same shape
				//this is probably not correct
				std::vector<PxMaterial*> materials(shape->getNbMaterials());
				shape->getMaterials(&materials.front(),materials.size());
				for (unsigned int j = 0; j < materials.size(); j++)
					materials[j] = new_material;
			}
		}

		PxShape* GetShape(PxU32 index=0)
		{
			std::vector<PxShape*> shapes(((PxRigidActor*)actor)->getNbShapes());
			if (index < ((PxRigidActor*)actor)->getShapes((PxShape**)&shapes.front(),shapes.size()))
				return shapes[index];
			else
				return 0;
		}

		virtual void CreateShape(const PxGeometry& geometry, PxReal density) {}
	};

	class DynamicActor : public Actor
	{
	public:
		DynamicActor(const PxTransform& pose) : Actor()
		{
			actor = (PxActor*)GetPhysics()->createRigidDynamic(pose);
		}

		~DynamicActor()
		{
			for (unsigned int i = 0; i < colors.size(); i++)
				delete (UserData*)GetShape(i)->userData;
		}

		void CreateShape(const PxGeometry& geometry, PxReal density)
		{
			PxShape* shape = ((PxRigidDynamic*)actor)->createShape(geometry,*GetMaterial());
			PxRigidBodyExt::setMassAndUpdateInertia(*(PxRigidDynamic*)actor, density);
			colors.push_back(default_color);
			//pass the color pointers to the renderer
			shape->userData = new UserData();
			for (unsigned int i = 0; i < colors.size(); i++)
				((UserData*)GetShape(i)->userData)->color = &colors[i];
		}
	};

	class StaticActor : public Actor
	{
	public:
		StaticActor(const PxTransform& pose)
		{
			actor = (PxActor*)GetPhysics()->createRigidStatic(pose);
		}

		~StaticActor()
		{
			for (unsigned int i = 0; i < colors.size(); i++)
				delete (UserData*)GetShape(i)->userData;
		}

		void CreateShape(const PxGeometry& geometry, PxReal density=0.f)
		{
			PxShape* shape = ((PxRigidStatic*)actor)->createShape(geometry,*GetMaterial());
			colors.push_back(default_color);
			//pass the color pointers to the renderer
			shape->userData = new UserData();
			for (unsigned int i = 0; i < colors.size(); i++)
				((UserData*)GetShape(i)->userData)->color = &colors[i];
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
		std::vector<PxVec3> sactor_color_orig;

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

