#pragma once

#include "Renderer.h"
#include <string>
#include <list>

namespace VisualDebugger
{
	using namespace std;

	///A single HUD screen
	class HUDScreen
	{
		PxVec3 color;
		PxReal font_size;
		vector<string> content;

	public:
		int id;

		HUDScreen(int screen_id, const PxVec3& _color=PxVec3(1.f,1.f,.1f), const PxReal& _font_size=0.024f) :
			id(screen_id), color(_color), font_size(_font_size)
		{
		}

		///Add a single line of text
		void AddLine(string line)
		{
			content.push_back(line);
		}

		///Render the screen
		void Render()
		{
			for (unsigned int i = 0; i < content.size(); i++)
				Renderer::RenderText(content[i], PxVec2(0.0,1.f-(i+1)*font_size));
		}
	};

	///HUD class containing multiple screens
	class HUD
	{
		int active_screen;
		list<HUDScreen> screens;

	public:
		///Add a single line to a specific screen
		void AddLine(int screen_id, string line)
		{
			for (list<HUDScreen>::iterator it = screens.begin(); it != screens.end(); it++)
			{
				if (it->id == screen_id)
				{
					it->AddLine(line);
					return;
				}
			}

			HUDScreen new_screen(screen_id);
			new_screen.AddLine(line);
			screens.push_back(new_screen);
		}

		///Set the active screen
		void ActiveScreen(int value)
		{
			active_screen = value;
		}

		///Get the active screen
		int ActiveScreen()
		{
			return active_screen;		
		}

		///Clear a specified screen (or all of them)
		void Clear(int screen_id=-1)
		{
			if (screen_id == -1)
				screens.clear();
			else
			{
				for (list<HUDScreen>::iterator it = screens.begin(); it != screens.end(); it++)
				{
					if (it->id == screen_id)
					{
						screens.erase(it);
						return;
					}
				}
			}
		}

		///Render the active screen
		void Render()
		{
			for (list<HUDScreen>::iterator it = screens.begin(); it != screens.end(); it++)
			{
				if (it->id == active_screen)
				{
					it->Render();
					return;
				}
			}
		}
	};
}