#include "des.h"

unsigned long long des::extendbit(unsigned int bit32){
	unsigned long long bit34 = ((unsigned long long)bit32 << 1) | ((unsigned long long)(bit32 & 1) << 33) | (bit32 >> 31);
	unsigned int lsh = 0;
	unsigned long long filter = (1 << 6) - 1;
	unsigned long long result = 0;
	for (int i = 0; i < 8; ++i) {
		result |= (bit34 & filter) << lsh;
		filter <<= 4;
		lsh += 2;
	}
	return result;
}

unsigned long long des::sbox(unsigned long long bit48){
	unsigned int rsh = 0;
	unsigned int lsh = 0;
	unsigned int const col_filter = ((1 << 4) - 1) << 1;
	unsigned long long filter = (1 << 6) - 1;
	unsigned long long result = 0;
	for (int i = 0; i < 8; ++i) {
		unsigned long long bit6 = (bit48 & filter) >> rsh;
		unsigned int col = (bit6 & col_filter) >> 1;
		unsigned int row = ((bit6 >> 5) << 1) | (bit6 & 1);
		result |= (unsigned long long)sbox_table[i][row][col] << lsh;
		lsh += 4;
		rsh += 6;
		filter <<= 6;
	}
	return result;
}

void des::round(int n, unsigned long long& left, unsigned long long& right){

	unsigned long long extended_right = extendbit((unsigned int)right);
	unsigned long long xored = extended_right ^ subkeys[n];
	unsigned long long changed_right = sbox(xored);
	unsigned long long result = left ^ changed_right;
	left = right;
	right = result;
}

unsigned long long des::next_subkey(int n, unsigned long long& lk, unsigned long long& rk){
	int shift = ((n + 1) * n / 2) % 28;
	lk = lk << shift;
	unsigned long long lk_outed = (lk & ~bitpattern28) >> 28;
	lk = (lk & bitpattern28) | lk_outed;
	rk = rk << shift;
	unsigned long long rk_outed = (rk & ~bitpattern28) >> 28;
	rk = (rk & bitpattern28) | rk_outed;
	unsigned long long subkey = ((lk >> 4) << 24) | (rk & bitpattern24);
	return subkey;
}

void des::setmode(std::string mode)
{
	if (mode == "ecb") {
		is_cbcmode = false;
	}
	else if (mode == "cbc") {
		is_cbcmode = true;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<unsigned long long > dis(0, UINT64_MAX);
		IV = dis(gen);
	}
	else {
		throw ("mode : 'ecb' or 'cbc' 만 허용");
	}
}

des::des(std::string mode)
{
	setmode(mode);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned long long > dis(0, key_maxvalue);
	setkey(dis(gen));
}

des::des(std::string mode, unsigned long long key)
{
	setmode(mode);
	setkey(key);
}

des::des(unsigned long long cbc_IV, unsigned long long key)
{
	setmode("cbc");
	setkey(key);
	setIV(IV);
}


unsigned long long des::encrypto_a_block(unsigned long long plaintext){
	unsigned long long left = plaintext >> 32;
	unsigned long long right = plaintext & UINT32_MAX;
	for (int i = 0; i < 16; ++i) {
		round(i,left, right);
	}
	return (left << 32) | (right);
}

unsigned long long des::decrypto_a_block(unsigned long long ciphertext){
	unsigned long long right = ciphertext >> 32;
	unsigned long long left = ciphertext & UINT32_MAX;
	for (int i = 15; i >= 0; --i) {
		round(i, left, right);
	}
	return (left) | (right << 32);
}

std::vector<unsigned char> des::encrypto(std::vector<unsigned char>& plainbytes, int& out_padding){
	int size = plainbytes.size();
	std::vector<unsigned char> cipherbytes(((size-1)/8+1) * 8, 0);
	out_padding = cipherbytes.size() - size;
	unsigned long long before_cipherblock;
	for (int i = 0; i < size;i+=8) {
		unsigned int rsh = 56;
		unsigned long long plain = 0;
		for (int j = 0; j < 8; ++j) {
			if (i + j >= size) {
				break;
			}
			plain |= (unsigned long long)plainbytes[i + j] << rsh;
			rsh -= 8;
		}
		//cbc mode일경우 전처리
		if (is_cbcmode) {
			plain ^= i ? before_cipherblock : IV;
		}
		auto cipher = encrypto_a_block(plain);
		before_cipherblock = cipher;
		unsigned long long filter = byte_filter_init;
		rsh = 56;
		for (int j = 0; j < 8; ++j) {
			cipherbytes[i + j] = (char)((cipher & filter) >> rsh);
			rsh -= 8;
			filter >>= 8;
		}
	}
	return cipherbytes;
}

