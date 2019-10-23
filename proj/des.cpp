#include "des.h"

string _getPlText()
{
	string str_plText;
	cout << "请输入明文：";
	cin  >> str_plText;
	return str_plText;
}

plainTextMode _getPlMode()
{
	string plMode;
	cout << "请输入明文模式(TEXT, BINARY, HEX)：";
	cin  >> plMode;
	transform(plMode.begin(), plMode.end(), plMode.begin(), ::toupper);

	if( plMode == "BINARY" ){
		cout << "（选择了BINARY模式！）" << endl;
		return BINARY;
	}else if( plMode == "HEX" ){
		cout << "（选择了HEX模式！）" << endl;
		return HEX;
	}else{
		cout << "（选择了TEXT模式！）" << endl;
		return TEXT;
	}
}

operateMode _getOpMode()
{
	string  opMode;
	cout << "请输入工作模式(ENCODE, DECODE)：";
	cin  >> opMode;
	transform(opMode.begin(), opMode.end(), opMode.begin(), ::toupper);

	if( opMode == "ENCODE" ){
		cout << "（选择了加密模式！）"  << endl;
		return ENCODE;
	}else{
		cout << "（选择了解密模式！）" << endl;
		return DECODE;
	}
}

encodeMode  _getEnMode()
{
	string enMode;
	cout << "请输入加密模式(ECB, CBC, CFB, OFB)：";
	cin  >> enMode;
	transform(enMode.begin(), enMode.end(), enMode.begin(), ::toupper);

	if( enMode == "CBC" ){
		cout << "（选择了CBC模式！）" << endl;
		return CBC;
	}else if( enMode == "CFB" ){
		cout << "（选择了CFB模式！）" << endl;
		return CFB;
	}else if( enMode == "OFB" ){
		cout << "（选择了OFB模式！）" << endl;
		return OFB;
	}else{
		cout << "（选择了ECB模式！）" << endl;
		return ECB;
	}
}

void getKeyTable(const bitset<SIZE_INPUT>& key, bitset<SIZE_SONKEY> Ki[])
{
    //  交换生成56bit的K
    bitset<SIZE_DIVIDE*2> K;
    DES_Permutation(key, K, PC_Table1, SIZE_DIVIDE*2);
    string str_K = K.to_string();
    //  选择置换生成初始C0, D0
    cout << "选择置换生成初始C0, D0" << endl;
    bitset<SIZE_DIVIDE> Ci(str_K.substr(0, SIZE_DIVIDE));
    bitset<SIZE_DIVIDE> Di(str_K.substr(SIZE_DIVIDE));
    cout << Ci << endl << Di << endl << endl;

    //  生成16个子密钥
    cout << "生成16个子密钥" << endl;
    for(Byte i = 0; i < NUM_SONKEY; ++i){
        //  旋转左移
        DES_LeftRotation(Ci, LeftMove_Table[i]);
        DES_LeftRotation(Di, LeftMove_Table[i]);
        //  合并置换
        bitset<2*SIZE_DIVIDE> bs_merge(Ci.to_string() + Di.to_string());
        DES_Permutation(bs_merge, Ki[i], PC_Table2, SIZE_SONKEY);
        cout << "K[" << i+1 << "] = " << Ki[i] << endl;
    }
    cout << endl;
}

//  true 加密， false 解密
void myDES(bitset<SIZE_INPUT>& plaintext, bitset<SIZE_SONKEY> Ki[],
    bitset<SIZE_OUTPUT>& ciphertext, operateMode opMode)
{
    //  明文初始置换
    bitset<SIZE_OUTPUT> IP;
    cout << "初始置换" << endl;
    cout << "data : " << plaintext << endl;
    DES_Permutation(plaintext, IP, InitPerm_Table, SIZE_INPUT);
    cout << "IP:    " << IP << endl << endl;
    //  生成初始L0, R0
    string str_IP = IP.to_string();
    bitset<SIZE_INPUT/2> Li(str_IP.substr(0, SIZE_INPUT/2)), tmpL;
    bitset<SIZE_INPUT/2> Ri(str_IP.substr(SIZE_INPUT/2))   , tmpR;
    //cout << "L0 : " << Li << endl;
    //cout << "R0 : " << Ri << endl << endl;
    //  16次加密
    for(Byte i = 0; i < NUM_SONKEY; ++i){
        tmpL = Li; tmpR = Ri;
        //cout << "Li : " << Li << endl;
        //cout << "Ri : " << Ri << endl;
        bitset<SIZE_SONKEY> Ei; //  保存中间结果
        //  扩展至48bit保存至Ei
        DES_Permutation(Ri, Ei, Expansion_Table, SIZE_SONKEY);
        //  与密钥Ki异或
        if( opMode == ENCODE ) Ei ^= Ki[i];
        else Ei ^= Ki[15 - i];
        //cout << "E" << i+1 << " : " << Ei << "  ";
        //  res暂存转化后的字符串
        //  转化时以6位为1组，映射到S盒
        string res = "";
        for(Byte j = 0; j < SIZE_SONKEY; j += 6){
            string str_col = Ei.to_string().substr(j+1, 4);
            //  转化行、列
            Byte row = Ei[SIZE_SONKEY-j-1]*2 + Ei[SIZE_SONKEY-j-6];
            Byte col = bitset<4>(str_col).to_ulong();
            //printf("(%d,%d)", row, col);
            //  获取S-BOX的值
            Byte value = SBox_Table[j/6][row][col];
            //printf("%d  ", value);
            res += bitset<4>(value).to_string();
        }
        //cout << endl;
        //cout << "S" << i+1 << " : " << res << endl;
        //  再转换回32bit
        bitset<32> bs_tmp(res);
        //  P盒置换
        DES_Permutation(bs_tmp, Ri, PBox_Table, SIZE_INPUT/2);
        Ri ^= tmpL;
        //cout << "R" << i+1 << " : " << Ri << endl << endl;
        //  最后一轮不用再交换
        Li  = tmpR;
    }
    cout << "L16 : " << Li << endl;
    cout << "R16 : " << Ri << endl;

    bitset<64> res(Ri.to_string() + Li.to_string());
    DES_Permutation(res, ciphertext, FinalPerm_Table, SIZE_OUTPUT);
}

