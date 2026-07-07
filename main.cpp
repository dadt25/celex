#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <windows.h>
#pragma comment(lib, "winmm.lib")
#include "auth.hpp"
#include "skStr.h"
#include "Memory/MemoryManager.h"
#include "overlay/renderer.h"
#include "features/misc.h"
#include "features/hitboxexpander.h"
#include "features/fly.h"
#include "features/speed.h"
#include "features/fling.h"
#include "features/teleport.h"
#include "features/spectate.h"
#include "features/silentaim.h"
#include "features/freecam.h"
#include "features/noclip.h"
#include "rbx/Caches/playercache.h"
#include "rbx/Caches/playerobjectscache.h"
#include "rbx/Caches/TPHandler.h"
#include "rbx/globals/globals.h"
#include "rbx/OffsetLoader.h"

using namespace KeyAuth;

// Copy values from your KeyAuth dashboard (license-key-only flow).
static std::string name = skCrypt("Loveisharddd's Application").decrypt();
static std::string ownerid = skCrypt("01wbx6HH0l").decrypt();
static std::string version = skCrypt("1.0").decrypt();
static std::string url = skCrypt("https://keyauth.win/api/1.3/").decrypt();
static std::string path = skCrypt("").decrypt();

static api KeyAuthApp(name, ownerid, version, url, path);


bool IsGameRunning(const wchar_t* windowTitle)
{
    HWND hwnd = FindWindowW(NULL, windowTitle);
    return hwnd != NULL;
}

std::string GetExecutableDir()
{
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::filesystem::path exePath(path);
    return exePath.parent_path().string();
}

void log(const std::string& message, int type = 0)
{
    std::string prefix;
    switch (type)
    {
    case 0:
        prefix = "[*]";
        break;
    case 1:
        prefix = "[+]";
        break;
    case 2:
        prefix = "[-]";
        break;
    default:
        prefix = "";
        break;
    }
    std::ofstream logFile("celex.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << prefix << " " << message << std::endl;
        logFile.close();
        SetFileAttributesA("celex.log", FILE_ATTRIBUTE_HIDDEN);
    }
}

template<typename T>
std::string toHexString(T value, bool prefix = false, bool uppercase = false)
{
    std::stringstream stream;
    if (uppercase)
        stream << std::uppercase;

    if (prefix)
        stream << "0x";

    stream << std::hex << value;
    return stream.str();
}

