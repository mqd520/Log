// Server1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "log/LogSrv.h"
using namespace llog;


int _tmain(int argc, _TCHAR* argv[])
{
	LogSrv::Init();

	LogSrv::WriteLine(ELogSrvType::Info, "ip: %s, port: %d", "192.168.0.69", 56745);
	LogSrv::WriteLine(ELogSrvType::Debug, "ip: %s, port: %d", "192.168.0.69", 56745);
	LogSrv::WriteLine(ELogSrvType::Warn, "ip: %s, port: %d", "192.168.0.69", 56745);
	LogSrv::WriteLine(ELogSrvType::Error, "ip: %s, port: %d", "192.168.0.69", 56745);
	LogSrv::WriteLine(ELogSrvType::Fatal, "ip: %s, port: %d", "192.168.0.69", 56745);

	LogSrv::Exit();

	getchar();
	return 0;
}

