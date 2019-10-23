#pragma once

#include <iostream>
#include <algorithm>    //  transform函数令模式强制转大写
#include "constvar.h"
#include "strConversion.h"

string        _getPlText();
plainTextMode _getPlMode();
operateMode   _getOpMode();
encodeMode    _getEnMode();

void getKeyTable(const bitset<SIZE_INPUT>& key, bitset<SIZE_SONKEY> Ki[]);

void myDES(bitset<SIZE_INPUT>& plaintext, bitset<SIZE_SONKEY> Ki[],
    bitset<SIZE_OUTPUT>& ciphertext, operateMode opMode);

string DES(string str_plText, plainTextMode plMode, bitset<SIZE_SONKEY> Ki[],
	operateMode opMode, encodeMode enMode);
	
string DES_CFB(string str_plText, plainTextMode plMode, bitset<SIZE_SONKEY> Ki[], operateMode opMode);

//  要特别注意的是bitset的index由0->size-1是从 低位->高位，即右边->左边
//  置换操作模板函数
template <typename Input, typename Output>
void DES_Permutation(const Input& input, Output& output
        , const Byte Table[], const Byte tableSize)
{
    for(Byte i = 0; i < tableSize; ++i)
        output[tableSize - i - 1] = input[input.size() - Table[i]];
}

//  旋转左移
template <typename Input>
void DES_LeftRotation(Input& bs, Byte count)
{
    while( count-- )
    {
        Byte bit = bs[SIZE_DIVIDE - 1];
        bs <<= 1;
        bs[0] = bit;
    }
}
