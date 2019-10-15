#pragma once

#include "constvar.h"
#include <bitset>
#include <string>
using namespace std;

void strFillZero(string& str, plainTextMode plMode);
string str_To_strBinary(const string& str);
string strBinary_To_hex(const string& str_binary);
string strHex_To_strBinary(const string& str_hex);

string getStrBinary(const string& str_plText, plainTextMode plMode);
