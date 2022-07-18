#ifndef NETSOCKET
#define NETSOCKET	// 为Linux生成的网络套接字 类,无法在Win上兼容
#include <arpa/inet.h>
#include <iostream>


class CNetSockt
{
public:
	CNetSockt();
	CNetSockt(int sockfd, struct sockaddr* addr);
	virtual ~CNetSockt();
	void SetSockInfo(int sock_fd = 0, struct sockaddr* addrs = NULL);
	struct sockaddr GetSockAddre();
	int GetSockFd();

	int send_message(const void* buf, size_t len);
	int recv_message(void* recv_buf, size_t len);
private:
	int m_sockfd = 0;		// 文件描述符
	struct sockaddr_in m_addrs = {0};	// 网路字节序套接字描述(通信协议, 端口, IP地址)
};


class  CServerSocket : public CNetSockt
{
public:
	CServerSocket();
	CServerSocket(int backlog);
	CServerSocket(unsigned short int family, unsigned int port, int backlog);
	~CServerSocket();
	// 初始化网络服务器套接字(通讯协议, 绑定端口, 最大连接数)

	bool InitServerSocket(unsigned short int family, unsigned int port, int maxnode = 20);
	CNetSockt* AcceptServer();
protected:
	unsigned short int m_family = AF_INET;
	unsigned int m_port = 0;
	unsigned int m_maxnode = 0;
};

class CClientSocket : public CNetSockt
{
public:
	CClientSocket(int sock_fd, struct sockaddr* addrs);
	CClientSocket(unsigned short int family, std::string& ip, unsigned int port);
	bool InitClientSocket(unsigned short int family, std::string& ip,unsigned int port);
};
#endif // !NETSOCKET


