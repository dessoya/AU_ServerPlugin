#include <API/ARK/Ark.h>
#include <fstream>
#include "M_config_reader.h"
#include "json.hpp"

Config *config = 0;

Config::Config(void *json_) : json(json_) {

}

Config::~Config() {
	auto json_ = (nlohmann::json *)json;
	delete json_;
}

std::string Config::getString(const char *key) {
	if (json == 0) return "";
	auto json_ = (nlohmann::json *)json;
	return (*json_)[key];
}


Config *ReadConfig() {
	
	Config *config = 0;
	nlohmann::json *json = 0;

	try {

		json = new nlohmann::json;

		const std::string config_path = ArkApi::Tools::GetCurrentDir() + "/../../ArkApi/Plugins/ExtendedEventLog/config.json";
		std::ifstream file{ config_path };
		if (!file.is_open())
			throw std::runtime_error("Can't open config.json");

		file >> *json;

		file.close();
		config = new Config(json);
	}
	catch (...) {
		if (json) {
			delete json;
		}
		config = new Config();
	}

	return config;
}

bool M_config_reader_init() {

	config = ReadConfig();
	if (!config->isReaded()) {
		Log::GetLog()->info("can't load config.json");
		delete config;
		return false;
	}

	return true;
}

bool M_config_reader_done() {

	delete config;
	return true;
}