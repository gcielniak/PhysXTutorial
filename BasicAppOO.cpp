#include <iostream>
#include <iomanip>
#include <windows.h> //delay function
#include "PxPhysicsAPI.h"

#include "MyPhysicsEngine.h"

using namespace std;
using namespace physx;

int main()
{
	try {
		PhysicsEngine2::InitPhysX();
		PhysicsEngine2::Scene scene;
		PhysicsEngine2::Sphere sphere(PxVec3(0.0f, 10.0f, 0.0f));
		PhysicsEngine2::PlaneXZ plane;

		scene.Add(sphere);
		scene.Add(plane);

		while (!GetAsyncKeyState(VK_ESCAPE))
		{
			scene.SimulationStep(PxReal(1./60.));
			PxVec3 position = sphere.Get()->getGlobalPose().p;
			cerr << setiosflags(ios::fixed) << setprecision(2) << "x=" << position.x << ", y=" << position.y << ", z=" << position.z << endl;
			Sleep(100);
		}
	}
	catch (Exception exc) { cerr << exc.what() << endl; return 1; }

	PhysicsEngine2::ReleasePhysX();

	/*
	MyPhysicsEngine physics_engine;

	try { physics_engine.Init(); }
	catch (Exception exc) { cerr << exc.what() << endl; exit(1); }

	//Init scene
	PxRigidDynamic* sphere = physics_engine.AddSphere(PxVec3(0.0f, 10.0f, 0.0f));
	PxRigidStatic* plane = physics_engine.AddStaticPlaneXZ();

	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		physics_engine.SimulationStep(1./60.);
		PxVec3 position = sphere->getGlobalPose().p;
		cerr << setiosflags(ios::fixed) << setprecision(2) << "x=" << position.x << ", y=" << position.y << ", z=" << position.z << endl;
		Sleep(100);
	}*/

	return 0;
}