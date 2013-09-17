#pragma once

#include <vector>
#include "SnippetRender/SnippetRender.h"
#include "SnippetRender/SnippetCamera.h"

using namespace physx;

class VisualDebugger
{
	static Snippets::Camera* sCamera;

public:
	void Init(const std::string window_name)
	{
		Snippets::setupDefaultWindow(window_name.c_str());
		Snippets::setupDefaultRenderState();

		glutIdleFunc(idleCallback);
		glutDisplayFunc(renderCallback);
		glutKeyboardFunc(keyboardCallback);
		glutMouseFunc(mouseCallback);
		glutMotionFunc(motionCallback);
		atexit(exitCallback);
		motionCallback(0,0);
	}

	void Start() { glutMainLoop(); }

	static void renderCallback()
	{
		PxScene* scene;
		PxGetPhysics().getScenes(&scene,1);

		//	stepPhysics(true);
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
	}

	static void motionCallback(int x, int y)
	{
		sCamera->handleMotion(x, y);
	}

	static void keyboardCallback(unsigned char key, int x, int y)
	{
		if(key==27)
			exit(0);

		if(!sCamera->handleKey(key, x, y))
			;//		keyPress(key, sCamera->getTransform());
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

Snippets::Camera* VisualDebugger::sCamera = new Snippets::Camera(PxVec3(0.0f, 10.0f, 20.0f), PxVec3(0.0f,-0.5f,-1.0f));
