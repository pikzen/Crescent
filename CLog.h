#ifndef CLOG_H
#define CLOG_H

#include <stdio.h>
#include <Windows.h>
#include <fstream>
#include <string>

class CLog
{
public:
    CLog();

    static void Write(std::string);
	static void Debug(char*);

    /* data */
};

#endif