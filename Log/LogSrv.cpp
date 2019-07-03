#include "stdafx.h"
#include "Include/log/LogSrv.h"
#include <time.h>
#include <fstream>

#define LLOG1_MAXLOGLEN (1024)		// 最大日志长度

namespace llog
{
	// file info
	typedef struct tagFileInfo
	{
		string strDirPath;		// 目录路径
		string strFilePath;		// 文件路径
		ofstream fs;			// 文件流对象
		bool bOpened;			// 是否已打开
	}FileInfo;

	string GetProgramDir();
	DWORD WINAPI Run(void* p);
	void SetFilePathInfo();
	void OpenFile();
	void CloseFile();
	FileInfo& GetFileInfo(ELogSrvType type);
	void ChangeFilePath();
	string GetCurDate();
	void InitDir();
	void CreateDir(string path);

	HANDLE hThread = NULL;		// 线程句柄
	bool bInited = false;		// 是否已初始化
	bool bExited = false;		// 指示while循环退出
	string dirExe;				// 可执行程序目录
	string dirLog;				// 日志目录路径
	FileInfo fiInfo;			// Info file info
	FileInfo fiDebug;			// Debug file info
	FileInfo fiWarn;			// Warn file info
	FileInfo fiError;			// Error file info
	FileInfo fiFatal;			// Fatal file info
	string curDate = "";		// 当前日期
	string strDirName1 = "Log";	// 默认目录名


	queue<LogData>			LogSrv::quLogs;
	CRITICAL_SECTION		LogSrv::section;



	LogSrv::LogSrv()
	{

	}


	void LogSrv::Init(string dirName /*= "Log"*/)
	{
		if (!bInited)
		{
			bInited = true;

			strDirName1 = dirName;
			curDate = GetCurDate();
			InitDir();
			SetFilePathInfo();

			InitializeCriticalSection(&section);

			if (hThread == NULL)
			{
				hThread = CreateThread(0, 0, Run, NULL, 0, 0);
			}
		}
	}

	void LogSrv::Exit()
	{
		if (!bExited)
		{
			bExited = true;
			LeaveCriticalSection(&section);
			DeleteCriticalSection(&section);
			CloseFile();
		}
	}

	void LogSrv::WriteToFile(LogData& data)
	{
		//FileInfo& fi = GetFileInfo(data.type);

		if (data.type == ELogSrvType::Info && fiInfo.bOpened == true)
		{
			fiInfo.fs << data.log.c_str() << endl;
		}
		else if (data.type == ELogSrvType::Debug && fiDebug.bOpened == true)
		{
			fiDebug.fs << data.log.c_str() << endl;
		}
		else if (data.type == ELogSrvType::Warn && fiWarn.bOpened == true)
		{
			fiWarn.fs << data.log.c_str() << endl;
		}
		else if (data.type == ELogSrvType::Error && fiError.bOpened == true)
		{
			fiError.fs << data.log.c_str() << endl;
		}
		else if (data.type == ELogSrvType::Fatal && fiFatal.bOpened == true)
		{
			fiFatal.fs << data.log.c_str() << endl;
		}
	}

	void LogSrv::WriteToConsole(LogData& data)
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

	void LogSrv::WriteLine(ELogSrvType type, string format, ...)
	{
		if (bExited)
		{
			return;
		}

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

		LogData data = { ch, type };

		EnterCriticalSection(&section);
		quLogs.push(data);
		LeaveCriticalSection(&section);
	}

