#include "../CppSDK/SDK/Basic.cpp"
#include "../CppSDK/SDK/CoreUObject_functions.cpp"
#include "../CppSDK/SDK/Engine_functions.cpp"
#include "../CppSDK/SDK/Engine_structs.hpp"


// hi!! pretty useful project to test injections on ANY UE game, it's universal, you just need to dump the game using https://github.com/Encryqed/Dumper-7, and replace the CppSDK folder with the one of your game dump!
// it's using gengine instead of uworld for better compatibility!!
using namespace SDK;
#define DEBUG


#include "Render.h"
#include "Menu.h"
#include <Windows.h>
#include <cmath>
#include <cfloat>
#include <vector>

SDK::UWorld* world{ nullptr };
SDK::ULevel* persistentLevel{ nullptr };
SDK::APlayerController* localPlayerController{ nullptr };
SDK::UGameViewportClient* gameViewPortClient{ nullptr };

bool       bHooked{ false };
uintptr_t* vTable{ nullptr };
static uintptr_t vTableHooked[4096];
void (*PostRender)(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas);

void HookedPostRender(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas)
{
    if (!world && _this && _this->World)
    {
        world = _this->World;
    }
    
    if (Canvas && Menu::bMenu)
    {
        Menu::DrawMenu(Canvas);
    }
    PostRender(_this, Canvas);
}

bool         bRButtonDown{ false };
int          localTeam{ 0 };
SDK::FVector viewPoint;
static inline bool IsValidObject(const SDK::UObject* obj)
{
    if (obj) return true;

    return false;
}

void UnHook() { *(PVOID*)gameViewPortClient = vTable; }

void InitHook()
{
    bHooked = true;
    vTable = *(uintptr_t**)gameViewPortClient;
    memcpy(vTableHooked, vTable, sizeof(vTableHooked));
    PostRender = (decltype(PostRender))vTable[0x63];
    vTableHooked[0x63] = (uintptr_t)HookedPostRender;
    *(PVOID*)gameViewPortClient = vTableHooked;
    Font::Initialize();
}

static inline bool IsValidPtr(uintptr_t ptr)
{
    return ptr > 0x10000 && ptr < 0x7FFFFFFFFFFF;
}

DWORD mainmeow(HMODULE hModule)
{
    
    do
    {
        SDK::UEngine* engine = SDK::UEngine::GetEngine();
        if (!engine) {
#ifdef DEBUG
            MessageBoxA(NULL, "getengine returned null", "DEBUG", MB_OK);
            Sleep(100);
#endif
            continue;
        }
        
        char buf[128];
#ifdef DEBUG
        sprintf_s(buf, "UEngine found: %p", engine);
        MessageBoxA(NULL, buf, "DEBUG", MB_OK);
#endif
        
        if (!engine->GameViewport) {
#ifdef DEBUG
            MessageBoxA(NULL, "GameViewport is null", "DEBUG", MB_OK);
            Sleep(100);
#endif
            continue;
        }
#ifdef DEBUG
        sprintf_s(buf, "GameViewport found: %p", engine->GameViewport);
        MessageBoxA(NULL, buf, "DEBUG", MB_OK);
#endif
        if (!engine->GameViewport->World) {
#ifdef DEBUG
            MessageBoxA(NULL, "Viewport->World is null", "DEBUG", MB_OK);
            Sleep(100);
#endif
            continue;
        }
#ifdef DEBUG
        sprintf_s(buf, "World found via Viewport: %p", engine->GameViewport->World);
        MessageBoxA(NULL, buf, "DEBUG", MB_OK);
#endif
        gameViewPortClient = engine->GameViewport;
        world = engine->GameViewport->World;
#ifdef DEBUG
        MessageBoxA(NULL, "Calling InitHook", "DEBUG", MB_OK);
#endif
        InitHook();
#ifdef DEBUG
        MessageBoxA(NULL, "InitHook returned", "DEBUG", MB_OK);
#endif
        
    } while (!bHooked);
#ifdef DEBUG
    MessageBoxA(NULL, "Hook initialized", "DEBUG", MB_OK);
#endif
    while (bHooked)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
#ifdef DEBUG
            MessageBoxA(NULL, "END pressed - unloading", "DEBUG", MB_OK);
#endif
            UnHook();
            FreeLibraryAndExitThread(hModule, 0);
            break;
        }
        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            Menu::bMenu = !Menu::bMenu;
        }
        Sleep(5);
    }
    return 0;
}

DWORD WINAPI ShowMessageBox(LPVOID lpParam)
{
    HMODULE hModule = (HMODULE)lpParam;

    MessageBoxA(
        NULL,
        "press ok once the game is fully loaded",
        "linou universal canva rendering",
        MB_OK
    );

    DisableThreadLibraryCalls(hModule);

    CloseHandle(CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)mainmeow,
        hModule,
        0,
        NULL
    ));

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        HANDLE hThread = CreateThread(
            NULL,
            0,
            ShowMessageBox,
            hModule,
            0,
            NULL
        );

        if (hThread)
            CloseHandle(hThread);
    }

    return TRUE;
}
