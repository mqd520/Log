// Server1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "log/LogSrv.h"
using namespace llog;

DWORD WINAPI Run(void* p);
HANDLE hThread1;

int _tmain(int argc, _TCHAR* argv[])
{
	LogSrv::Init("Server1.exe.Log");

	LogSrv::WriteLine(ELogSrvType::Info, "ip: %s, port: %d", "192.168.0.69", 56745);
	LogSrv::WriteLine(ELogSrvType::Debug, "ip: %s, port: %d", "192.168.0.69", 56745);
	LogSrv::WriteLine(ELogSrvType::Warn, "ip: %s, port: %d", "192.168.0.69", 56745);
	LogSrv::WriteLine(ELogSrvType::Error, "ip: %s, port: %d", "192.168.0.69", 56745);
	LogSrv::WriteLine(ELogSrvType::Fatal, "ip: %s, port: %d", "192.168.0.69", 56745);

	printf("ip: %s, port: %d \n", "192.168.0.69", 56745);
	printf("ip: %s, port: %d \n", "192.168.0.69", 56745);
	printf("ip: %s, port: %d \n", "192.168.0.69", 56745);
	printf("ip: %s, port: %d \n", "192.168.0.69", 56745);
	printf("ip: %s, port: %d \n", "192.168.0.69", 56745);

	//hThread1 = CreateThread(0, 0, Run, NULL, 0, 0);


	char ch[256] = { 0 };
	while (true)
	{
		string str1 = gets_s(ch);

		LogSrv::WriteLine(ELogSrvType::Info, str1);

		if (str1 == "quit")
		{
			break;
		}
	}

	LogSrv::Exit();
	return 0;
}

DWORD WINAPI Run(void* p)
{
	while (true)
	{
		LogSrv::WriteLine(ELogSrvType::Info, "ip: %s, port: %d", "192.168.0.69", 56745);
		LogSrv::WriteLine(ELogSrvType::Info, "ip: %s, port: %d", "192.168.0.69", 56745);
		LogSrv::WriteLine(ELogSrvType::Info, "ip: %s, port: %d", "192.168.0.69", 56745);
		LogSrv::WriteLine(ELogSrvType::Info, "ip: %s, port: %d", "192.168.0.69", 56745);
		LogSrv::WriteLine(ELogSrvType::Info, "ip: %s, port: %d", "192.168.0.69", 56745);

		Sleep(10);
	}

	if (hThread1)
	{
		CloseHandle(hThread1);
	}

	return 0;
}

