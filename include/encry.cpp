#include "encry.h"
#include <iostream>
#include<string.h>


//#ifdef __cplusplus
//extern "C" {
//#endif 
//#include<openssl/applink.c>
//#ifdef __cplusplus
//}
//#endif

static char* cipher__bin2tobase64(const char* in, int in_size, char* (&out), int out_szie)
{
	int buf_size = out_szie;
	BUF_MEM* memptr = nullptr;
	BIO* base64 = BIO_new(BIO_f_base64());  // 创建一个base64操作 BIO对象
	BIO* mem = BIO_new(BIO_s_mem());// 创建一个内存操作 BIO对象

	BIO_push(base64, mem);  // 组织bio链(逻辑关系: 先编码,再存放内存BIO)
	BIO_write(base64, in, in_size + 1);    // 编码
	BIO_flush(base64);
	BIO_get_mem_ptr(mem, &memptr);  // 获取bio内存对象中编码后数据的属性
	if (out != nullptr && out_szie < memptr->length)
	{// 缓冲区大小< 数据大小时
		BIO_free_all(base64);
		return nullptr;
	}
	if (out == nullptr)
	{
		out = new char[memptr->length + 2];
		memset(out + memptr->length, 0, 2);
		buf_size = memptr->length;
	}
	memcpy(out, memptr->data, buf_size);
	BIO_free_all(base64);
	return out;
}

static int cipher__base64tobin2(const char* in, int in_size, char* (&out), int out_szie)
{
	int outbuf_isNull = (out == nullptr ? true : false);
	int dec_size = in_size / 4 * 3;
	// 解码后的数据或者缓冲区的长度,谁短用谁
	BIO* base64 = nullptr;
	BIO* mem = nullptr;

	if (out != nullptr && dec_size > out_szie)
		return 0;
	if (out == nullptr)
	{   // 当接收存储数据的指针为null或者缓冲区大小<数据大小时
		out = new char[dec_size + 2];
		memset(out, 0, dec_size + 2);
	}
	base64 = BIO_new(BIO_f_base64());
	mem = BIO_new_mem_buf(in, in_size);
	BIO_push(base64, mem);
	dec_size = BIO_read(base64, out, dec_size);
	if (dec_size < 1)
	{   // 返回值为解码数据个数,判断解码是否成功
		if (!outbuf_isNull)
			delete[] out;
		BIO_free_all(base64);
		return 0;
	}
	BIO_flush(base64);
	BIO_free_all(base64);
	return dec_size;
}

CEncryAES::CEncryAES(const  char* userKey, const int bitSize, const   char* ivec)
{
	this->InitAesKey(userKey, bitSize, ivec);
	return;
}

bool CEncryAES::InitAesKey(const  char* userKey, const int bitSize, const  char* ivec)
{
	this->m_userKey = (char*)userKey;
	this->m_bitSize = bitSize;
	memcpy(&this->ivec, ivec, AES_BLOCK_SIZE);
	AES_set_encrypt_key((unsigned char*)userKey, bitSize, &this->m_enc_AESKey);
	AES_set_decrypt_key((unsigned char*)userKey, bitSize, &this->m_dec_AESKey);
	return true;
}



