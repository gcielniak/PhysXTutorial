#pragma once

#include <vector>
#include "PhysicsEngine.h"
#include "Renderer.h"
#include "Camera.h"

namespace VisualDebugger
{
	using namespace physx;

	Camera* camera = new Camera(PxVec3(0.0f, 3.0f, 15.0f), PxVec3(0.f,-5.f,-20.f));
	PhysicsEngine::Scene* scene;
	PxReal delta_time = 1.f/60.f;

	void renderCallback()
	{
		scene->Update(delta_time);

		Renderer::Start(camera->getEye(), camera->getDir());

		PxU32 nbActors = scene->Get()->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC);
		if(nbActors)
		{
			std::vector<PxRigidActor*> actors(nbActors);
			scene->Get()->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC, (PxActor**)&actors[0], nbActors);
			Renderer::Render(&actors[0], (PxU32)actors.size());
		}

		Renderer::Finish();
	}

	void specialKey(int key, int x, int y)
	{
		//simulation control
		switch (key)
		{
		case GLUT_KEY_F12:
			scene->Reset();
			break;
		case GLUT_KEY_F10:
			scene->Pause(!scene->Pause());
			break;
		case GLUT_KEY_F5:
			Renderer::ShowShadows(!Renderer::ShowShadows());
			break;
		default:
			break;
		}
	}

	void keyboardCallback(unsigned char key, int x, int y)
	{
		//exit
		if (key == 27)
			exit(0);

		//camera control
		switch (toupper(key))
		{
		case 'W':
			camera->MoveForward(); break;
		case 'S':
			camera->MoveBackward(); break;
		case 'A':
			camera->MoveLeft(); break;
		case 'D':
			camera->MoveRight(); break;
		default:
			break;
		}
	}

	int mMouseX = 0;
	int mMouseY = 0;

	void motionCallback(int x, int y)
	{
		int dx = mMouseX - x;
		int dy = mMouseY - y;

		camera->Motion(dx, dy);

		mMouseX = x;
		mMouseY = y;
	}

	void mouseCallback(int button, int state, int x, int y)
	{
		mMouseX = x;
		mMouseY = y;
	}

	void exitCallback(void)
	{
		delete camera;
	}

	void Init(const char *window_name, int width=512, int height=512)
	{
		Renderer::SetRenderDetail(40);
		Renderer::InitWindow(window_name, width, height);
		Renderer::Init();

		glutDisplayFunc(renderCallback);
		glutKeyboardFunc(keyboardCallback);
		glutSpecialFunc(specialKey);
		glutMouseFunc(mouseCallback);
		glutMotionFunc(motionCallback);
		atexit(exitCallback);
		motionCallback(0,0);
	}

	void Start()
	{ 
		glutMainLoop(); 
	}

	void SetActiveScene(PhysicsEngine::Scene& _scene)
	{
		scene = &_scene;
	}
}