void des::encrypto_file(const char* inputfile, const char* outputfile, int& out_padding)
{
	std::fstream in, out;
	in.open(inputfile, std::ios::in | std::ios::binary | std::ios::ate);
	if (!in.is_open()) {
		throw ("입력 파일 열기 실패\n");
	}
	out.open(outputfile, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!out.is_open()) {
		throw ("출력 파일 열기 실패\n");
	}
	auto size = in.tellg();
	char* buf = new char[size];
	in.seekg(0, std::ios::beg);
	in.read(buf, size);
	std::vector<unsigned char> bytes(buf, buf + size);
	delete[] buf;
	auto e = encrypto(bytes, out_padding);
	buf = new char[e.size()];
	for (int i = 0; i < e.size(); ++i) {
		buf[i] = e[i];
	}
	out.write(buf, e.size());
	delete[] buf;
	
	in.close();
	out.close();
}

std::vector<unsigned char> des::decrypto(std::vector<unsigned char>& cipherbytes, int padding){
	int size = cipherbytes.size();
	if (size % 8 != 0) {
		throw ("cipherbytes : length error");
	}
	if (padding < 0 || padding > 7) {
		throw ("padding : range error");
	}
	std::vector<unsigned char> plainbytes(size - padding, 0);
	unsigned long long before_cipherblock;

	for (int i = 0; i < size; i += 8) {
		unsigned int rsh = 56;
		unsigned long long cipher = 0;
		for (int j = 0; j < 8; ++j) {
			cipher |= (unsigned long long)cipherbytes[i + j] << rsh;
			rsh -= 8;
		}
		auto plain = decrypto_a_block(cipher);

		//cbc mode일경우 후처리
		if (is_cbcmode) {
			plain ^= i ? before_cipherblock : IV;
		}
		before_cipherblock = cipher;

		unsigned long long filter = byte_filter_init;
		rsh = 56;
		for (int j = 0; j < 8; ++j) {
			if (i + j >= size - padding) {
				break;
			}
			plainbytes[i + j] = (char)((plain & filter) >> rsh);
			rsh -= 8;
			filter >>= 8;
		}
	}
	return plainbytes;
}

void des::decrypto_file(const char* inputfile, const char* outputfile, int padding)
{
	std::fstream in, out;
	in.open(inputfile, std::ios::in | std::ios::binary | std::ios::ate);
	if (!in.is_open()) {
		throw ("입력 파일 열기 실패\n");
	}
	out.open(outputfile, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!out.is_open()) {
		throw ("출력 파일 열기 실패\n");
	}
	auto size = in.tellg();
	char* buf = new char[size];
	in.seekg(0, std::ios::beg);
	in.read(buf, size);
	std::vector<unsigned char> bytes(buf, buf + size);
	delete[] buf;
	auto d = decrypto(bytes, padding);
	buf = new char[d.size()];
	for (int i = 0; i < d.size(); ++i) {
		buf[i] = d[i];
	}
	out.write(buf, d.size());
	delete[] buf;

	in.close();
	out.close();
}

void des::loadsetting(const char* filename)
{
	std::fstream in;
	in.open(filename, std::ios::in);
	if (!in.is_open()) {
		throw ("입력 파일 열기 실패\n");
	}
	std::string line;
	std::getline(in, line);
	auto key = std::stoull(line);
	setkey(key);
	if (is_cbcmode) {
		std::getline(in, line);
		auto IV = std::stoull(line);
		setIV(IV);
	}
	in.close();
}

void des::savesetting(const char* filename)
{
	std::fstream out;
	out.open(filename, std::ios::out | std::ios::trunc);
	if (!out.is_open()) {
		throw ("출력 파일 열기 실패\n");
	}
	out << getkey() << "\n";
	if (is_cbcmode) {
		out << getIV() << "\n";
	}
	out.close();
}


unsigned long long des::getkey(){
	return key;
}

void des::setkey(unsigned long long key){
	if (key > key_maxvalue) {
		throw ("require key length : 56 bit");
	}
	this->key = key;
	unsigned long long lk = key >> 28;
	unsigned long long rk = key & bitpattern28;
	for (int i = 0; i < 16; ++i) {
		subkeys[i] = next_subkey(i + 1, lk, rk);
	}
}

unsigned long long des::getIV()
{
	if (!is_cbcmode) {
		throw ("cbc 모드가 아닙니다.");
	}
	return IV;
}

void des::setIV(unsigned long long IV)
{
	if (!is_cbcmode) {
		throw ("cbc 모드가 아닙니다.");
	}
	this->IV = IV;
}
