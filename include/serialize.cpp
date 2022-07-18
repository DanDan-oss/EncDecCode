#include "serialize.h"
#include <sys/types.h>

Codec::Codec()
{
}


Request::Request()
{
}

Request::Request(std::string encstr)
{
	this->initMessage(encstr);
	return;
}

Request::Request(int cmd, std::string clientID, std::string serverID, std::string sign, std::string data)
{
	this->initMessage(cmd, clientID, serverID, sign, data);
	return;
}

Request::Request(RequestMsg& requestMsg)
{
	this->initMessage(requestMsg.cmdtype(), requestMsg.clientid(), requestMsg.serverid(), requestMsg.sign(), requestMsg.data());
	return;
}

Request::Request(RequestMSG_Info& request_info)
{
	this->initMessage(request_info.cmdType, request_info.clientId, request_info.serverId, request_info.sign,
		request_info.data);
	return;
}
Request::~Request()
{

}

void Request::initMessage(std::string encstr)
{
	this->m_encstr = encstr;
	this->m_msg.ParseFromString(encstr);
	return;
}

void Request::initMessage(int cmd, std::string clientID, std::string serverID, std::string sign, std::string data)
{
	this->m_msg.set_cmdtype(cmd);
	this->m_msg.set_clientid(clientID);
	this->m_msg.set_serverid(serverID);
	this->m_msg.set_sign(sign);
	this->m_msg.set_data(data);
	this->m_msg.SerializeToString(&this->m_encstr);
	return;
}

std::string Request::encodeMsg()
{
	return this->m_encstr;
}

void* Request::decodeMsg()
{
	return (RequestMsg*)&this->m_msg;

}

bool Request::Reset_info(void* reMSG, int msgSize)
{
	RequestMsg* requestMSG = (RequestMsg*)reMSG;
	if (msgSize != sizeof(Request))
		return false;
	this->initMessage(requestMSG->cmdtype(), requestMSG->clientid(), requestMSG->serverid(),
		requestMSG->sign(), requestMSG->data());
	return true;
}

bool Request::ResetEncdata(std::string encstr)
{
	this->initMessage(encstr);
	return true;
}

bool Request::ResetDecdata(std::string decstr)
{
	this->m_msg.set_data(decstr);
	this->m_msg.SerializeToString(&this->m_encstr);
	return true;
}


Response::Response()
{
}
Response::Response(std::string encstr)
{
	this->initMessage(encstr);
	return;
}

Response::Response(int rev, std::string clientID, std::string serverID, int seckeyid, std::string data)
{
	this->initMessage(rev, clientID, serverID, seckeyid, data);
	return;
}

Response::Response(RespondMsg& respondMsg)
{
	this->initMessage(respondMsg.rv(), respondMsg.clientid(), respondMsg.serverid(), respondMsg.seckeyid(), respondMsg.data());
	return;
}

Response::Response(RespondMSG_Info& response_info)
{
	this->initMessage(response_info.rv, response_info.clientId, response_info.serverId, response_info.seckeyid,
		response_info.data);
	return;
}

Response::~Response()
{
}

void Response::initMessage(std::string encstr)
{
	this->m_encstr = encstr;
	this->m_msg.ParseFromString(encstr);
	return;
}

void Response::initMessage(int rev, std::string clientID, std::string serverID, int seckeyid, std::string data)
{
	this->m_msg.set_rv(rev);
	this->m_msg.set_clientid(clientID);
	this->m_msg.set_serverid(serverID);
	this->m_msg.set_seckeyid(seckeyid);
	this->m_msg.set_data(data);
	this->m_msg.SerializeToString(&this->m_encstr);
	return;
}

std::string Response::encodeMsg()
{
	return this->m_encstr;
}

void* Response::decodeMsg()
{
	return (Response*)&this->m_msg;
}

bool Response::Reset_info(void* reMSG, int msgSize)
{
	RespondMsg* responseMSG = (RespondMsg*)reMSG;
	if (msgSize != sizeof(RespondMsg))
		return false;
	std::string cli = responseMSG->clientid();
	this->initMessage(responseMSG->rv(), responseMSG->clientid(), responseMSG->serverid(),
		responseMSG->seckeyid(), responseMSG->data() );
	return true;
}

bool Response::ResetEncdata(std::string encstr)
{
	this->initMessage(encstr);
	return true;
}

bool Response::ResetDecdata(std::string decstr)
{
	this->m_msg.set_data(decstr);
	this->m_msg.SerializeToString(&this->m_encstr);
	return true;
}


// =====================================
// ¹¤³§Àà

RequestFactory::RequestFactory()
{

}
RequestFactory::RequestFactory(struct RequestMSG_Info& request_info)
{
	this->request_Info.clientId = request_info.clientId;
	this->request_Info.cmdType = request_info.cmdType;
	this->request_Info.data = request_info.data;
	this->request_Info.serverId = request_info.serverId;
	this->request_Info.sign = request_info.sign;
}
RequestFactory::~RequestFactory()
{

}

Codec* RequestFactory::CreatCodec()
{
	if(this->request_Info.cmdType)
		return (new Request(this->request_Info));
	return (new Request());
}

ResponseFactory::ResponseFactory()
{
}
ResponseFactory::ResponseFactory(RespondMSG_Info& response_info)
{
	this->m_respondMSG.clientId = response_info.clientId;
	this->m_respondMSG.data = response_info.data;
	this->m_respondMSG.rv = response_info.rv;
	this->m_respondMSG.seckeyid = response_info.seckeyid;
	this->m_respondMSG.serverId = response_info.serverId;
	
}
ResponseFactory::~ResponseFactory()
{

}

Codec* ResponseFactory::CreatCodec()
{
	if (this->m_respondMSG.rv)
		return (new Response(this->m_respondMSG));
	return (new Response());
}