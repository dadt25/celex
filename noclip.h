#pragma once
#include <atomic>
#include <cmath>
#include "../overlay/utils/W2S.h"
#include "../overlay/imgui/imgui.h"
#include "../overlay/imgui/KeyBind.h"
#include "../rbx/globals/options.h"
#include "../rbx/globals/globals.h"

namespace NoclipRuntime
{
    inline std::atomic<bool> Enabled{ false };
    inline std::atomic<bool> NoclipActive{ false };
}

inline bool IsNoclipKeyActive()
{
    if (!Options::Noclip::Enabled)
        return false;

    if (Options::Noclip::NoclipKey == 0)
        return true;

    static bool wasKeyPressed = false;
    const bool isKeyPressed = KeyBind::IsPressed(Options::Noclip::NoclipKey);

    if (Options::Noclip::ToggleType == 1)
    {
        if (isKeyPressed && !wasKeyPressed)
            Options::Noclip::Toggled = !Options::Noclip::Toggled;
        wasKeyPressed = isKeyPressed;
        return Options::Noclip::Toggled;
    }

    Options::Noclip::Toggled = false;
    return isKeyPressed;
}

inline void DisableCharacterCollisions(const RobloxPlayer& player)
{
    if (!player.Character.address)
        return;

    try
    {
        // Find all parts in character and disable collision
        for (const auto& child : player.Character.GetChildren())
        {
            if (child.Class() != "BasePart")
                continue;

            // Set CanCollide to false
            const uintptr_t canCollideOffset = Offsets::BasePart::CanCollide;
            Memory->write<bool>(child.address + canCollideOffset, false);
        }
    }
    catch (...)
    {
        // Silently fail
    }
}

inline void EnableCharacterCollisions(const RobloxPlayer& player)
{
    if (!player.Character.address)
        return;

    try
    {
        // Find all parts in character and enable collision
        for (const auto& child : player.Character.GetChildren())
        {
            if (child.Class() != "BasePart")
                continue;

            // Set CanCollide to true
            const uintptr_t canCollideOffset = Offsets::BasePart::CanCollide;
            Memory->write<bool>(child.address + canCollideOffset, true);
        }
    }
    catch (...)
    {
        // Silently fail
    }
}

inline void RunNoclip(ImDrawList* drawList)
{
    if (!Options::Noclip::Enabled)
    {
        if (NoclipRuntime::NoclipActive.load())
        {
            // Disable noclip when option is turned off
            EnableCharacterCollisions(Globals::Roblox::LocalPlayer);
            NoclipRuntime::NoclipActive.store(false);
        }
        return;
    }

    if (!Globals::Roblox::DataModel.address || !Globals::Roblox::LocalPlayer.address)
        return;

    const bool shouldNoclip = IsNoclipKeyActive();

    if (shouldNoclip && !NoclipRuntime::NoclipActive.load())
    {
        // Enable noclip
        DisableCharacterCollisions(Globals::Roblox::LocalPlayer);
        NoclipRuntime::Enabled.store(true);
        NoclipRuntime::NoclipActive.store(true);
    }
    else if (!shouldNoclip && NoclipRuntime::NoclipActive.load())
    {
        // Disable noclip
        EnableCharacterCollisions(Globals::Roblox::LocalPlayer);
        NoclipRuntime::Enabled.store(false);
        NoclipRuntime::NoclipActive.store(false);
    }

    if (Options::Noclip::ShowStatus && NoclipRuntime::NoclipActive.load())
    {
        drawList->AddText(ImVec2(10.f, 70.f), IM_COL32(0, 255, 0, 255), "Noclip: ON");
    }
    else if (Options::Noclip::ShowStatus)
    {
        drawList->AddText(ImVec2(10.f, 70.f), IM_COL32(255, 0, 0, 255), "Noclip: OFF");
    }
}