	void LogSrv::ProcessLog()
	{
		EnterCriticalSection(&section);

		if (!quLogs.empty())
		{
			OpenFile();

			while (!quLogs.empty())
			{
				LogData data = quLogs.front();
				quLogs.pop();

				WriteToFile(data);
			}

			CloseFile();
		}

		ChangeFilePath();

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

		CloseFile();

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

	void SetFilePathInfo()
	{
		fiInfo.strFilePath = fiInfo.strDirPath + curDate + ".log";
		fiInfo.bOpened = false;

		fiDebug.strFilePath = fiDebug.strDirPath + curDate + ".log";
		fiDebug.bOpened = false;

		fiWarn.strFilePath = fiWarn.strDirPath + curDate + ".log";
		fiWarn.bOpened = false;

		fiError.strFilePath = fiError.strDirPath + curDate + ".log";
		fiError.bOpened = false;

		fiFatal.strFilePath = fiFatal.strDirPath + curDate + ".log";
		fiFatal.bOpened = false;
	}

	void OpenFile()
	{
		if (!fiInfo.bOpened)
		{
			fiInfo.fs.open(fiInfo.strFilePath, std::ios_base::app);
			fiInfo.bOpened = true;
		}

		if (!fiDebug.bOpened)
		{
			fiDebug.fs.open(fiDebug.strFilePath, std::ios_base::app);
			fiDebug.bOpened = true;
		}

		if (!fiWarn.bOpened)
		{
			fiWarn.fs.open(fiWarn.strFilePath, std::ios_base::app);
			fiWarn.bOpened = true;
		}

		if (!fiError.bOpened)
		{
			fiError.fs.open(fiError.strFilePath, std::ios_base::app);
			fiError.bOpened = true;
		}

		if (!fiFatal.bOpened)
		{
			fiFatal.fs.open(fiFatal.strFilePath, std::ios_base::app);
			fiFatal.bOpened = true;
		}
	}

	void CloseFile()
	{
		if (fiInfo.bOpened)
		{
			fiInfo.fs.close();
			fiInfo.bOpened = false;
		}

		if (fiDebug.bOpened)
		{
			fiDebug.fs.close();
			fiDebug.bOpened = false;
		}

		if (fiWarn.bOpened)
		{
			fiWarn.fs.close();
			fiWarn.bOpened = false;
		}

		if (fiError.bOpened)
		{
			fiError.fs.close();
			fiError.bOpened = false;
		}

		if (fiFatal.bOpened)
		{
			fiFatal.fs.close();
			fiFatal.bOpened = false;
		}
	}

	FileInfo& GetFileInfo(ELogSrvType type)
	{
		if (type == ELogSrvType::Info)
		{
			return fiInfo;
		}
		else if (type == ELogSrvType::Debug)
		{
			return fiDebug;
		}
		else if (type == ELogSrvType::Warn)
		{
			return fiWarn;
		}
		else if (type == ELogSrvType::Error)
		{
			return fiError;
		}
		else if (type == ELogSrvType::Fatal)
		{
			return fiFatal;
		}

		return fiInfo;
	}

	void ChangeFilePath()
	{
		string date1 = GetCurDate();

		if (date1 != curDate)
		{
			curDate = date1;
			CloseFile();
			SetFilePathInfo();
		}
	}

	string GetCurDate()
	{
		SYSTEMTIME t;
		GetLocalTime(&t);

		char ch[20] = { 0 };
		sprintf_s(ch, "%04d-%02d-%02d", t.wYear, t.wMonth, t.wDay);

		return ch;
	}

	void InitDir()
	{
		dirExe = GetProgramDir();
		dirLog = dirExe + strDirName1 + "\\";
		fiInfo.strDirPath = dirLog + "Info\\";
		fiDebug.strDirPath = dirLog + "Debug\\";
		fiWarn.strDirPath = dirLog + "Warn\\";
		fiError.strDirPath = dirLog + "Error\\";
		fiFatal.strDirPath = dirLog + "Fatal\\";

		CreateDir(dirLog);
		CreateDir(fiInfo.strDirPath);
		CreateDir(fiDebug.strDirPath);
		CreateDir(fiWarn.strDirPath);
		CreateDir(fiError.strDirPath);
		CreateDir(fiFatal.strDirPath);
	}

	void CreateDir(string path)
	{
		if ((GetFileAttributesA(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY) > 0)
		{
			BOOL b = CreateDirectoryA(path.c_str(), NULL);
			if (b = FALSE)
			{
				throw new exception;
			}
		}
	}
}