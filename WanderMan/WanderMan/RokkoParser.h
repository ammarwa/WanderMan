#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <string> 

using namespace std;

class RokkoParser
{
    
public:
    static void objToTxt(const string aInFilename,
                         const string aOutFilename,
                         bool aVerbose = false);
    static vector<std::string> explode(string aStr, char aDelim);
};