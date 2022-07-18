
#ifndef CLIENTOP_H
#define CLIENTOP_H
#include <iostream>
#include <json/json.h>
#include <Windows.h>
#include "encry.h"
    
void __cdecl OutputDebugStringFA(const char* format, ...);
void __cdecl OutputDebugStringFW(const wchar_t* format, ...);

#ifdef _DEBUG
#define WinPrintA OutputDebugStringFA
#define WinPrintW OutputDebugStringFW
#else
#define WinPrintA
#define WinPrintW
#endif

class ClientOP
{
public:
    ClientOP(const char* confPath = "./config.json");

    void seckeyAgree();     // √‹‘ø–≠…Ã
private:
    std::string clientID;
    std::string serverID;
    std::string serverIP;
    std::string shmkey;
    int serverPort = 0;
    int maxnode = 0;
};


#endif // !CLIENTOP_H