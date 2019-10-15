#include <iostream>
#include <bitset>
#include <cstdio>
using namespace std;

typedef unsigned char Byte;

void strFillZero(string& str)
{
    int size = str.size() / 8;
    if( 8*size != str.size() )
        str += string(8*(size+1) - str.size(), '\0');    
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

int main()
{
    string str = "vq8w1wf24qs";
    cout << str.size() << " , pre_str : " << str << endl;

    strFillZero(str);
    cout << str.size() << " ,now_str : " << str << endl;

    string str_binary = str_To_strBinary(str);
    cout << str_binary.size() << " ,str_binary : " << str_binary << endl;

    string str_hex = "A3D50F";
    cout << "str_hex:" << str_hex << ", " << strHex_To_strBinary(str_hex) << endl;

    return 0;
}