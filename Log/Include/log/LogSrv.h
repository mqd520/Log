#pragma once
#include <string>
#include <queue>
using namespace std;

namespace llog
{
	// ��־��������
	enum class ELogSrvType : int
	{
		Info,		// ��Ϣ
		Debug,		// ����
		Warn,		// ����
		Error,		// ����
		Fatal,		// ���ش���

		None		// ��
	};

	// ��־����
	struct LogData
	{
		string	log;		// ��־����
		ELogSrvType type;	// ��־����
		bool b;				// �Ƿ����������̨
	};


	// log srv
	class LogSrv
	{
	private:
		LogSrv();

	private:
		static string strFilePath;						// ��־�ļ�·��
		static queue<struct LogData> LogSrv::quLogs;	// ��־����
		static CRITICAL_SECTION section;				// section
		static string strDirName;						// Ŀ¼��

	private:
		//************************************
		// Method:    д����־���ļ�
		//************************************
		static void WriteToFile(struct LogData& data);

		//************************************
		// Method:    д����־������̨
		//************************************
		static void WriteToConsole(struct LogData& data);

	public:
		//************************************
		// Method:    ��ʼ��
		// Method:    ����Ŀ¼��
		//************************************
		static void Init(/*string dirName = "Log"*/);

		//************************************
		// Method:    �˳�
		//************************************
		static void Exit();

		//************************************
		// Method:    д��һ����־
		// Parameter: ��־����
		// Parameter: �Ƿ����������̨
		// Parameter: ��־��ʽ
		// Parameter: ...
		//************************************
		static void WriteLine(ELogSrvType type, bool b, string format, ...);

		//************************************
		// Method:    ������־
		//************************************
		static void ProcessLog();
	};
}