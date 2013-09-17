#include <iostream>
#include <iomanip>
#include <windows.h> //delay function
#include "PxPhysicsAPI.h"

#include "MyPhysicsEngine.h"

using namespace std;
using namespace physx;

int main()
{
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
	}

	return 0;
}