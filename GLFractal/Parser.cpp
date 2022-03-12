#include "Parser.h"

#include <string>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>

using std::string, std::ostringstream;

using std::invalid_argument, std::out_of_range;

namespace Parser
{
    bool tryParse(char* str, double* num)
    {
        try
        {
            *num = std::stod(str);
            return true;
        }
        catch (const invalid_argument& e) { }
        return false;
    }

    bool tryParse(char* str, int* num)
    {
        try
        {
            *num = std::stoi(str);
            return true;
        }
        catch (const invalid_argument& e) { }
        return false;
    }

    bool tryParse(char* str, float* num)
    {
        try
        {
            *num = std::stof(str);
            return true;
        }
        catch (const invalid_argument& e) { }
        return false;
    }

    bool tryParseHex(char* str, unsigned int* num)
    {
        try
        {
            *num = std::stoul(str, NULL, 16);
            return true;
        }
        catch (const invalid_argument& e) { }
        return false;
    }

    string toString(double d)
    {
        ostringstream ss;
        ss.precision(12);
        ss << d;
        return ss.str();
    }
}