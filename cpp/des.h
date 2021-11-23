#pragma once
#include"crypto_module.h"
#include<string>
#include<sstream>
#include<random>
#include<bitset>
#include<iomanip>
#include<iostream>
#include<vector>
class des : public crypto_module
{
private:
	unsigned long long key;
	unsigned short sbox_table[8][4][16] = {
		{
			{0xE, 4, 0xD, 1, 2, 0xF, 0xB, 8, 3, 0xA, 6, 0xC, 5, 9, 0, 7},
			{0, 0xF, 7, 4, 0xE, 2, 0xD, 1, 0xA, 6, 0xC, 0xB, 9, 5, 3, 8},
			{4, 1, 0xE, 8, 0xD, 6, 2, 0xB, 0xF, 0xC, 9, 7, 3, 0xA, 5, 0},
			{0xF, 0xC, 8, 2, 4, 9, 1, 7, 5, 0xB, 3, 0xE, 0xA, 0, 6, 0xD}
		},
		{
			{0xF, 1, 8, 0xE, 6, 0xB, 3, 4, 9, 7, 2, 0xD, 0xC, 0, 5, 0xA},
			{3, 0xD, 4, 7, 0xF, 2, 8, 0xE, 0xC, 0, 1, 0xA, 6, 9, 0xB, 5},
			{0, 0xE, 7, 0xB, 0xA, 4, 0xD, 1, 5, 8, 0xC, 6, 9, 3, 2, 0xF},
			{0xD, 8, 0xA, 1, 3, 0xF, 4, 2, 0xB, 6, 7, 0xC, 0, 5, 0xE, 9}
		},
		{
			{0xA, 0, 9, 0xE, 6, 3, 0xF, 5, 1, 0xD, 0xC, 7, 0xB, 4, 2, 8},
			{0xD, 7, 0, 9, 3, 4, 6, 0xA, 2, 8, 5, 0xE, 0xC, 0xB, 0xF, 1},
			{0xD, 6, 4, 9, 8, 0xF, 3, 0, 0xB, 1, 2, 0xC, 5, 0xA, 0xE, 7},
			{1, 0xA, 0xD, 0, 6, 9, 8, 7, 4, 0xF, 0xE, 3, 0xB, 5, 2, 0xC}
		},
		{
			{7, 0xD, 0xE, 3, 0, 6, 9, 0xA, 1, 2, 8, 5, 0xB, 0xC, 4, 0xF},
			{0xD, 8, 0xB, 5, 6, 0xF, 0, 3, 4, 7, 2, 0xC, 1, 0xA, 0xE, 9},
			{0xA, 6, 9, 0, 0xC, 0xB, 7, 0xD, 0xF, 1, 3, 0xE, 5, 2, 8, 4},
			{3, 0xF, 0, 6, 0xA, 1, 0xD, 8, 9, 4, 5, 0xB, 0xC, 7, 2, 0xE}
		},
		{
			{2, 0xC, 4, 1, 7, 0xA, 0xB, 6, 8, 5, 3, 0xF, 0xD, 0, 0xE, 9},
			{0xE, 0xB, 2, 0xC, 4, 7, 0xD, 1, 5, 0, 0xF, 0xA, 3, 9, 8, 6},
			{4, 2, 1, 0xB, 0xA, 0xD, 7, 8, 0xF, 9, 0xC, 5, 6, 3, 0, 0xE},
			{0xB, 8, 0xC, 7, 1, 0xE, 2, 0xD, 6, 0xF, 0, 9, 0xA, 4, 5, 3}
		},
		{
			{0xC, 1, 0xA, 0xF, 9, 2, 6, 8, 0, 0xD, 3, 4, 0xE, 7, 5, 0xB},
			{0xA, 0xF, 4, 2, 7, 0xC, 9, 5, 6, 1, 0xD, 0xE, 0, 0xB, 3, 8},
			{9, 0xE, 0xF, 5, 2, 8, 0xC, 3, 7, 0, 4, 0xA, 1, 0xD, 0xB, 6},
			{4, 3, 2, 0xC, 9, 5, 0xF, 0xA, 0xB, 0xE, 1, 7, 6, 0, 8, 0xD}
		},
		{
			{4, 0xB, 2, 0xE, 0xF, 0, 8, 0xD, 3, 0xC, 9, 7, 5, 0xA, 6, 1},
			{0xD, 0, 0xB, 7, 4, 9, 1, 0xA, 0xE, 3, 5, 0xC, 2, 0xF, 8, 6},
			{1, 4, 0xB, 0xD, 0xC, 3, 7, 0xE, 0xA, 0xF, 6, 8, 0, 5, 9, 2},
			{6, 0xB, 0xD, 8, 1, 4, 0xA, 7, 9, 5, 0, 0xF, 0xE, 2, 3, 0xC}
		},
		{
			{0xD, 2, 8, 4, 6, 0xF, 0xB, 1, 0xA, 9, 3, 0xE, 5, 0, 0xC, 7},
			{1, 0xF, 0xD, 8, 0xA, 3, 7, 4, 0xC, 5, 6, 0xB, 0, 0xE, 9, 2},
			{7, 0xB, 4, 1, 9, 0xC, 0xE, 2, 0, 6, 0xA, 0xD, 0xF, 3, 5, 8},
			{2, 1, 0xE, 7, 4, 0xA, 8, 0xD, 0xF, 0xC, 9, 0, 3, 5, 6, 0xB}
		},
	};
	unsigned long long subkeys[16];
	unsigned long long const key_maxvalue = ((unsigned long long)1 << 56) - 1;
	unsigned long long const bitpattern28 = (1 << 28) - 1;
	unsigned long long const bitpattern24 = (1 << 24) - 1;
	unsigned long long const byte_filter_init = (unsigned long long)UINT8_MAX << 56;
	unsigned long long extendbit(unsigned int bit32);
	unsigned long long sbox(unsigned long long bit48);
	unsigned long long encrypto_a_block(unsigned long long plaintext);
	unsigned long long decrypto_a_block(unsigned long long ciphertext);
	void round(int n, unsigned long long& left, unsigned long long& right);
	unsigned long long next_subkey(int n, unsigned long long& lk, unsigned long long& rk);
	bool is_cbcmode;
	unsigned long long IV;
	void setmode(std::string mode);
public:
	des(std::string mode);
	des(std::string mode, unsigned long long key);
	des(unsigned long long cbc_IV, unsigned long long key);
	virtual std::vector<unsigned char> encrypto(std::vector<unsigned char>& plainbytes, int& out_padding);
	virtual void encrypto_file(const char* inputfile, const char* outputfile, int& out_padding);
	virtual std::vector<unsigned char> decrypto(std::vector<unsigned char>& cipherbytes, int padding);
	virtual void decrypto_file(const char* inputfile, const char* outputfile, int padding);
	virtual void loadsetting(const char* filename);
	virtual void savesetting(const char* filename);
	unsigned long long getkey();
	void setkey(unsigned long long key);
	unsigned long long getIV();
	void setIV(unsigned long long IV);
};

