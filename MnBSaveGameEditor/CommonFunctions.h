#ifndef COMMONFUNCTIONS_H_INCLUDED
#define COMMONFUNCTIONS_H_INCLUDED

//#include "CommonFunctions.h"
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <stdlib.h>

#define D_LogLevel 3

#if D_LogLevel > 0
#define ErrorLog(message) { stringstream log; log << message ; CommonFunctions::LogError(log.str());}
#else
#define ErrorLog(message)
#endif

#if D_LogLevel > 1
#define WarningLog(message) { stringstream log; log << message ; CommonFunctions::LogWarning(log.str());}
#else
#define WarningLog(message)
#endif

#if D_LogLevel > 2
#define DebugLog(message) { stringstream log; log << message ; CommonFunctions::LogDebug(log.str());}
#else
#define DebugLog(message)
#endif


#define InfoLog(message) { stringstream log; log << message ; CommonFunctions::LogInfo(log.str());}

using namespace std;

namespace CommonFunctions
{
    string string_to_hex(const string& input);
    void LogError(const string&  error);
    void LogWarning(const string& msg);
    void LogDebug(const string& msg);
    void LogInfo(const string& msg);
    string Int64ToString(int64_t value);
    string UInt64ToString(uint64_t value);
    int64_t ToInt64(string value);
    void CleanString(string& str);
    ostream& DataStringToOStream(ostream& oStream, string& str);
    int64_t strtoll(string value);
    int64_t atoll(string value);

    void ResetLogFile();
    void SaveLogFile();

    extern ofstream LogFile;

    /*
    template <class T>
    T strToNum(const string &inputString, ios_base &(*f)(ios_base&) = std::dec);*/

    template <class T>
    T strToNum(const std::string &inputString,
               std::ios_base &(*f)(std::ios_base&) = std::dec)
    {
        T t;
        std::istringstream stringStream(inputString);

        if ((stringStream >> f >> t).fail())
        {
            ErrorLog("fail conversion of " << inputString);
        }
        return t;
    }

    extern unsigned int LogLevel;
    //unsigned int LogLevel;
}


//CommonFunctions::LogLevel



#endif // COMMONFUNCTIONS_H_INCLUDED
