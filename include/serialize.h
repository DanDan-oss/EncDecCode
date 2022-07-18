#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <queue>
#include "SocketMSG.pb.h"
struct RequestMSG_Info
{
	//1 密钥协商 //2 密钥校验; // 3 密钥注销
	int cmdType; // 报文类型
	std::string clientId; // 客户端编号
	std::string serverId; // 服务器端编号
	std::string sign;	// 签名
	std::string data;	// 发送的数据(公钥，)
};

struct RespondMSG_Info
{
	int rv; // 返回值
	std::string clientId; 	// 客户端编号
	std::string serverId; 	// 服务器编号
	int seckeyid; 		// 对称密钥编号 keysn
	std::string data; 	// 服务器端回复数据
};

typedef class Codec
{
public:
	Codec();
	virtual ~Codec() { };
	virtual std::string encodeMsg() = 0;			// 序列化
	virtual void* decodeMsg() = 0;				// 反序列化
	virtual bool Reset_info(void* reMSG, int msgSize) = 0;	// 重新设定数据()
	virtual bool ResetDecdata(std::string decstr) = 0;
	virtual bool ResetEncdata(std::string encstr) = 0;			// 重新设定数据(加密后的数据)
private:

}CODEC, * PCODEC;


typedef class Request : public Codec
{
public:
	/* 客户端数据
	int32 cmdType = 1;		// 报文类型 (1 密钥协商 / 2 密钥校验 / 3 密钥注销)
	bytes clientId = 2;		// 客户端编号
	bytes serverId = 3;		// 服务端编号
	bytes sign = 4;
	bytes data = 5; */
	Request();
	Request(std::string encstr);		// 初始化(编码后的字符串)
	Request(int cmd, std::string clientID, std::string serverID, std::string sign, std::string data);
	Request(RequestMsg& requestMsg);
	Request(RequestMSG_Info& request_info);
	~Request();
		void initMessage(std::string encstr);		// 通过字符串初始化类
	// 通过序列化数据初始化类
		void initMessage(int cmd, std::string clientID, std::string serverID, std::string sign, std::string data);

		virtual bool Reset_info(void* reMSG, int msgSize);		// 多态,重新设定结构
		virtual bool ResetEncdata(std::string encstr);			// 多态,通过已序列化数据重新
		virtual bool ResetDecdata(std::string decstr);			// 多态,通过未序列化据重新

	// 序列化和反序列化
		virtual std::string encodeMsg();		// 多态,序列化数据
		virtual void* decodeMsg();				// 多态,反序列化
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
	Response(std::string encstr);		// 编码后的字符串
	Response(int rev, std::string clientID, std::string serverID, int seckeyid, std::string data);
	Response(RespondMsg& respondMsg);
	Response(RespondMSG_Info& response_info);
	~Response();
	void initMessage(std::string encstr);
	void initMessage(int rev, std::string clientID, std::string serverID, int seckeyid, std::string data);

	virtual bool Reset_info(void* reMSG, int msgSize);
	virtual bool ResetEncdata(std::string encstr);
	virtual bool ResetDecdata(std::string decstr);

	// 序列化和反序列化,多态
	virtual std::string encodeMsg();
	virtual void* decodeMsg();

private:
	std::string m_encstr;
	RespondMsg m_msg;
}RESPONSE, * PRESPONSE;


// =====================================
// 工厂类
// =====================================

// 工厂类基类
typedef class BaseFactory
{
public:
	BaseFactory() { }
	virtual ~BaseFactory() { }
	virtual Codec* CreatCodec() = 0;
}BASEFACTORY, * PBASEFACTORY;

// 工厂类生成客户端序列化类
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

// 工厂类生成服务端序列化类
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

