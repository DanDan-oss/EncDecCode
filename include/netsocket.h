#ifndef NETSOCKET
#define NETSOCKET	// ΪLinux���ɵ������׽��� ��,�޷���Win�ϼ���
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
	int m_sockfd = 0;		// �ļ�������
	struct sockaddr_in m_addrs = {0};	// ��·�ֽ����׽�������(ͨ��Э��, �˿�, IP��ַ)
};


class  CServerSocket : public CNetSockt
{
public:
	CServerSocket();
	CServerSocket(int backlog);
	CServerSocket(unsigned short int family, unsigned int port, int backlog);
	~CServerSocket();
	// ��ʼ������������׽���(ͨѶЭ��, �󶨶˿�, ���������)

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


