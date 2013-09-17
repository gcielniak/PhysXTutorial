#include <iostream>
#include <iomanip>
#include <windows.h> //delay function
#include "PxPhysicsAPI.h"

using namespace std;
using namespace physx;

PxPhysics* physics;
PxFoundation* foundation;
PVD::PvdConnection* vd_connection;
PxScene* scene;
PxRigidDynamic* sphere;
PxRigidStatic* plane;

bool InitPhysX()
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	//Init PhysX
	//foundation
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

	if(!foundation)
		return false;

	//physics
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());

	if(!physics)
		return false;

	//visual debugger
	vd_connection = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 100, 
		PxVisualDebuggerExt::getAllConnectionFlags());

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
		return false;

	return true;
}

void ReleasePhysX()
{
	if (vd_connection)
		vd_connection->release();
	if (physics)
		physics->release();
	if (foundation)
		foundation->release();
}

void InitScene()
{
	//default gravity
	scene->setGravity(PxVec3(0.0f, -9.81f, 0.0f));

	//materials
	PxMaterial* default_material = physics->createMaterial(0.5f, 0.5f, 0.1f);    //static friction, dynamic friction, restitution

	//sphere
	sphere = physics->createRigidDynamic(PxTransform(PxVec3(0,10,0)));
	PxShape* mSphereShape = sphere->createShape(PxSphereGeometry(1.0), *default_material);
	PxRigidBodyExt::updateMassAndInertia(*sphere, 10.);
	scene->addActor(*sphere);

	//plane
	plane = PxCreatePlane(*physics, PxPlane(PxVec3(0,1,0), 0), *default_material);
	scene->addActor(*plane);
}

void SimulationStep(double dt)
{
	scene->simulate(PxReal(dt));
	scene->fetchResults(true);
}

int main()
{
	
	if (!InitPhysX())
	{
		cerr << "Could not initialise PhysX." << endl;
		exit(1);
	}

	InitScene();

	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		SimulationStep(1./60.);
		PxVec3 position = sphere->getGlobalPose().p;
		cerr << setiosflags(ios::fixed) << setprecision(2) << "x=" << position.x << ", y=" << position.y << ", z=" << position.z << endl;
		Sleep(100);
	}

	ReleasePhysX();

	return 0;
}