#pragma once

#include <string>
#include <thread>

#include "httplib.h"

#include "logger.h"
#include "config.h"

static std::string ms_LastFoundLeaderType;
static bool ms_IsConfirmed;

class Bridge
{
public:
	static Bridge* Get()
	{
		static Bridge* instance;
		if (instance != nullptr)
		{
			return instance;
		}
		instance = new Bridge();
		return instance;
	}

	void Start()
	{
		Logger::Log(Info, "Starting bridge server...");

		std::string host = "127.0.0.1";
		int port = Config::Get()->iPort;

		m_ServerThread = std::thread([this, host, port]()
		{
			m_Server.listen(host, port);
		});
	}

	void Stop()
	{
		Logger::Log(Info, "Stopping bridge server...");

		m_Server.stop();
	}

	void SetLastFoundLeaderType(const std::string& type)
	{
		ms_LastFoundLeaderType = type;
		ms_IsConfirmed = false;
	}

	void ConfirmLoadingHijack()
	{
		ms_IsConfirmed = true;
	}

private:
	httplib::Server m_Server;
	std::thread m_ServerThread;

	Bridge()
	{
		m_Server.Get("/ping", [](const httplib::Request&, httplib::Response& res)
		{
			res.set_content("OK", "text/plain");
			res.status = 200;
		});

		m_Server.Get("/.creco/leader", [](const httplib::Request&, httplib::Response& res)
		{
			Logger::Log(Info, "Leader requested: " + ms_LastFoundLeaderType + " | " + std::to_string(ms_IsConfirmed));

			if (ms_LastFoundLeaderType.empty() || !ms_IsConfirmed)
			{
				res.set_content("", "text/plain");
				res.status = 204;
			}
			else
			{
				res.set_content(ms_LastFoundLeaderType, "text/plain");
				res.status = 200;
			}
		});
	}
};