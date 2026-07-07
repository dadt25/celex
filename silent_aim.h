#pragma once
#include <atomic>
#include <cmath>
#include <thread>
#include <chrono>
#include <mutex>
#include "../overlay/utils/W2S.h"
#include "../overlay/imgui/imgui.h"
#include "../overlay/imgui/KeyBind.h"
#include "../rbx/globals/options.h"
#include "../rbx/globals/globals.h"
#include "aimbot.h"

namespace SilentAimRuntime
{
    inline std::atomic<bool> HasTarget{ false };
    inline std::atomic<bool> ShouldWrite{ false };
    inline Vectors::Vector2 ClientTargetPos{ -1.f, -1.f };
    inline Vectors::Vector2 ClientRealPos{ -1.f, -1.f };
    inline Vectors::Vector2 ScreenTargetPos{ -1.f, -1.f };
    inline RobloxPlayer CurrentTarget;
    inline std::mutex WriteMutex;
}

struct SilentAimWriteTargets
{
    uintptr_t mouseService = 0;
    uintptr_t inputObject = 0;
    uintptr_t playerMouse = 0;
};

inline constexpr int64_t DA_HOOD_PLACE_ID = 2788229376;

inline bool IsDaHoodPlace()
{
    if (Options::SilentAim::DaHoodMode)
        return true;

    return Globals::Roblox::lastPlaceID == DA_HOOD_PLACE_ID;
}

inline bool IsSilentAimKeyActive()
{
    if (!Options::SilentAim::Enabled)
        return false;

    if (Options::SilentAim::SilentAimKey == 0)
        return true;

    static bool wasKeyPressed = false;
    const bool isKeyPressed = KeyBind::IsPressed(Options::SilentAim::SilentAimKey);

    if (Options::SilentAim::ToggleType == 1)
    {
        if (isKeyPressed && !wasKeyPressed)
            Options::SilentAim::Toggled = !Options::SilentAim::Toggled;
        wasKeyPressed = isKeyPressed;
        return Options::SilentAim::Toggled;
    }

    Options::SilentAim::Toggled = false;
    return isKeyPressed;
}

inline bool IsFinite2D(const Vectors::Vector2& v)
{
    return std::isfinite(v.x) && std::isfinite(v.y);
}

inline bool IsReasonableClientPos(const Vectors::Vector2& v)
{
    return IsFinite2D(v) && std::abs(v.x) <= 10000.f && std::abs(v.y) <= 10000.f;
}

inline HWND GetRobloxWindowHandle()
{
    static HWND cached = nullptr;
    if (!cached || !IsWindow(cached))
        cached = FindWindowA(nullptr, "Roblox");
    return cached;
}

inline Vectors::Vector2 ScreenToClientPos(const Vectors::Vector2& screenPos)
{
    HWND window = GetRobloxWindowHandle();
    if (!window)
        return { -1.f, -1.f };

    POINT point{
        static_cast<LONG>(screenPos.x),
        static_cast<LONG>(screenPos.y)
    };

    if (!ScreenToClient(window, &point))
        return { -1.f, -1.f };

    return { static_cast<float>(point.x), static_cast<float>(point.y) };
}

inline Vectors::Vector2 GetClientCursorPos()
{
    POINT cursor{};
    if (!GetCursorPos(&cursor))
        return { -1.f, -1.f };

    return ScreenToClientPos({
        static_cast<float>(cursor.x),
        static_cast<float>(cursor.y)
        });
}

inline bool IsValidGamePointer(uintptr_t ptr)
{
    return ptr >= 0x100000 && ptr < 0x7FFFFFFFFFFF;
}

inline bool CanReadMousePosition(uintptr_t address)
{
    if (!IsValidGamePointer(address))
        return false;

    const Vectors::Vector2 current = Memory->read<Vectors::Vector2>(
        address + Offsets::MouseService::MousePosition);

    return IsReasonableClientPos(current);
}

