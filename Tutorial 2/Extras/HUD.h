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
		vector<string> content;

	public:
		int id;
		PxReal font_size;
		PxVec3 color;

		HUDScreen(int screen_id, const PxVec3& _color=PxVec3(1.f,1.f,1.f), const PxReal& _font_size=0.024f) :
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
				Renderer::RenderText(content[i], PxVec2(0.0, 1.f-(i+1)*font_size), color, font_size);
		}

		///Clear content of the screen
		void Clear()
		{
			content.clear();
		}
	};

	///HUD class containing multiple screens
	class HUD
	{
		int active_screen;
		vector<HUDScreen*> screens;

	public:
		~HUD()
		{
			for (unsigned int i = 0; i < screens.size(); i++)
				delete screens[i];
		}

		///Add a single line to a specific screen
		void AddLine(int screen_id, string line)
		{
			for (unsigned int i = 0; i < screens.size(); i++)
			{
				if (screens[i]->id == screen_id)
				{
					screens[i]->AddLine(line);
					return;
				}
			}

			screens.push_back(new HUDScreen(screen_id));
			screens.back()->AddLine(line);
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
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					screens[i]->Clear();
				}
			}
			else
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					if (screens[i]->id == screen_id)
					{
						screens[i]->Clear();
						return;
					}
				}
			}
		}

		///Change the font size for a specified screen (-1 = all)
		void FontSize(PxReal font_size, unsigned int screen_id=-1)
		{
			if (screen_id == -1)
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					screens[i]->font_size = font_size;
				}
			}
			else
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					if (screens[i]->id == screen_id)
					{
						screens[i]->font_size = font_size;
						return;
					}
				}
			}
		}

		///Change the color for a specified screen (-1 = all)
		void Color(PxVec3 color, unsigned int screen_id=-1)
		{
			if (screen_id == -1)
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					screens[i]->color = color;
				}
			}
			else
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					if (screens[i]->id == screen_id)
					{
						screens[i]->color = color;
						return;
					}
				}
			}
		}

		///Render the active screen
		void Render()
		{
			for (unsigned int i = 0; i < screens.size(); i++)
			{
				if (screens[i]->id == active_screen)
				{
					screens[i]->Render();
					return;
				}
			}
		}
	};
}