#include "function.h"
#include "overlay.h"
#include "Mhyprot/mhyprot.hpp"

namespace OverlayWindow
{
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCSTR Name;
}

namespace DirectX9Interface
{
	IDirect3D9Ex* Direct3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParams = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
}
typedef struct _EntityList
{
	// Players EntityList
	uintptr_t player;
	uintptr_t player_pawn;
	uintptr_t player_mesh;
	uintptr_t player_level;
	int player_id;
	FString player_name;
	Vector3 player_pos;


	// Structure EntityList
	uintptr_t structure;
	FString structure_name;
	Vector3 structure_pos;


	// General EntityList

	uintptr_t actor;
	int actor_id;
	uintptr_t actor_pawn;
	uintptr_t actor_mesh;
	uintptr_t actor_level;
	uintptr_t actor_root;
	FString actor_name;
	Vector3 actor_pos;


}EntityList;
std::vector<EntityList> entityActorsList;
std::vector<EntityList> entityDinoList;
std::vector<EntityList> entityPlayerList;

auto CallHacks()->VOID
{
	while (true)
	{
		if (CFG.b_FreeCam)
		{
			//write<float>(GameVars.FreeCam - 0x4 1200);
			//write<float>(GameVars.FreeCam + 0x100, 0); // HorizontalCurve
			//write<float>(GameVars.FreeCam + 0x108, 0); // HorizontalCurve
		}
		Sleep(10);
	}
}

auto GameCache()->VOID
{
	while (true)
	{
		std::vector<EntityList> dinoscopy;
		std::vector<EntityList> playerscopy;
		std::vector<EntityList> actorscopy;

		GameVars.GNamesAddress = read<DWORD_PTR>(GameVars.dwProcess_Base + GameOffset.offset_g_names);
		if (!GameVars.GNamesAddress) continue;

		GameVars.u_world = read<DWORD_PTR>(GameVars.dwProcess_Base + GameOffset.offset_u_world);
		if (!GameVars.u_world) continue;

		GameVars.game_instance = read<DWORD_PTR>(GameVars.u_world + GameOffset.offset_game_instance);
		if (!GameVars.game_instance) continue;

		GameVars.local_player_array = read<DWORD_PTR>(GameVars.game_instance + GameOffset.offset_local_players_array);
		if (!GameVars.local_player_array) continue;

		GameVars.local_player = read<DWORD_PTR>(GameVars.local_player_array);
		if (!GameVars.local_player) continue;

		GameVars.local_player_controller = read<DWORD_PTR>(GameVars.local_player + GameOffset.offset_player_controller);
		if (!GameVars.local_player_controller) continue;

		GameVars.local_player_pawn = read<DWORD_PTR>(GameVars.local_player_controller + GameOffset.offset_apawn);
		if (!GameVars.local_player_pawn) continue;

		GameVars.local_player_root = read<DWORD_PTR>(GameVars.local_player_pawn + GameOffset.offset_root_component);
		if (!GameVars.local_player_root) continue;

		GameVars.local_player_state = read<DWORD_PTR>(GameVars.local_player_pawn + GameOffset.offset_player_state);
		if (!GameVars.local_player_state) continue;

		GameVars.persistent_level = read<DWORD_PTR>(GameVars.u_world + GameOffset.offset_persistent_level);
		if (!GameVars.persistent_level) continue;

		GameVars.actors = read<DWORD_PTR>(GameVars.persistent_level + GameOffset.offset_actor_array);
		if (!GameVars.actors) continue;

		GameVars.actor_count = read<int>(GameVars.persistent_level + GameOffset.offset_actor_count);
		if (!GameVars.actor_count) continue;


		GameVars.CameraManager = read<uintptr_t>(GameVars.local_player_controller + GameOffset.offset_camera_manager);
		if (!GameVars.CameraManager) continue;

		// This loop is for all Actors in UWorld - ULevel

		for (int index = 0; index < GameVars.actor_count; ++index)
		{

			auto actor = read<uintptr_t>(GameVars.actors + index * 0x8);
			if (!actor) continue;

			auto actor_id = read<int>(actor + 0x18);
			if (!actor_id) continue;

			auto actor_name = getNameFromID(actor_id);
			const char* c = actor_name.c_str();

			auto actor_root = read<uintptr_t>(actor + GameOffset.offset_root_component);
			if (!actor_root) continue;

			auto actor_pos = read<Vector3>(actor_root + GameOffset.offset_relative_location);
			if (actor_pos.x == 0 || actor_pos.y == 0 || actor_pos.z == 0) continue;

			auto actor_mesh = read<uintptr_t>(actor + GameOffset.offset_actor_mesh); // You can use both actor_pawn | actor
			//if (!actor_mesh) continue;

			auto actor_status = read<uintptr_t>(actor + GameOffset.offset_status_component);
			//if (!actor_status) continue;

			auto actor_level = read<uintptr_t>(actor_status + GameOffset.offset_base_character_level);
			//if (!actor_level) continue;
			
			if (actor_id > 235000 && actor_id < 235281) // Filtering for the Range of Players, you can also filter for the GName which is way better than going for the ID (since the ID changes on different Servers)
			{
				//if (actor != NULL || actor_mesh != NULL || actor_pos.x != 0 || actor_pos.y != 0 || actor_pos.z != 0)
				//{
					EntityList Player{ };
					Player.player = actor;
					Player.player_mesh = actor_mesh;
					Player.player_pos = actor_pos;
					Player.player_level = actor_level;
					playerscopy.push_back(Player);
				//}
			}
			
			// else
			else // Drawing all Actors except Players
			{
				//if (actor != NULL || actor_mesh != NULL || actor_pos.x != 0 || actor_pos.y != 0 || actor_pos.z != 0)
				//{
					EntityList Dino{ };
					Dino.actor = actor;
					Dino.actor_id = actor_id;
					Dino.actor_mesh = actor_mesh;
					Dino.actor_pos = actor_pos;
					Dino.actor_level = actor_level;
					dinoscopy.push_back(Dino);
				//}
			}
		}


			entityPlayerList = playerscopy;

			entityDinoList = dinoscopy;

			entityActorsList = actorscopy;

			Sleep(1);


			/* Coloring Characters Externally
			
			for (int i = 0; i < GameVars.player_array_count; i++)
			{
				auto player_array = read<uintptr_t>(GameVars.player_array + i * 0x8);
				if (player_array == (uintptr_t)nullptr || player_array == -1 || player_array == NULL) continue;

				auto player_state = read<uintptr_t>(player_array + GameOffset.offset_player_state);
				if (player_state)
				{
					FPrimalPlayerCharacterConfigStructReplicated data;
					data.bIsFemale = true;
					data.BodyColors[0] = FLinearColor(255, 0, 0, 255); //red
					data.BodyColors[1] = FLinearColor(255, 0, 0, 255); //red
					data.BodyColors[2] = FLinearColor(255, 0, 0, 255); //red
					data.BodyColors[3] = FLinearColor(255, 0, 0, 255); //red

					for (int i = 0; i < 22; i++)
						data.RawBoneModifiers[i] = -1.0f;

					data.PlayerCharacterName = FString "redman";
					data.PlayerSpawnRegionIndex = 0;

					player_state->ServerRequestCreateNewPlayer(data);
				}
			}
			*/
	
	}
}

