// Definition of the Socket class

#ifndef Socket_class
#define Socket_class


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <poll.h>

#include "logger.h"

const int MAXCONNECTIONS = 5;
const int MAXRECV = 80;

class BaseSocket {
	public:
		BaseSocket();
  		virtual ~BaseSocket();

  		// Server initialization
  		bool create();
  		bool bind(const int port);
  		bool listen(int maxconnections = MAXCONNECTIONS) const;
  		bool accept(BaseSocket&) const;

  		// Client initialization
  		bool connect(const std::string host, const int port);

  		// Data Transimission
  		bool send(const std::string) const;
  		int recv(std::string&) const;
  		bool poll(int timeoutms) const;

  		void set_non_blocking(const bool);
		bool is_valid() const {return m_sock != -1;}
		int get_fd(){return m_sock;}
		void set_fd(int fd){m_sock = fd;}
 	private:
		int m_sock;
  		sockaddr_in m_addr;
};

class ClientSocket : private BaseSocket {
	public:
		ClientSocket(std::string host, int port);
		ClientSocket(int id);
		ClientSocket(){};
  		virtual ~ClientSocket(){};
  		void set_non_blocking(const bool);
  		bool poll(int timeoutms);
		int get_id();
		void set_id(int);

  		const ClientSocket& operator << (const std::string&) const;
  		const ClientSocket& operator >> (std::string&) const;
};

class Socket : public ClientSocket {};

class ServerSocket : private BaseSocket {
	public:
		ServerSocket(int port, int maxconnections = MAXCONNECTIONS);
		virtual ~ServerSocket(){};

		bool accept(Socket&);
		void set_non_blocking(const bool);
};

class SocketException {
	public:
  		SocketException(std::string s) : m_s(s) {
  			//Logger::Log(LOG_ERROR,s.c_str());
  		};
  		~SocketException(){};

		std::string description() {return m_s;}

	private:
		std::string m_s;
};

#endif