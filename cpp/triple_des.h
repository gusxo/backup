#pragma once
#include"des.h"
#include"crypto_module.h"

class triple_des : public crypto_module
{
private:
	des des1;
	des des2;
	std::string mode;
public:
	triple_des(std::string mode);
	triple_des(std::string mode, unsigned long long key1, unsigned long long key2);
	virtual std::vector<unsigned char> encrypto(std::vector<unsigned char>& plainbytes, int& out_padding);
	virtual void encrypto_file(const char* inputfile, const char* outputfile, int& out_padding);
	virtual std::vector<unsigned char> decrypto(std::vector<unsigned char>& cipherbytes, int padding);
	virtual void decrypto_file(const char* inputfile, const char* outputfile, int padding);
	virtual void loadsetting(const char* filename);
	virtual void savesetting(const char* filename);
	void getkeys(unsigned long long& key1, unsigned long long& key2);
	void setkeys(unsigned long long key1, unsigned long long key2);
	void getIVs(unsigned long long& IV1, unsigned long long& IV2);
	void setIVs(unsigned long long IV1, unsigned long long IV2);
};

