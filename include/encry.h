#ifndef ENCRY_H
#define ENCRY_H

#include <iostream>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>


/* start如果传入得指针引用指向nullptr，函数会自动申请内存,注意自己释放*/
static char* cipher__bin2tobase64(const char* in, int in_size, char* (&out) , int out_szie);
static int cipher__base64tobin2(const char* in, int in_size, char* (&out), int out_szie);
/* end*/

class CEncryAES
{
public:
    CEncryAES(const char* userKey, const int bitSize = 128, 
        const char* ivec = "csaersacfkacseacv");
	bool InitAesKey(const  char* userKey, const int bitSize, const char* ivec);

    std::string AesCbcEncrypt(std::string enc_data);     // 加密
    std::string AesCbcDecrypt(std::string dec_data);     // 解密
private:
    CEncryAES() { }
	std::string m_userKey;
	int m_bitSize = 0;
	char ivec[16] = {0};
	AES_KEY m_dec_AESKey = {0};
	AES_KEY m_enc_AESKey = {0};
};

class CEncryRSA
{
public:
    CEncryRSA();
    // 为了创建一个新密钥对
    CEncryRSA(int bits, const char* pubFilePath, const char* privFilePath); 
    CEncryRSA(const char* pubFilePath, const char* privFilePath);            // 打开一个新密钥对 
    ~CEncryRSA();
    bool RsaOpenKey(const char* pubFilePath, const char* privFilePath);     // 打开已存的的公私钥文件
//创建新的密钥对保存文件,如果路径为空则不保存在磁盘(密钥对长度,公钥路径, 私钥路径);
    void RsaGenerateRsaKey(int bits, const char* pubFilePath = nullptr, const char* privFilePath = nullptr);

    std::string  RsaPublicEncrypt(std::string  from);     // 公钥加密
    std::string RsaPrivateDecrypt(std::string from);    // 私钥解密
    std::string RsaPrivateEncrypt(std::string from);    // 私钥加密
    std::string RsaPublicDecrypt(std::string from);     // 公钥解密

    // 签名函数, type表示签名使用的加密算法,还没使用
    std::string RSASign(int type, const std::string from);      
    bool RSAVerify(std::string& org_data, std::string& sign_data);        // 签名验证     

private:
    RSA* m_rsa = nullptr;
};

class CHashSha224
{
public:
    CHashSha224();
    void InitSha224();
    void addData(std::string data);
    std::string GetDataHash();
    std::string GetDataHash(const std::string from);
private:
    SHA256_CTX m_sha224 = {0};
};

#endif // !ENCRY_H