auto RenderVisual()->VOID
{
	auto DinoList_Copy = entityDinoList;
	auto PlayersList_Copy = entityPlayerList;
	auto ExtraActors_Copy = entityActorsList;

	for (int index = 0; index < PlayersList_Copy.size(); ++index)
	{
		auto Player = PlayersList_Copy[index];

		if (Player.player == GameVars.local_player_pawn)
			continue;

		if (!Player.player_mesh || !Player.player)
			continue;

		auto local_pos = read<Vector3>(GameVars.local_player_root + GameOffset.offset_relative_location);

		auto p_head_pos = GetBoneWithRotation(Player.player_mesh, 8);
		auto p_bottom_pos_a = GetBoneWithRotation(Player.player_mesh, 1);
		auto p_bottom_pos = Vector3(p_bottom_pos_a.x, p_bottom_pos_a.y, p_bottom_pos_a.z - 100);

		auto BottomBox = ProjectWorldToScreen(p_bottom_pos);
		auto TopBox = ProjectWorldToScreen(Vector3(p_head_pos.x, p_head_pos.y, p_head_pos.z + 15));

		auto entity_distance = local_pos.Distance(p_bottom_pos);

		auto CornerHeight = abs(TopBox.y - BottomBox.y);
		auto CornerWidth = CornerHeight * 0.65;

		auto bVisible = isVisible(Player.actor_mesh);
		//auto ESP_Color = GetVisibleColor(bVisible); // Externally VisCheck doesnt really work for ARK (not optimized) with LastSubmitTime or LastRenderTime...
		auto ESP_Color = CFG.VisibleColor;

		// Players ESP
		if (CFG.p_Visual)
		{
			if (entity_distance < CFG.p_max_distance)
			{
				if (CFG.p_Box)
				{
					if (CFG.BoxType == 0)
					{
						DrawBox(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, ESP_Color);
					}
					else if (CFG.BoxType == 1)
					{
						DrawCorneredBox(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, ESP_Color, 1.5);
					}
				}
				if (CFG.p_Nearby)
				{
					if (entity_distance < 600)
					{
						char dist[64];
						sprintf_s(dist, "Enemy nearby:%.fm", entity_distance);
						DrawOutlinedText(Verdana, dist, ImVec2(960, 100), 34.0f, ImColor(255, 255, 255), true);
					}
				}
				if (CFG.p_Line)
				{

					if (CFG.LineType == 0)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), static_cast<float>(GameVars.ScreenHeight)), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM BOTTOM SCREEN
					}
					if (CFG.LineType == 1)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), 0.f), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM TOP SCREEN
					}
					if (CFG.LineType == 2)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), static_cast<float>(GameVars.ScreenHeight / 2)), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM CROSSHAIR
					}
				}
				if (CFG.p_Distance)
				{
					char dist[64];
					sprintf_s(dist, "[%.fm]", entity_distance);
					DrawOutlinedText(Verdana, dist, ImVec2(BottomBox.x, BottomBox.y + 10), 13.5f, ImColor(255, 255, 255), true);
				}
				if (CFG.p_Level)
				{
					char level[64];
					sprintf_s(level, "Lvl: %i", Player.player_level);
					DrawOutlinedText(Verdana, level, ImVec2(BottomBox.x, BottomBox.y - 5), 14.0f, ImColor(255, 255, 255), true);
				}
				if (CFG.p_Name)
				{
					//auto actor_name = read<FString>(Player.player + GameOffset.offset_descriptivename);
					// Not needed except you want to see that the Playernames are "PrimalCharacter"
					//DrawOutlinedText(Verdana, actor_name.ToString(), ImVec2(TopBox.x, TopBox.y - 20), 14.0f, ImColor(255, 255, 255), true);

					//if (actor_name.ToString() == "PrimalCharacter")
					//{
						auto player_name = read<FString>(Player.player + GameOffset.offset_playername);
						DrawOutlinedText(Verdana, player_name.ToString(), ImVec2(TopBox.x, TopBox.y - 30), 14.0f, ImColor(255, 255, 255), true);
					//}
				}
				if (CFG.p_TribeName)
				{
					auto actor_tribe_name = read<FString>(Player.player + GameOffset.offset_tribename);
					DrawOutlinedText(Verdana, actor_tribe_name.ToString(), ImVec2(TopBox.x, TopBox.y - 40), 14.0f, ImColor(255, 255, 255), true);
				}
				if (CFG.p_Health_Bar)
				{
					auto Health = read<float>(Player.player + GameOffset.offset_health);
					if (!Health) continue;
					auto MaxHealth = read<float>(Player.player + GameOffset.offset_max_health);
					if (!MaxHealth) continue;
					auto procentage = Health * 100 / MaxHealth;


					float width = CornerWidth / 10;
					if (width < 2.f) width = 2.;
					if (width > 3) width = 3.;

					HealthBar(TopBox.x - (CornerWidth / 2) - 8, TopBox.y, width, BottomBox.y - TopBox.y, procentage, true);
				}
				if (CFG.p_Health_Value)
				{
					auto Health = read<float>(Player.player + GameOffset.offset_health);
					auto MaxHealth = read<float>(Player.player + GameOffset.offset_max_health);

					char currenthealth[64];
					sprintf_s(currenthealth, "%.f |", Health);
					DrawOutlinedText(Verdana, currenthealth, ImVec2(BottomBox.x - 15, BottomBox.y + 22), 14.4f, ImColor(255, 0, 132), true);

					char maxhealth[64];
					sprintf_s(maxhealth, "%.f", MaxHealth);
					DrawOutlinedText(Verdana, maxhealth, ImVec2(BottomBox.x + 15, BottomBox.y + 22), 14.4f, ImColor(255, 0, 132), true);

				}

				if (CFG.p_Torpor_Bar)
				{
					auto Health = read<float>(Player.player + GameOffset.offset_health);
					if (Health > 0.1)
					{
						auto Torpor = read<float>(Player.player + GameOffset.offset_torpor);
						if (!Torpor) continue;
						auto TorporMax = read<float>(Player.player + GameOffset.offset_max_torpor);
						if (!TorporMax) continue;
						auto procentage = Torpor * 100 / TorporMax;


						float width = CornerWidth / 10;
						if (width < 2.f) width = 2.;
						if (width > 3) width = 3.;

						TorporBar(TopBox.x - (CornerWidth / 2) - 14, TopBox.y, width, BottomBox.y - TopBox.y, procentage, true);

					}
				}
				if (CFG.p_Torpor_Value)
				{
				/*	auto Torpor = read<float>(Player.player + GameOffset.offset_torpor);
					auto MaxTorpor = read<float>(Player.player + GameOffset.offset_max_torpor);

					char currenttorpor[64];
					sprintf_s(currenttorpor, "%.f |", Torpor);
					DrawOutlinedText(Verdana, currenttorpor, ImVec2(BottomBox.x - 40, BottomBox.y + 10), 14.4f, ImColor(255, 0, 132), true);

					char maxtorpor[64];
					sprintf_s(maxtorpor, "%.f", MaxTorpor);
					DrawOutlinedText(Verdana, currenthealth2, ImVec2(BottomBox.x + 40, BottomBox.y + 10), 14.4f, ImColor(255, 0, 132), true);
				*/
				}


				if (CFG.p_Weight)
				{
					auto player_weight = read<float>(Player.player + GameOffset.offset_current_weight);

					char value[64];
					sprintf_s(value, "%.f Weight", player_weight);

					DrawOutlinedText(Verdana, value, ImVec2(BottomBox.x, BottomBox.y + 35), 14.6f, ImColor(255, 255, 132), true);
				}

				if (CFG.p_Debug)
				{
					for (int a = 0; a < 110; ++a) {
						auto BonePos = GetBoneWithRotation(Player.player_mesh, a);
						auto Screen = ProjectWorldToScreen(BonePos);

						DrawString(ImVec2(Screen.x, Screen.y), std::to_string(a), IM_COL32_WHITE);
					}
				}

				if (CFG.p_Skeleton)
				{

					// Female Bones
					Vector3 vHeadOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 8));
					Vector3 vNeckOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 6));
					Vector3 vSpine1Out = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 5));
					Vector3 vSpine2Out = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 3));
					Vector3 vSpine3Out = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 2));
					Vector3 vPelvisOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 1));
					Vector3 vRightThighOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 88));
					Vector3 vLeftThighOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 82));
					Vector3 vRightCalfOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 90));
					Vector3 vLeftCalfOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 84));
					Vector3 vRightFootOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 92));
					Vector3 vLeftFootOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 86));
					Vector3 vUpperArmRightOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 59));
					Vector3 vUpperArmLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 33));
					Vector3 vLowerArmRightOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 62));
					Vector3 vLowerArmLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 36));
					Vector3 vHandRightOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 64));
					Vector3 vHandLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 38));
					Vector3 vFingerRightOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 71));
					Vector3 vFingerLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 45));
					//Vector3 vFootOutOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 0));


					DrawLine(ImVec2(vHeadOut.x, vHeadOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ESP_Color, 1.5f);
					DrawLine(ImVec2(vNeckOut.x, vNeckOut.y), ImVec2(vSpine1Out.x, vSpine1Out.y), ESP_Color, 1.5f);

					DrawLine(ImVec2(vNeckOut.x, vNeckOut.y), ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ESP_Color, 1.5f);
					DrawLine(ImVec2(vNeckOut.x, vNeckOut.y), ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ESP_Color, 1.5f);

					DrawLine(ImVec2(vSpine1Out.x, vSpine1Out.y), ImVec2(vPelvisOut.x, vPelvisOut.y), ESP_Color, 1.5f);

					DrawLine(ImVec2(vPelvisOut.x, vPelvisOut.y), ImVec2(vRightThighOut.x, vRightThighOut.y), ESP_Color, 1.5f);
					DrawLine(ImVec2(vPelvisOut.x, vPelvisOut.y), ImVec2(vLeftThighOut.x, vLeftThighOut.y), ESP_Color, 1.5f);

					DrawLine(ImVec2(vRightThighOut.x, vRightThighOut.y), ImVec2(vRightCalfOut.x, vRightCalfOut.y), ESP_Color, 1.5f);
					DrawLine(ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ESP_Color, 1.5f);

					DrawLine(ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImVec2(vRightFootOut.x, vRightFootOut.y), ESP_Color, 1.5f);
					DrawLine(ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImVec2(vLeftFootOut.x, vLeftFootOut.y), ESP_Color, 1.5f);

					DrawLine(ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImVec2(vLowerArmRightOut.x, vLowerArmRightOut.y), ESP_Color, 1.5f);
					DrawLine(ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImVec2(vLowerArmLeftOut.x, vLowerArmLeftOut.y), ESP_Color, 1.5f);

					DrawLine(ImVec2(vLowerArmLeftOut.x, vLowerArmLeftOut.y), ImVec2(vHandLeftOut.x, vHandLeftOut.y), ESP_Color, 1.5f);
					DrawLine(ImVec2(vLowerArmRightOut.x, vLowerArmRightOut.y), ImVec2(vHandRightOut.x, vHandRightOut.y), ESP_Color, 1.5f);

					DrawLine(ImVec2(vHandLeftOut.x, vHandLeftOut.y), ImVec2(vFingerLeftOut.x, vFingerLeftOut.y), ESP_Color, 1.5f);
					DrawLine(ImVec2(vHandRightOut.x, vHandRightOut.y), ImVec2(vFingerRightOut.x, vFingerRightOut.y), ESP_Color, 1.5f);

				}

				// Male Bones
				/*
				Vector3 vHeadOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 8));
				Vector3 vNeckOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 6));
				Vector3 vSpine1Out = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 5));
				Vector3 vSpine2Out = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 3));
				Vector3 vSpine3Out = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 2));
				Vector3 vPelvisOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 1));
				Vector3 vRightThighOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 86));
				Vector3 vLeftThighOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 80));
				Vector3 vRightCalfOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 88));
				Vector3 vLeftCalfOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 82));
				Vector3 vRightFootOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 90));
				Vector3 vLeftFootOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 84));
				Vector3 vUpperArmRightOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 57));
				Vector3 vUpperArmLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 33));
				Vector3 vLowerArmRightOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 60));
				Vector3 vLowerArmLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 36));
				Vector3 vHandRightOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 62));
				Vector3 vHandLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 38));
				Vector3 vFingerRightOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 69));
				Vector3 vFingerLeftOut = ProjectWorldToScreen(GetBoneWithRotation(Player.player_mesh, 45));
				//Vector3 vFootOutOut = ProjectWorldToScreen(GetBoneWithRotation(Entity.actor_mesh, 0));


				DrawLine(ImVec2(vHeadOut.x, vHeadOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ESP_Color, 1.5f);
				DrawLine(ImVec2(vNeckOut.x, vNeckOut.y), ImVec2(vSpine1Out.x, vSpine1Out.y), ESP_Color, 1.5f);

				DrawLine(ImVec2(vNeckOut.x, vNeckOut.y), ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ESP_Color, 1.5f);
				DrawLine(ImVec2(vNeckOut.x, vNeckOut.y), ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ESP_Color, 1.5f);

				DrawLine(ImVec2(vSpine1Out.x, vSpine1Out.y), ImVec2(vPelvisOut.x, vPelvisOut.y), ESP_Color, 1.5f);

				DrawLine(ImVec2(vPelvisOut.x, vPelvisOut.y), ImVec2(vRightThighOut.x, vRightThighOut.y), ESP_Color, 1.5f);
				DrawLine(ImVec2(vPelvisOut.x, vPelvisOut.y), ImVec2(vLeftThighOut.x, vLeftThighOut.y), ESP_Color, 1.5f);

				DrawLine(ImVec2(vRightThighOut.x, vRightThighOut.y), ImVec2(vRightCalfOut.x, vRightCalfOut.y), ESP_Color, 1.5f);
				DrawLine(ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ESP_Color, 1.5f);

				DrawLine(ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImVec2(vRightFootOut.x, vRightFootOut.y), ESP_Color, 1.5f);
				DrawLine(ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImVec2(vLeftFootOut.x, vLeftFootOut.y), ESP_Color, 1.5f);

				DrawLine(ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImVec2(vLowerArmRightOut.x, vLowerArmRightOut.y), ESP_Color, 1.5f);
				DrawLine(ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImVec2(vLowerArmLeftOut.x, vLowerArmLeftOut.y), ESP_Color, 1.5f);

				DrawLine(ImVec2(vLowerArmLeftOut.x, vLowerArmLeftOut.y), ImVec2(vHandLeftOut.x, vHandLeftOut.y), ESP_Color, 1.5f);
				DrawLine(ImVec2(vLowerArmRightOut.x, vLowerArmRightOut.y), ImVec2(vHandRightOut.x, vHandRightOut.y), ESP_Color, 1.5f);

				DrawLine(ImVec2(vHandLeftOut.x, vHandLeftOut.y), ImVec2(vFingerLeftOut.x, vFingerLeftOut.y), ESP_Color, 1.5f);
				DrawLine(ImVec2(vHandRightOut.x, vHandRightOut.y), ImVec2(vFingerRightOut.x, vFingerRightOut.y), ESP_Color, 1.5f);
				*/


			}
		}
	}

	for (int index = 0; index < DinoList_Copy.size(); ++index)
	{
		auto Dino = DinoList_Copy[index];

		auto local_pos = read<Vector3>(GameVars.local_player_root + GameOffset.offset_relative_location);

		//auto d_head_pos = GetBoneWithRotation(Dino.actor_mesh, 8);
		//auto d_bottom_pos = GetBoneWithRotation(Dino.actor_mesh, 0);

		auto d_head_pos = Vector3(Dino.actor_pos.x, Dino.actor_pos.y, Dino.actor_pos.z + 40);
		auto d_bottom_pos = Vector3(Dino.actor_pos.x, Dino.actor_pos.y, Dino.actor_pos.z - 40);

		auto BottomBox = ProjectWorldToScreen(d_bottom_pos);
		auto TopBox = ProjectWorldToScreen(Vector3(d_head_pos.x, d_head_pos.y, d_head_pos.z + 15));

		auto entity_distance = local_pos.Distance(d_bottom_pos);

		auto CornerHeight = abs(TopBox.y - BottomBox.y);
		auto CornerWidth = CornerHeight * 0.65;

		auto bVisible = isVisible(Dino.actor_mesh);
		//auto ESP_Color = GetVisibleColor(bVisible);
		auto ESP_Color = CFG.VisibleColor;

		auto actor_name = getNameFromID(Dino.actor_id);
		const char* c = actor_name.c_str();

		//if (actor_name == "SupplyCrate_OceanInstant_C") // ESP only on specific Items shitty option but works :D
		//{
		if (CFG.b_Aimbot)
		{
			if (CFG.b_AimbotFOV)
			{
				DrawCircle(GameVars.ScreenWidth / 2, GameVars.ScreenHeight / 2, CFG.AimbotFOV, CFG.FovColor, 0);
			}
		}
		if (CFG.d_Visual)
		{
			if (entity_distance < CFG.d_max_distance)
			{
				if (CFG.d_Box)
				{
					if (CFG.BoxType == 0)
					{
						DrawBox(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, ESP_Color);
					}
					else if (CFG.BoxType == 1)
					{
						DrawCorneredBox(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, ESP_Color, 1.5);
					}
				}
				if (CFG.d_Nearby)
				{
					if (entity_distance < 100)
					{
						char dist[64];
						sprintf_s(dist, "Enemy nearby:%.fm", entity_distance);
						DrawOutlinedText(Verdana, dist, ImVec2(960, 100), 34.0f, ImColor(255, 255, 255), true);
					}
				}
				if (CFG.d_Line)
				{

					if (CFG.LineType == 0)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), static_cast<float>(GameVars.ScreenHeight)), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM BOTTOM SCREEN
					}
					if (CFG.LineType == 1)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), 0.f), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM TOP SCREEN
					}
					if (CFG.LineType == 2)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), static_cast<float>(GameVars.ScreenHeight / 2)), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM CROSSHAIR
					}
				}
				if (CFG.d_Distance)
				{
					char dist[64];
					sprintf_s(dist, "[%.fm]", entity_distance);
					DrawOutlinedText(Verdana, dist, ImVec2(BottomBox.x, BottomBox.y + 10), 13.5f, ImColor(255, 255, 255), true);
				}
				if (CFG.d_Level)
				{
					char level[64];
					sprintf_s(level, "Lvl: %i", Dino.actor_level);
					DrawOutlinedText(Verdana, level, ImVec2(BottomBox.x, BottomBox.y - 5), 14.0f, ImColor(255, 255, 255), true);
				}
				if (CFG.d_Name)
				{
					//auto actor_name = read<std::string>(Dino.actor + GameOffset.offset_descriptivename);

					DrawOutlinedText(Verdana, c, ImVec2(TopBox.x, TopBox.y - 20), 14.0f, ImColor(255, 255, 255), true);
				}
				if (CFG.d_TribeName)
				{
					//	auto actor_tribe_name = read<FString>(Dino.actor + GameOffset.offset_tribename);
					//	DrawOutlinedText(Verdana, actor_tribe_name.ToString(), ImVec2(TopBox.x, TopBox.y - 40), 14.0f, ImColor(255, 255, 255), true);
				}
				if (CFG.d_Health_Bar)
				{
					auto Health = read<float>(Dino.actor + GameOffset.offset_health);
					if (!Health) continue;
					auto MaxHealth = read<float>(Dino.actor + GameOffset.offset_max_health);
					if (!MaxHealth) continue;
					auto procentage = Health * 100 / MaxHealth;


					float width = CornerWidth / 10;
					if (width < 2.f) width = 2.;
					if (width > 3) width = 3.;

					HealthBar(TopBox.x - (CornerWidth / 2) - 8, TopBox.y, width, BottomBox.y - TopBox.y, procentage, true);
				}
				if (CFG.d_Health_Value)
				{
					auto Health = read<float>(Dino.actor + GameOffset.offset_health);
					auto MaxHealth = read<float>(Dino.actor + GameOffset.offset_max_health);

					char currenthealth[64];
					sprintf_s(currenthealth, "%.f |", Health);
					DrawOutlinedText(Verdana, currenthealth, ImVec2(BottomBox.x - 15, BottomBox.y + 22), 14.4f, ImColor(255, 0, 132), true);

					char currenthealth2[64];
					sprintf_s(currenthealth2, "%.f", MaxHealth);
					DrawOutlinedText(Verdana, currenthealth2, ImVec2(BottomBox.x + 15, BottomBox.y + 22), 14.4f, ImColor(255, 0, 132), true);

				}

				if (CFG.d_Torpor_Bar)
				{
					auto Torpor = read<float>(Dino.actor + GameOffset.offset_torpor);
					if (Torpor > 0.1)
					{
						auto Torpor = read<float>(Dino.actor + GameOffset.offset_torpor);
						if (!Torpor) continue;
						auto TorporMax = read<float>(Dino.actor + GameOffset.offset_max_torpor);
						if (!TorporMax) continue;
						auto procentage = Torpor * 100 / TorporMax;


						float width = CornerWidth / 10;
						if (width < 2.f) width = 2.;
						if (width > 3) width = 3.;

						TorporBar(TopBox.x - (CornerWidth / 2) - 14, TopBox.y, width, BottomBox.y - TopBox.y, procentage, true);

					}
				}
				if (CFG.d_Torpor_Value)
				{
					auto Torpor = read<float>(Dino.actor + GameOffset.offset_torpor);
					auto MaxTorpor = read<float>(Dino.actor + GameOffset.offset_max_torpor);

					char currenthealth[64];
					sprintf_s(currenthealth, "%.f |", Torpor);
					DrawOutlinedText(Verdana, currenthealth, ImVec2(BottomBox.x - 15, BottomBox.y + 36), 14.4f, ImColor(255, 0, 132), true);

					char currenthealth2[64];
					sprintf_s(currenthealth2, "%.f", MaxTorpor);
					DrawOutlinedText(Verdana, currenthealth2, ImVec2(BottomBox.x + 15, BottomBox.y + 36), 14.4f, ImColor(255, 0, 132), true);

				}

				if (CFG.d_Taming_Percent)
				{
					auto current_tamee_fficiency = read<float>(Dino.actor + GameOffset.offset_current_tame_affinity);
					if (!current_tamee_fficiency) continue;
					auto required_tame_efficiency = read<float>(Dino.actor + GameOffset.offset_required_tame_affinity);
					if (!required_tame_efficiency) continue;
					auto procentage = current_tamee_fficiency * 100 / required_tame_efficiency;


					char procentage1[64];
					sprintf_s(procentage1, "Taming: %.f", procentage);
					DrawOutlinedText(Verdana, procentage1, ImVec2(BottomBox.x, BottomBox.y - 25), 14.0f, ImColor(255, 255, 255), true);

				}


				if (CFG.d_Debug)
				{
					char id[64];
					sprintf_s(id, "ID: %i", Dino.actor_id);
					DrawOutlinedText(Verdana, id, ImVec2(TopBox.x, TopBox.y - 40), 14.0f, ImColor(255, 255, 255), true);

				}

				if (CFG.g_Debug)
				{

					//write<float>(GameVars.local_player_pawn + 0x84, 20.0);

					//write<float>(GameVars.dwProcess_Base + 0x286CE60, 1);

					write<float>(GameVars.local_player_pawn + GameOffset.offset_riderrotation, 5);
					write<float>(GameVars.local_player_pawn + GameOffset.offset_walkingrotation, 5);
					write<float>(GameVars.local_player_pawn + GameOffset.offset_swimmingrotation, 5);
					write<float>(GameVars.local_player_pawn + GameOffset.offset_flyingrotation, 5);
				}
			}
			//}
		}
	}

	for (int index = 0; index < ExtraActors_Copy.size(); ++index)
	{
		auto Extra = ExtraActors_Copy[index];

		if (Extra.actor == GameVars.local_player_pawn)
			continue;

		if (!Extra.actor)
			continue;

		auto local_pos = read<Vector3>(GameVars.local_player_root + GameOffset.offset_relative_location);

		auto e_top_pos = Vector3(Extra.actor_pos.x, Extra.actor_pos.y, Extra.actor_pos.z + 15);
		auto e_bottom_pos = Vector3(Extra.actor_pos.x, Extra.actor_pos.y, Extra.actor_pos.z - 40);

		auto BottomBox = ProjectWorldToScreen(e_bottom_pos);
		auto TopBox = ProjectWorldToScreen(Vector3(e_top_pos.x, e_top_pos.y, e_top_pos.z + 15));

		auto entity_distance = local_pos.Distance(e_bottom_pos);


		auto CornerHeight = abs(TopBox.y - BottomBox.y);
		auto CornerWidth = CornerHeight * 0.65;

		auto ESP_Color = CFG.VisibleColor;

		// Extra ESP
		if (CFG.a_Visual)
		{
			if (entity_distance < CFG.a_max_distance)
			{
				if (CFG.a_Box)
				{
					if (CFG.BoxType == 0)
					{
						DrawBox(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, ESP_Color);
					}
					else if (CFG.BoxType == 1)
					{
						DrawCorneredBox(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, ESP_Color, 1.5);
					}
				}
				if (CFG.a_Line)
				{

					if (CFG.LineType == 0)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), static_cast<float>(GameVars.ScreenHeight)), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM BOTTOM SCREEN
					}
					if (CFG.LineType == 1)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), 0.f), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM TOP SCREEN
					}
					if (CFG.LineType == 2)
					{
						DrawLine(ImVec2(static_cast<float>(GameVars.ScreenWidth / 2), static_cast<float>(GameVars.ScreenHeight / 2)), ImVec2(BottomBox.x, BottomBox.y), ESP_Color, 1.5f); //LINE FROM CROSSHAIR
					}
				}
				if (CFG.a_Nearby)
				{
					if (entity_distance < 500)
					{
						char dist[64];
						sprintf_s(dist, "Actor - Nearby: %.fm", entity_distance);
						DrawOutlinedText(Verdana, dist, ImVec2(960, 100), 34.0f, ImColor(255, 255, 255), true);
					}
				}
				if (CFG.a_Distance)
				{
					char dist[64];
					sprintf_s(dist, "[%.fm]", entity_distance);
					DrawOutlinedText(Verdana, dist, ImVec2(BottomBox.x, BottomBox.y + 10), 13.5f, ImColor(255, 255, 255), true);
				}
				if (CFG.a_Level)
				{
					// Required Level to Access Supply Crate
					auto actor_level = read<int32_t>(Extra.actor + 0xe58);
					if (!actor_level) continue;

					char level[64];
					sprintf_s(level, "Lvl: %i", actor_level);
					DrawOutlinedText(Verdana, level, ImVec2(BottomBox.x, BottomBox.y - 5), 14.0f, ImColor(255, 255, 255), true);
				}
				if (CFG.a_Name)
				{
					char id[64];
					sprintf_s(id, "ID: %i", Extra.actor_id);
					DrawOutlinedText(Verdana, id, ImVec2(TopBox.x, TopBox.y - 40), 14.0f, ImColor(255, 255, 255), true);
				}
			}
		}
	}
}


