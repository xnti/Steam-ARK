#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "singleton.h"
#include "imgui/imgui.h"

DWORD keys[] = { VK_LMENU, VK_MENU, VK_SHIFT, VK_RSHIFT, VK_CONTROL, VK_RCONTROL, VK_LBUTTON, VK_RBUTTON };
const char* keyItems[] = { "LAlt", "RAlt", "LShift", "RShift", "LControl", "RControl", "LMouse", "RMouse" };

inline namespace Configuration
{
	class Settings : public Singleton<Settings>
	{
	public:

		const char* BoxTypes[2] = { "Full Box","Cornered Box" };
		const char* LineTypes[3] = { "Bottom To Enemy","Top To Enemy","Crosshair To Enemy" };
		const char* HealthTypes[2] = { "HealthBar","Value" };


		bool b_MenuShow = true;

		// Player ESP
		bool p_Visual = true;
		bool p_Box = false;
		bool p_Skeleton = true;
		bool p_Line = false;
		bool p_Nearby = false;
		bool p_Distance = true;
		bool p_Health_Bar = true;
		bool p_Health_Value = true;
		bool p_Torpor_Bar = true;
		bool p_Torpor_Value = true;
		bool p_Name = true;
		bool p_PlayerName = true;
		bool p_TribeName = false;
		bool p_Level = true;
		bool p_Weight = true;
		bool p_Debug = false;
		float p_max_distance = 1100.0f;


		// Dino ESP
		bool d_Visual = true;
		bool d_Box = false;
		bool d_Skeleton = false;
		bool d_Line = false;
		bool d_Nearby = false;
		bool d_Distance = true;
		bool d_Health_Bar = true;
		bool d_Health_Value = true;
		bool d_Torpor_Bar = true;
		bool d_Torpor_Value = true;
		bool d_Taming_Percent = true;
		bool d_Name = true;
		bool d_DinoName = true;
		bool d_TribeName = false;
		bool d_Level = true;
		bool d_Debug = true;
		float d_max_distance = 1100.0f;


		// Additional ESP
		bool a_Visual = true;
		bool a_Box = true;
		bool a_Line = true;
		bool a_Nearby = false;
		bool a_Distance = true;
		bool a_Name = true;
		bool a_Level = true;
		bool a_Weight = true;
		float a_max_distance = 1100.0f;


		//Debug
		bool g_Bones = true;
		bool g_Debug = false;




		bool b_Aimbot = false;
		bool b_AimbotFOV = true;
		bool b_AimbotSmooth = false;

		bool b_FreeCam = false;
		bool b_NoRecoil = false;

		int BoxType = 0;
		int LineType = 0;
		int HealthType = 0;
		int tab_index = 0;
		int AimKey = 0;

		float Smoothing = 1.5f;
		float AimbotFOV = 120.0f;
		float max_distance = 1100.0f;

	
		ImColor VisibleColor = ImColor(255.f / 255, 0.f, 0.f);
		float fl_VisibleColor[3] = { 0.f,255.f / 255,0.f };  //

		ImColor InvisibleColor = ImColor(0.f, 255.f / 255, 0.f);
		float fl_InvisibleColor[3] = { 255.f / 255,0.f,0.f };  //

		ImColor FovColor = ImColor(255.f / 255, 0.f, 0.f);
		float fl_FovColor[3] = { 255.f / 255,0.f,0.f };  //


		// Player Specific ESP Colors

		ImColor PName = ImColor(0.f, 255.f / 255, 0.f);
		float p_PName[3] = { 255.f / 255,0.f,0.f };  //

		ImColor PTribeName = ImColor(0.f, 255.f / 255, 0.f);
		float p_PTribeName[3] = { 255.f / 255,0.f,0.f };  //

		ImColor PBox = ImColor(0.f, 255.f / 255, 0.f);
		float p_PBox[3] = { 255.f / 255,0.f,0.f };  //

		ImColor PSkeleton = ImColor(0.f, 255.f / 255, 0.f);
		float p_PSkeleton[3] = { 255.f / 255,0.f,0.f };  //

		ImColor PNearby = ImColor(0.f, 255.f / 255, 0.f);
		float p_PNearby[3] = { 255.f / 255,0.f,0.f };  //

		ImColor PDistance = ImColor(0.f, 255.f / 255, 0.f);
		float p_PDistance[3] = { 255.f / 255,0.f,0.f };  //

		ImColor PLevel = ImColor(0.f, 255.f / 255, 0.f);
		float p_PLevel[3] = { 255.f / 255,0.f,0.f };  //

		ImColor PHealth = ImColor(0.f, 255.f / 255, 0.f);
		float p_PHealth[3] = { 255.f / 255,0.f,0.f };  //

	};
#define CFG Configuration::Settings::Get()
}
bool GetAimKey()
{
	return GetAsyncKeyState(keys[CFG.AimKey]);
}