inline RobloxInstance FindMouseServiceInstance()
{
    const auto dataModel = Globals::Roblox::DataModel;
    if (!dataModel.address)
        return RobloxInstance(0);

    RobloxInstance service = dataModel.FindFirstChildWhichIsA("MouseService");
    if (service.address)
        return service;

    for (const auto& child : dataModel.GetChildren())
    {
        if (child.Class() == "MouseService")
            return child;
    }

    return RobloxInstance(0);
}

inline SilentAimWriteTargets ResolveSilentAimTargets()
{
    SilentAimWriteTargets targets;

    const RobloxInstance mouseService = FindMouseServiceInstance();
    if (mouseService.address)
    {
        targets.mouseService = mouseService.address;

        const uintptr_t inputObject = Memory->read<uintptr_t>(
            mouseService.address + Offsets::MouseService::InputObject);

        if (CanReadMousePosition(inputObject))
            targets.inputObject = inputObject;
    }

    if (Globals::Roblox::LocalPlayer.address)
    {
        const uintptr_t playerMouse = Memory->read<uintptr_t>(
            Globals::Roblox::LocalPlayer.address + Offsets::Player::Mouse);

        if (CanReadMousePosition(playerMouse))
            targets.playerMouse = playerMouse;
    }

    return targets;
}

inline bool HasAnyWriteTarget(const SilentAimWriteTargets& targets)
{
    return targets.mouseService || targets.inputObject || targets.playerMouse;
}

inline void WriteMousePosToAddress(uintptr_t address, const Vectors::Vector2& clientPos)
{
    if (!CanReadMousePosition(address) || !IsReasonableClientPos(clientPos))
        return;

    try
    {
        Memory->write<Vectors::Vector2>(
            address + Offsets::MouseService::MousePosition,
            clientPos);
    }
    catch (...)
    {
        // Silently fail if write fails
    }
}

inline void WriteSilentMousePosition(const SilentAimWriteTargets& targets, const Vectors::Vector2& clientPos)
{
    if (targets.inputObject)
        WriteMousePosToAddress(targets.inputObject, clientPos);

    if (targets.playerMouse)
        WriteMousePosToAddress(targets.playerMouse, clientPos);
}

inline void ApplySilentShot(const SilentAimWriteTargets& targets,
    const Vectors::Vector2& targetClientPos,
    const Vectors::Vector2& realClientPos)
{
    if (!HasAnyWriteTarget(targets))
        return;

    if (!IsReasonableClientPos(targetClientPos) || !IsReasonableClientPos(realClientPos))
        return;

    WriteSilentMousePosition(targets, targetClientPos);
    for (int i = 0; i < 6; ++i)
        WriteSilentMousePosition(targets, targetClientPos);
}

inline bool IsDaHoodKnocked(const RobloxPlayer& player)
{
    if (!player.Character.address)
        return false;

    const auto bodyEffects = player.Character.FindFirstChild("BodyEffects");
    if (!bodyEffects.address)
        return false;

    const auto ko = bodyEffects.FindFirstChild("K.O");
    if (!ko.address)
        return false;

    return Memory->read<bool>(ko.address + Offsets::Misc::Value);
}

inline bool IsDaHoodGrabbed(const RobloxPlayer& player)
{
    if (!player.Character.address)
        return false;

    return player.Character.FindFirstChild("GRABBING_CONSTRAINT").address != 0;
}

