#include "des.h"
#include "constvar.h"

int main()
{
    cout << "------------------DES CODE------------------" << endl;

    //  TEXT    ofi1kl(6F6669316B6C0000) -> E23142959B9F43B1(ECB) | D579EF25025D07E1(CBC)
    //  HEX     3BAFC1BD1ABDD48B -> 1CFF344E5CA5770D(ECB) | C00B3C4C15854405(CBC)
    //  BINARY  1000101010101101010(8AAD400000000000) -> 654DCDC20D5D6B45(ECB) | 88855BEC00870014(CBC)
    
	string key = "0001001100110100010101110111100110011011101111001101111111110001";
    string str_plText = "88855BEC00870014";
    string str_hexCpText = DES(str_plText, HEX, key, DECODE, CBC);
    
    cout << "str_hexCpText : " << str_hexCpText << endl;

    return 0;
}
