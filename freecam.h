#pragma once
#include <atomic>
#include <cmath>
#include <thread>
#include <chrono>
#include "../overlay/utils/W2S.h"
#include "../overlay/imgui/imgui.h"
#include "../overlay/imgui/KeyBind.h"
#include "../rbx/globals/options.h"
#include "../rbx/globals/globals.h"

namespace FreecamRuntime
{
    inline std::atomic<bool> Enabled{ false };
    inline Vectors::Vector3 CameraPosition{ 0.f, 0.f, 0.f };
    inline Vectors::Vector3 CameraRotation{ 0.f, 0.f, 0.f };
    inline float CameraSpeed{ 50.f };
    inline std::mutex UpdateMutex;
}

inline bool IsFreecamKeyActive()
{
    if (!Options::Freecam::Enabled)
        return false;

    if (Options::Freecam::FreecamKey == 0)
        return true;

    static bool wasKeyPressed = false;
    const bool isKeyPressed = KeyBind::IsPressed(Options::Freecam::FreecamKey);

    if (Options::Freecam::ToggleType == 1)
    {
        if (isKeyPressed && !wasKeyPressed)
            Options::Freecam::Toggled = !Options::Freecam::Toggled;
        wasKeyPressed = isKeyPressed;
        return Options::Freecam::Toggled;
    }

    Options::Freecam::Toggled = false;
    return isKeyPressed;
}

inline void UpdateFreecamPosition()
{
    if (!Globals::Roblox::DataModel.address || !Globals::Roblox::VisualEngine)
        return;

    if (!IsFreecamKeyActive())
        return;

    FreecamRuntime::Enabled.store(true);

    try
    {
        const auto workspace = Globals::Roblox::DataModel.FindFirstChild("Workspace");
        if (!workspace.address)
            return;

        const auto camera = workspace.FindFirstChild("Camera");
        if (!camera.address)
            return;

        // Get keyboard input for movement
        float moveX = 0.f, moveY = 0.f, moveZ = 0.f;

        if (GetAsyncKeyState('W') & 0x8000)
            moveZ += FreecamRuntime::CameraSpeed;
        if (GetAsyncKeyState('S') & 0x8000)
            moveZ -= FreecamRuntime::CameraSpeed;
        if (GetAsyncKeyState('D') & 0x8000)
            moveX += FreecamRuntime::CameraSpeed;
        if (GetAsyncKeyState('A') & 0x8000)
            moveX -= FreecamRuntime::CameraSpeed;
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
            moveY += FreecamRuntime::CameraSpeed;
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
            moveY -= FreecamRuntime::CameraSpeed;

        // Update camera position
        Vectors::Vector3 currentPos = camera.Position();
        currentPos.x += moveX * 0.016f;
        currentPos.y += moveY * 0.016f;
        currentPos.z += moveZ * 0.016f;

        camera.SetPosition(currentPos);
        
        std::lock_guard<std::mutex> lock(FreecamRuntime::UpdateMutex);
        FreecamRuntime::CameraPosition = currentPos;
    }
    catch (...)
    {
        // Silently fail if camera update fails
    }
}

inline void RunFreecam(ImDrawList* drawList)
{
    if (!Options::Freecam::Enabled)
    {
        FreecamRuntime::Enabled.store(false);
        return;
    }

    UpdateFreecamPosition();

    if (Options::Freecam::ShowInfo && FreecamRuntime::Enabled.load())
    {
        std::lock_guard<std::mutex> lock(FreecamRuntime::UpdateMutex);
        const std::string info = "Freecam: " 
            + std::to_string(static_cast<int>(FreecamRuntime::CameraPosition.x)) + ", "
            + std::to_string(static_cast<int>(FreecamRuntime::CameraPosition.y)) + ", "
            + std::to_string(static_cast<int>(FreecamRuntime::CameraPosition.z));
        drawList->AddText(ImVec2(10.f, 50.f), IM_COL32(255, 255, 0, 255), info.c_str());
    }
}

inline void RunFreecamLoop()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        
        if (!Options::Freecam::Enabled)
            continue;

        UpdateFreecamPosition();
    }
}