inline Vectors::Vector3 GetSilentTargetPosition(const RobloxPlayer& player)
{
    RobloxInstance targetPart = player.Head.address ? player.Head : player.HumanoidRootPart;
    if (!targetPart.address)
        return {};

    Vectors::Vector3 basePos = targetPart.Position();
    if (!Options::SilentAim::Prediction)
        return basePos;

    const Vectors::Vector3 velocity = GetVelocity(player.HumanoidRootPart);

    if (IsDaHoodPlace())
    {
        const float prediction = Options::SilentAim::DaHoodPrediction;
        return Vectors::Vector3{
            basePos.x + velocity.x * prediction,
            basePos.y + velocity.y * prediction,
            basePos.z + velocity.z * prediction
        };
    }

    return Vectors::Vector3{
        basePos.x + velocity.x / Options::SilentAim::PredictionX,
        basePos.y + velocity.y / Options::SilentAim::PredictionY,
        basePos.z + velocity.z / Options::SilentAim::PredictionX
    };
}

inline bool IsSilentTargetValid(const RobloxPlayer& player, const RobloxInstance& localTeam)
{
    if (!player.address || !player.HumanoidRootPart.address)
        return false;

    if (player.address == Globals::Roblox::LocalPlayer.address)
        return false;

    if (Options::SilentAim::TeamCheck && player.Team.address == localTeam.address)
        return false;

    if (Options::Whitelist::IsWhitelisted(player.Name))
        return false;

    if (player.Health <= 0.0f)
        return false;

    if (Options::SilentAim::DownedCheck && player.Health <= 5.0f)
        return false;

    if (IsDaHoodPlace())
    {
        if (IsDaHoodKnocked(player))
            return false;

        if (IsDaHoodGrabbed(player))
            return false;
    }

    return true;
}

inline RobloxPlayer GetClosestSilentTarget(const POINT& cursorScreen)
{
    RobloxPlayer target;
    float bestDistance = FLT_MAX;
    const auto localTeam = Globals::Roblox::LocalPlayer.Team();
    const auto localCharacter = Globals::Roblox::LocalPlayer.Character();
    const auto localHRP = localCharacter.FindFirstChild("HumanoidRootPart");

    for (const auto& player : Globals::Caches::CachedPlayerObjects)
    {
        if (!IsSilentTargetValid(player, localTeam))
            continue;

        const auto targetPos = GetSilentTargetPosition(player);
        const auto targetPos2D = WorldToScreen(targetPos);

        if (targetPos2D.x < 0.f || targetPos2D.y < 0.f)
            continue;

        if (localHRP.address)
        {
            const Vectors::Vector3 diff = localHRP.Position() - targetPos;
            if (diff.Magnitude() > Options::SilentAim::Range)
                continue;
        }

        const float distance = targetPos2D.Distance({
            static_cast<float>(cursorScreen.x),
            static_cast<float>(cursorScreen.y)
            });

        if (Options::SilentAim::UseFOV && distance > Options::SilentAim::FOV)
            continue;

        if (distance < bestDistance)
        {
            bestDistance = distance;
            target = player;
        }
    }

    return target;
}

inline void SilentAimWorkerLoop()
{
    static SilentAimWriteTargets cachedTargets;
    static int refreshTimer = 0;

    while (true)
    {
        // FIXED: Added timeout to prevent infinite blocking
        std::this_thread::sleep_for(std::chrono::milliseconds(2));

        // FIXED: Check if write should happen before acquiring mutex
        if (!SilentAimRuntime::ShouldWrite.load())
        {
            continue;
        }

        // FIXED: Added mutex to prevent race conditions
        std::lock_guard<std::mutex> lock(SilentAimRuntime::WriteMutex);

        if (refreshTimer++ > 200 || !HasAnyWriteTarget(cachedTargets))
        {
            refreshTimer = 0;
            cachedTargets = ResolveSilentAimTargets();
        }

        if (!HasAnyWriteTarget(cachedTargets))
            continue;

        const Vectors::Vector2 targetPos = SilentAimRuntime::ClientTargetPos;
        const Vectors::Vector2 realPos = GetClientCursorPos();

        if (!IsReasonableClientPos(targetPos) || !IsReasonableClientPos(realPos))
            continue;

        ApplySilentShot(cachedTargets, targetPos, realPos);
    }
}

