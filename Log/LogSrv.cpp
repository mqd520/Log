#include "stdafx.h"
#include "Include/log/LogSrv.h"
#include <time.h>
#include <fstream>

#define LLOG1_MAXLOGLEN (1024)		// 最大日志长度

namespace llog
{
	string GetProgramDir();
	DWORD WINAPI Run(void* p);

	HANDLE hThread = NULL;		// 线程句柄
	bool bInited = false;		// 是否已初始化
	bool bExited = false;		// 指示while循环退出


	string					LogSrv::strFilePath = "";
	queue<struct LogData>	LogSrv::quLogs;
	CRITICAL_SECTION		LogSrv::section;

	ofstream file;

	LogSrv::LogSrv()
	{

	}

	void LogSrv::Init()
	{
		if (!bInited)
		{
			bInited = true;

			if (hThread == NULL)
			{
				hThread = CreateThread(0, 0, Run, NULL, 0, 0);
			}

			InitializeCriticalSection(&section);
			strFilePath = GetProgramDir() + "log.txt";
		}
	}

	void LogSrv::Exit()
	{
		if (!bExited)
		{
			bExited = true;
			DeleteCriticalSection(&section);
		}
	}

	void LogSrv::WriteToFile(struct LogData& data)
	{
		file << data.log.c_str() << endl;
	}

	void LogSrv::WriteToConsole(struct LogData& data)
	{
		WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

		if (data.type == ELogSrvType::Debug)
		{
			color = FOREGROUND_BLUE | FOREGROUND_GREEN;
		}
		else if (data.type == ELogSrvType::Warn)
		{
			color = FOREGROUND_RED | FOREGROUND_GREEN;
		}
		else if (data.type == ELogSrvType::Error)
		{
			color = FOREGROUND_RED;
		}
		else if (data.type == ELogSrvType::Fatal)
		{
			color = FOREGROUND_RED | FOREGROUND_INTENSITY;
		}

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		printf("%s \n", data.log.c_str());
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	void LogSrv::WriteLine(ELogSrvType type, bool b, string format, ...)
	{
		SYSTEMTIME t;
		GetLocalTime(&t);

		char log[LLOG1_MAXLOGLEN] = { 0 };
		va_list argPtr;
		va_start(argPtr, format);
		vsprintf_s(log, LLOG1_MAXLOGLEN - 1, format.c_str(), argPtr);
		va_end(argPtr);

		string title = "None";
		if (type == ELogSrvType::Info)
		{
			title = "Info";
		}
		else if (type == ELogSrvType::Debug)
		{
			title = "Debug";
		}
		else if (type == ELogSrvType::Warn)
		{
			title = "Warn";
		}
		else if (type == ELogSrvType::Error)
		{
			title = "Error";
		}
		else if (type == ELogSrvType::Fatal)
		{
			title = "Fatal";
		}

		char ch[LLOG1_MAXLOGLEN] = { 0 };
		sprintf_s(ch, "[%s] [%02d-%02d-%02d %02d:%02d:%02d.%03d] %s", title.c_str(), t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, log);

		struct LogData data = { ch, type, b };

		EnterCriticalSection(&section);
		quLogs.push(data);
		LeaveCriticalSection(&section);
	}

	void LogSrv::ProcessLog()
	{
		EnterCriticalSection(&section);

		if (!quLogs.empty())
		{
			file.open(strFilePath, ios::app);

			while (!quLogs.empty())
			{
				struct LogData data = quLogs.front();
				quLogs.pop();

				if (data.b)
				{
					WriteToConsole(data);
				}
				WriteToFile(data);
			}

			file.close();
		}

		LeaveCriticalSection(&section);
	}

	DWORD WINAPI Run(void* p)
	{
		while (!bExited)
		{
			LogSrv::ProcessLog();

			Sleep(100);
		}

		if (hThread)
		{
			CloseHandle(hThread);
		}

		return 0;
	}

	string GetProgramDir()
	{
		char exeFullPath[MAX_PATH] = { 0 };

		GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
		string strPath = exeFullPath;
		int pos = strPath.find_last_of('\\', strPath.length());
		return strPath.substr(0, pos + 1);
	}
}