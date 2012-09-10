#include "CLog.h"

CLog::CLog()
{
    
}
void CLog::Write(std::string message)
{
	
    std::ofstream out;
	out.open("Log.txt", std::ios::app);
	MessageBox(NULL, message.c_str(), "Error", MB_OK);
    out << message;
    out.close();
}

void CLog::Debug(char* message)
{
	std::ofstream out;
	out.open("Debug.txt");
	out << message;
	out.close();
}