#pragma once

#include "PxPhysicsAPI.h"
#include "GLFontRenderer.h"
#include <GL/glut.h>
#include <string>

namespace VisualDebugger
{
	namespace Renderer
	{
		using namespace physx;

		///Init rendering window
		void InitWindow(const char *name, int width, int height);

		///Init renderer
		void Init();

		///Start rendering a single frame
		void Start(const PxVec3& cameraEye, const PxVec3& cameraDir);

		///Render actors
		void Render(PxActor** actors, const PxU32 numActors);

		///Render debug information
		void Render(const PxRenderBuffer& data, PxReal line_width=1.f);

		///Render text
		void RenderText(const std::string& text, const physx::PxVec2& location, 
			const PxVec3& color, PxReal size);

		///Set background color
		void BackgroundColor(const PxVec3& background_color);

		///Finish rendering a single frame
		void Finish();

		///Set rendering detail for spheres and capsules.
		void SetRenderDetail(int value);

		///Set show shadows
		void ShowShadows(bool value);

		///Get show shadows
		bool ShowShadows();
	}
}