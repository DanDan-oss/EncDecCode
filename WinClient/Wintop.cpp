#include <iostream>
#include "resource.h"
#include <Windows.h>
#include <fstream>
#include <json/json.h>
#include <json/writer.h>
#include "ClientOP.h"
HINSTANCE hAppInstance = NULL;
HWND hwndWinDialog = NULL;
HWND hwndEditText = nullptr;

ClientOP* ciletOP = nullptr;
std::string szOutText;
__int64 CALLBACK DialogProc(HWND hDia, UINT uMsg, WPARAM wParam, LPARAM lPARAM);
__int64 CALLBACK DialogConfPrc(HWND hDia, UINT uMsg, WPARAM wParam, LPARAM lPARAM);
VOID CALLBACK WinPrint(const char* outText);

typedef INT_PTR(CALLBACK* DLGPROC)(HWND, UINT, WPARAM, LPARAM);
//IDD_MAIN_DIALOG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	hAppInstance = hInstance;
	std::ifstream file("./config.json", std::ios::in);
	if (!file.good())
	{
		if (0 == DialogBoxW(hInstance, (LPCWSTR)IDD_DIALOG_CONFIG, NULL, DialogConfPrc))
			return 0;
	}
    DialogBoxW(hInstance, (LPCWSTR)IDD_MAIN_DIALOG, NULL, DialogProc);
	return 0;
}


__int64 CALLBACK DialogProc(HWND hDia, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		hwndWinDialog = hDia;
		WinPrint("窗口开始创建");
		ciletOP = new ClientOP();
		return NULL;
	}
	case WM_COMMAND:
		OutputDebugStringW(L"[mhDbg]:哈哈");
		if (wParam == IDC_BUTTON1)
		{
			WinPrint("密钥协商按钮被操作");
			ciletOP->seckeyAgree();
		}
			
		
		break;
	case WM_CLOSE:
		EndDialog(hDia, NULL);
		delete ciletOP;
		break;

	default:
		return NULL;
	}

	return NULL;
}

 //初始化配置文件对话框消息回调
__int64 CALLBACK DialogConfPrc(HWND hDia, UINT uMsg, WPARAM wParam, LPARAM lPARAM)
{
	
	switch (uMsg)
	{
	case WM_CLOSE:
		EndDialog(hDia, NULL);
		break;

	case WM_COMMAND:
		if (CANFIG_QUIT == wParam) // 单击了退出按钮
		{
			SendMessageA(hDia, WM_CLOSE, 0, 0);
			return NULL;
		}
		if (CONFIG_IDSAVE == wParam)	//单击了保存按钮
		{
			char serverIP[30] = {0};
			int serverPort = 0;
			char serverShmkey[30] = { 0 };
			std::string jsonVal;
			Json::Value ObjVal;
			Json::FastWriter JsonW;
			std::ofstream fp;
			if (!GetDlgItemTextA(hDia, CONFIG_EDIT_SERVERIP, serverIP, sizeof(serverIP)))
			{
				MessageBoxW(hDia, TEXT("未设置服务器IP"), TEXT("配置文件"), MB_OK);
				return NULL;
			}
			serverPort = GetDlgItemInt(hDia, CONFIG_EDIT_SERVERIP, NULL, FALSE);
			if (!serverPort)
			{
				MessageBoxW(hDia, TEXT("未设置服务器端口"), TEXT("配置文件"), MB_OK);
				return NULL;
			}
			if (!GetDlgItemTextA(hDia, CONFIG_EDIT_SERVERIP, serverIP, sizeof(serverIP)))
			{
				MessageBoxW(hDia, TEXT("未设置服务器链接符号名"), TEXT("配置文件"), MB_OK);
				return NULL;;
			}
			fp.open("config.json", std::ios::out | std::ios::trunc);
			ObjVal["serverIP"] = serverIP;
			ObjVal["shmkey"] = serverShmkey;
			ObjVal["serverPort"] = serverPort;
			jsonVal = ObjVal.toStyledString();
			fp<< jsonVal ;
			EndDialog(hDia, 1);
		}
	}

	return NULL;
}


VOID CALLBACK WinPrint(const char* outText)
{
	if (!hwndEditText)
			hwndEditText = GetDlgItem(hwndWinDialog, IDC_EDIT_OUTTEXT);
	szOutText += outText ;
	szOutText += " \r\n";
	SetWindowTextA(hwndEditText, szOutText.data());
	return;
}