int main()
{
    SetConsoleTitleA("finesse loader");
    timeBeginPeriod(1); // Set Windows sleep resolution to 1ms
    auto& data = KeyAuthApp.response; // keep "data.success" semantics (license-only requirement)

    std::cout << "[*] Connecting to KeyAuth..." << std::endl;
    KeyAuthApp.init();
    if (!data.success)
    {
        std::cout << "[-] " << data.message << std::endl;
        Sleep(3000);
        exit(0);
    }

    std::string license_key;
    bool key_from_file = false;
    std::ifstream infile("key.txt");
    if (infile.is_open())
    {
        std::getline(infile, license_key);
        infile.close();
    }

    if (!license_key.empty())
    {
        std::cout << "[*] Found saved License Key. Authenticating..." << std::endl;
        key_from_file = true;
        SetFileAttributesA("key.txt", FILE_ATTRIBUTE_HIDDEN);
    }
    else
    {
        // Prompt strictly for a License Key
        std::cout << "Enter License Key: ";
        std::getline(std::cin >> std::ws, license_key);
        if (license_key.empty())
        {
            std::cout << "[-] Empty license key." << std::endl;
            Sleep(5000);
            exit(0);
        }
    }
    
    // Authenticate license key
    KeyAuthApp.license(license_key);

    if (!data.success)
    {
        std::cout << "[-] " << data.message << std::endl;
        if (key_from_file) {
            std::cout << "[-] Invalid saved key. Please delete key.txt and restart." << std::endl;
        }
        Sleep(8000);
        exit(0);
    }
    
    if (!key_from_file) {
        std::ofstream outfile("key.txt");
        if (outfile.is_open()) {
            outfile << license_key;
            outfile.close();
            SetFileAttributesA("key.txt", FILE_ATTRIBUTE_HIDDEN);
        }
    }

    std::cout << "[+] " << data.message << std::endl;

    if (!KeyAuthApp.user_data.subscriptions.empty()) {
        std::string expiry_str = KeyAuthApp.user_data.subscriptions[0].expiry;
        try {
            long long expiry_ts = std::stoll(expiry_str);
            std::time_t now = std::time(nullptr);
            long long diff = expiry_ts - (long long)now;
            
            if (diff > 31536000) { // More than 1 year is considered Lifetime in most setups
                std::cout << "[+] Key Duration: Lifetime" << std::endl;
            } else if (diff > 0) {
                int days = diff / 86400;
                int hours = (diff % 86400) / 3600;
                int mins = (diff % 3600) / 60;
                std::cout << "[+] Key Duration: " << days << " Days " << hours << " Hours " << mins << " Minutes" << std::endl;
            } else {
                std::cout << "[+] Key Duration: Expired" << std::endl;
            }
        } catch (...) {
            // Fallback if parsing fails
            std::cout << "[+] Key Duration: Lifetime" << std::endl;
        }
    }

    Sleep(1000); // Give user time to read the key duration

    // Print gg/finesse.lol 104 times sideways
    for (int i = 0; i < 104; ++i) {
        std::cout << "gg/finesse.lol ";
        std::cout.flush();
        Sleep(25);
    }
    std::cout << std::endl;
    Sleep(1000); // Pause so the user can see the text

    std::cout << "[*] Fetching latest Roblox offsets..." << std::endl;
    if (OffsetLoader::LoadLatest())
    {
        const char* source = OffsetLoader::UsedNetworkFetch() ? "imtheo.lol" : "local cache";
        std::cout << "[+] Offsets loaded from " << source
            << " (" << OffsetLoader::GetLoadedVersion() << ")" << std::endl;
        log(std::string("Offsets loaded from ") + source + " (" + OffsetLoader::GetLoadedVersion() + ")", 1);
    }
    else
    {
        std::cout << "[-] Offset auto-update failed: " << OffsetLoader::GetLastError()
            << " (using built-in defaults)" << std::endl;
        log(std::string("Offset auto-update failed: ") + OffsetLoader::GetLastError(), 2);
    }

    // Completely close the console window right after successful KeyAuth license.
    // (ImGui/overlay and background threads will keep running.)
    FreeConsole();


    // Start ImGui immediately after successful auth (before waiting on Roblox).
    std::thread(ShowImgui).detach();

    if (!IsGameRunning(L"Roblox"))
    {
        log("Roblox not found!", 2);
        log("Waiting for Roblox...", 0);
        while (!IsGameRunning(L"Roblox"))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    log("Roblox found!", 1);

    log("Attaching to Roblox...", 0);
    if (!Memory->attachToProcess("RobloxPlayerBeta.exe"))
    {
        log("Failed to attach to Roblox!", 2);
        log("Press any key to exit...", 0);
        std::cin.get();
        return -1;
    }

    log("Succesfully attached!", 1);

    if (Memory->getProcessId("RobloxPlayerBeta.exe") == 0)
    {
        log("Failed to get Roblox's PID!", 2);
        log("Press any key to exit...", 0);
        std::cin.get();
        return -1;
    }

    log(std::string("Roblox PID -> " + std::to_string(Memory->getProcessId())), 1);
    log(std::string("Roblox Base Address -> 0x" + toHexString(std::to_string(Memory->getBaseAddress()), false, true)), 1);

    Globals::executablePath = GetExecutableDir();
    Globals::configsPath = Globals::executablePath + "\\configs";
    std::filesystem::create_directories(Globals::configsPath);

    auto fakeDataModel = Memory->read<uintptr_t>(Memory->getBaseAddress() + Offsets::FakeDataModel::Pointer);
    auto dataModel = RobloxInstance(Memory->read<uintptr_t>(fakeDataModel + Offsets::FakeDataModel::RealDataModel));

    while (dataModel.Name() != "Ugc")
    {
        fakeDataModel = Memory->read<uintptr_t>(Memory->getBaseAddress() + Offsets::FakeDataModel::Pointer);
        dataModel = RobloxInstance(Memory->read<uintptr_t>(fakeDataModel + Offsets::FakeDataModel::RealDataModel));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    Globals::Roblox::DataModel = dataModel;

    auto visualEngine = Memory->read<uintptr_t>(Memory->getBaseAddress() + Offsets::VisualEngine::Pointer);

    while (visualEngine == 0)
    {
        visualEngine = Memory->read<uintptr_t>(Memory->getBaseAddress() + Offsets::VisualEngine::Pointer);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    Globals::Roblox::VisualEngine = visualEngine;

    Globals::Roblox::Workspace = Globals::Roblox::DataModel.FindFirstChildWhichIsA("Workspace");
    Globals::Roblox::Players = Globals::Roblox::DataModel.FindFirstChildWhichIsA("Players");
    Globals::Roblox::Camera = Globals::Roblox::Workspace.FindFirstChildWhichIsA("Camera");

    Globals::Roblox::LocalPlayer = RobloxInstance(Memory->read<uintptr_t>(Globals::Roblox::Players.address + Offsets::Player::LocalPlayer));

    Globals::Roblox::lastPlaceID = Memory->read<int>(Globals::Roblox::DataModel.address + Offsets::DataModel::PlaceId);;

    log(std::string("DataModel -> 0x" + toHexString(std::to_string(Globals::Roblox::DataModel.address), false, true)), 1);
    log(std::string("VisualEngine -> 0x" + toHexString(std::to_string(Globals::Roblox::VisualEngine), false, true)), 1);

    log(std::string("Workspace -> 0x" + toHexString(std::to_string(Globals::Roblox::Workspace.address), false, true)), 1);
    log(std::string("Players -> 0x" + toHexString(std::to_string(Globals::Roblox::Players.address), false, true)), 1);
    log(std::string("Camera -> 0x" + toHexString(std::to_string(Globals::Roblox::Camera.address), false, true)), 1);

    log(std::string("Logged in as " + Globals::Roblox::LocalPlayer.Name()), 1);
    
    log("Starting cheat...", 1);
    
    std::thread(CachePlayers).detach();
    std::thread(CachePlayerObjects).detach();
    std::thread(TPHandler).detach();
    std::thread(MiscLoop).detach();
    std::thread(RunHitboxExpander).detach();
    std::thread(FlyLoop).detach();
    std::thread(SpeedLoop).detach();
    std::thread(FlingLoop).detach();
    std::thread(RunTeleport).detach();
    std::thread(RunSpectate).detach();
    std::thread(SilentAimWorkerLoop).detach();
    std::thread(RunFreecamLoop).detach();

    // Wait for F1 key press to exit the cheat completely
    while (true)
    {
        if (GetAsyncKeyState(VK_F1) & 0x8000)
        {
            log("F1 Pressed. Killing cheat...", 1);
            exit(0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 1;
}
