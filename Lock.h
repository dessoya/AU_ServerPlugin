#pragma once

#include "windows.h"

class Lock {
	HANDLE mutex;
public:
	Lock() {
		mutex = CreateMutex(NULL, FALSE, NULL);
	}
	~Lock() {
		CloseHandle(mutex);
	}
	void lock() {
		auto dwWaitResult = WaitForSingleObject(mutex, INFINITE);
		if (WAIT_OBJECT_0 != dwWaitResult) {
			// error
		}
	}

	void unlock() {
		ReleaseMutex(mutex);
	}
	
};