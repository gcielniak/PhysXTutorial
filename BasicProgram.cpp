#include <iostream> //cout, cerr
#include <iomanip> //stream formatting
#include <windows.h> //delay function
#include "PxPhysicsAPI.h" //PhysX

using namespace std;
using namespace physx;

//PhysX objects
PxPhysics* physics;
PxFoundation* foundation;
debugger::comm::PvdConnection* vd_connection;

//simulation objects
PxScene* scene;
PxRigidDynamic* box;
PxRigidStatic* plane;

///Initialise PhysX objects
bool PxInit()
{
	//default error and allocator callbacks
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

	//connect to an external visual debugger (if exists)
	vd_connection = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 100, 
		PxVisualDebuggerExt::getAllConnectionFlags());

	//create a default scene
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

/// Release all allocated resources
void PxRelease()
{
	if (vd_connection)
		vd_connection->release();
	if (physics)
		physics->release();
	if (foundation)
		foundation->release();
}

///Initialise the scene
void InitScene()
{
	//default gravity
	scene->setGravity(PxVec3(0.0f, -9.81f, 0.0f));

	//materials
	PxMaterial* default_material = physics->createMaterial(0.0f, 0.0f, 0.0f);    //static friction, dynamic friction, restitution

	//create a static plane (XZ)
	plane = PxCreatePlane(*physics, PxPlane(PxVec3(0.0f, 1.0f, 0.0f), 0.0f), *default_material);
	scene->addActor(*plane);

	//create a dynamic actor and place it 10 m above the ground
	box = physics->createRigidDynamic(PxTransform(PxVec3(0.0f, 10.0f, 0.0f)));
	//create a box shape of 1m x 1m x 1m size (values are provided in halves)
	box->createShape(PxBoxGeometry(0.5f, 0.5f, 0.5f), *default_material);
	//update the box's mass
	PxRigidBodyExt::updateMassAndInertia(*box, 1.0f); //density of 1kg/m^3
	scene->addActor(*box);
}

/// Perform a single simulation step
void Update(double dt)
{
	scene->simulate(PxReal(dt));
	scene->fetchResults(true);
}

/// The main function
int main()
{
	//initialise PhysX	
	if (!PxInit())
	{
		cerr << "Could not initialise PhysX." << endl;
		return 0;
	}

	//initialise the scene
	InitScene();

	//set the simulation step to 1/60th of a second
	PxReal delta_time = 1.f/60.f;

	//simulate until the 'Esc' is pressed
	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		//perform a single simulation step
		Update(delta_time);
		
		//'visualise' position and velocity of the box
		PxVec3 position = box->getGlobalPose().p;
		PxVec3 velocity = box->getLinearVelocity();
		cout << setiosflags(ios::fixed) << setprecision(2) << "x=" << position.x << ", y=" << position.y << ", z=" << position.z << ",  ";
		cout << setiosflags(ios::fixed) << setprecision(2) << "vx=" << velocity.x << ", vy=" << velocity.y << ", vz=" << velocity.z << endl;
		
		//introduce 100ms delay for easier analysis of the results
		Sleep(100);
	}

	//Release all resources
	PxRelease();

	return 0;
}
