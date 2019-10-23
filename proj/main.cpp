#include "des.h"
#include "constvar.h"

//  playruneterra.com

int main()
{
    cout << "------------------DES CODE------------------" << endl;

    //  TEXT    ofi1kl(6F6669316B6C0000) -> E23142959B9F43B1(ECB) | D579EF25025D07E1(CBC)
	//										AA53A1E4CF5F(CFB)     | AA39876590AC(OFB)
    //  HEX     3BAFC1BD1ABDD48B -> 1CFF344E5CA5770D(ECB) | C00B3C4C15854405(CBC)
	//							 -> FE5A77BDF1049849(CFB) | FEF02FE9E17D4100(OFB)
    //  BINARY  1000101010101101010(8AAD400000000000) -> 654DCDC20D5D6B45(ECB) | 88855BEC00870014(CBC)
	//                                                -> 4F76F0(CFB)           | 4FF2A(OFB)
	string key = "0001001100110100010101110111100110011011101111001101111111110001";
	//  ªÒ»°√‹‘ø◊È Ki[]
	bitset<SIZE_SONKEY> Ki[NUM_SONKEY];
	getKeyTable(bitset<SIZE_INPUT>(key), Ki);
	while( true )
	{
	    plainTextMode plMode = _getPlMode();
		string str_plText    = _getPlText();
	    operateMode   opMode = _getOpMode();
		encodeMode    enMode = _getEnMode();
		
        string str_hexCpText;
        if( enMode == CFB )
            str_hexCpText = DES_CFB(str_plText, plMode, Ki, opMode);
        else
	        str_hexCpText = DES(str_plText, plMode, Ki, opMode, enMode);
    	cout << "str_hexCpText : " << str_hexCpText << endl << endl;
	}
    

    return 0;
}
