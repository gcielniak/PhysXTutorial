#pragma once

#include "PxPhysicsAPI.h"
#include <glut.h>

namespace VisualDebugger
{
	namespace Renderer
	{
		using namespace physx;

		void InitWindow(const char *name, int width, int height);

		void Init();

		void Start(const PxVec3& cameraEye, const PxVec3& cameraDir);

		void AdjustSaturation(PxVec3& color, float value);

		void Render(PxRigidActor** actors, const PxU32 numActors);

		void Finish();

		void SetRenderDetail(int value);

		void ShowShadows(bool value);

		bool ShowShadows();
	}
}