std::string  CEncryAES::AesCbcEncrypt(std::string enc_data)
{
	//unsigned int data_Len = wcslen((const wchar_t*)enc_data) * 2;
	unsigned int data_Len = enc_data.size();
	char* encData = nullptr, *encBioData = nullptr;
	unsigned char ivec[AES_BLOCK_SIZE] = { 0 };
	std::string encStrData;
	if ((data_Len % AES_BLOCK_SIZE) != 0)
		data_Len = (data_Len / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
	encData = new char[data_Len + 2];
	memset(encData, 0, data_Len + 2);
	memcpy(ivec, this->ivec, AES_BLOCK_SIZE);
	
	AES_cbc_encrypt((unsigned char*)enc_data.data(), (unsigned char*)encData, data_Len, &this->m_enc_AESKey,
		ivec, AES_ENCRYPT);

	cipher__bin2tobase64(encData, data_Len, encBioData, 0);
	encStrData = encBioData;
	delete[] encData;
	delete[] encBioData;
	return encStrData;
}

std::string  CEncryAES::AesCbcDecrypt(std::string dec_data)
{
	//unsigned int data_Len = wcslen((const wchar_t*)dec_data) * 2;
	unsigned int data_Len = 0;
	char* decData = nullptr, *decBioData = nullptr;
	unsigned char ivec[AES_BLOCK_SIZE] = { 0 };
	std::string encStrData;

	data_Len= cipher__base64tobin2(dec_data.data(), dec_data.size(), decBioData, 0);
	if ((data_Len % AES_BLOCK_SIZE) != 0)
		data_Len = (data_Len / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
	
	decData = new char[data_Len + 2];
	memset(decData, 0, data_Len + 2);
	memcpy(ivec, this->ivec, AES_BLOCK_SIZE);
	AES_cbc_encrypt((unsigned char*)decBioData, (unsigned char*)decData, data_Len, &this->m_dec_AESKey,
		ivec, AES_DECRYPT);

	encStrData = decData;
	delete[] decBioData;
	delete[] decData;
	return encStrData;
}


CEncryRSA::CEncryRSA()
{
	this->m_rsa = RSA_new();
}

CEncryRSA::CEncryRSA(int bits, const char* pubFilePath, const char* privFilePath)
{
	this->m_rsa = RSA_new();
	this->RsaGenerateRsaKey(bits, pubFilePath, privFilePath);
	return;
}

CEncryRSA::CEncryRSA(const char* pubFilePath, const char* privFilePath)
{
	this->m_rsa = RSA_new();
	this->RsaOpenKey(pubFilePath, privFilePath);
	return;
}

CEncryRSA::~CEncryRSA()
{
	if (this->m_rsa)
		RSA_free(this->m_rsa);
}

// 打开已存的的公私钥文件
bool CEncryRSA::RsaOpenKey(const char* pubFilePath, const char* privFilePath)
{
	BIO* publie_File = nullptr;
	BIO* private_File = nullptr;
	bool status = false;
	if (pubFilePath)
	{	// 公钥文件路径存在
		publie_File = BIO_new_file(pubFilePath, "rb");
		if(PEM_read_bio_RSAPublicKey(publie_File, &this->m_rsa, NULL, NULL))
			status = true;
		BIO_free(publie_File);
		
	}
	if (privFilePath)
	{	// 私钥文件路径存在
		private_File = BIO_new_file(privFilePath, "rb");
		if(PEM_read_bio_RSAPrivateKey(private_File, &this->m_rsa, NULL, NULL))
			status = true;
		BIO_free(private_File);
	}
	return status;
}

// 创建新的密钥对,如果路径为空则不保存在磁盘文件中(密钥对长度,公钥路径, 私钥路径);
void CEncryRSA::RsaGenerateRsaKey(int bits, const char* pubFilePath, const char* privFilePath)
{
	BIGNUM* bigNum = BN_new();
	BIO* publicFile = nullptr;
	BIO* privateFile = nullptr;

	int i = BN_set_word(bigNum, 123456789);					// 初始化生成密钥时的随机数种子
	RSA_generate_key_ex(this->m_rsa, bits, bigNum, NULL);	// 生成密钥对
	if (pubFilePath)
	{	// 公钥设置保存路径
		publicFile = BIO_new_file(pubFilePath, "wb");			// 用bio的方式打开公钥文件,没有会被创建
		PEM_write_bio_RSAPublicKey(publicFile, this->m_rsa);	// 覆盖写入
		BIO_flush(publicFile);									// 刷新公钥文件的新数据
		BIO_free(publicFile);									// 释放文件资源
	}
	if (privFilePath)
	{	// 私钥设置保存路径
		privateFile = BIO_new_file(privFilePath, "wb");			// 用bio的方式打开私钥文件,没有会被创建
		PEM_write_bio_RSAPrivateKey(privateFile, this->m_rsa, NULL, NULL, 0, NULL, NULL); // 覆盖写入
		BIO_flush(privateFile);									//  刷新私钥文件的新数据
		BIO_free(privateFile);									// 释放文件资源
	}
	BN_free(bigNum);
}

// RSA私钥加密
std::string  CEncryRSA::RsaPrivateEncrypt(std::string from)
{
	int data_Len = from.size();					// 计算要加密的数据总长度
	int buf_size = 0;							// 数据加密后加上填充数据一共需要的长度
	int rsa_size = RSA_size(this->m_rsa);		// 计算每次加密块的长度(包含填充数据 )
	int rsa_encdata_size = rsa_size - RSA_PKCS1_PADDING_SIZE;		// 单次每块可加密数据长度

	int orgdata_offset = 0;			// 每次加密后的 原始数据未加密偏移
	int encdata_offset = 0;			// 加密数据长度偏移

	const char* form_data = from.data();
	char* data_buf = nullptr, *enc_data = nullptr;
	std::string enc_strdata;

	// 计算数据加密后所需要内存的总大小
	if (data_Len > rsa_encdata_size)
	{	// 原始数据长度 > 单次可加密数据长度
		// ((原始数据长度/每次可加密数据长度+1 == 一共要加密多少块 ) *每块的大小) +2
		buf_size = (data_Len / rsa_encdata_size + 1) * rsa_size + 2;
	}
	else
		buf_size = rsa_size + 2;

	data_buf = new char[buf_size];
	memset(data_buf, 0, buf_size);
	while (true)
	{
		int i = 0;
		if (data_Len - orgdata_offset > rsa_encdata_size)
		{	// 剩余未加密数据 > 每次可加密数据长度
			i = RSA_private_encrypt(rsa_encdata_size, (const unsigned char*)form_data + orgdata_offset,
				(unsigned char*)data_buf + encdata_offset, this->m_rsa, RSA_PKCS1_PADDING);
			orgdata_offset += rsa_encdata_size;
			encdata_offset += rsa_size;
			continue;
		}
		i = RSA_private_encrypt(data_Len - orgdata_offset, (const unsigned char*)form_data + orgdata_offset,
			(unsigned char*)data_buf + encdata_offset, this->m_rsa, RSA_PKCS1_PADDING);
		orgdata_offset += (data_Len - orgdata_offset);
		encdata_offset += rsa_size;
		break;
	}
	cipher__bin2tobase64(data_buf, encdata_offset, enc_data, 0);
	enc_strdata = enc_data;
	delete[] enc_data;
	delete[] data_buf;
	return enc_strdata;
}

// RSA公钥加密
std::string  CEncryRSA::RsaPublicEncrypt(std::string  from)
{
	int data_Len = from.size();					// 计算要加密的数据总长度
	int rsa_size = RSA_size(this->m_rsa);		// 计算每次加密块的长度(包含填充数据 )
	int rsa_encdata_size = rsa_size - RSA_PKCS1_PADDING_SIZE;		// 单次每块可加密数据长度
	int buf_size = 0;							// 数据加密后加上填充数据 一共需要的长度
	int addre_offset = 0;						//每次加密后的 原始数据未加密偏移、
	int data_offset = 0;						// 已加密数据长度偏移

	const char* from_data = from.data();
	char* data_buf = nullptr, *enc_data = nullptr;
	std::string enc_strdata;

	if (data_Len > rsa_size)
		buf_size = (data_Len / rsa_encdata_size + 1) * rsa_size + 2;
	else
		buf_size = rsa_size + 2;

	data_buf = new char[buf_size];
	memset(data_buf, 0, buf_size);
	while (true)
	{
		if (data_Len - data_offset > rsa_encdata_size)
		{
			RSA_public_encrypt(rsa_encdata_size, (const unsigned char*)from_data + data_offset,
				(unsigned char*)data_buf + addre_offset, this->m_rsa, RSA_PKCS1_PADDING);
			addre_offset += rsa_size;
			data_offset += rsa_encdata_size;
			continue;
		}
		RSA_public_encrypt(data_Len - data_offset, (const unsigned char*)from_data + data_offset,
			(unsigned char*)data_buf + addre_offset, this->m_rsa, RSA_PKCS1_PADDING);
		addre_offset += rsa_size;
		data_offset += data_Len - data_offset;
		break;
	}
	cipher__bin2tobase64(data_buf, addre_offset, enc_data, 0);
	enc_strdata = enc_data;
	delete[] enc_data;
	delete[] data_buf;
	return enc_strdata;
}

// RSA私钥解密
std::string CEncryRSA::RsaPrivateDecrypt(std::string from)
{
	int rsa_size = RSA_size(this->m_rsa);
	int buf_size = 0; int data_Len = 0;
	int rsa_decdata_size = rsa_size - RSA_PKCS1_PADDING_SIZE;	// 每次可解密数据的长度
	int decdata_offset = 0;	// 加密数据已经解密的数据长度
	int encdata_offset = 0;	// 解密数据后的数据长度(rsa_size - RSA_PKCS1_PADDING_SIZE*第几轮解密)
	char* data_buf = nullptr, *dec_data = nullptr;
	std::string dec_strdata;

	data_Len = cipher__base64tobin2(from.data(), from.size(), data_buf, 0);
	if (data_Len > rsa_size)
		buf_size = (data_Len / rsa_decdata_size + 1) * rsa_size + 2;
	else
		buf_size = rsa_decdata_size + 2;

	dec_data = new char[buf_size];
	memset(dec_data, 0, buf_size);
	while (true)
	{
		if (data_Len - decdata_offset > rsa_size)
		{
			RSA_private_decrypt(rsa_size, (const unsigned char*)data_buf + decdata_offset,
				(unsigned char*)dec_data + encdata_offset, this->m_rsa, RSA_PKCS1_PADDING);
			decdata_offset += rsa_size;
			encdata_offset += (rsa_size - 11);
			continue;
		}
		RSA_private_decrypt(data_Len - decdata_offset, (const unsigned char*)data_buf + decdata_offset,
			(unsigned char*)dec_data + encdata_offset, this->m_rsa, RSA_PKCS1_PADDING);
		break;
	}
	dec_strdata = dec_data;
	delete[] data_buf;
	delete[] dec_data;
	return dec_strdata;
}

// RSA公钥解密
std::string CEncryRSA::RsaPublicDecrypt(std::string from)
{
	int rsa_size = RSA_size(this->m_rsa);
	int decdata_offset = 0;	// 加密数据已经解密的数据长度
	int encdata_offset = 0;	// 解密数据后的数据长度(rsa_size - RSA_PKCS1_PADDING_SIZE*第几轮解密)
	int rsa_decdata_size = rsa_size - RSA_PKCS1_PADDING_SIZE;	// 每次解密出来的数据的长度


	int data_Len = 0, buf_size = 0;
	char* data_buf = nullptr, *dec_data = nullptr;
	std::string dec_strdata;

	data_Len = cipher__base64tobin2(from.data(), from.size(), data_buf, 0);
	if (data_Len > rsa_size)
		buf_size = (data_Len / rsa_decdata_size + 1) * rsa_size + 2;
	else
		buf_size = rsa_decdata_size + 2;

	dec_data = new char[buf_size];
	memset(dec_data, 0, buf_size);

	while (true)
	{
		if (data_Len - decdata_offset > rsa_size)
		{
			RSA_public_decrypt(rsa_size, (const unsigned char*)data_buf + decdata_offset,
				(unsigned char*)dec_data + encdata_offset, this->m_rsa, RSA_PKCS1_PADDING);
			decdata_offset += rsa_size;
			encdata_offset += rsa_decdata_size;
			continue;
		}
		RSA_public_decrypt(data_Len - decdata_offset, (const unsigned char*)data_buf + decdata_offset,
			(unsigned char*)dec_data + encdata_offset, this->m_rsa, RSA_PKCS1_PADDING);
		break;
	}
	dec_strdata = dec_data;
	delete[] data_buf;
	delete[] dec_data;
	return dec_strdata;
}

// 签名函数, 计算from的哈希并将哈希使用私钥加密,返回私钥加密后的哈希(type参数表示使用的hash算法,暂未使用)
std::string CEncryRSA::RSASign(int type, const std::string from)
{
	//哈希算法写sha224
	CHashSha224 sha224;
	unsigned int hash_encdata_size = SHA224_DIGEST_LENGTH + 2;
	std::string hash_enc_strdata;
	std::string rsa_enc_strdata;
	
	hash_enc_strdata = sha224.GetDataHash(from);
	if (hash_enc_strdata.size() <= 0)
		return rsa_enc_strdata;
	rsa_enc_strdata = this->RsaPrivateEncrypt(hash_enc_strdata);
	if (rsa_enc_strdata.size() <= 0)
		std::cout << "签名生成失败" << std::endl;
	return rsa_enc_strdata;
}

//  RSA签名验证
bool  CEncryRSA::RSAVerify(std::string& org_data, std::string& sign_data)
{
	int status = 0;
	CHashSha224 sha224;
	std::string hash_enc_strdata;
	std::string sign_rsadec_strdata;

	hash_enc_strdata = sha224.GetDataHash(org_data.data());
	sign_rsadec_strdata = this->RsaPublicDecrypt(sign_data);
	status = memcmp(hash_enc_strdata.data(), sign_rsadec_strdata.data(), SHA224_DIGEST_LENGTH);
	return status== 0? true:false;
}

// =============================================== HashSha
CHashSha224::CHashSha224()
{
	this->InitSha224();
	return;
}

void CHashSha224::InitSha224()
{
	SHA224_Init(&this->m_sha224);
}

void CHashSha224::addData(std::string data)
{
	int data_len = data.size();
	SHA224_Update(&this->m_sha224, data.data(), data_len);
	return;
}

std::string CHashSha224::GetDataHash()
{
	char* buf = new char[SHA224_DIGEST_LENGTH + 2];
	std::string data;
	memset(buf, 0, SHA224_DIGEST_LENGTH + 2);
	SHA224_Final((unsigned char*)buf, &this->m_sha224);
	data = buf;
	delete[] buf;
	return data;
}

std::string CHashSha224::GetDataHash(const std::string from)
{
	char* hash_data = new char[SHA224_DIGEST_LENGTH + 2];
	std::string hash_strdata;

	memset(hash_data, 0, SHA224_DIGEST_LENGTH + 2);
	if (SHA224((unsigned char*)from.data(), from.size(), (unsigned char*)hash_data) != NULL)
		hash_strdata = hash_data;
	delete[] hash_data;
	return hash_strdata;
}
