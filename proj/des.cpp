#include "des.h"

void getKeyTable(const bitset<SIZE_INPUT>& key, bitset<SIZE_SONKEY> Ki[])
{
    //  ��������56bit��K
    bitset<SIZE_DIVIDE*2> K;
    DES_Permutation(key, K, PC_Table1, SIZE_DIVIDE*2);
    string str_K = K.to_string();
    //  ѡ���û����ɳ�ʼC0, D0
    cout << "ѡ���û����ɳ�ʼC0, D0" << endl;
    bitset<SIZE_DIVIDE> Ci(str_K.substr(0, SIZE_DIVIDE));
    bitset<SIZE_DIVIDE> Di(str_K.substr(SIZE_DIVIDE));
    cout << Ci << endl << Di << endl << endl;

    //  ����16������Կ
    cout << "����16������Կ" << endl;
    for(Byte i = 0; i < NUM_SONKEY; ++i){
        //  ��ת����
        DES_LeftRotation(Ci, LeftMove_Table[i]);
        DES_LeftRotation(Di, LeftMove_Table[i]);
        //  �ϲ��û�
        bitset<2*SIZE_DIVIDE> bs_merge(Ci.to_string() + Di.to_string());
        DES_Permutation(bs_merge, Ki[i], PC_Table2, SIZE_SONKEY);
        cout << "K[" << i+1 << "] = " << Ki[i] << endl;
    }
}

//  true ���ܣ� false ����
void myDES(bitset<SIZE_INPUT>& plaintext, bitset<SIZE_SONKEY> Ki[],
    bitset<SIZE_OUTPUT>& ciphertext, operateMode opMode)
{
    //  ���ĳ�ʼ�û�
    bitset<SIZE_OUTPUT> IP;
    cout << "��ʼ�û�" << endl;
    cout << "data : " << plaintext << endl;
    DES_Permutation(plaintext, IP, InitPerm_Table, SIZE_INPUT);
    cout << "IP:    " << IP << endl << endl;
    //  ���ɳ�ʼL0, R0
    string str_IP = IP.to_string();
    bitset<SIZE_INPUT/2> Li(str_IP.substr(0, SIZE_INPUT/2)), tmpL;
    bitset<SIZE_INPUT/2> Ri(str_IP.substr(SIZE_INPUT/2))   , tmpR;
    //cout << "L0 : " << Li << endl;
    //cout << "R0 : " << Ri << endl << endl;
    //  16�μ���
    for(Byte i = 0; i < NUM_SONKEY; ++i){
        tmpL = Li; tmpR = Ri;
        //cout << "Li : " << Li << endl;
        //cout << "Ri : " << Ri << endl;
        bitset<SIZE_SONKEY> Ei; //  �����м���
        //  ��չ��48bit������Ei
        DES_Permutation(Ri, Ei, Expansion_Table, SIZE_SONKEY);
        //  ����ԿKi���
        if( opMode == ENCODE ) Ei ^= Ki[i];
        else Ei ^= Ki[15 - i];
        //cout << "E" << i+1 << " : " << Ei << "  ";
        //  res�ݴ�ת������ַ���
        //  ת��ʱ��6λΪ1�飬ӳ�䵽S��
        string res = "";
        for(Byte j = 0; j < SIZE_SONKEY; j += 6){
            string str_col = Ei.to_string().substr(j+1, 4);
            //  ת���С���
            Byte row = Ei[SIZE_SONKEY-j-1]*2 + Ei[SIZE_SONKEY-j-6];
            Byte col = bitset<4>(str_col).to_ulong();
            //printf("(%d,%d)", row, col);
            //  ��ȡS-BOX��ֵ
            Byte value = SBox_Table[j/6][row][col];
            //printf("%d  ", value);
            res += bitset<4>(value).to_string();
        }
        //cout << endl;
        //cout << "S" << i+1 << " : " << res << endl;
        //  ��ת����32bit
        bitset<32> bs_tmp(res);
        //  P���û�
        DES_Permutation(bs_tmp, Ri, PBox_Table, SIZE_INPUT/2);
        Ri ^= tmpL;
        //cout << "R" << i+1 << " : " << Ri << endl << endl;
        //  ���һ�ֲ����ٽ���
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
	//  ��ʼ IV
	bitset<SIZE_INPUT> IV(string("1010001010111011001110001010011011100110110111001100110111011111"));
	//  ��ȡ��Կ�� Ki[]
	bitset<SIZE_SONKEY> Ki[NUM_SONKEY];
	getKeyTable(bitset<SIZE_INPUT>(key), Ki);
    //  8�ֽ�Ϊһ�飬��������0
    strFillZero(str_plText, plMode);
    //	��ȡ���ĵĶ�������
    string str_binaryPlText = getStrBinary(str_plText, plMode);
    //  ���Ķ����������
    string str_binaryCpText = "";
    for(int i = 0; i < str_binaryPlText.size(); i += SIZE_INPUT)
    {
		//  ÿ�ν�ȡ64bit���Ľ��м���
		string sub_binary = str_binaryPlText.substr(i, SIZE_INPUT);
		bitset<SIZE_INPUT>  plaintext(sub_binary);
		bitset<SIZE_OUTPUT> ciphertext;
        
		//  ECBģʽ
		if( enMode == ECB ){
			myDES(plaintext, Ki, ciphertext, opMode);
		}
		//  CBCģʽ
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
        //  64bit���ļ��ܺ�������ۼ�
        str_binaryCpText += ciphertext.to_string();
    }
    
    return strBinary_To_hex(str_binaryCpText);
}
