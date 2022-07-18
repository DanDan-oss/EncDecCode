#include <fstream>
#include <json/Reader.h>
#include<fstream>
#include <sstream>
#include "serialize.h"
#include "ClientOP.h"

ClientOP::ClientOP(const char* confPath)
{
	Json::Value val;
	Json::Reader reader;
	std::ifstream file(confPath, std::ios::in);

	reader.parse(file, val);
	if (val["clientID"].isString())
		this->clientID = val["clientID"].asString();
	if (val["serverID"].isString())
		this->serverID = val["serverID"].asString();
	if (val["serverIP"].isString())
		this->serverIP = val["serverIP"].asString();
	if (val["shmkey"].isString())
		this->shmkey = val["shmkey"].asString();
	if (val["serverPort"].isInt())
		this->serverPort = val["serverPort"].asInt();
	if (val["maxnode"].isInt())
		this->maxnode = val["maxnode"].asInt();
	file.close();
	return;
}

void ClientOP::seckeyAgree()
{
	// ��ԿЭ��
	EncryRSA rsa;
	std::ifstream ifs("./public.pem");
	std::stringstream strbuf;
	std::string public_rsafile;
	std::string rsa_enc_hash;
	std::string sort_data;
	Codec* codec = nullptr; 
	RequestMSG_Info reqmsg_info = { 0 };

	 // 1:	// ׼�����ݲ����л�
	// TODO: ����RSA��Կ�ԣ�Ȼ�����ǩ��,֮��ǩ������˽Կ����
	rsa.RsaOpenKey("./public.pem", "private.pem");
	strbuf << ifs.rdbuf();
	public_rsafile = strbuf.str();
	rsa_enc_hash = rsa.RSASign(0, public_rsafile);

	// ���л�����
	reqmsg_info.cmdType = 1;
	reqmsg_info.clientId = this->clientID;
	reqmsg_info.serverId = this->serverID;
	reqmsg_info.sign = rsa_enc_hash;
	reqmsg_info.data = public_rsafile;

	//PEM_write_bio_RSAPublicKey
	
	codec = new Request(reqmsg_info);
	sort_data = codec->encodeMsg();
	
	if(codec)
		delete codec;

	std::string  aaa;
	//rsa.RsaPublicEncrypt(public_rsafile.c_str(), public_rsafile.size(), &aaa);
	bool aa = rsa.RSAVerify(public_rsafile, rsa_enc_hash);


	// ��������

	// �ȴ��������ظ�

	// �����������ظ�������



}

void __cdecl OutputDebugStringFA(const char* format, ...)
{
	va_list v1Args;
	char* strBuffer = (char*)GlobalAlloc(GPTR, 4009);

	va_start(v1Args, format);
	_vsnprintf(strBuffer, 4096 - 1, format, v1Args);
	va_end(v1Args);
	strcat(strBuffer, "\n");
	OutputDebugStringA(strBuffer);
	GlobalFree(strBuffer);
	return;
}

void __cdecl OutputDebugStringFW(const wchar_t* format, ...)
{
	va_list v1Args;
	wchar_t* strBuffer = (wchar_t*)GlobalAlloc(GPTR, 4009);

	va_start(v1Args, format);
	_vswprintf_c(strBuffer, 4096 - 1, format, v1Args);
	va_end(v1Args);
	wcscat(strBuffer, L"\n");
	OutputDebugStringW(strBuffer);
	GlobalFree(strBuffer);
	return;
}
