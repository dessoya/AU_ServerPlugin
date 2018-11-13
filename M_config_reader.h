#pragma once

class Config {
public:
	void *json;
	Config(void *json_ = 0);
	~Config();
	bool isReaded() {
		return json != 0;
	}
	std::string getString(const char *);
};

Config *ReadConfig();
extern Config *config;

bool M_config_reader_init();
bool M_config_reader_done();