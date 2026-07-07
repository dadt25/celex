#pragma once
#include "globals.h"
#include <algorithm>

namespace Options
{
	namespace Misc
	{
		inline bool Bypass = false;
		inline bool FOVEnabled = false;
		inline float FOV = 70.f;
		inline bool CacheNPCs = true;
		inline bool KeybindList = false;
		inline float KeybindListX = 20.0f;
		inline float KeybindListY = 80.0f;
		inline bool StreamProof = false;
		inline float MenuAccentColor[3] = {1.0f, 0.41f, 0.71f}; // Pink (255, 105, 180) converted to 0-1 range
		
		inline bool AutoKill = false;
		inline float AutoKillDistance = 50.0f;
		inline float AutoKillTickRate = 0.5f; // Wait between teleports/hits
		
		inline bool TargetSpecificPlayer = false;
		inline RobloxPlayer SelectedTarget = RobloxPlayer(0);
		
		inline bool AntiFling = false;
	}
	namespace HitboxExpander
	{
		inline bool Enabled = false;
		inline float HorizontalSize = 10.0f;
		inline float VerticalSize = 10.0f;
		inline bool ShowHitbox = false;
		inline float HitboxTransparency = 0.5f;
		inline bool WalkThrough = true;
		inline bool DownedCheck = false;
	}
	namespace ESP
	{
		inline bool TeamCheck = false;
		inline int BoxType = 0; // 0 = None, 1 = Normal Box, 2 = 3D Box
		inline bool Tracers = false;
		inline int TracersStart = 0;
		inline bool Skeleton = false;
		inline bool Name = false;
		inline bool Distance = false;
		inline bool Health = false;
		inline bool HeadCircle = false;
		inline bool CornerESP = false;
		inline bool Headless = false;
		inline bool Weapon = false;
		inline bool LookDirection = false;
		inline bool TargetName = false;
		inline bool Chams = false;
		inline int TracerStart = 0;

		inline float Color[3] = {1.0f, 1.0f, 1.0f};
		inline float BoxColor[3] = {1.0f, 1.0f, 1.0f};
		inline float CornerColor[3] = {1.0f, 1.0f, 1.0f};
		inline float SkeletonColor[3] = {1.0f, 1.0f, 1.0f};
		inline float DistanceColor[3] = {1.0f, 1.0f, 1.0f};
		inline float TracerColor[3] = {1.0f, 1.0f, 1.0f};
		inline float TracerThickness = 1.0f;
		inline float BoxThickness = 1.0f;
		inline float SkeletonThickness = 1.0f;
		inline float ESP3DThickness = 1.0f;
		inline bool RemoveBorders = false;
		inline float ESP3DColor[3] = {1.0f, 1.0f, 1.0f};
		inline float HeadCircleColor[3] = {1.0f, 1.0f, 1.0f};
		inline float HeadCircleThickness = 1.0f;
		inline float HeadCircleMaxScale = 2.5f;
		inline float ChamsColor[3] = {1.0f, 1.0f, 1.0f}; // White color
	}
	namespace Skybox
	{
		inline bool Enabled = false;
		inline int Type = 0; // 0 = Galaxy, 1 = Purple, 2 = Red, 3 = Blue
		inline bool ChangeTime = false;
		inline float TimeOfDay = 14.0f;
	}
	namespace Aimbot
	{
		inline int AimbotKey = 0;
		inline int AimingType = 0;

		inline int ToggleType = 0;

		inline bool Aimbot = false;
		inline bool TeamCheck = false;
		inline bool DownedCheck = false;
		inline bool StickyAim = false;
		inline float FOV = 100.f;
		inline float Smoothness = 0.f;
		inline int SmoothnessCurve = 0; // 0=Linear, 1=Ease In, 2=Ease Out, 3=Ease In-Out, 4=Custom
		inline bool CustomCurveEnabled = false;
		inline float CustomCurveP1[2] = {0.25f, 0.25f}; // First control point
		inline float CustomCurveP2[2] = {0.75f, 0.75f}; // Second control point
		inline bool ShowFOV = false;
		inline bool ShowFOVFill = false;
		inline float Range = 100.f;

		inline float FOVColor[3] = {1.0f, 1.0f, 1.0f}; // White color
		inline float FOVFillColor[4] = {1.0f, 1.0f, 1.0f, 0.1f}; // White with transparency
		inline float FOVThickness = 1.0f;
		
		inline int TargetBone = 0;
		inline int AirTargetBone = 0; // Air part selection
		
		inline bool Prediction = false;
		inline float PredictionX = 1.0f;
		inline float PredictionY = 1.0f;
		
		inline bool Shake = false;
		inline float ShakeIntensity = 1.0f;
		
		inline bool Stutter = false;
		inline int StutterTicks = 5;
		
		inline RobloxPlayer CurrentTarget;
		inline bool Toggled = false;
	}
	namespace Triggerbot
	{
		inline int TriggerbotKey = 0;
		inline int ToggleType = 0;

		inline bool Enabled = false;
		inline bool TeamCheck = false;
		inline bool DownedCheck = false;
		inline float Radius = 15.f;
		inline float Range = 100.f;
		inline int Delay = 50;
		
		inline bool AdvancedFOV = false;
		inline bool ShowAdvancedFOV = false;
		
