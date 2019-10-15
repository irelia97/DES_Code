#include "des.h"

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

string DES(string str_plText, plainTextMode plMode, string key,
	operateMode opMode, encodeMode enMode)
{
	//  初始 IV
	bitset<SIZE_INPUT> IV(string("1010001010111011001110001010011011100110110111001100110111011111"));
	//  获取密钥组 Ki[]
	bitset<SIZE_SONKEY> Ki[NUM_SONKEY];
	getKeyTable(bitset<SIZE_INPUT>(key), Ki);
    //  8字节为一组，不足的填充0
    strFillZero(str_plText, plMode);
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
        
		//  ECB模式
		if( enMode == ECB ){
			myDES(plaintext, Ki, ciphertext, opMode);
		}
		//  CBC模式
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

        cout << "plaintext  : " << plaintext  << endl;
        cout << "ciphertext : " << ciphertext << endl << endl;
        //  64bit明文加密后的密文累加
        str_binaryCpText += ciphertext.to_string();
    }
    
    return strBinary_To_hex(str_binaryCpText);
}
