#include "strConversion.h"

void strFillZero(string& str, plainTextMode plMode)
{
    int p;
    if( plMode == BINARY )   p = 64;
    else if( plMode == HEX ) p = 16;
    else if( plMode == TEXT) p = 8;

    int size = str.size() / p;
    if( p*size != str.size() )
        if( plMode == BINARY )
            str += string(p*(size+1) - str.size(),  '0');
        else
            str += string(p*(size+1) - str.size(), '\0');
}

string str_To_strBinary(const string& str)
{
    string binary(str.size()*8, '0');
    for(Byte i = 1; i <= str.size(); ++i){
        Byte ch = str[i - 1];
        for(Byte j = 0; j < 8; ++j){
            binary[8*i - 1 - j] = (ch & 0x01) ? '1' : '0';
            ch >>= 1;
        }
    }
    return binary;
}

string strBinary_To_hex(const string& str_binary)
{
    string str = "";
    for(Byte i = 0; i < str_binary.size(); i += 4)
    {
        bitset<4> bs( str_binary.substr(i, 4) );
        Byte ch = 0, p = 1;
        for(Byte k = 0; k < 4; ++k, p*=2)
            ch += bs[k] * p;

        if( ch < 10 ) ch = '0' + ch;
        else ch = 'A' + ch - 10;
        str.append(1, ch);
    }

    return str;
}

string strHex_To_strBinary(const string& str_hex)
{
    string str = "";
    for(Byte i = 0; i < str_hex.size(); ++i)
    {
        Byte ch = str_hex[i];
        Byte x  = (ch < 'A') ? (ch-'0') : (ch-'A'+10);
        bitset<4> bs(x);
        str += bs.to_string();
    }

    return str;
}

string getStrBinary(const string& str_plText, plainTextMode plMode)
{
	string str_binaryPlText;
	if( plMode == TEXT )
        str_binaryPlText = str_To_strBinary(str_plText);
    else if( plMode == HEX )
        str_binaryPlText = strHex_To_strBinary(str_plText);
    else
        str_binaryPlText = str_plText;
        
	return str_binaryPlText;
}
