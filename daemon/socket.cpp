// Implementation of the Socket class.

#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream> //Remove is cout >> LOGGER
#include "socket.h"

using namespace std;

/******** SOCKET CLASS **********/

BaseSocket::BaseSocket() : m_sock(-1) {
  memset(&m_addr, 0, sizeof(m_addr));
}

BaseSocket::~BaseSocket() {
	if (is_valid())
		::close (m_sock);
}

bool BaseSocket::create() {
	m_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (!is_valid())
		return false;

	// TIME_WAIT - argh
	int on = 1;
	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on)) == -1)
		return false;

	return true;
}

bool BaseSocket::bind(const int port) {
	if (!is_valid()) {
    	return false;
	}

	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_port = htons(port);

	int bind_return = ::bind (m_sock, (struct sockaddr *)&m_addr, sizeof(m_addr));

	if (bind_return == -1) {
    	return false;
	}

	return true;
}

bool BaseSocket::listen(int maxconnections) const {
	if (!is_valid()) {
    	return false;
 	}

	int listen_return = ::listen(m_sock, maxconnections);


	if (listen_return == -1) {
    	return false;
	}

	return true;
}

bool BaseSocket::accept(BaseSocket& new_socket) const {
	int addr_length = sizeof(m_addr);
	new_socket.m_sock = ::accept(m_sock, (sockaddr*)&m_addr, (socklen_t*)&addr_length);

	if (new_socket.m_sock <= 0)
    	return false;
	else
    	return true;
}

bool BaseSocket::send(const string s) const {
	int status = ::send(m_sock, s.c_str(), s.size(), MSG_NOSIGNAL);
	if (status == -1) {
      	return false;
	} else {
      	return true;
	}
}

int BaseSocket::recv(string& s) const {
	char buf[MAXRECV + 1];

	s = "";

	memset(buf, 0, MAXRECV + 1);

	int status = ::recv(m_sock, buf, MAXRECV, 0); //MSG_DONTWAIT);
	//int status = ::read(m_sock, buf, MAXRECV);

	if (status == -1) {
    	//cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
    	return 0;
	} else if (status == 0) {
      	return 0;
	} else {
    	s = buf;
    	return status;
	}
}

bool BaseSocket::poll(int timeoutms) const {
	if (!is_valid()) {
    	return (true); // next read will fail in try/ catch
	}

	struct pollfd fd;

	fd.fd = m_sock; // your socket handler 
	fd.events = POLLIN;
	return (::poll(&fd, 1, timeoutms) != 0);
}

bool BaseSocket::connect(const string host, const int port) {
	if (!is_valid()) return false;

	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);

	int status = inet_pton (AF_INET, host.c_str(), &m_addr.sin_addr);

	if (errno == EAFNOSUPPORT) return false;

	status = ::connect (m_sock, (sockaddr*)&m_addr, sizeof(m_addr));

	if (status == 0)
    	return true;
	else
    	return false;
}

void BaseSocket::set_non_blocking(const bool b) {
	int opts;
	
	opts = fcntl(m_sock, F_GETFL);

	if (opts < 0) {
		return;
	}

	if (b)
   		opts = (opts | O_NONBLOCK);
  	else
    	opts = (opts & ~O_NONBLOCK);

	fcntl(m_sock, F_SETFL,opts);
}

/******** CLIENTSOCKET CLASS **********/

ClientSocket::ClientSocket(std::string host, int port) {
	if (!BaseSocket::create()) {
    	throw SocketException("Could not create client socket.");
	}

	if (!BaseSocket::connect(host, port)) {
    	throw SocketException ("Could not bind to port.");
	}
}

const ClientSocket& ClientSocket::operator << (const std::string& s) const {
	if (!BaseSocket::send(s)) {
    	throw SocketException("Could not write to socket.");
	}

	return *this;
}

const ClientSocket& ClientSocket::operator >> (std::string& s) const {
	if (!BaseSocket::recv(s)) {
    	throw SocketException ( "Could not read from socket." );
	}

	return *this;
}

void ClientSocket::set_non_blocking(const bool b) {
	BaseSocket::set_non_blocking(b);
}

bool ClientSocket::poll(int timeoutms) {
	return (BaseSocket::poll(timeoutms));
}

int ClientSocket::get_id() {
	return (BaseSocket::get_fd());
}

void ClientSocket::set_id(int id) {
	BaseSocket::set_fd(id);
}

/******** SERVERSOCKET CLASS **********/

ServerSocket::ServerSocket(int port, int maxconnections) {
	if (!BaseSocket::create()) {
    	throw SocketException("Could not create server socket.");
    }

	if (!BaseSocket::bind(port)) {
    	throw SocketException("Could not bind to port.");
    }

	if (!BaseSocket::listen(maxconnections)) {
    	throw SocketException("Could not listen to socket.");
    }
}

void ServerSocket::set_non_blocking(const bool b) {
	BaseSocket::set_non_blocking(b);
}

bool ServerSocket::accept(Socket& sock) {
	return BaseSocket::accept((BaseSocket&)sock);
}
