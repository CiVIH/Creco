#pragma once

#include "hook.h"
#include "bridge.h"

typedef bool(__fastcall* tLocalization_StringDictionary_HasKey)(void*, const char*, unsigned int, unsigned int);
tLocalization_StringDictionary_HasKey oLocalization_StringDictionary_HasKey;

bool Patched_Localization_StringDictionary_HasKey(void* a1, const char* a2, unsigned int a3, unsigned int a4)
{
	std::string key(a2);
	if (key.find("LOC_LOADING_INFO_") == 0)
	{
		std::string leaderType = key.substr(std::string("LOC_LOADING_INFO_").length());
		Logger::Log(Info, "LoadingScreen LeaderType found: " + leaderType);

		Bridge::Get()->SetLastFoundLeaderType(leaderType);
	}

	return oLocalization_StringDictionary_HasKey(a1, a2, a3, a4);
}

class Localization_StringDictionary_HasKeyHook : public Hook
{
public:
	Localization_StringDictionary_HasKeyHook()
		: Hook("Localization::StringDictionary::HasKey", "48 89 5C 24 18 56 57 41 57 48 83 EC 20")
	{
		SetAddressKey(ADDR_KEY_LOCALIZATION);
	}
	static Localization_StringDictionary_HasKeyHook* Get()
	{
		static Localization_StringDictionary_HasKeyHook* instance;
		if (!instance)
			instance = new Localization_StringDictionary_HasKeyHook();
		return instance;
	}
	void OnInitialized() override
	{
		Logger::Log(Info, "Patching Localization_StringDictionary_HasKey for hijacking...");
		AddDetour((void*)Patched_Localization_StringDictionary_HasKey, (void**)&oLocalization_StringDictionary_HasKey);
		Logger::Log(Info, "Localization_StringDictionary_HasKey patched!");
	}
};


typedef void(__fastcall* tPlaySound)(__int64, const char*);
tPlaySound oPlaySound;

void Patched_PlaySound(__int64 a1, const char* cSoundName)
{
	oPlaySound(a1, cSoundName);

	const std::string soundName(cSoundName);

	if (soundName == "Play_DawnOfMan_Speech")
	{
		Logger::Log(Info, "LoadingScreen DawnOfMan Speech was played! Hijacked LeaderType confirmed!");
		Bridge::Get()->ConfirmLoadingHijack();
	}
	else if (soundName == "Main_Menu_Panel_Expand_Top_Level")
	{
		Logger::Log(Info, "MainMenu shown! Resetting LeaderType...");
		Bridge::Get()->ResetLeader();
	}
}

class PlaySoundHook : public Hook
{
public:
	PlaySoundHook()
		: Hook("PlaySound", "48 83 EC 28 48 85 D2 74 37")
	{}
	static PlaySoundHook* Get()
	{
		static PlaySoundHook* instance;
		if (!instance)
			instance = new PlaySoundHook();
		return instance;
	}
	void OnInitialized() override
	{
		Logger::Log(Info, "Patching PlaySound for hijacking...");
		AddDetour((void*)Patched_PlaySound, (void**)&oPlaySound);
		Logger::Log(Info, "PlaySound patched!");
	}
};