#include <iostream> //cout, cerr
#include <iomanip> //stream formatting
#include <windows.h> //delay function, keyboard

#include "MyPhysicsEngine.h"

using namespace std;
using namespace PhysicsEngine;

int main()
{
	MyScene scene;

	//initialise PhysX and the scene	
	try 
	{ 
		PxInit();
		scene.Init();
	}
	catch (Exception exc) 
	{ 
		cerr << exc.what() << endl;
		return 0; 
	}

	//set the simulation step to 1/60th of a second
	PxReal delta_time = 1.f/60.f;

	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		scene.Update(delta_time);
		Sleep(100);
	}

	PxRelease();

	return 0;
}