string DES(string str_plText, plainTextMode plMode, bitset<SIZE_SONKEY> Ki[],
	operateMode opMode, encodeMode enMode)
{
	//  初始 IV
	bitset<SIZE_INPUT> IV(string("1010001010111011001110001010011011100110110111001100110111011111"));
    //  8字节为一组，不足的填充0
    int fillSize = strFillZero(str_plText, plMode);
    //	获取明文的二进制流
    string str_binaryPlText = getStrBinary(str_plText, plMode);
    //  密文二进制流输出
    string str_binaryCpText = "";
    for(int i = 0; i < str_binaryPlText.size(); i += SIZE_INPUT)
    {
		//  每次截取64bit明文进行加密
		string sub_binary = str_binaryPlText.substr(i, SIZE_INPUT);
		bitset<SIZE_INPUT>  plaintext(sub_binary);
		bitset<SIZE_OUTPUT> ciphertext;

		cout << "plaintext  : " << plaintext  << endl;
		//  ECB 电子密本模式
		if( enMode == ECB ){
			myDES(plaintext, Ki, ciphertext, opMode);
		}
		//  CBC 密文分组链接模式
		else if( enMode == CBC )
		{
			if( opMode == ENCODE )
				plaintext ^= IV;
			myDES(plaintext, Ki, ciphertext, opMode);
			if( opMode == ENCODE )
				IV = ciphertext;
			else{
				ciphertext ^= IV;
				IV = plaintext;
			}
		}
        //  OFB 输出反馈模式
        else if( enMode == OFB ){
        	//  OFB模式加/解密时使用密钥Ki的顺序一致
            myDES(IV, Ki, ciphertext, ENCODE);
            IV = ciphertext;
            ciphertext ^= plaintext;
        }
		cout << "ciphertext : " << ciphertext << endl << endl;

		//  64bit明文加密后的密文累加
        str_binaryCpText += ciphertext.to_string();
    }
    
    //  OFB模式需要舍去填充的位
    if( enMode == OFB ){
    	int size = str_binaryCpText.size();
    	str_binaryCpText = str_binaryCpText.substr(0, size - fillSize);
	}
    return strBinary_To_hex(str_binaryCpText);
}

//  CFB 密文反馈模式
//  CFB一次操作的是流密码(8位)，与其它模式冲突
//  强行抽象需要写很多的判断，故单独写一个函数
string DES_CFB(string str_plText, plainTextMode plMode, bitset<SIZE_SONKEY> Ki[]
    , operateMode opMode)
{
	//  初始 IV
	bitset<SIZE_INPUT> IV(string("1010001010111011001110001010011011100110110111001100110111011111"));
    //	获取明文的二进制流
    string str_binaryPlText = getStrBinary(str_plText, plMode);
    //  8位补齐
    int size = str_binaryPlText.size() / 8;
    if( 8*size != str_binaryPlText.size() )
    	str_binaryPlText += string((size+1)*8 - str_binaryPlText.size(), '0');
    cout << "str_binaryPlText.size() = " << str_binaryPlText.size() << endl;
    //  密文二进制流输出
    string str_binaryCpText = "";
    for(int i = 0; i < str_binaryPlText.size(); i += 8)
    {
		//  每次截取8bit明文进行加密
		string sub_binary = str_binaryPlText.substr(i, 8);
		bitset<8>  plaintext(sub_binary);
		bitset<SIZE_OUTPUT> IV_DES;
		cout << "plaintext  : " << plaintext  << endl;
		
		myDES(IV, Ki, IV_DES, ENCODE);
        bitset<8> ciphertext(IV_DES.to_string().substr(0, 8));
        ciphertext ^= plaintext;

        cout << "ciphertext : " << ciphertext << endl << endl;
        str_binaryCpText += ciphertext.to_string();

        IV <<= 8;
        if( opMode == ENCODE )
            IV |= (bitset<64>(ciphertext.to_string()));
        else
            IV |= (bitset<64>(plaintext.to_string()));
	}
    return strBinary_To_hex(str_binaryCpText);
}
