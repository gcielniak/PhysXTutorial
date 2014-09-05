#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	class Box2 : public Actor
	{
		PxReal density;
		const PxMaterial* material;

	public:
		Box2(PxTransform pose=PxTransform(PxIdentity), PxReal _density=1.f,	const PxVec3& _color=PxVec3(.9f,0.f,0.f), const PxMaterial* _material=GetDefaultMaterial())
			: Actor(pose, _color), density(_density), material(_material)
		{
		}

		virtual void Create()
		{
			PxRigidDynamic* box = GetPhysics()->createRigidDynamic(pose);
			PxShape* shape1 = box->createShape(PxBoxGeometry(PxVec3(1.5f,1.5f,1.5f)), *material);
			PxShape* shape2 = box->createShape(PxBoxGeometry(PxVec3(1.5f,1.5f,1.5f)), *material);
			shape2->setLocalPose(PxTransform(PxVec3(2.f,.0f,.0f)));
			PxRigidBodyExt::setMassAndUpdateInertia(*box, density);
			actor = box;
			actor->userData = &color; //pass color parameter to renderer
		}

		PxRigidDynamic* Get() 
		{
			return (PxRigidDynamic*)actor; 
		}
	};

	//Distance joint with the springs switched on
	class DistanceJoint
	{
	public:
		DistanceJoint(PxRigidActor* actor0, PxTransform& localFrame0, PxRigidActor* actor1, PxTransform& localFrame1, PxReal stiffness=1.f, PxReal damping=1.f)
		{
			PxDistanceJoint* joint = PxDistanceJointCreate(*GetPhysics(), actor0, localFrame0, actor1, localFrame1);
			joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION,true);
			joint->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);
			joint->setStiffness(stiffness);
			joint->setDamping(damping);
		}
	};

	///Revolute Joint
	class RevoluteJoint
	{
		PxRevoluteJoint* joint;
	public:
		RevoluteJoint(PxRigidActor* actor0, PxTransform& localFrame0, PxRigidActor* actor1, PxTransform& localFrame1)
		{
			joint = PxRevoluteJointCreate(*GetPhysics(), actor0, localFrame0, actor1, localFrame1);
			joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION,true);
		}

		void DriveVelocity(PxReal value)
		{
			joint->setDriveVelocity(value);
			joint->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		}

		PxReal DriveVelocity()
		{
			return joint->getDriveVelocity();
		}

		void SetLimits(PxReal lower, PxReal upper)
		{
			joint->setLimit(PxJointAngularLimitPair(lower, upper));
			joint->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		}
	};

	///An example class showing the use of springs (distance joints).
	class Trampoline
	{
	public:
		Trampoline(Scene* scene, const PxVec3& dimensions=PxVec3(1.f,1.f,1.f), PxReal stiffness=1.f, PxReal damping=1.f)
		{
			PxReal thickness = .1f;
			Box *bottom = new Box(PxTransform(PxVec3(0.f,thickness,0.f)),PxVec3(dimensions.x,thickness,dimensions.z));
			Box *top = new Box(PxTransform(PxVec3(0.f,dimensions.y+thickness,0.f)),PxVec3(dimensions.x,thickness,dimensions.z));
			scene->Add(*bottom);
			scene->Add(*top);
			DistanceJoint spring1(bottom->Get(),PxTransform(PxVec3(dimensions.x,thickness,dimensions.z)),top->Get(),PxTransform(PxVec3(dimensions.x,-dimensions.y,dimensions.z)),stiffness,damping);
			DistanceJoint spring2(bottom->Get(),PxTransform(PxVec3(dimensions.x,thickness,-dimensions.z)),top->Get(),PxTransform(PxVec3(dimensions.x,-dimensions.y,-dimensions.z)),stiffness,damping);
			DistanceJoint spring3(bottom->Get(),PxTransform(PxVec3(-dimensions.x,thickness,dimensions.z)),top->Get(),PxTransform(PxVec3(-dimensions.x,-dimensions.y,dimensions.z)),stiffness,damping);
			DistanceJoint spring4(bottom->Get(),PxTransform(PxVec3(-dimensions.x,thickness,-dimensions.z)),top->Get(),PxTransform(PxVec3(-dimensions.x,-dimensions.y,-dimensions.z)),stiffness,damping);
		}
	};

	///A customised collision class, implemneting various callbacks
	class MySimulationEventCallback : public PxSimulationEventCallback
	{
	public:
		//an example variable that will be checked in the main simulation loop
		bool trigger;

		MySimulationEventCallback() : trigger(false) {}

		///Method called when the contact with the trigger object is detected.
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) 
		{
			//you can read the trigger information here
			for (PxU32 i = 0; i < count; i++)
			{
				//filter out contact with the planes
				if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
				{
					//check if eNOTIFY_TOUCH_FOUND trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						cerr << "eNOTIFY_TOUCH_FOUND" << endl;
						trigger = true;
					}
					//check if eNOTIFY_TOUCH_LOST trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						cerr << "eNOTIFY_TOUCH_LOST" << endl;
						trigger = false;
					}
				}
			}
		}

		///Other types of events
		virtual void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) {}
		virtual void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) {}
		virtual void onWake(PxActor **actors, PxU32 count) {}
		virtual void onSleep(PxActor **actors, PxU32 count) {}
	};

	static const PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
	static const PxU32 pyramid_trigs[] = {0, 1, 4, 0, 1, 3, 0, 3, 2, 0, 2, 4, 1, 2, 3, 1, 2, 4};

	///Custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		Box2* box;
		Capsule* capsule;
		Cloth* cloth;
		ConvexMesh* pyramid;
		TriangleMesh* pyramid_2;
		MySimulationEventCallback* my_callback;

	public:
		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
			//cloth visualisation
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_BENDING, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_HORIZONTAL, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_SHEARING, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_VERTICAL, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_VIRTUAL_PARTICLES, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetDefaultMaterial()->setDynamicFriction(.2f);

			///Initialise and set the customised event callback
			my_callback = new MySimulationEventCallback();
			px_scene->setSimulationEventCallback(my_callback);

			plane = new Plane();
			Add(plane);

			cloth = new Cloth(PxTransform(PxVec3(-4.f,9.f,0.f)), PxVec2(8.f,8.f), 40, 40);
			Add(*cloth);

			box = new Box2(PxTransform(PxVec3(.0f,5.f,.0f)),1.f,PxVec3(.9,.0f,.0f));
			Add(box);

			pyramid = new ConvexMesh(pyramid_verts, sizeof(pyramid_verts), PxTransform(PxVec3(-5.0f,5.f,.0f)));
			Add(pyramid);

			pyramid_2 = new TriangleMesh(pyramid_verts, sizeof(pyramid_verts), pyramid_trigs, sizeof(pyramid_trigs), PxTransform(PxVec3(-3.0f,1.f,3.0f)));
			Add(pyramid_2);

//			capsule = new Capsule(PxTransform(PxVec3(.0f,10.f,.0f)));
//			Add(capsule);

			//setting custom cloth parameters
			//cloth->Get()->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(1.f));
		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{
			///an example showing how to interface the collision callbacks with the simulation
			if (my_callback->trigger)
			{
				cerr << "Trigger on" << endl;
			}
		}

		/// An example use of key release handling
		void ExampleKeyReleaseHandler()
		{
			cerr << "I am realeased!" << endl;
			PxTransform t = box->PxRigidActor()->getGlobalPose();
			t.q *= PxQuat(.01f,PxVec3(1.0f,1.f,1.0f));
			box->PxRigidActor()->setGlobalPose(t);
		}

		/// An example use of key presse handling
		void ExampleKeyPressHandler()
		{
			cerr << "I am pressed!" << endl;
		}
	};
}
