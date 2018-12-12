#include <iostream>
#include "M_storage.h"
#include "M_config_reader.h"
#include <API/ARK/Ark.h>

#include "hdr/sqlite_modern_cpp.h"
// #include "fmt/core.h"

using namespace sqlite;
using namespace std;

typedef map<unsigned long long, vector<string>> DinoFolder;
DinoFolder *dinoFolder = 0;

typedef void(*LogOuter)(string str);
typedef void(*ProcessTableVersion)(database *db);

class Storage {
public:
	database *db_;
	static LogOuter logOuter;
	bool inited;
	Storage(string dbfilepath, int mod);
	~Storage() {
		if (inited) {
			delete db_;
		}
	}

	void initSchema();

	void installTable(string table, int version, ProcessTableVersion *versions);
	int getTableVersion(string table);
	void dumpStructure();
	void dumpCustomFolders();
};

Storage::Storage(string dbfilepath, int mode) {
	inited = true;
	try {
		sqlite_config config;
		// config.flags = OpenFlags::READONLY; OpenFlags::READWRITE
		int f1 = mode | (int)OpenFlags::NOMUTEX;
		if (mode != (int)OpenFlags::READONLY) f1 = f1 | (int)OpenFlags::CREATE;
		config.flags = (OpenFlags)f1;
		config.encoding = Encoding::UTF8;
		db_ = new database(dbfilepath, config);

		if (mode != (int)OpenFlags::READONLY) {
			*db_ << "PRAGMA auto_vacuum = FULL;";
		}
	}
	catch (exception& e) {
		logOuter("error: Storage::Storage");
		logOuter(e.what());
		inited = false;
	}
}

void Storage::initSchema() {
	try {
		bool schemaExists = false;
		*db_ << R"sql(SELECT name FROM sqlite_master WHERE type = 'table' and name = 'schema';)sql" >> [&]() {
			schemaExists = true;
		};

		logOuter(fmt::format("schemaExists {}", schemaExists));

		if (!schemaExists) {
			*db_ << R"sql(create table schema (
				name text primary key not null,
				version integer not null
			);)sql";
		}

	}
	catch (exception& e) {
		logOuter("error: initSchema");
		logOuter(e.what());
		inited = false;
	}
}

LogOuter Storage::logOuter = 0;

int Storage::getTableVersion(string table) {
	int version = -1;
	try {
		*db_ << R"sql(SELECT version FROM schema WHERE name = ? ;)sql" << table >> [&](int version_) {
			version = version_;
		};
		if (version == -1) {
			*db_ << R"sql(insert into schema (name,version) values (?,?);)sql" << table << 0;
			version = 0;
		}
	}
	catch (exception& e) {
		logOuter("error: getTableVersion");
		logOuter(e.what());
		version = 0;
	}
	logOuter(fmt::format("getTableVersion {} {}", table, version));
	return version;
}

void Storage::installTable(string table, int version, ProcessTableVersion *versions) {
	auto currentVersion = getTableVersion(table);
	if (currentVersion < version) {
		while (currentVersion < version) {
			auto tableUpdater = versions[currentVersion];
			try {
				tableUpdater(db_);
			}
			catch (exception& e) {
				logOuter(e.what());
				logOuter(fmt::format("can't upgrade '{}' from {} to {}", table, currentVersion, version));
				return;
			}

			currentVersion++;
		}
		*db_ << R"sql(update schema set version = ? where name = ?;)sql" << version << table;
	}
}

void Storage::dumpStructure() {
	try {
		*db_ << R"sql(SELECT m.name AS table_name, 
  p.cid AS col_id,
  p.name AS col_name,
  p.type AS col_type
 FROM sqlite_master m
			LEFT OUTER JOIN pragma_table_info((m.name)) p
			ON m.name <> p.name
			WHERE m.type = 'table'
			ORDER BY table_name, col_id;)sql"
			>> [](string table_name, string col_id, string col_name, string col_type) {
			logOuter(fmt::format("a: {} {} {} {}", table_name, col_id, col_name, col_type));
		};
	}
	catch (exception& e) {
		logOuter("error: dumpStructure");
		logOuter(e.what());
	}
}

void Storage::dumpCustomFolders() {
	try {
		*db_ << R"sql(SELECT dino_id, folder from custom_folders order by dino_id;)sql"
			>> [](unsigned long long dino_id, string folder) {
			logOuter(fmt::format("dino_id {} folder '{}'", dino_id, folder));
			auto i = dinoFolder->find(dino_id);
			if (i == dinoFolder->end()) (*dinoFolder)[dino_id] = vector<string>();
			(*dinoFolder)[dino_id].push_back(folder);
		};
	}
	catch (exception& e) {
		logOuter("error: dumpCustomFolders");
		logOuter(e.what());
	}
}


ProcessTableVersion custom_folders_versions[] = {
	[](database *db) {
		*db << "create table custom_folders(dino_id integer not null,folder text not null);";
		*db << "CREATE INDEX idx_dino_id ON custom_folders(dino_id);";
	}
};

class StorageWriter {
public:
	HANDLE hthread;
	volatile bool exitFlag, running;
	string dbfilepath;

	queue<SCMD *> cmds;
	volatile HANDLE queue_mutex;

	StorageWriter(string dbfilepath_);
	~StorageWriter();
	DWORD thread();
	void push(SCMD *cmd);
};

DWORD WINAPI StorageWriterThreadFunction(LPVOID lpParam) {
	auto storageWriter = (StorageWriter *)lpParam;
	return storageWriter->thread();
}

