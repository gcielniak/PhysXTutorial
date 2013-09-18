#include <iostream>
#include <iomanip>
#include <windows.h> //delay function
#include "VisualDebugger.h"
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

	VisualDebugger debugger;

	debugger.SetEngine(physics_engine);

	debugger.Init("PhysX Tutorial");

	debugger.Start();

	return 0;
}