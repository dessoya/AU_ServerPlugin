#include <API/ARK/Ark.h>
#include "M_event_writer.h"
#include "M_config_reader.h"
#include "string.h"
#include "windows.h"
#include <string>
#include <vector>

EventWriter *eventWriter = 0;
EventWriter *eventStringWriter = 0;

bool M_event_writer_init() {

	std::string eventsFolder = config->getString("events");
	std::string stringsFolder = config->getString("strings");
	Log::GetLog()->info("events folder {}", eventsFolder.c_str());
	Log::GetLog()->info("strings folder {}", stringsFolder.c_str());


	eventStringWriter = new EventWriter(stringsFolder);
	eventWriter = new EventWriter(eventsFolder, eventStringWriter, stringsFolder);

	eventStringWriter->start();

	eventWriter->start();

	return true;
}

bool M_event_writer_done() {

	delete eventStringWriter;
	delete eventWriter;

	return true;
}


wchar_t *char2tchar(const char *s) {
	size_t size = strlen(s) + 1;
	wchar_t *wbuf = new wchar_t[size];
	size_t outSize;
	mbstowcs_s(&outSize, wbuf, size, s, size - 1);
	return wbuf;
}

char *tchar2char(const wchar_t *t) {
	auto len = wcslen(t) + 1;
	auto s = new char[len];
	size_t sz;
	wcstombs_s(&sz, s, len, t, len);
	return s;
}

void readFolder(std::string folder, std::vector<std::string> &folderList) {

	WIN32_FIND_DATA ffd;
	auto tb = char2tchar(folder.c_str());
	auto hFind = FindFirstFile(tb, &ffd);
	delete tb;

	if (INVALID_HANDLE_VALUE == hFind) {
		return;
	}

	do {
		if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			char *filename = tchar2char(ffd.cFileName);
			folderList.push_back(std::string(filename));
			delete filename;

		}
	} while (FindNextFile(hFind, &ffd) != 0);

}


void EventWriter::push_string_to_message(EventMessage *message, std::string string) {

	if (exitFlag) return;

	if (string.length() > 16) {
		auto iter = stringLibrary->find(string);
		if (iter == stringLibrary->end()) {

			EventMessage *stringMessage = new EventMessage;
			stringMessage->push_array(3);
			stringMessage->push_uint(m_system_add_string_to_library);
			stringMessage->push_string(string);
			stringMessage->push_uint(stringId);
			stringMessage->set_size();
			push(stringMessage);

			stringMessage = new EventMessage;
			stringMessage->push_array(3);
			stringMessage->push_uint(m_system_add_string_to_library);
			stringMessage->push_string(string);
			stringMessage->push_uint(stringId);
			stringMessage->set_size();

			eventStringWriter->push(stringMessage);

			stringLibrary->insert(std::pair<std::string, unsigned long long>(string, stringId));
			stringId++;


			iter = stringLibrary->find(string);
		}
		message->push_stringid(iter->second);
	}
	else {
		message->push_string(string);
	}
}

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
	auto eventWriter = (EventWriter *)lpParam;
	return eventWriter->thread();
}

EventWriter::EventWriter(std::string folder_, EventWriter *eventStringWriter_, std::string stringfolder_) {

	eventStringWriter = eventStringWriter_;
	folder = folder_;
	queue = new MessageQueue();
	queue_mutex = CreateMutex(NULL, FALSE, NULL);
	exitFlag = false;
	running = true;
	stringLibrary = new StringLibrary();
	stringId = 0;

	if (stringfolder_.length() > 0) {
		char messagebuffer[1024 * 16];
		char b[1024];
		std::vector<std::string> folderList;
		sprintf_s(b, "%s/*.eventlog", stringfolder_.c_str());
		readFolder(b, folderList);

		int index = 1;
		for (auto i = folderList.begin(); i != folderList.end(); i++) {
			auto file = *i;
			auto num = std::stoi(file.substr(0, file.length() - 9));
			if (num > index) index = num;
		}

		for (int i = 1; i <= index; i++) {
			sprintf_s(b, "%s/%d.eventlog", stringfolder_.c_str(), (int)i);

			int fd = 0;
			auto err = _sopen_s(&fd, b, _O_RDWR | _O_BINARY, _SH_DENYNO, _S_IREAD);
			if (err) {
				Log::GetLog()->info("can't open {}", b);
			}
			else {
				_lseeki64(fd, 0, SEEK_END);
				auto currentFilesize = _telli64(fd);
				_lseeki64(fd, 0, SEEK_SET);

				long long pos = 0;
				while (pos < currentFilesize) {
					int size = 0;
					int readed = _read(fd, &size, 4);
					pos += 4;
					if (readed < 4) {
						Log::GetLog()->info("can't read message header in file {} at pos {}", b, pos);
						break;
					}
					readed = _read(fd, messagebuffer, size - 4);
					pos += size - 4;
					if (readed < (size - 4)) {
						Log::GetLog()->info("can't read message body in file {} at pos {}", b, pos);
						break;
					}

					/*
					stringMessage->push_array(3); // 2 bytes
					stringMessage->push_uint(m_system_add_string_to_library); // 2 bytes
					stringMessage->push_string(string);
					stringMessage->push_uint(stringId);
					*/

					std::string string = "";
					unsigned long long stringId_ = 123456;

					unsigned char *message = (unsigned char *)&messagebuffer[4];
					auto string_type = *message;
					message++;
					int string_len = 0;
					switch (string_type) {
					case dt_string8:
						string_len = *message;
						message++;
						break;
					case dt_string16:
						string_len = *((unsigned short *)message);
						message += 2;
						break;
					}
					string = std::string((char *)message);
					message += string_len;

					auto id_type = *message;
					// Log::GetLog()->info("string id type {}", id_type);
					message++;
					switch (id_type) {
					case dt_uint8:
						stringId_ = *message;
						break;
					case dt_uint16:
						stringId_ = *((unsigned short *)message);
						break;
					}

					if (stringId <= stringId_) {
						stringId = stringId_ + 1;
					}

					stringLibrary->insert(std::pair<std::string, unsigned long long>(string, stringId_));
					// Log::GetLog()->info("read string const {} = {}", string.c_str(), stringId_);

				}

				_close(fd);
			}

		}
		Log::GetLog()->info("start with string id {}", stringId);
	}
}

