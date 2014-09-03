#pragma once

#include "PhysicsEngine.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	class Cloth : public Actor
	{
		PxVec2 size;
		PxU32 width, height;
		bool fix_top;

	public:
		//constructor
		Cloth(PxTransform pose=PxTransform(PxIdentity), const PxVec2& _size=PxVec2(1.f,1.f), PxU32 _width=1, PxU32 _height=1, bool _fix_top = true, const PxVec3& _color=PxVec3(.9f,0.f,0.f))
			: Actor(pose, _color), size(_size), width(_width), height(_height), fix_top(_fix_top)
		{
		}

		virtual void Create()
		{
			//prepare vertices
			PxReal w_step = size.x/width;
			PxReal h_step = size.y/height;

			PxClothParticle* vertices = new PxClothParticle[(width+1)*(height+1)*4];
			PxU32* primitives = new PxU32[width*height*4];

			for (PxU32 j = 0; j < (height+1); j++)
			for (PxU32 i = 0; i < (width+1); i++)
			{
				PxU32 offset = i + j*(width+1);
				vertices[offset].pos = PxVec3(w_step*i,0.f,h_step*j);
				if (fix_top && (j == 0)) //fix the top row of vertices
					vertices[offset].invWeight = 0.f;
				else
					vertices[offset].invWeight = 1.f;
			}

			for (PxU32 j = 0; j < height; j++)
			for (PxU32 i = 0; i < width; i++)
			{
				PxU32 offset = (i + j*width)*4;
				primitives[offset + 0] = (i+0) + (j+0)*(width+1);
				primitives[offset + 1] = (i+1) + (j+0)*(width+1);
				primitives[offset + 2] = (i+1) + (j+1)*(width+1);
				primitives[offset + 3] = (i+0) + (j+1)*(width+1);
			}

			//init cloth mesh description
			PxClothMeshDesc meshDesc;
			meshDesc.points.data = vertices;
			meshDesc.points.count = (width+1)*(height+1);
			meshDesc.points.stride = sizeof(PxClothParticle);

			meshDesc.invMasses.data = &vertices->invWeight;
			meshDesc.invMasses.count = (width+1)*(height+1);
			meshDesc.invMasses.stride = sizeof(PxClothParticle);

			meshDesc.quads.data = primitives;
			meshDesc.quads.count = width*height;
			meshDesc.quads.stride = sizeof(PxU32) * 4;

			//create cloth fabric (cooking)
			PxClothFabric* fabric = PxClothFabricCreate(*GetPhysics(), meshDesc, PxVec3(0, -1, 0));

			//create cloth
			PxCloth* cloth = GetPhysics()->createCloth(pose, *fabric, vertices, PxClothFlags());
			//collisions with the scene objects
			cloth->setClothFlag(PxClothFlag::eSCENE_COLLISION, true);

			actor = cloth;
			actor->userData = &color; //pass a color parameter to the renderer
		}

		PxCloth* Get() 
		{
			return (PxCloth*)actor; 
		}
	};

	static const PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};

	///The Pyramid class demonstrating convex meshes
	class Pyramid : public Actor
	{
		PxReal density;

	public:
		//constructor
		Pyramid(PxTransform pose=PxTransform(PxIdentity), PxReal _density=1.f,
			const PxVec3& _color=PxVec3(.9f,0.f,0.f))
			: Actor(pose, _color), density(_density)
		{
		}

		//mesh cooking (preparation)
		PxConvexMesh* CookMesh()
		{
			PxConvexMeshDesc convexDesc;
			convexDesc.points.count     = sizeof(pyramid_verts)/sizeof(PxVec3);
			convexDesc.points.stride    = sizeof(PxVec3);
			convexDesc.points.data      = pyramid_verts;
			convexDesc.flags            = PxConvexFlag::eCOMPUTE_CONVEX;
			convexDesc.vertexLimit      = 256;

			PxDefaultMemoryOutputStream stream;

			if(!GetCooking()->cookConvexMesh(convexDesc, stream))
				throw new Exception("Pyramid::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createConvexMesh(input);
		}

		virtual void Create()
		{
			PxRigidDynamic* pyramid = GetPhysics()->createRigidDynamic(pose);
			pyramid->createShape(PxConvexMeshGeometry(CookMesh()), *GetDefaultMaterial());
			PxRigidBodyExt::setMassAndUpdateInertia(*pyramid, density);
			actor = pyramid;
			actor->userData = &color; //pass color parameter to renderer
		}

		PxRigidDynamic* Get() 
		{
			return (PxRigidDynamic*)actor; 
		}
	};

	class Capsule : public Actor
	{
		PxVec2 dimensions;
		PxReal density;
		PxShape* shape;
	public:
		Capsule(PxTransform pose=PxTransform(PxIdentity), PxVec2 _dimensions=PxVec2(1.f,1.f), PxReal _density=1.f,
			const PxVec3& _color=PxVec3(.9f,0.f,0.f)) 
			: Actor(pose, _color), dimensions(_dimensions), density(_density)
		{
		}

		virtual void Create()
		{
			PxRigidDynamic* capsule = GetPhysics()->createRigidDynamic(pose);
			shape = capsule->createShape(PxCapsuleGeometry(dimensions.x, dimensions.y), *GetDefaultMaterial());
			PxRigidBodyExt::setMassAndUpdateInertia(*capsule, density);
			actor = capsule;
			actor->userData = &color; //pass color parameter to renderer
		}
	};

	///Box class
	class Box : public Actor
	{
		PxVec3 dimensions;
		PxReal density;
		PxShape* shape;

	public:
		//a Box with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m x 1m x 1m
		// - denisty: 1kg/m^3
		Box(PxTransform pose=PxTransform(PxIdentity), PxVec3 _dimensions=PxVec3(.5f,.5f,.5f), PxReal _density=1.f,
			const PxVec3& _color=PxVec3(.9f,0.f,0.f)) 
			: Actor(pose, _color), dimensions(_dimensions), density(_density)
		{ 
		}

		virtual void Create()
		{
			PxRigidDynamic* box = GetPhysics()->createRigidDynamic(pose);
			shape = box->createShape(PxBoxGeometry(dimensions), *GetDefaultMaterial());
			PxRigidBodyExt::setMassAndUpdateInertia(*box, density);
			actor = box;
			actor->userData = &color; //pass color parameter to renderer
		}

		PxRigidDynamic* Get() 
		{
			return (PxRigidDynamic*)actor; 
		}

		//get a single shape
		PxShape* GetShape()
		{
			return shape;
		}
	};

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
			PxShape* shape1 = box->createShape(PxBoxGeometry(PxVec3(.5f,.5f,.5f)), *material);
			PxShape* shape2 = box->createShape(PxBoxGeometry(PxVec3(.5f,.5f,.5f)), *material);
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

	///Plane class
	class Plane : public Actor
	{
		PxVec3 normal;
		PxReal distance;

	public:
		//A plane with default paramters: XZ plane centred at (0,0,0)
		Plane(PxVec3 _normal=PxVec3(0.f, 1.f, 0.f), PxReal _distance=0.f,
			const PxVec3& color=PxVec3(.5f,.5f,.5f)) 
			: Actor(PxTransform(PxIdentity), color), normal(_normal), distance(_distance)
		{
		}

		virtual void Create()
		{
			PxRigidStatic* plane = PxCreatePlane(*GetPhysics(), PxPlane(normal, distance), *GetDefaultMaterial());
			actor = plane;
			actor->userData = &color; //pass color parameter to renderer
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

	///Custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		Box2* box;
		Capsule* capsule;
		Cloth* cloth;
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

//			cloth = new Cloth(PxTransform(PxVec3(-4.f,9.f,0.f)), PxVec2(8.f,8.f), 40, 40);
//			Add(*cloth);

			box = new Box2(PxTransform(PxVec3(.0f,10.f,.0f)),1.f,PxVec3(.9,.0f,.0f));
//			Add(box);

			capsule = new Capsule(PxTransform(PxVec3(.0f,10.f,.0f)));
			Add(capsule);

			//setting custom cloth parameters
			//cloth->Get()->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(1.f));
		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{
			
			PxTransform t = capsule->PxRigidActor()->getGlobalPose();
			t.q *= PxQuat(.01f,PxVec3(1.0f,1.f,1.0f));
			capsule->PxRigidActor()->setGlobalPose(t);

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
			capsule->Color(PxVec3(.9f,.0f,.0f));
		}

		/// An example use of key presse handling
		void ExampleKeyPressHandler()
		{
			cerr << "I am pressed!" << endl;
			capsule->Color(PxVec3(.9f,.9f,.0f));
		}
	};
}
