#include <iostream>
#include <bitset> 
#include <vector>
#include <string>
#include <cstdio>
using namespace std;
//  要特别注意的是bitset的index由0->size-1是从 低位->高位，即右边->左边

typedef unsigned char Byte;

const Byte SIZE_INPUT  = 64;
const Byte SIZE_OUTPUT = 64;
const Byte SIZE_DIVIDE = 28;
const Byte NUM_SONKEY  = 16;
const Byte SIZE_SONKEY = 48;

enum plainTextMode {TEXT, BINARY, HEX}; //  明文模式：文本，二进制，十六进制
enum operateMode   {ENCODE, DECODE};    //  工作模式：加密，解密

// 初始置换表64bit
const Byte InitPerm_Table[]  = {   
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

// 选择置换表1，28*2 bit，其中未出现的 8,16,24,32,40,48,56,64做奇偶校验位
const Byte PC_Table1[]   = {     
    //  Ci
    57, 49, 41, 33, 25, 17,  9,  
    1,  58, 50, 42, 34, 26, 18, 
    10,  2, 59, 51, 43, 35, 27, 
    19, 11,  3, 60, 52, 44, 36,
    //  Di
    63, 55, 47, 39, 31, 23, 15,  
    7,  62, 54, 46, 38, 30, 22, 
    14,  6, 61, 53, 45, 37, 29, 
    21, 13,  5, 28, 20, 12,  4    
};
// 选择置换表2，6*8 bit
const Byte PC_Table2[] = {
    14, 17, 11, 24,  1,  5,  3, 28, 
    15,  6, 21, 10, 23, 19, 12,  4, 
    26,  8, 16,  7, 27, 20, 13,  2, 
    41, 52, 31, 37, 47, 55, 30, 40, 
    51, 45, 33, 48, 44, 49, 39, 56, 
    34, 53, 46, 42, 50, 36, 29, 32
};
//  左移位数表
const Byte LeftMove_Table[] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};
//  扩展表
const Byte Expansion_Table[] = {
    32,  1,  2,  3,  4,  5,
    4,   5,  6,  7,  8,  9,
    8,   9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};
//  S盒，出于简化只用一个
const Byte SBox_Table[][4][16] = {
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},
    },
    {
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9},
    },
    {
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12},
    },
    {
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14},
    },
    {
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3},
    },
    {
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13},
    },
    {
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12},
    },
    {
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11},
    }  
};
//  P盒
const Byte PBox_Table[] = {
    16, 7,  20, 21, 29, 12, 28, 17, 
    1,  15, 23, 26, 5,  18, 31, 10,
	2,  8,  24, 14, 32, 27, 3,  9, 
    19, 13, 30, 6,  22, 11, 4,  25    
};

// 逆初始置换表64bit
const Byte FinalPerm_Table[] = {   
    40, 8, 48, 16, 56, 24, 64, 32, 
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 
    33, 1, 41,  9, 49, 17, 57, 25
};

//  置换操作模板函数
template <class Input, class Output>
void DES_Permutation(const Input& input, Output& output
        , const Byte Table[], const Byte tableSize)
{
    for(Byte i = 0; i < tableSize; ++i)
        output[tableSize - i - 1] = input[input.size() - Table[i]];
}

//  旋转左移
template <class Input>
void DES_LeftRotation(Input& bs, Byte count)
{
    while( count-- )
    {
        Byte bit = bs[SIZE_DIVIDE - 1];
        bs <<= 1;
        bs[0] = bit;
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
}

//  true 加密， false 解密
void myDES(bitset<SIZE_INPUT>& plaintext, bitset<SIZE_SONKEY> Ki[], 
    bitset<SIZE_OUTPUT>& ciphertext, operateMode opMode, bitset<SIZE_INPUT>& IV)
{
    if( opMode == ENCODE )
        plaintext ^= IV;
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

    if( opMode == ENCODE )
        IV = ciphertext;
    else{
        ciphertext ^= IV;
        IV = plaintext;
    }
}

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


string DES_ECB(string str_plText, plainTextMode plMode, bitset<SIZE_SONKEY> Ki[], operateMode opMode)
{
    bitset<SIZE_INPUT> IV(string("1010001010111011001110001010011011100110110111001100110111011111"));
    //  填充0
    strFillZero(str_plText, plMode);
    //  原有明文
    cout << "str_plText : " << str_plText << endl;
    //  若为文本或十六进制，转换为对应的二进制流
    string str_binaryPlText;
    if( plMode == TEXT )
        str_binaryPlText = str_To_strBinary(str_plText);
    else if( plMode == HEX )
        str_binaryPlText = strHex_To_strBinary(str_plText);
    else
        str_binaryPlText = str_plText;
    //  原有明文二进制和十六进制
    cout << "str_binaryPlText : " << str_binaryPlText << endl;
    cout << "str_plHexText : " << strBinary_To_hex(str_binaryPlText) << endl;
    //  密文二进制流输出
    string str_binaryCpText = "";
    for(int i = 0; i < str_binaryPlText.size(); i += 64)
    {
        //  每次截取64bit明文进行加密
        string sub_binary = str_binaryPlText.substr(i, 64);
        bitset<64> plaintext(sub_binary);
        bitset<64> ciphertext;
        myDES(plaintext, Ki, ciphertext, opMode, IV);

        cout << "plaintext  : " << plaintext  << endl;
        cout << "ciphertext : " << ciphertext << endl << endl;
        //  64bit明文加密后的密文累加
        str_binaryCpText += ciphertext.to_string();
    }
    //  返回密文十六进制
    return strBinary_To_hex(str_binaryCpText);
}

int main()
{
    cout << "------------------DES CODE------------------" << endl;
    bitset<SIZE_INPUT>   key(string("0001001100110100010101110111100110011011101111001101111111110001"));
    cout << "key  : " << key  << endl << endl;
    bitset<SIZE_SONKEY> Ki[NUM_SONKEY];
    getKeyTable(key, Ki);

    //  TEXT    ofi1kl(6F6669316B6C0000) -> E23142959B9F43B1(ECB) | D579EF25025D07E1(CBC)
    //  HEX     3BAFC1BD1ABDD48B -> 1CFF344E5CA5770D
    //  BINARY  1000101010101101010(8AAD400000000000) -> 654DCDC20D5D6B45
    string str_plText = "D579EF25025D07E1";
    string str_hexCpText = DES_ECB(str_plText, HEX, Ki, DECODE);
    cout << "str_hexCpText : " << str_hexCpText << endl;

    return 0;
}