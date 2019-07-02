#pragma once
#include <string>
#include <queue>
using namespace std;

namespace llog
{
	// 日志服务类型
	enum class ELogSrvType : int
	{
		Info,		// 信息
		Debug,		// 调试
		Warn,		// 警告
		Error,		// 错误
		Fatal,		// 严重错误

		None		// 无
	};

	// 日志数据
	typedef struct tagLogData
	{
		string	log;		// 日志内容
		ELogSrvType type;	// 日志类型
	}LogData;


	// log srv
	class LogSrv
	{
	private:
		LogSrv();

	private:
		static string strFilePath;						// 日志文件路径
		static queue<LogData> LogSrv::quLogs;			// 日志集合
		static CRITICAL_SECTION section;				// section
		static string strDirName;						// 目录名

	private:
		//************************************
		// Method:    写入日志到文件
		//************************************
		static void WriteToFile(LogData& data);

		//************************************
		// Method:    写入日志到控制台
		//************************************
		static void WriteToConsole(LogData& data);

	public:
		//************************************
		// Method:    初始化
		// Method:    设置目录名
		//************************************
		static void Init(string dirName = "Log");

		//************************************
		// Method:    退出
		//************************************
		static void Exit();

		//************************************
		// Method:    写入一行日志
		// Parameter: 日志类型
		// Parameter: 是否输出到控制台
		// Parameter: 日志格式
		// Parameter: ...
		//************************************
		static void WriteLine(ELogSrvType type, string format, ...);

		//************************************
		// Method:    处理日志
		//************************************
		static void ProcessLog();
	};
}