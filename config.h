#pragma once

#include <filesystem>
#include <toml++/toml.h>

class Config
{
public:
	static Config* Get()
	{
		static Config* instance;
		if (instance != nullptr)
		{
			return instance;
		}

		const auto file = std::filesystem::path("config.toml");

		if (!std::filesystem::exists(file))
		{
			instance = new Config();

			toml::table table = instance->Serialize();

			auto file_stream = std::ofstream(file);
			file_stream << table;
		}
		else
		{
			auto table = toml::parse_file(file.string());

			instance = new Config();
			instance->Deserialize(table);
		}

		return instance;
	}

	bool bDebug = false;
	int iPort = 45054;

private:
	toml::table Serialize()
	{
		return toml::table{
			{"debug", bDebug},
			{"port", iPort}
		};
	}

	void Deserialize(toml::table& table)
	{
		bDebug = table["debug"].value_or(bDebug);
		iPort = table["port"].value_or(iPort);
	}

};