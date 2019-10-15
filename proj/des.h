#pragma once

#include <iostream>
#include <cstdio>
#include "constvar.h"
#include "strConversion.h"

//  Ҫ�ر�ע�����bitset��index��0->size-1�Ǵ� ��λ->��λ�����ұ�->���
//  �û�����ģ�庯��
template <class Input, class Output>
void DES_Permutation(const Input& input, Output& output
        , const Byte Table[], const Byte tableSize)
{
    for(Byte i = 0; i < tableSize; ++i)
        output[tableSize - i - 1] = input[input.size() - Table[i]];
}

//  ��ת����
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

void getKeyTable(const bitset<SIZE_INPUT>& key, bitset<SIZE_SONKEY> Ki[]);

void myDES(bitset<SIZE_INPUT>& plaintext, bitset<SIZE_SONKEY> Ki[],
    bitset<SIZE_OUTPUT>& ciphertext, operateMode opMode);
    
string DES_ECB(string str_plText, plainTextMode plMode, bitset<SIZE_SONKEY> Ki[],
	operateMode opMode);
	
string DES(string str_plText, plainTextMode plMode, string key,
	operateMode opMode, encodeMode enMode);


