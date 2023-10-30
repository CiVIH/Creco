#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <chrono>
#include <fstream>

enum LogLevel
{
	Info,
	Warning,
	Error
};

class Logger
{
public:
	static void Log(LogLevel level, const std::string& message)
	{
		std::string prefix;

		switch (level)
		{
		case Info:
			prefix = "[INF]";
			break;
		case Warning:
			prefix = "[WRN]";
			break;
		case Error:
			prefix = "[ERR]";
			break;
		}

		const std::string complete_message = GetTime() + " " + prefix + " " + message + "\n";

		PrintToConsole(complete_message);
		AppendToFile(complete_message);
	}

private:
	static std::string GetTime()
	{
		auto time = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(time);
		tm time_info;
		localtime_s(&time_info, &time_t);
		char time_string[20];
		strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", &time_info);
		return time_string;
	}

	static void PrintToConsole(const std::string& message)
	{
		std::cout << message;
	}

	static void AppendToFile(const std::string& message)
	{
		const auto file = std::filesystem::path("creco.log");
		if (!std::filesystem::exists(file))
		{
			auto file_stream = std::ofstream(file);
			file_stream << message;
		}
		else
		{
			auto file_stream = std::ofstream(file, std::ios::app);
			file_stream << message;
		}
	}
};
