#pragma once
#include<vector>
#include<fstream>
#include<sstream>
#include<iomanip>
class crypto_module
{
public:
	virtual std::vector<unsigned char> encrypto(std::vector<unsigned char>& plainbytes, int& out_padding)=0;
	virtual void encrypto_file(const char* inputfile, const char* outputfile, int& out_padding)=0;
	virtual std::vector<unsigned char> decrypto(std::vector<unsigned char>& cipherbytes, int padding)=0;
	virtual void decrypto_file(const char* inputfile, const char* outputfile, int padding)=0;
	virtual void loadsetting(const char* filename)=0;
	virtual void savesetting(const char* filename)=0;
	std::vector<unsigned char> hexstr_to_bytes(std::string& str);
	std::string bytes_to_hexstr(std::vector<unsigned char>& bytes);
};
