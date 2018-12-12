#pragma once

#include <queue>
#include <map>
#include <vector>

bool M_storage_init();
bool M_storage_done();

enum StorageCommand {
	_SCMD_AddFolder,
	_SCMD_DelFolder,
	_SCMD_DinoDie
};

class SCMD {
public:
	StorageCommand cmd;
	SCMD(StorageCommand cmd_) : cmd(cmd_) { }
	virtual void write2storage(void *db) = 0;
	virtual void write2memory() = 0;
};

class SCMD_AddFolder : public SCMD {
public:
	std::string folder;
	unsigned long long did;
	SCMD_AddFolder(unsigned long long did_, std::string name_) : SCMD(_SCMD_AddFolder), did(did_), folder(name_) { }
	virtual void write2storage(void *db);
	virtual void write2memory();
};

class SCMD_DelFolder : public SCMD {
public:
	std::string folder;
	unsigned long long did;
	SCMD_DelFolder(unsigned long long did_, std::string name_) : SCMD(_SCMD_DelFolder), did(did_), folder(name_) { }
	virtual void write2storage(void *db);
	virtual void write2memory();
};

class SCMD_DinoDie : public SCMD {
public:
	unsigned long long did;
	SCMD_DinoDie(unsigned long long did_) : SCMD(_SCMD_DinoDie), did(did_) { }
	virtual void write2storage(void *db);
	virtual void write2memory();
};


void storage_write(SCMD *cmd);
std::vector<std::string> *get_dino_folders(unsigned long long did);
