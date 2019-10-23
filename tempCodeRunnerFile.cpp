#include <iostream>
#include <string>
#include <bitset>
using namespace std;

typedef unsigned char Byte;

string strHex_To_strBinary(const string& str_hex)
{
    string str = "";
    for(Byte i = 0; i < str_hex.size(); ++i)
    {
        Byte ch = str_hex[i];
        Byte x;
        if( ch>='0' && ch<='9' ) x = ch - '0';
        else if( ch>='a' && ch<='f' ) x = ch - 'a' + 10;
        else if( ch>='A' && ch<='F' ) x = ch - 'A' + 10;
		else x = 0;
        bitset<4> bs(x);
        str += bs.to_string();
    }

    return str;
}

int main()
{
	string str_hex = "0f1571c947d9e859";
	string str_binary = strHex_To_strBinary(str_hex);
	cout << str_binary << endl;
	//"0000111100010101011100011100100101000111110110011110100001011001"
	
	return 0;
}