void EventWriter::start() {
	hthread = CreateThread(NULL, 0, ThreadFunction, this, 0, NULL);
}

EventWriter::~EventWriter() {

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
	delete queue;
	delete stringLibrary;
}

void EventWriter::push(EventMessage *message) {

	if (exitFlag) {
		delete message;
		return;
	}

	auto dwWaitResult = WaitForSingleObject(queue_mutex, INFINITE);
	if (WAIT_OBJECT_0 != dwWaitResult) {
		Log::GetLog()->error("writerThread can't open .eventlog file");
	}
	queue->push(message);
	ReleaseMutex(queue_mutex);
}


DWORD EventWriter::thread() {

	int writeBufferSize = 1024 * 128;
	int messagePerPacket = 256;
	int fileSize = 1024 * 1024 * 128;
	int continueMessageCount = 64;

	Log::GetLog()->info("WriteThread started");

	// init buffers
	char b[1024];
	char *writeBuffer = new char[writeBufferSize];

	// read eventlogs files
	std::vector<std::string> folderList;
	sprintf_s(b, "%s/*.eventlog", folder.c_str());
	readFolder(b, folderList);

	// search latest
	int index = 1;
	for (auto i = folderList.begin(); i != folderList.end(); i++) {
		auto file = *i;
		auto num = std::stoi(file.substr(0, file.length() - 9));
		if (num > index) index = num;
	}

	// open or create latest
	sprintf_s(b, "%s/%d.eventlog", folder.c_str(), index);
	int fd = 0;
	auto err = _sopen_s(&fd, b, _O_RDWR | _O_CREAT | _O_APPEND | _O_BINARY, _SH_DENYNO, _S_IWRITE);
	if (err) {
		Log::GetLog()->error("writerThread can't open .eventlog file");
		goto error_exit;
	}

	// take file size
	_lseeki64(fd, 0, SEEK_END);
	auto currentFilesize = _telli64(fd);

	int queueSize = 1;
	while (true) {
		if (exitFlag && queueSize < 1) break;

		bool continueFlag = true;
		while (continueFlag) {
			continueFlag = false;

			// check for file overflow
			if (currentFilesize + writeBufferSize > fileSize) {

				// make next file
				_close(fd);
				index++;
				sprintf_s(b, "%s/%d.eventlog", folder.c_str(), index);
				auto err = _sopen_s(&fd, b, _O_RDWR | _O_CREAT | _O_BINARY, _SH_DENYNO, _S_IWRITE);
				if (err) {
					Log::GetLog()->error("writerThread can't open .eventlog file");
					goto error_exit;
				}

				currentFilesize = 0;
			}

			char *writeBufferPosition = writeBuffer;
			int packetSize = 0;
			int messagesTaken = 0;

			auto dwWaitResult = WaitForSingleObject(queue_mutex, INFINITE);
			if (WAIT_OBJECT_0 != dwWaitResult) {
				// OutputDebugStringA("queue_mutex WaitForSingleObject problem\n");
				Log::GetLog()->info("writerThread queue_mutex err0r");
				goto error_exit;
			}

			while (true) {

				if (queue->size() == 0) break;
				if (messagesTaken >= messagePerPacket) break;

				auto m = queue->front();
				if (packetSize + m->getSize() > writeBufferSize) break;

				packetSize += m->getSize();

				m->write(writeBufferPosition);
				writeBufferPosition += m->getSize();

				delete m;
				queue->pop();
				messagesTaken++;
			}
			queueSize = (int)queue->size();

			ReleaseMutex(queue_mutex);

			auto writed = _write(fd, writeBuffer, packetSize);
			if (writed < packetSize) {
				Log::GetLog()->info("writerThread write file error !!! writed < packetSize");
				goto error_exit;
			}
			currentFilesize += packetSize;

			// check for continue writing
			if (exitFlag) {
				continueFlag = true;
			}
			else {
				if (queueSize > continueMessageCount) {
					continueFlag = true;
				}
			}

			if (messagesTaken > 0) {
				// sprintf_s(b, "[*] writed %d messages", (int)messagesTaken);
				// Log::GetLog()->info(b);
			}
		}

		Sleep(250);
	}

error_exit:

	// cleanup
	if (writeBuffer) {
		delete writeBuffer;
	}
	if (fd) {
		_close(fd);
	}

	Log::GetLog()->info("WriteThread stopped");

	running = false;

	return 0;
}
