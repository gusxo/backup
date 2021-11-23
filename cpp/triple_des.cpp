#include "triple_des.h"
triple_des::triple_des(std::string mode) : des1(mode), des2(mode), mode(mode) {}

triple_des::triple_des(std::string mode, unsigned long long key1, unsigned long long key2) : des1(mode, key1), des2(mode, key2), mode(mode) {}

std::vector<unsigned char> triple_des::encrypto(std::vector<unsigned char>& plainbytes, int& out_padding)
{
	auto c = des1.encrypto(plainbytes, out_padding);
	int tmp;
	auto d = des2.decrypto(c, 0);
	return des1.encrypto(d, tmp);
}

void triple_des::encrypto_file(const char* inputfile, const char* outputfile, int& out_padding)
{
	int tmp;
	des1.encrypto_file(inputfile, "3DEStmp1.bin", out_padding);
	des2.decrypto_file("3DEStmp1.bin", "3DEStmp2.bin", 0);
	des1.encrypto_file("3DEStmp2.bin", outputfile, tmp);
}

std::vector<unsigned char> triple_des::decrypto(std::vector<unsigned char>& cipherbytes, int padding)
{
	auto d = des1.decrypto(cipherbytes, 0);
	int tmp;
	auto c = des2.encrypto(d, tmp);
	return des1.decrypto(c, padding);
}

void triple_des::decrypto_file(const char* inputfile, const char* outputfile, int padding)
{
	int tmp;
	des1.decrypto_file(inputfile, "3DEStmp1.bin", 0);
	des2.encrypto_file("3DEStmp1.bin", "3DEStmp2.bin", tmp);
	des1.decrypto_file("3DEStmp2.bin", outputfile, padding);
}

void triple_des::loadsetting(const char* filename)
{
	std::fstream in;
	in.open(filename, std::ios::in);
	if (!in.is_open()) {
		throw ("입력 파일 열기 실패\n");
	}
	std::string line;
	std::getline(in, line);
	auto key1 = std::stoull(line);
	std::getline(in, line);
	auto key2 = std::stoull(line);
	setkeys(key1, key2);
	if (mode == "cbc") {
		std::getline(in, line);
		auto IV1 = std::stoull(line);
		std::getline(in, line);
		auto IV2 = std::stoull(line);
		setIVs(IV1, IV2);
	}
	in.close();
}

void triple_des::savesetting(const char* filename)
{
	std::fstream out;
	out.open(filename, std::ios::out | std::ios::trunc);
	if (!out.is_open()) {
		throw ("출력 파일 열기 실패\n");
	}
	unsigned long long key1, key2, IV1, IV2;
	getkeys(key1, key2);
	out << key1 << "\n" << key2 << "\n";
	if (mode == "cbc") {
		getIVs(IV1, IV2);
		out << IV1 << "\n" << IV2 << "\n";
	}
	out.close();
}

void triple_des::getkeys(unsigned long long& key1, unsigned long long& key2)
{
	key1 = des1.getkey();
	key2 = des2.getkey();
}

void triple_des::setkeys(unsigned long long key1, unsigned long long key2)
{
	des1.setkey(key1);
	des2.setkey(key2);
}

void triple_des::getIVs(unsigned long long& IV1, unsigned long long& IV2)
{
	IV1 = des1.getIV();
	IV2 = des2.getIV();
}

void triple_des::setIVs(unsigned long long IV1, unsigned long long IV2)
{
	des1.setIV(IV1);
	des2.setIV(IV2);
}
