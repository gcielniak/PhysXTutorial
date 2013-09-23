#pragma once

#include "PhysicsEngine.h"
#include <iostream>

namespace PhysicsEngine
{
	using namespace std;

	PxVec3 default_color = PxVec3(0.9f,0.1f,0.1f);

	///Box class
	class Box : public Actor
	{
		PxVec3 dimensions;
		PxReal density;
		PxRigidDynamic* actor;

	public:
		Box(PxTransform pose=PxTransform(PxIdentity), PxVec3 _dimensions=PxVec3(.5f,.5f,.5f), PxReal _density=1.f,
			PxVec3 color=default_color, std::string name="") 
			: Actor(pose,color,name), dimensions(_dimensions), density(_density)
		{ 
		}

		virtual PxActor* Create()
		{
			actor = GetPhysics()->createRigidDynamic(initial_pose);
			actor->createShape(PxBoxGeometry(dimensions.x, dimensions.y, dimensions.z), *GetDefaultMaterial());
			PxRigidBodyExt::setMassAndUpdateInertia(*actor, density);
			actor->userData = &user_data; //used for visualisation
			return actor;
		}

		PxRigidDynamic* Get() 
		{
			return actor; 
		}
	};

	///Plane class
	class Plane : public Actor
	{
		PxVec3 normal;
		PxReal distance;
		PxRigidStatic* actor;

	public:
		Plane(PxVec3 _normal=PxVec3(0.f, 1.f, 0.f), PxReal _distance=0.f, 
			PxVec3 color=PxVec3(0.2f,0.3f,0.4f), std::string name="") 
			: Actor(PxTransform(PxIdentity),color,name), normal(_normal), distance(_distance)
		{
		}

		virtual PxActor* Create()
		{
			actor = PxCreatePlane(*GetPhysics(), PxPlane(normal, distance), *GetDefaultMaterial());
			actor->userData = &user_data; //used for visualisation
			return actor;
		}
	};

	///Custom scene class
	class MyScene : public Scene
	{
		Plane plane;
		Box* box;

	public:
		virtual void CustomInit() 
		{
			Add(plane);

			box = new Box(PxTransform(PxVec3(0.f,10.f,0.f)));
			Add(*box);
		}

		virtual void CustomUpdate() 
		{
			//'visualise' position and velocity of the box
			PxVec3 position = box->Get()->getGlobalPose().p;
			PxVec3 velocity = box->Get()->getLinearVelocity();
//			cout << setiosflags(ios::fixed) << setprecision(2) << "x=" << position.x << ", y=" << position.y << ", z=" << position.z << ",  ";
//			cout << setiosflags(ios::fixed) << setprecision(2) << "vx=" << velocity.x << ", vy=" << velocity.y << ", vz=" << velocity.z << endl;
		}
	};
}