void InputHandler() {
	for (int i = 0; i < 5; i++) ImGui::GetIO().MouseDown[i] = false;
	int button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) button = 0;
	if (button != -1) ImGui::GetIO().MouseDown[button] = true;
}
void Render()
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
		CFG.b_MenuShow = !CFG.b_MenuShow;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	RenderVisual();
	ImGui::GetIO().MouseDrawCursor = CFG.b_MenuShow;

	if (CFG.b_MenuShow)
	{
		InputHandler();
		ImGui::SetNextWindowSize(ImVec2(850, 500)); // 450,426
		ImGui::PushFont(DefaultFont);
		ImGui::Begin("Fensky || Kyners#6470 ", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

		ImGui::Columns(2);
		ImGui::SetColumnOffset(1, 230);

		{
		// Left Side of the Menu 


		ImGui::Spacing;
		Button1("Visual", &CFG.tab_index, 0, ImVec2(230 - 15, 41), true);
		ImGui::Spacing;
		Button1("Aimbot", &CFG.tab_index, 1, ImVec2(230 - 15, 41), true);
		ImGui::Spacing;
		Button1("Debug", &CFG.tab_index, 2, ImVec2(230 - 15, 41), true);
		ImGui::Spacing;
		Button1("Player Visual", &CFG.tab_index, 3, ImVec2(230 - 15, 41), true);
		ImGui::Spacing;
		Button1("Extra Visual", &CFG.tab_index, 4, ImVec2(230 - 15, 41), true);
		ImGui::Spacing;
		}


		ImGui::NextColumn();
		{
			// Right Side of the Menu
			if (CFG.tab_index == 0)
			{
				ImGui::Checkbox("Enabled Visual", &CFG.d_Visual);
				if (CFG.d_Visual)
				{
					ImGui::Checkbox("Box", &CFG.d_Box);
					if (CFG.d_Box)
					{
						ImGui::Combo("Box Type", &CFG.BoxType, CFG.BoxTypes, 2);
					}
					ImGui::Checkbox("Bones", &CFG.g_Bones);
					ImGui::Checkbox("Skeleton", &CFG.d_Skeleton);
					ImGui::Checkbox("Line", &CFG.d_Line);
					if (CFG.d_Line)
					{
						ImGui::Combo("ESP Line Type", &CFG.LineType, CFG.LineTypes, 3);
					}
					ImGui::Checkbox("Name", &CFG.d_Name);
					ImGui::Checkbox("Tribename", &CFG.d_TribeName);
					ImGui::Checkbox("Nearby", &CFG.d_Nearby);
					ImGui::Checkbox("Distance", &CFG.d_Distance);
					ImGui::Checkbox("Level", &CFG.d_Level);
					ImGui::Checkbox("Health Bar", &CFG.d_Health_Bar);
					ImGui::Checkbox("Health Value", &CFG.d_Health_Value);

					ImGui::Checkbox("Torpor Bar", &CFG.d_Torpor_Bar);
					ImGui::Checkbox("Torpor Value", &CFG.d_Torpor_Value);

					ImGui::Checkbox("Taming Percent", &CFG.d_Taming_Percent);


					ImGui::SliderFloat("Max Distance", &CFG.d_max_distance, 1.0f, 10000.0f);

					if (ImGui::ColorEdit3("Visible Color", CFG.fl_VisibleColor, ImGuiColorEditFlags_NoDragDrop))
					{
						CFG.VisibleColor = ImColor(CFG.fl_VisibleColor[0], CFG.fl_VisibleColor[1], CFG.fl_VisibleColor[2]);
					}
					if (ImGui::ColorEdit3("Invisible Color", CFG.fl_InvisibleColor, ImGuiColorEditFlags_NoDragDrop))
					{
						CFG.InvisibleColor = ImColor(CFG.fl_InvisibleColor[0], CFG.fl_InvisibleColor[1], CFG.fl_InvisibleColor[2]);
					}
				}
			}


			if (CFG.tab_index == 1)
			{
				ImGui::Checkbox("Enabled Aimbot", &CFG.b_Aimbot);
				if (CFG.b_Aimbot)
				{
					ImGui::Checkbox("Show FOV", &CFG.b_AimbotFOV);
					if (CFG.b_AimbotFOV)
					{
						ImGui::SliderFloat("FOV Size", &CFG.AimbotFOV, 1.0f, 460.0f);
						if (ImGui::ColorEdit3("FOV Color", CFG.fl_FovColor, ImGuiColorEditFlags_NoDragDrop))
						{
							CFG.FovColor = ImColor(CFG.fl_FovColor[0], CFG.fl_FovColor[1], CFG.fl_FovColor[2]);
						}
					}
					ImGui::Combo("Aimbot Key", &CFG.AimKey, keyItems, IM_ARRAYSIZE(keyItems));
					ImGui::SliderFloat("Smoothing Value", &CFG.Smoothing, 1.0f, 10.0f);

				}
			}

			// Debug Info
			if (CFG.tab_index == 2)
			{
				ImGui::Text("ActorCount: %i", GameVars.actor_count);
				ImGui::Text("UWorld: 0x%p", GameVars.u_world);
				ImGui::Text("Base  : 0x%p", GameVars.dwProcess_Base);
				ImGui::Checkbox("Test Function", &CFG.g_Debug);
			}


			// Player ESP
			if (CFG.tab_index == 3)
			{
				ImGui::Checkbox("Enabled Visual", &CFG.p_Visual);
				if (CFG.p_Visual)
				{
					ImGui::Checkbox("Name", &CFG.p_Name);
					ImGui::Checkbox("TribeName", &CFG.p_TribeName);
					ImGui::Checkbox("Box", &CFG.p_Box);
					if (CFG.p_Box)
					{
						ImGui::Combo("Player Box Type", &CFG.BoxType, CFG.BoxTypes, 2);
					}
					ImGui::Checkbox("Skeleton", &CFG.p_Skeleton);
					ImGui::Checkbox("Nearby Players", &CFG.p_Nearby);
					ImGui::Checkbox("Distance", &CFG.p_Distance);
					ImGui::Checkbox("Level", &CFG.p_Level);
					ImGui::Checkbox("Health Bar", &CFG.p_Health_Bar);
					ImGui::Checkbox("Health Value", &CFG.p_Health_Value);
					ImGui::Checkbox("Torpor Bar", &CFG.p_Torpor_Bar);
					ImGui::Checkbox("Torpor Value", &CFG.p_Torpor_Value);
					ImGui::Checkbox("Weight", &CFG.p_Weight);
					ImGui::Checkbox("Debug", &CFG.p_Debug);


					ImGui::SliderFloat("Max Distance", &CFG.p_max_distance, 1.0f, 1100.0f);
				}
			}

			if (CFG.tab_index == 4)
			{
				ImGui::Checkbox("Enabled Visual", &CFG.a_Visual);
				if (CFG.a_Visual)
				{
					ImGui::Checkbox("Name", &CFG.a_Name);
					ImGui::Checkbox("Nearby Actors", &CFG.a_Nearby);
					ImGui::Checkbox("Distance", &CFG.a_Distance);
					ImGui::Checkbox("Level", &CFG.a_Level);

					ImGui::SliderFloat("Max Distance", &CFG.a_max_distance, 1.0f, 1100.0f);
				}
			}
		}

		ImGui::PopFont();
		ImGui::End();
	}
	ImGui::EndFrame();

	DirectX9Interface::pDevice->SetRenderState(D3DRS_ZENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

	DirectX9Interface::pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9Interface::pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		DirectX9Interface::pDevice->EndScene();
	}

	HRESULT result = DirectX9Interface::pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9Interface::pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

void MainLoop() {
	static RECT OldRect;
	ZeroMemory(&DirectX9Interface::Message, sizeof(MSG));

	while (DirectX9Interface::Message.message != WM_QUIT) {
		if (PeekMessage(&DirectX9Interface::Message, OverlayWindow::Hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&DirectX9Interface::Message);
			DispatchMessage(&DirectX9Interface::Message);
		}
		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == GameVars.gameHWND) {
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(OverlayWindow::Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(GameVars.gameHWND, &TempRect);
		ClientToScreen(GameVars.gameHWND, &TempPoint);

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameVars.gameHWND;

		POINT TempPoint2;
		GetCursorPos(&TempPoint2);
		io.MousePos.x = TempPoint2.x - TempPoint.x;
		io.MousePos.y = TempPoint2.y - TempPoint.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}

		if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			GameVars.ScreenWidth = TempRect.right;
			GameVars.ScreenHeight = TempRect.bottom;
			DirectX9Interface::pParams.BackBufferWidth = GameVars.ScreenWidth;
			DirectX9Interface::pParams.BackBufferHeight = GameVars.ScreenHeight;
			SetWindowPos(OverlayWindow::Hwnd, (HWND)0, TempPoint.x, TempPoint.y, GameVars.ScreenWidth, GameVars.ScreenHeight, SWP_NOREDRAW);
			DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		}
		Render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9Interface::pDevice != NULL) {
		DirectX9Interface::pDevice->EndScene();
		DirectX9Interface::pDevice->Release();
	}
	if (DirectX9Interface::Direct3D9 != NULL) {
		DirectX9Interface::Direct3D9->Release();
	}
	DestroyWindow(OverlayWindow::Hwnd);
	UnregisterClass(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);
}

bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::Direct3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = OverlayWindow::Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = GameVars.ScreenWidth;
	Params.BackBufferHeight = GameVars.ScreenHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(DirectX9Interface::Direct3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, OverlayWindow::Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9Interface::pDevice))) {
		DirectX9Interface::Direct3D9->Release();
		return false;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowTitleAlign = ImVec2(0.5, 0.5);
	style->WindowMinSize = ImVec2(650, 425);


	ImGui_ImplWin32_Init(OverlayWindow::Hwnd);
	ImGui_ImplDX9_Init(DirectX9Interface::pDevice);
	DirectX9Interface::Direct3D9->Release();
	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message) {
	case WM_DESTROY:
		if (DirectX9Interface::pDevice != NULL) {
			DirectX9Interface::pDevice->EndScene();
			DirectX9Interface::pDevice->Release();
		}
		if (DirectX9Interface::Direct3D9 != NULL) {
			DirectX9Interface::Direct3D9->Release();
		}
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (DirectX9Interface::pDevice != NULL && wParam != SIZE_MINIMIZED) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9Interface::pParams.BackBufferWidth = LOWORD(lParam);
			DirectX9Interface::pParams.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void SetupWindow() {
	OverlayWindow::WindowClass = {
		sizeof(WNDCLASSEX), 0, WinProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, OverlayWindow::Name, LoadIcon(nullptr, IDI_APPLICATION)
	};

	RegisterClassEx(&OverlayWindow::WindowClass);
	if (GameVars.gameHWND) {
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(GameVars.gameHWND, &TempRect);
		ClientToScreen(GameVars.gameHWND, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		GameVars.ScreenWidth = TempRect.right;
		GameVars.ScreenHeight = TempRect.bottom;
	}

	OverlayWindow::Hwnd = CreateWindowEx(NULL, OverlayWindow::Name, OverlayWindow::Name, WS_POPUP | WS_VISIBLE, GameVars.ScreenLeft, GameVars.ScreenTop, GameVars.ScreenWidth, GameVars.ScreenHeight, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(OverlayWindow::Hwnd, &DirectX9Interface::Margin);
	SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	ShowWindow(OverlayWindow::Hwnd, SW_SHOW);
	UpdateWindow(OverlayWindow::Hwnd);
}

bool CreateConsole = true;

int main()
{
	system("sc stop mhyprot2"); // RELOAD DRIVER JUST IN CASE
	system("CLS"); // CLEAR

	GameVars.dwProcessId = GetProcessId(GameVars.dwProcessName);
	if (!GameVars.dwProcessId)
	{
		printf("[!] process \"%s\ was not found\n", GameVars.dwProcessName);
	}
	if (!mhyprot::init())
	{
		printf("[!] failed to initialize vulnerable driver\n");
		return -1;
	}

	if (!mhyprot::driver_impl::driver_init(
		false, // print debug
		false // print seedmap
	))
	{
		printf("[!] failed to initialize driver properly\n");
		mhyprot::unload();
		return -1;
	}
	GameVars.dwProcess_Base = GetProcessBase(GameVars.dwProcessId);
	if (!GameVars.dwProcess_Base)
	{
		printf("[!] failed to get baseadress\n");
	}

	printf("[+] ProcessName: %s ID: (%d) base: %llx\n", GameVars.dwProcessName, GameVars.dwProcessId, GameVars.dwProcess_Base);

	/*
	// Dump all GNames
	std::ofstream myfile;
	myfile.open("Gnames.txt");

	for (int i = 0; i < 250000; i++)
	{
		if (getNameFromID(i) != "FAIL")
		{
			myfile << i << "  |  " << getNameFromID(i) << "\n";
		}
	}

	myfile.close();
	printf("Dumped GNames\n");
	*/

	CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(GameCache), nullptr, NULL, nullptr);
	CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(CallHacks), nullptr, NULL, nullptr);
	//CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(CallAimbot), nullptr, NULL, nullptr);

	if (CreateConsole == false)
		ShowWindow(GetConsoleWindow(), SW_HIDE);

	bool WindowFocus = false;
	while (WindowFocus == false)
	{
		DWORD ForegroundWindowProcessID;
		GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
		if (GetProcessId(GameVars.dwProcessName) == ForegroundWindowProcessID)
		{

			GameVars.gameHWND = GetForegroundWindow();

			RECT TempRect;
			GetWindowRect(GameVars.gameHWND, &TempRect);
			GameVars.ScreenWidth = TempRect.right - TempRect.left;
			GameVars.ScreenHeight = TempRect.bottom - TempRect.top;
			GameVars.ScreenLeft = TempRect.left;
			GameVars.ScreenRight = TempRect.right;
			GameVars.ScreenTop = TempRect.top;
			GameVars.ScreenBottom = TempRect.bottom;

			WindowFocus = true;
		}
	}

	OverlayWindow::Name = RandomString(10).c_str();
	SetupWindow();
	DirectXInit();

	ImGuiIO& io = ImGui::GetIO();
	DefaultFont = io.Fonts->AddFontDefault();
	Verdana = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 16.0f);
	io.Fonts->Build();

	SpecialFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\smallest_pixel-7.ttf", 7.0f);
	io.Fonts->Build();

	while (TRUE)
	{
		MainLoop();
	}

}
