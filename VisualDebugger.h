#pragma once

#include "PxPhysicsAPI.h"
#include <gl/gl.h>

using namespace physx;

	// Camera globals
	float	gCameraAspectRatio = 1.0f;
	PxVec3	gCameraPos(0,5,-15);
	PxVec3	gCameraForward(0,0,1);
	PxVec3	gCameraRight(-1,0,0);

class VisualDebugger
{
	int mx;
	int my;
public:
	VisualDebugger()
	{
		//mouse functions
		mx = 0;
		my = 0;

	}

	void Init(int argc, char** argv)
	{
		glutInit(&argc, argv);
		glutInitWindowSize(512, 512);
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

		//callbacks
		glutSetWindow(glutCreateWindow("Workshop 1"));
		glutDisplayFunc(RenderCallback);
		glutReshapeFunc(ReshapeCallback);
		glutIdleFunc(IdleCallback);
		glutKeyboardFunc(KeyPress);
		glutKeyboardUpFunc(KeyRelease);
		glutSpecialFunc(KeySpecial);
		glutMouseFunc(MouseCallback);
		glutMotionFunc(MotionCallback);
		atexit(ExitCallback);

		//default render states
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_CULL_FACE);

		//lighting
		glEnable(GL_LIGHTING);
		float AmbientColor[]    = { 0.0f, 0.1f, 0.2f, 0.0f };
		float AmbientColor2[]    = { 0.2f, 0.1f, 0.0f, 0.0f };			
		float DiffuseColor[]    = { 0.2f, 0.2f, 0.2f, 0.0f };			
		float SpecularColor[]   = { 0.5f, 0.5f, 0.5f, 0.0f };			
		float Position[]        = { 100.0f, 100.0f, -400.0f, 1.0f };

		//default lighting
		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);	
		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);	
		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);	
		glLightfv(GL_LIGHT0, GL_POSITION, Position);
		glEnable(GL_LIGHT0);

		//alternative lighting
		glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientColor2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseColor);
		glLightfv(GL_LIGHT1, GL_SPECULAR, SpecularColor);
		glLightfv(GL_LIGHT1, GL_POSITION, Position);

		MotionCallback(0,0);

		//	PrintControls();

		//	InitHUD();
	}

	///
	/// Enter the GLUT's main loop.
	///
	void Start() { glutMainLoop(); }

	virtual void UpdateScene()=0;

	void Display()
	{
		//clear display buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//set the camera view
		SetupCamera();

		//display scene
		//		RenderActors();

		glFlush();
		glutSwapBuffers();
	}

	///
	/// Perform one step of simulation, process input keys and render the scene.
	///
	void RenderCallback()
	{
		Display();
	}

	///
	/// Setup the camera view veriables.
	///
	void SetupCamera()
	{
		gCameraAspectRatio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);

		// Setup camera
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0f, gCameraAspectRatio, 1.0f, 10000.0f);
		gluLookAt(gCameraPos.x, gCameraPos.y, gCameraPos.z,
			gCameraPos.x+gCameraForward.x, gCameraPos.y+gCameraForward.y, gCameraPos.z+gCameraForward.z,
			0.0f, 1.0f, 0.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	///
	/// Setup the camera view variables.
	///
	void ResetCamera()
	{
		gCameraAspectRatio = 1.0f;
		gCameraPos = NxVec3(0,5,-15);
		gCameraForward = NxVec3(0,0,1);
		gCameraRight = NxVec3(-1,0,0);
	}

	///
	/// Change the view point and aspect ratio.
	///
	void ReshapeCallback(int width, int height)
	{
		glViewport(0, 0, width, height);
		gCameraAspectRatio = float(width)/float(height);
	}

	///
	/// Handle mouse events.
	///
	void MouseCallback(int button, int state, int x, int y)
	{
		mx = x;
		my = y;
	}

	///
	/// Set-up camera view variables depending on mouse movement.
	///
	void MotionCallback(int x, int y)
	{
		int dx = mx - x;
		int dy = my - y;

		gCameraForward.normalize();
		gCameraRight.cross(gCameraForward,NxVec3(0,1,0));

		NxQuat qx(NxPiF32 * dx * 20 / 180.0f, NxVec3(0,1,0));
		qx.rotate(gCameraForward);
		NxQuat qy(NxPiF32 * dy * 20 / 180.0f, gCameraRight);
		qy.rotate(gCameraForward);

		mx = x;
		my = y;
	}

	void IdleCallback() { glutPostRedisplay(); }

	///
	/// Release PhysX SDK on exit.
	///
	void ExitCallback() { }

};

