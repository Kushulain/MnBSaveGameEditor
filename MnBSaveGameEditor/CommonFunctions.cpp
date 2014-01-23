#include "CommonFunctions.h"



using namespace std;
unsigned int CommonFunctions::LogLevel = 3;
ofstream CommonFunctions::LogFile;

void CommonFunctions::LogError(const string& msg)
{
    if (LogLevel > 0)
        cout << "=============ERROR : " << msg << endl;

    if (LogFile)
        LogFile<< "=============ERROR : "  << msg << endl;
}

void CommonFunctions::LogWarning(const string& msg)
{
    if (CommonFunctions::LogLevel > 1)
        cout << "======Warning : " << msg << endl;

    if (LogFile)
        LogFile  << "======Warning : " << msg << endl;
}

void CommonFunctions::LogDebug(const string& msg)
{
    if (CommonFunctions::LogLevel > 2)
        cout << msg << endl;
/*
    if (LogFile)
        LogFile << msg << endl;*/
}

void CommonFunctions::LogInfo(const string& msg)
{
    cout << msg << endl;

    if (LogFile)
        LogFile << msg << endl;
}


string CommonFunctions::Int64ToString(int64_t value)
{
    ostringstream buffer;
    buffer << value;
    return buffer.str();
}

string CommonFunctions::UInt64ToString(uint64_t value)
{
    ostringstream buffer;
    buffer << value;

    return buffer.str();
}


int64_t CommonFunctions::ToInt64(string value)
{
    int64_t out;
    stringstream(value) >> out;
    return out;
}

int64_t CommonFunctions::strtoll(string value)
{
    int64_t out;
    stringstream(value) >> out;
    return out;
}

int64_t CommonFunctions::atoll(string value)
{
    int64_t out;
    stringstream(value) >> out;
    return out;
}

void CommonFunctions::CleanString(string& str)
{
    for (int i=str.size()-1; i >= 0; i--)
        if (str[i] == ' ' || str[i] == 0x00 || str[i] == '\t')
            str.erase(str.begin()+i);
}


string CommonFunctions::string_to_hex(const string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}


ostream& CommonFunctions::DataStringToOStream(ostream& oStream, string& str)
{
    for (string::iterator it = str.begin(); it != str.end(); ++it )
        oStream << *it;

    return oStream;
}


void CommonFunctions::ResetLogFile()
{
    LogFile.open("Log.txt", ofstream::out | ofstream::trunc);
}

void CommonFunctions::SaveLogFile()
{
    LogFile.close();
    ResetLogFile();
}
