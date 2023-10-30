#include "pch.h"

#include "config.h"
#include "logger.h"

#include "hook.h"
#include "hooks.h"

#define TARGET_PROCESS_NAME "CivilizationVI.exe"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        char aProcessName[MAX_PATH];
        GetModuleFileNameA(NULL, aProcessName, MAX_PATH);
        const std::string processPath = aProcessName;
        const std::string processName = processPath.substr(processPath.find_last_of("\\") + 1);

        if (processName != TARGET_PROCESS_NAME)
        {
        	return TRUE;
		}

        const auto config = Config::Get();

        if (config->bDebug)
        {
            AllocConsole();
            SetConsoleTitle(L"Creco by CiVIH");
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        }

        Logger::Log(Info, "Starting Creco...");

        Localization_StringDictionary_HasKeyHook::Get();
        PlaySoundHook::Get();

        Hook::Initialize();

        Logger::Log(Info, "Creco hooked and ready!");

        Bridge::Get()->Start();
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        Logger::Log(Info, "Stopping Creco...");
        Bridge::Get()->Stop();
        Logger::Log(Info, "Creco stopped!");
    }

    return TRUE;
}

