#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <queue>
#include "SocketMSG.pb.h"
struct RequestMSG_Info
{
	//1 ��ԿЭ�� //2 ��ԿУ��; // 3 ��Կע��
	int cmdType; // ��������
	std::string clientId; // �ͻ��˱��
	std::string serverId; // �������˱��
	std::string sign;	// ǩ��
	std::string data;	// ���͵�����(��Կ��)
};

struct RespondMSG_Info
{
	int rv; // ����ֵ
	std::string clientId; 	// �ͻ��˱��
	std::string serverId; 	// ���������
	int seckeyid; 		// �Գ���Կ��� keysn
	std::string data; 	// �������˻ظ�����
};

typedef class Codec
{
public:
	Codec();
	virtual ~Codec() { };
	virtual std::string encodeMsg() = 0;			// ���л�
	virtual void* decodeMsg() = 0;				// �����л�
	virtual bool Reset_info(void* reMSG, int msgSize) = 0;	// �����趨����()
	virtual bool ResetDecdata(std::string decstr) = 0;
	virtual bool ResetEncdata(std::string encstr) = 0;			// �����趨����(���ܺ������)
private:

}CODEC, * PCODEC;


typedef class Request : public Codec
{
public:
	/* �ͻ�������
	int32 cmdType = 1;		// �������� (1 ��ԿЭ�� / 2 ��ԿУ�� / 3 ��Կע��)
	bytes clientId = 2;		// �ͻ��˱��
	bytes serverId = 3;		// ����˱��
	bytes sign = 4;
	bytes data = 5; */
	Request();
	Request(std::string encstr);		// ��ʼ��(�������ַ���)
	Request(int cmd, std::string clientID, std::string serverID, std::string sign, std::string data);
	Request(RequestMsg& requestMsg);
	Request(RequestMSG_Info& request_info);
	~Request();
		void initMessage(std::string encstr);		// ͨ���ַ�����ʼ����
	// ͨ�����л����ݳ�ʼ����
		void initMessage(int cmd, std::string clientID, std::string serverID, std::string sign, std::string data);

		virtual bool Reset_info(void* reMSG, int msgSize);		// ��̬,�����趨�ṹ
		virtual bool ResetEncdata(std::string encstr);			// ��̬,ͨ�������л���������
		virtual bool ResetDecdata(std::string decstr);			// ��̬,ͨ��δ���л�������

	// ���л��ͷ����л�
		virtual std::string encodeMsg();		// ��̬,���л�����
		virtual void* decodeMsg();				// ��̬,�����л�
private:
	std::string m_encstr;
	RequestMsg m_msg;
}REQUEST, * PREQUEST;

typedef class Response : public Codec
{
	/*
	bool rv = 1;
	bytes clientId = 2;
	bytes serverId = 3;
	int32 seckeyid = 4;
	bytes data = 5;
	*/
public:
	Response();
	Response(std::string encstr);		// �������ַ���
	Response(int rev, std::string clientID, std::string serverID, int seckeyid, std::string data);
	Response(RespondMsg& respondMsg);
	Response(RespondMSG_Info& response_info);
	~Response();
	void initMessage(std::string encstr);
	void initMessage(int rev, std::string clientID, std::string serverID, int seckeyid, std::string data);

	virtual bool Reset_info(void* reMSG, int msgSize);
	virtual bool ResetEncdata(std::string encstr);
	virtual bool ResetDecdata(std::string decstr);

	// ���л��ͷ����л�,��̬
	virtual std::string encodeMsg();
	virtual void* decodeMsg();

private:
	std::string m_encstr;
	RespondMsg m_msg;
}RESPONSE, * PRESPONSE;


// =====================================
// ������
// =====================================

// ���������
typedef class BaseFactory
{
public:
	BaseFactory() { }
	virtual ~BaseFactory() { }
	virtual Codec* CreatCodec() = 0;
}BASEFACTORY, * PBASEFACTORY;

// ���������ɿͻ������л���
class RequestFactory : public BaseFactory
{
public:
	RequestFactory();
	RequestFactory(struct RequestMSG_Info& request_info);
	~RequestFactory();
	Codec* CreatCodec();
private:
	RequestMSG_Info request_Info;
};

// ���������ɷ�������л���
class ResponseFactory : public BaseFactory
{
	ResponseFactory();
	ResponseFactory(RespondMSG_Info& response_info);
	~ResponseFactory();
	Codec* CreatCodec();
private:
	RespondMSG_Info m_respondMSG = { 0 };
};

#endif // !1

