#include"crypto_module.h"

std::vector<unsigned char> crypto_module::hexstr_to_bytes(std::string& str) {
	std::vector<unsigned char> bytes;

	for (unsigned int i = 0; i < str.length(); i += 2) {
		std::string byteString = str.substr(i, 2);
		unsigned char byte = (unsigned char)std::stoul(byteString, NULL, 16);
		bytes.push_back(byte);
	}

	return bytes;
}
std::string crypto_module::bytes_to_hexstr(std::vector<unsigned char>& bytes) {
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (auto& x : bytes) {
		ss << std::setw(2) << (unsigned int)x;
	}
	return ss.str();
}