		// Advanced FOV per body part (X = horizontal, Y = vertical)
		inline float HeadFOV_X = 0.0f;
		inline float HeadFOV_Y = 0.0f;
		inline float TorsoFOV_X = 0.0f;
		inline float TorsoFOV_Y = 0.0f;
		inline float UpperTorsoFOV_X = 0.0f;
		inline float UpperTorsoFOV_Y = 0.0f;
		inline float LowerTorsoFOV_X = 0.0f;
		inline float LowerTorsoFOV_Y = 0.0f;
		inline float LeftUpperArmFOV_X = 0.0f;
		inline float LeftUpperArmFOV_Y = 0.0f;
		inline float LeftLowerArmFOV_X = 0.0f;
		inline float LeftLowerArmFOV_Y = 0.0f;
		inline float LeftHandFOV_X = 0.0f;
		inline float LeftHandFOV_Y = 0.0f;
		inline float RightUpperArmFOV_X = 0.0f;
		inline float RightUpperArmFOV_Y = 0.0f;
		inline float RightLowerArmFOV_X = 0.0f;
		inline float RightLowerArmFOV_Y = 0.0f;
		inline float RightHandFOV_X = 0.0f;
		inline float RightHandFOV_Y = 0.0f;
		inline float LeftUpperLegFOV_X = 0.0f;
		inline float LeftUpperLegFOV_Y = 0.0f;
		inline float LeftLowerLegFOV_X = 0.0f;
		inline float LeftLowerLegFOV_Y = 0.0f;
		inline float LeftFootFOV_X = 0.0f;
		inline float LeftFootFOV_Y = 0.0f;
		inline float RightUpperLegFOV_X = 0.0f;
		inline float RightUpperLegFOV_Y = 0.0f;
		inline float RightLowerLegFOV_X = 0.0f;
		inline float RightLowerLegFOV_Y = 0.0f;
		inline float RightFootFOV_X = 0.0f;
		inline float RightFootFOV_Y = 0.0f;
		
		inline bool Toggled = false;
	}
	namespace Macro
	{
		inline int MacroKey = 0;
		inline int ToggleType = 0;

		inline bool Enabled = false;
		inline int Delay = 100;
		
		inline bool Toggled = false;
	}
	namespace Crosshair
	{
		inline bool Enabled = false;
		inline int Style = 0; // 0 = Static, 1 = Pulse
		inline float Size = 10.0f;
		inline float Gap = 5.0f;
		inline float Thickness = 2.0f;
		inline float SpinSpeed = 50.0f;
		inline float GapSpeed = 1.0f;
		inline bool GapTween = false;
		inline bool ShowText = true;
		inline float Color[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // White with full alpha
	}
	namespace Fly
	{
		inline int FlyKey = 0;
		inline int ToggleType = 0;
		
		inline bool Enabled = false;
		inline float Speed = 50.0f;
		
		inline bool Toggled = false;
	}
	namespace WalkSpeed
	{
		inline int WalkSpeedKey = 0;
		inline int ToggleType = 0;
		
		inline bool Enabled = false;
		inline float Speed = 16.0f;
		
		inline bool Toggled = false;
		inline bool VisualizeWalkSpeed = false;
		inline int Method = 0; // 0 = Velocity-based, 1 = Humanoid-based
	}
	namespace SilentAim
	{
		inline int SilentAimKey = 0;
		inline int ToggleType = 0; // 0 = Hold, 1 = Toggle

		inline bool Enabled = false;
		inline bool UseFOV = true;
		inline bool ShowFOV = false;
		inline float FOV = 100.f;
		inline float Range = 500.f;

		inline bool TeamCheck = false;
		inline bool DownedCheck = false;
		inline bool StickyAim = false;
		inline int TargetBone = 0;

		inline bool Prediction = true;
		inline float PredictionX = 7.5f;
		inline float PredictionY = 7.5f;

		inline bool DaHoodMode = true;
		inline float DaHoodPrediction = 0.135f;

		inline bool UseMouseService = true;
		inline bool UseFramePosition = false;
		inline bool CompatibilityAssist = false;
		inline bool DrawLine = false;
		inline bool ShowStats = false;

		inline float FOVColor[3] = { 1.0f, 0.5f, 0.0f };
		inline float FOVThickness = 1.0f;

		inline RobloxPlayer CurrentTarget;
		inline bool Toggled = false;
	}

	namespace Teleport {
		inline bool Enabled = false;
		inline int TeleportKey = 0;
		inline int ToggleType = 0;
		
		inline RobloxPlayer SelectedTarget = RobloxPlayer(0);
		inline float TargetX = 0.0f;
		inline float TargetY = 10.0f;
		inline float TargetZ = 0.0f;
		
		inline bool TriggerPlayerTP = false;
		inline bool TriggerCoordinateTP = false;
	}

	namespace Fling {
		inline bool Enabled = false;
		inline int FlingKey = 0;
		inline int ToggleType = 0;
		inline bool Toggled = false;
		inline float FlingSpeed = 99999.0f;
		
		inline RobloxPlayer SelectedTarget = RobloxPlayer(0);
		inline bool TargetFlingActive = false;
	}

	namespace Spectate {
		inline bool Enabled = false;
		inline bool IsSpectating = false;
		inline RobloxPlayer SelectedTarget = RobloxPlayer(0);
	}

	namespace Freecam {
		inline int FreecamKey = 0;
		inline int ToggleType = 0; // 0 = Hold, 1 = Toggle

		inline bool Enabled = false;
		inline float Speed = 50.0f;
		inline bool ShowInfo = true;

		inline bool Toggled = false;
	}

	namespace Noclip {
		inline int NoclipKey = 0;
		inline int ToggleType = 0; // 0 = Hold, 1 = Toggle

		inline bool Enabled = false;
		inline bool ShowStatus = true;

		inline bool Toggled = false;
	}

    namespace Menu {
        inline int MenuKey = VK_INSERT;
    }

    namespace Whitelist {
        inline std::vector<std::string> Players;
        inline bool IsWhitelisted(const std::string& name) {
            return std::find(Players.begin(), Players.end(), name) != Players.end();
        }
    }
}
