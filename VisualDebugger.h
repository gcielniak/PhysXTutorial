#pragma once

#include <vector>
#include "PhysicsEngine.h"
#include "SnippetRender/SnippetRender.h"
#include "SnippetRender/SnippetCamera.h"

using namespace physx;

int timer_start;
PhysicsEngine* engine;

class VisualDebugger
{
	static Snippets::Camera* sCamera;

public:
	static void Init(const std::string window_name)
	{
		Snippets::setRenderQuality(40);
		Snippets::setupDefaultWindow(window_name.c_str());
		Snippets::setupDefaultRenderState();

		glutIdleFunc(idleCallback);
		glutDisplayFunc(renderCallback);
		glutKeyboardFunc(keyboardCallback);
		glutMouseFunc(mouseCallback);
		glutMotionFunc(motionCallback);
		atexit(exitCallback);
		motionCallback(0,0);
		timer_start = glutGet(GLUT_ELAPSED_TIME);
	}

	static void Start() { glutMainLoop(); }

	static void SetEngine(PhysicsEngine& _engine)
	{
		engine = &_engine;
		engine->InitScene();
	}

	static void renderCallback()
	{
		PxScene* scene;
		PxGetPhysics().getScenes(&scene,1);

		//physics step
		scene->simulate(PxReal(1./60.));
		scene->fetchResults(true);

		Snippets::startRender(sCamera->getEye(), sCamera->getDir());

		PxU32 nbActors = scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC);
		if(nbActors)
		{
			std::vector<PxRigidActor*> actors(nbActors);
			scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC, (PxActor**)&actors[0], nbActors);
			Snippets::renderActors(&actors[0], (PxU32)actors.size(), true);
		}

		Snippets::finishRender();

		engine->UpdateScene();
	}

	static void motionCallback(int x, int y)
	{
		sCamera->handleMotion(x, y);
	}

	static void keyboardCallback(unsigned char key, int x, int y)
	{
		if (key == 27)
			exit(0);

		switch (toupper(key))
		{
		case 'W':
			sCamera->MoveForward(); break;
		case 'S':
			sCamera->MoveBackward(); break;
		case 'A':
			sCamera->MoveLeft(); break;
		case 'D':
			sCamera->MoveRight(); break;
		default:
			break;
		}
		int time_now = glutGet(GLUT_ELAPSED_TIME);
		std::cout << "dt: " << time_now - timer_start << std::endl;
		timer_start = time_now;
	}

	static void mouseCallback(int button, int state, int x, int y)
	{
		sCamera->handleMouse(button, state, x, y);
	}

	static void idleCallback()
	{
		glutPostRedisplay();
	}

	static void exitCallback(void)
	{
		delete sCamera;
	}
};

Snippets::Camera* VisualDebugger::sCamera = new Snippets::Camera(PxVec3(0.0f, 5.0f, 30.0f), PxVec3(0.0f,-5.0f,-30.0f));