inline void RunSilentAim(ImDrawList* drawList)
{
    SilentAimRuntime::ShouldWrite.store(false);
    SilentAimRuntime::HasTarget.store(false);

    if (!Options::SilentAim::Enabled)
        return;

    if (!Globals::Roblox::DataModel.address || !Globals::Roblox::VisualEngine)
        return;

    POINT cursorScreen{};
    if (!GetCursorPos(&cursorScreen))
        return;

    const ImColor fovColor = IM_COL32(
        static_cast<int>(Options::SilentAim::FOVColor[0] * 255.f),
        static_cast<int>(Options::SilentAim::FOVColor[1] * 255.f),
        static_cast<int>(Options::SilentAim::FOVColor[2] * 255.f),
        255);

    if (Options::SilentAim::UseFOV && Options::SilentAim::ShowFOV)
    {
        drawList->AddCircle(
            ImVec2(static_cast<float>(cursorScreen.x), static_cast<float>(cursorScreen.y)),
            Options::SilentAim::FOV,
            fovColor,
            0,
            Options::SilentAim::FOVThickness);
    }

    if (!IsSilentAimKeyActive())
    {
        Options::SilentAim::CurrentTarget = RobloxPlayer(0);
        return;
    }

    if (Globals::Caches::CachedPlayerObjects.empty() || !GetRobloxWindowHandle())
        return;

    RobloxPlayer target;
    if (Options::SilentAim::StickyAim && Options::SilentAim::CurrentTarget.address)
    {
        bool foundValid = false;
        for (const auto& player : Globals::Caches::CachedPlayerObjects)
        {
            if (player.address != Options::SilentAim::CurrentTarget.address)
                continue;

            if (IsSilentTargetValid(player, Globals::Roblox::LocalPlayer.Team()))
            {
                target = player;
                foundValid = true;
            }
            break;
        }

        if (!foundValid)
            target = GetClosestSilentTarget(cursorScreen);
    }
    else
    {
        target = GetClosestSilentTarget(cursorScreen);
    }

    Options::SilentAim::CurrentTarget = target;
    if (!target.address)
        return;

    const auto targetPos2D = WorldToScreen(GetSilentTargetPosition(target));
    if (targetPos2D.x < 0.f || targetPos2D.y < 0.f)
        return;

    if (Options::SilentAim::UseFOV)
    {
        const float distance = targetPos2D.Distance({
            static_cast<float>(cursorScreen.x),
            static_cast<float>(cursorScreen.y)
            });

        if (distance > Options::SilentAim::FOV)
            return;
    }

    const Vectors::Vector2 clientTargetPos = ScreenToClientPos(targetPos2D);
    if (!IsReasonableClientPos(clientTargetPos))
        return;

    // FIXED: Use mutex when updating runtime data
    {
        std::lock_guard<std::mutex> lock(SilentAimRuntime::WriteMutex);
        SilentAimRuntime::CurrentTarget = target;
        SilentAimRuntime::ScreenTargetPos = targetPos2D;
        SilentAimRuntime::ClientTargetPos = clientTargetPos;
        SilentAimRuntime::ClientRealPos = GetClientCursorPos();
        SilentAimRuntime::HasTarget.store(true);
    }

    if (Options::SilentAim::DrawLine)
    {
        drawList->AddLine(
            ImVec2(static_cast<float>(cursorScreen.x), static_cast<float>(cursorScreen.y)),
            ImVec2(targetPos2D.x, targetPos2D.y),
            fovColor,
            1.5f);
    }

    if (Options::SilentAim::ShowStats)
    {
        const std::string stats = target.Name + " | "
            + std::to_string(static_cast<int>(target.Health)) + " HP";
        drawList->AddText(ImVec2(10.f, 30.f), IM_COL32(255, 255, 255, 255), stats.c_str());
    }

    const bool shooting = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    if (shooting)
        SilentAimRuntime::ShouldWrite.store(true);
}
