
#pragma once

#ifndef ARK_H

#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <iostream>
#include <dwmapi.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "singleton.h"
#include "vector.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")


inline namespace ARK
{
	class Variables : public Singleton<Variables>
	{
	public:
		const char* dwProcessName = "ShooterGame.exe";
		DWORD dwProcessId = NULL;
		uint64_t dwProcess_Base = NULL;
		HWND gameHWND = NULL;


		int test = NULL;

		int player_array_count = NULL;
		int actor_count = NULL;
		int ScreenHeight = NULL;
		int ScreenWidth = NULL;
		int ScreenLeft = NULL;
		int ScreenRight = NULL;
		int ScreenTop = NULL;
		int ScreenBottom = NULL;

		float ScreenCenterX = ScreenWidth / 2;
		float ScreenCenterY = ScreenHeight / 2;


		DWORD_PTR GNamesAddress = NULL;

		DWORD_PTR game_instance = NULL;
		DWORD_PTR gamestate = NULL;
		DWORD_PTR u_world = NULL;
		DWORD_PTR local_player_pawn = NULL;
		DWORD_PTR local_player_array = NULL;
		DWORD_PTR local_player = NULL;
		DWORD_PTR local_player_root = NULL;
		DWORD_PTR local_player_controller = NULL;
		DWORD_PTR local_player_state = NULL;
		DWORD_PTR persistent_level = NULL;
		DWORD_PTR actors = NULL;
		DWORD_PTR player_array = NULL;
		DWORD_PTR ranged_weapon_component = NULL;
		DWORD_PTR equipped_weapon_type = NULL;

		DWORD_PTR CameraManager = NULL;

		DWORD_PTR FreeCam = NULL;
	};
#define GameVars ARK::Variables::Get()

	class Offsets : public Singleton<Offsets>
	{
	public:

		// General ARK Offsets
		DWORD offset_u_world = 0x4999198; // IDA searching for Names: UWorldProxy GWorld
		DWORD offset_g_names = 0x496D2F8;
		DWORD offset_camera_manager = 0x4f0; // APlayerController->PlayerCameraManager
		DWORD offset_camera_cache = 0x4d0; //APlayerCameraManager->CameraCache
		DWORD offset_persistent_level = 0xf8; //UWorld->PersistentLevel
		DWORD offset_game_instance = 0x290; //UWorld->OwningGameInstance
		DWORD offset_gamestate = 0x128; //UWorld->GameState
		DWORD offset_local_players_array = 0x38; //UGameInstance->LocalPlayers
		DWORD offset_player_controller = 0x30; //UPlayer->PlayerController
		DWORD offset_apawn = 0x4d0;  //APlayerController->AcknowledgedPawn
		DWORD offset_instigator = 0x220; //AActor->Instigator
		DWORD offset_root_component = 0x250; //AActor->RootComponent
		DWORD offset_player_array = 0x4c0; //GameState->PlayerArray

		// Additional Information
		DWORD offset_npc_count = 0x540; // AShooterGameState->NumActiveNPC | 0x540 | Info read as: int32_t | Count for all Wild Dinos + Tamed Dinos
		DWORD offset_player_count = 0x54c;// AShooterGameState->NumPlayerConnected | 0x54c | Info read as: int32_t | Count for all Connected Players

		DWORD offset_actor_array = 0x88;
		DWORD offset_actor_count = 0x90;
		DWORD offset_actor_id = 0x18;
		DWORD offset_player_name = 0x470; // APlayerState->PlayerName 0x14b0	0x470
		DWORD offset_player_state = 0x488; // APawn->PlayerState
		DWORD offset_status_component = 0xcd0; // MyCharacterStatusComponent->UPrimalCharacterStatusComponent | 0xcd0
		DWORD offset_actor_mesh = 0x4f8; // ACharacter->Mesh
		DWORD offset_bone_array = 0x680; // UStaticMeshComponent->StaticMesh
		DWORD offset_component_to_world = 0xe0; // USceneComponent->ComponentToWorld
		DWORD offset_relative_location = 0x158; // USceneComponent->RelativeLocation
		DWORD offset_last_submit_time = 0x4d8; // Not really working for ARK (not optimized)
		DWORD offset_last_render_time = 0x4e0; // Not really working for ARK (not optimized)


		// Players ARK --- All Connected to Entity.actor and APrimalCharacter or AShooterCharacter

		DWORD offset_health = 0x92c; // APrimalCharacter->ReplicatedMaxHealth | 0x930 | float
		DWORD offset_max_health = 0x930; // APrimalCharacter->ReplicatedCurrentHealth | 0x92c | float
		DWORD offset_current_weight = 0x1bcc; // AShooterCharacter->ReplicatedWeight | 0x1bcc | float
		DWORD offset_playername = 0x14b0; // AShooterCharacter->PlayerName | 0x14b0 | FString
		DWORD offset_descriptivename = 0xbe8; // APrimalCharacter->DescriptiveName | 0xbe8 | FString
		DWORD offset_tribename = 0x790; // APrimalCharacter->TribeName | 0x790 | FString

		// Players ARK --- Weapon Connected requires more research



		// Players ARK --- All Connected to Entity.actor and UPrimalCharacterStatusComponent + The Offset

		DWORD offset_base_character_level = 0x6cc; //UPrimalCharacterStatusComponent->BaseCharacterLevel
		DWORD offset_extra_character_level = 0x6d0; //UPrimalCharacterStatusComponent->ExtraCharacterLevel

		

		// Structures ARK
		DWORD offset_structure_id = 0x8d0;
		DWORD offset_structure_owner_name = 0x8a0;
		DWORD offset_structure_name = 0x4e8;
		DWORD offset_structure_count = 0xeb0;
		DWORD offset_structure_count_max = 0xc08;



		// Dinos ARK
		DWORD offset_torpor = 0x934; // APrimalCharacter->ReplicatedCurrentTorpor | 0x934 | float
		DWORD offset_max_torpor = 0x938; // APrimalCharacter->ReplicatedMaxTorpor | 0x938 | float
		DWORD offset_current_tame_affinity = 0x1750; // APrimalDinoCharacter->CurrentTameAffinity | 0x1750 | float
		DWORD offset_required_tame_affinity = 0x173c; // APrimalDinoCharacter->RequiredTameAffinity | 0x173c | float

		DWORD offset_dino_name = 0xbe8; // APrimalCharacter->DescriptiveName | FString
		DWORD offset_tamer_name = 0x1270; // APrimalDinoCharacter->TamerString | FString
		DWORD offset_tamed_named = 0x1280; // APrimalDinoCharacter->TamedName | FString


		// Exploits Dino ---- Connected to LocalPlayer Pawn + Offset
		DWORD offset_riderrotation = 0x1698; // APrimalDinoCharacter->RiderRotationRateModifier | 0x1698 | float
		DWORD offset_walkingrotation = 0x1ed4; // APrimalDinoCharacter->WalkingRotationRateModifier | 0x1ed4 | float
		DWORD offset_swimmingrotation = 0x169c; // APrimalDinoCharacter->SwimmingRotationRateModifier | 0x169c | float
		DWORD offset_flyingrotation = 0x14ec; // APrimalDinoCharacter->FlyingForceRotationRateModifier | 0x14ec | float


	};

#define GameOffset ARK::Offsets::Get()
}
#endif  !ARK_H

