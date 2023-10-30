#pragma once
#include <MinHook.h>
#include <string>
#include <Windows.h>
#include <map>
#include <Psapi.h>

#include "logger.h"
#include "Patterns.h"

using DWORD64 = unsigned long long;

class Hook;

struct AddressInfo
{
	DWORD64 BeginAddr;
	DWORD64 EndAddr;
};

static std::map<std::string, Hook*> ms_Hooks;
static std::map<std::string, AddressInfo> ms_Addresses;

#define ADDR_KEY_GAME "<Game>"
#define ADDR_KEY_LOCALIZATION "Localization_FinalRelease.dll"

class Hook
{
public:
	static void Initialize()
	{
		Logger::Log(Info, "Initializing hooks...");
		MH_STATUS status = MH_Initialize();
		if (status != MH_OK)
		{
			Logger::Log(Error, "Initialization of MinHook failed");
			return;
		}

		Logger::Log(Info, "Search for addresses...");

		FindAddresses(ADDR_KEY_GAME);
		FindAddresses(ADDR_KEY_LOCALIZATION);

		for (auto& hook : ms_Hooks)
		{
			hook.second->Init();
		}
	}

	static Hook* Get(const std::string& name)
	{
		return ms_Hooks[name];
	}

	Hook(const std::string& name, std::string signature)
		: m_Name(name), m_Signature(std::move(signature)), m_FuncPtr(nullptr)
	{
		ms_Hooks[name] = this;
	}
	std::string GetName() const
	{
		return m_Name;
	}
	std::string GetSignature() const
	{
		return m_Signature;
	}
	void* GetFuncPtr() const
	{
		return m_FuncPtr;
	}

	bool IsValid() const
	{
		return m_FuncPtr != nullptr;
	}

	template <typename T>
	T Function() const
	{
		return reinterpret_cast<T>(m_FuncPtr);
	}

protected:
	bool m_IsInitialized = false;

	MH_STATUS AddDetour(void* detour, void* original) const
	{
		auto result = MH_CreateHook(m_FuncPtr, detour, reinterpret_cast<void**>(original));

		if (result != MH_OK)
		{
			Logger::Log(Error, "Failed to create hook");
			return result;
		}

		result = MH_EnableHook(m_FuncPtr);
		if (result != MH_OK)
		{
			Logger::Log(Error, "Failed to enable hook");
			return result;
		}

		return result;
	}

	void SetAddressKey(std::string k)
	{
		m_AddrKey = std::move(k);
	}

	virtual void OnInitialized()
	{
		// Do nothing
	}

private:
	std::string m_Name;
	std::string m_Signature;
	std::string m_AddrKey = ADDR_KEY_GAME;

	void* m_FuncPtr;

	void Init()
	{
		Logger::Log(Info, "Hooking " + m_Name + "...");

		auto result = FindPattern(m_Signature);
		if (result == 0)
		{
			Logger::Log(Error, "Failed to find pattern for " + m_Name);
			return;
		}

		m_FuncPtr = reinterpret_cast<void*>(result);
		m_IsInitialized = true;
		Logger::Log(Info, "Found address: " + std::to_string(reinterpret_cast<DWORD64>(m_FuncPtr)));

		OnInitialized();
	}


	DWORD64 FindPattern(const std::string& pattern)
	{
		if (ms_Addresses.find(m_AddrKey) == ms_Addresses.end())
		{
			Logger::Log(Error, "Failed to find address key " + m_AddrKey);
			return 0;
		}

		auto copy = pattern;
		for (size_t pos = copy.find("??"); pos != std::string::npos; pos = copy.find("??", pos + 1))
		{
			copy.replace(pos, 2, "?");
		}

		auto addrInfo = ms_Addresses[m_AddrKey];

		auto thePattern = addrInfo.BeginAddr == 0 && addrInfo.EndAddr == 0
			? hook::pattern(copy)
			: hook::pattern(addrInfo.BeginAddr, addrInfo.EndAddr, copy);
		if (!thePattern.size())
		{
			Logger::Log(Error, "Failed to find pattern " + pattern);
			return 0;
		}

		return uintptr_t(thePattern.get_first());
	}

	static void FindAddresses(std::string addrKey)
	{
		Logger::Log(Info, "Finding addresses for " + addrKey + "...");

		MODULEINFO moduleInfo;
		HMODULE hModule = NULL;

		if (addrKey == ADDR_KEY_GAME)
		{
			hModule = GetModuleHandle(NULL);
		}
		else if (addrKey == ADDR_KEY_LOCALIZATION)
		{
			hModule = GetModuleHandleA(addrKey.c_str());
		}

		if (hModule == NULL)
		{
			Logger::Log(Error, "Failed to find module " + addrKey);
			return;
		}

		GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo));
		auto baseAddr = reinterpret_cast<DWORD64>(moduleInfo.lpBaseOfDll);
		auto endAddr = baseAddr + moduleInfo.SizeOfImage;

		ms_Addresses.insert_or_assign(addrKey, AddressInfo{ baseAddr, endAddr });

		Logger::Log(Info, "Found addresses for " + addrKey + ": " + std::to_string(baseAddr) + " - " + std::to_string(endAddr));
	}
};