DWORD StorageWriter::thread() {
	exitFlag = false;
	running = true;
	queue_mutex = CreateMutex(NULL, FALSE, NULL);

	Log::GetLog()->info("StorageWriter started");

	auto writeStorage = new Storage(dbfilepath, (int)OpenFlags::READWRITE);

	int queueSize = 1;
	while (true) {

		if (exitFlag && queueSize < 1) break;

		auto dwWaitResult = WaitForSingleObject(queue_mutex, INFINITE);
		if (WAIT_OBJECT_0 != dwWaitResult) {
			// OutputDebugStringA("queue_mutex WaitForSingleObject problem\n");
			Log::GetLog()->info("StorageWriter queue_mutex err0r");
			goto error_exit;
		}

		while (true) {

			if (cmds.size() == 0) break;

			auto cmd = cmds.front();

			cmd->write2storage(writeStorage);

			delete cmd;
			cmds.pop();
		}
		queueSize = (int)cmds.size();

		ReleaseMutex(queue_mutex);

		Sleep(250);
	}


error_exit:

	delete writeStorage;

	Log::GetLog()->info("StorageWriter stopped");

	running = false;

	return 0;
}

StorageWriter::StorageWriter(string dbfilepath_) : dbfilepath(dbfilepath_) {
	hthread = CreateThread(NULL, 0, StorageWriterThreadFunction, this, 0, NULL);
}

StorageWriter::~StorageWriter() {

	exitFlag = true;

	volatile auto ttgghf = running;

	int cnt = 0;
	while (ttgghf && cnt < 1000 * 5) {
		ttgghf = !!running;
		Sleep(10);
		cnt += 10;
		ttgghf = !!running;
	}

	CloseHandle(queue_mutex);

	while (!cmds.empty()) {
		auto m = cmds.front();
		delete m;
		cmds.pop();
	}
}

void StorageWriter::push(SCMD *cmd) {

	cmd->write2memory();

	if (exitFlag) {
		delete cmd;
		return;
	}

	auto dwWaitResult = WaitForSingleObject(queue_mutex, INFINITE);
	if (WAIT_OBJECT_0 != dwWaitResult) {
		Log::GetLog()->error("StorageWriter queue_mutex error");
	}

	cmds.push(cmd);
	ReleaseMutex(queue_mutex);
}


Storage *readStorage = 0;
StorageWriter *storageWriter = 0;

void storage_write(SCMD *cmd) {
	if (storageWriter) {
		storageWriter->push(cmd);
	}
}

void SCMD_AddFolder::write2storage(void *db) {
	auto s = (Storage *)db;
	auto d = s->db_;

	try {
		*d << "insert into custom_folders (dino_id,folder) values (?,?);" << did << folder;
	}
	catch (exception& e) {
		Storage::logOuter("error: SCMD_AddFolder::write2storage");
		Storage::logOuter(e.what());
	}
}

void SCMD_AddFolder::write2memory() {
	auto i = dinoFolder->find(did);
	if (i == dinoFolder->end()) (*dinoFolder)[did] = vector<string>();
	(*dinoFolder)[did].push_back(folder);
}

void SCMD_DelFolder::write2storage(void *db) {
	auto s = (Storage *)db;
	auto d = s->db_;

	try {
		*d << "delete from custom_folders where dino_id = ? and folder = ?;" << did << folder;
	}
	catch (exception& e) {
		Storage::logOuter("error: SCMD_DelFolder::write2storage");
		Storage::logOuter(e.what());
	}
}

void SCMD_DelFolder::write2memory() {
	auto i = dinoFolder->find(did);
	if (i == dinoFolder->end()) return;
	vector<string> &l = (*dinoFolder)[did];
	for (auto it = l.begin(); it != l.end(); it++) {
		auto f = *it;
		if (f == folder) {
			l.erase(it);
			return;
		}
	}	
}

void SCMD_DinoDie::write2storage(void *db) {
	auto s = (Storage *)db;
	auto d = s->db_;

	try {
		*d << "delete from custom_folders where dino_id = ?;" << did;
	}
	catch (exception& e) {
		Storage::logOuter("error: SCMD_DelFolder::write2storage");
		Storage::logOuter(e.what());
	}
}

void SCMD_DinoDie::write2memory() {
	auto i = dinoFolder->find(did);
	if (i == dinoFolder->end()) return;
	dinoFolder->erase(i);
}





std::vector<std::string> *get_dino_folders(unsigned long long did) {
	auto i = dinoFolder->find(did);
	if (i != dinoFolder->end()) return &i->second;
	return 0;
}

bool M_storage_init() {

	Storage::logOuter = [](string str) {
		Log::GetLog()->info(str);
	};

	auto dbfilepath = config->getString("storageFilepath");
	if (dbfilepath.length() < 1) {
		return false;
	}

	dinoFolder = new DinoFolder;

	// OpenFlags::READONLY; OpenFlags::READWRITE
	readStorage = new Storage(dbfilepath, (int)OpenFlags::READWRITE);
	if (!readStorage->inited) return false;

	readStorage->initSchema();
	readStorage->installTable("custom_folders", 1, custom_folders_versions);
	readStorage->dumpStructure();
	delete readStorage;

	readStorage = new Storage(dbfilepath, (int)OpenFlags::READONLY);
	readStorage->dumpCustomFolders();

	storageWriter = new StorageWriter(dbfilepath);

	return true;
}

bool M_storage_done() {

	if (readStorage) {
		auto s = readStorage;
		readStorage = 0;
		delete s;
	}

	if (storageWriter) {
		auto s = storageWriter;
		storageWriter = 0;
		delete s;
	}

	delete dinoFolder;

	return true;
}