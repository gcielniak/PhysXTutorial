#include <iostream>
#include "VisualDebugger.h"

using namespace std;

int main()
{
	try 
	{ 
		VisualDebugger::Init("PhysX Tutorial", 512, 512); 
	}
	catch (Exception exc) 
	{ 
		cerr << exc.what() << endl;
		return 0; 
	}

	VisualDebugger::Start();

	return 0;
}