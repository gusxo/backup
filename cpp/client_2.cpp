#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include"aes.h"

// client1에 symmetric key crypto 적용
// 적용 알고리즘 : AES (ECB mode)


unsigned int parseuint8(char* bytes){
	unsigned int val = 0;
	for(int i = 0;i<4;++i){
		val <<= 8;
		val |= (unsigned char)bytes[i];
	}
	return val;
}
void uint8_to_bytes(char* bytes, unsigned int val){
	for(int i =0;i<4;++i){
		bytes[i] = (char)(val >> (3-i) * 8);
	}
}
std::vector<unsigned char> bytes_to_byteVector(char* bytes, int size){
	std::vector<unsigned char> result(size, 0);
	for(int i =0;i<size;++i){
		result[i] = bytes[i];
	}
	return result;
}
void byteVector_to_bytes(char* bytes, std::vector<unsigned char>& byteVector){
	for(auto byte : byteVector){
		*bytes = byte;
		bytes++;
	}
}
std::vector<unsigned char> uint8_to_byteVector(unsigned int val){
	std::vector<unsigned char> result(4, 0);
	for(int i =0;i<4;++i){
		result[i] = (char)(val >> (3-i) * 8);
	}
	return result;
}
unsigned int parseuint8(std::vector<unsigned char>& byteVector, int start){
	unsigned int val = 0;
	for(int i = 0;i<4;++i){
		val <<= 8;
		val |= byteVector[i + start];
	}
	return val;
}
int main(int argc, char** argv) {
	if(argc < 5){
		printf("usage : %s 'server-ip' 'id' 'password' 'unsigned int'\n", argv[0]);
		return 0;
	}
	unsigned int val = std::stoul(argv[4]);
	auto idsize = strlen(argv[2]);
	auto pwsize = strlen(argv[3]);
	if(idsize > 100 || pwsize > 100){
		std::cout<< "error : arguments max length : 100\n";
		return 0;
	}

	//작업 수행
	try {
		char buf[120];

		int sock = socket(PF_INET, SOCK_STREAM, 0);
		struct sockaddr_in server_addr;
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family=AF_INET;
		server_addr.sin_addr.s_addr=inet_addr(argv[1]);
		server_addr.sin_port=htons(5000);

		std::cout<<"connecting.....\n";
		if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
			throw "connect fail\n";

		std::cout<<"connect 성공\n";

		//recv key
		recv(sock, buf, 16, 0);
		auto key = bytes_to_byteVector(buf, 16);
		aes engine("ecb", key);
		std::cout << "recv key\n";


		//암호화 수행
		int pad_userId, pad_userPw, pad_unused;
		auto userId = bytes_to_byteVector(argv[2], idsize);
		auto enc_userId = engine.encrypto(userId, pad_userId);
		auto userPw = bytes_to_byteVector(argv[3], pwsize);
		auto enc_userPw = engine.encrypto(userPw, pad_userPw);
		auto val_vec = uint8_to_byteVector(val);
		auto enc_val = engine.encrypto(val_vec, pad_unused);
		std::cout << "encrypto complete\n\n";
		std::cout << "- E(userId) size : " << enc_userId.size() << " / hex : " << engine.bytes_to_hexstr(enc_userId) << "\n";
		std::cout << "- E(userPw) size : " << enc_userPw.size() << " / hex : " << engine.bytes_to_hexstr(enc_userPw) << "\n";
		std::cout << "- E(val) size : " << enc_val.size() << " / hex : " << engine.bytes_to_hexstr(enc_val) << "\n";

		//암호화 결과에 따른 길이 정보 전송
		char header[16];
		uint8_to_bytes(header, enc_userId.size());
		uint8_to_bytes(header+4, pad_userId);
		uint8_to_bytes(header+8, enc_userPw.size());
		uint8_to_bytes(header+12, pad_userPw);
		auto headerVec = bytes_to_byteVector(header, 16);
		auto enc_header = engine.encrypto(headerVec, pad_unused);
		byteVector_to_bytes(buf, enc_header);
		send(sock, buf, 16, 0);

		std::cout << "- E(header) size : " << enc_header.size() << " / hex : " << engine.bytes_to_hexstr(enc_header) << "\n";
		std::cout << "\nheader send\n";

		//id-password 전송
		byteVector_to_bytes(buf, enc_userId);
		send(sock, buf, enc_userId.size(), 0);
		byteVector_to_bytes(buf, enc_userPw);
		send(sock, buf, enc_userPw.size(), 0);
		std::cout<<"send id/password\n";

		//인증 결과 수신
		recv(sock, buf, 16, 0);
		auto enc_authresult = bytes_to_byteVector(buf, 16);
		auto authresult = engine.decrypto(enc_authresult, 15);
		if(authresult[0] != 1){
			std::cout<<"auth failed.\n";
			close(sock);
			return 0;
		}
		std::cout<<"auth success\n";

		//send val
		byteVector_to_bytes(buf, enc_val);
		send(sock, buf, 16, 0);
		std::cout<<"send value\n";

		//recv result
		recv(sock, buf, 16, 0);
		auto enc_result = bytes_to_byteVector(buf, 16);
		auto result = engine.decrypto(enc_result, 12);
		std::cout << "recv result : " << parseuint8(result, 0) << "\n";

		close(sock);
	}
	catch (std::exception err) {
		std::cout << err.what();
		exit(1);
	}
}