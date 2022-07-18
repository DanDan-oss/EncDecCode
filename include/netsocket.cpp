#include "netsocket.h"
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

CNetSockt::CNetSockt()
{
	
}

CNetSockt::CNetSockt(int sockfd, struct sockaddr* addr)
{
	this->m_sockfd = sockfd;
	std::memcpy(&this->m_addrs, addr, sizeof(struct sockaddr_in));
}

CNetSockt::~CNetSockt()
{
	if (this->m_sockfd)
	{
		close(this->m_sockfd);
		this->m_sockfd = 0;
	}
	std::cout << " free func: CNetSockt::~CNetSockt()" << std::endl;
}

void CNetSockt::SetSockInfo(int sock_fd, struct sockaddr* addrs)
{
	if (sock_fd)
		this->m_sockfd = sock_fd;
	if (addrs)
		std::memcpy(&this->m_addrs, addrs, sizeof(struct sockaddr_in));
	return;
}

int CNetSockt::GetSockFd()
{
	return this->m_sockfd;
}

struct sockaddr CNetSockt::GetSockAddre()
{
	struct sockaddr sock_addr = { 0 };
	memcpy(&sock_addr, &this->m_addrs, sizeof(struct sockaddr));
	return sock_addr;
}


int CNetSockt::send_message(const void* buf, size_t len)
{
	int sockfd = this->GetSockFd();

	if (!buf)
		return 0;
	send(sockfd, buf, len, 0);
	return len;
}

int CNetSockt::recv_message(void* recv_buf, size_t len)
{
	return recv(this->m_sockfd, recv_buf, len, 0);
}


// CServerSocket ============================

CServerSocket::CServerSocket()
{
	
}

CServerSocket::CServerSocket(int backlog)
{
	this->m_maxnode = backlog;
	return;
}

CServerSocket::CServerSocket(unsigned short int family, unsigned int port, int backlog)
	:m_family(family), m_port(port), m_maxnode(backlog)
{
	return;
}

CServerSocket::~CServerSocket()
{
	std::cout << " free func: CServerSocket::~CServerSocket()" << std::endl;
}

bool CServerSocket::InitServerSocket(unsigned short int family, unsigned int port, int maxnode)
{
	int sockfd = 0;
	struct sockaddr_in locl_addr = { 0 };
	int optval = 0;

	this->m_family = family;
	this->m_port = port;
	this->m_maxnode = maxnode;
	sockfd = socket(family, SOCK_STREAM, 0);	// 创建套接字
	if (sockfd == -1)
	{
		std::cout << "Init socket fail! [" << __FILE__ << "]:FuncLine | " << __LINE__ << std::endl;
		return false;
	}
	do
	{
		// 初始化本地网络通信
		locl_addr.sin_family = family;
		locl_addr.sin_port = htons(port);
		locl_addr.sin_addr.s_addr = INADDR_ANY;
		// 设置端口复用
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
		// 绑定端口
		if (-1 == bind(sockfd, (const struct sockaddr*)&locl_addr, sizeof(struct sockaddr_in)))
		{
			std::cout << "bind port fail! [" << __FILE__ << "]:FuncLine | " << __LINE__ << std::endl;
			break;
		}
		if (-1 == listen(sockfd, this->m_maxnode))
		{
			std::cout << "listen port fail! [" << __FILE__ << "]:FuncLine | " << __LINE__ << std::endl;
			break;
		}
		this->SetSockInfo(sockfd, (struct sockaddr*)&locl_addr);
		std::cout << "InitServerSocket ojbk..." << std::endl;
		return true;
	} while (0);
	if (sockfd)
	{
		close(sockfd);
		sockfd = 0;
	}
	return false;

}

CNetSockt* CServerSocket::AcceptServer()
{
	int new_sockfd = 0;
	int sockfd = this->GetSockFd();
	socklen_t addrlen = sizeof(struct sockaddr);
	struct sockaddr addrs = { 0 };

	if (!sockfd)
		return nullptr;
	new_sockfd = accept(sockfd, &addrs, &addrlen);
	if(-1 == new_sockfd)
		return nullptr;
	return  new CClientSocket(new_sockfd, &addrs);
}


// CClientSocket ============================================

CClientSocket::CClientSocket(int sock_fd, struct sockaddr* addrs)
{
	this->SetSockInfo(sock_fd, addrs);
	return;
}

CClientSocket::CClientSocket(unsigned short int family, std::string& ip, unsigned int port)
{
	this->InitClientSocket(family, ip, port);
	return;
}

bool CClientSocket::InitClientSocket(unsigned short int family, std::string& ip, unsigned int port)
{
	int sock_fd = socket(family, SOCK_STREAM, 0);
	struct sockaddr_in addr = { 0 };

	if (-1 == sock_fd)
		return false;
	addr.sin_family = family;
	addr.sin_port = htons(port);
	inet_pton(family, ip.data(), &addr.sin_addr.s_addr);
	if (-1 == connect(sock_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr)))
		return false;
	this->SetSockInfo(sock_fd, (struct sockaddr*)&addr);
	return true;
}

