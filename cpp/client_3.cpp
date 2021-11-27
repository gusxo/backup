#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include"aes.h"

// client2에서
// 키교환 방식 변경(diffle-hellman)
// id/password 전송 전 hash 적용


unsigned int parseuint(char* bytes){
	unsigned int val = 0;
	for(int i = 0;i<4;++i){
		val <<= 8;
		val |= (unsigned char)bytes[i];
	}
	return val;
}
void uint_to_bytes(char* bytes, unsigned int val){
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
std::vector<unsigned char> uint_to_byteVector(unsigned int val){
	std::vector<unsigned char> result(4, 0);
	for(int i =0;i<4;++i){
		result[i] = (char)(val >> (3-i) * 8);
	}
	return result;
}
unsigned int parseuint(std::vector<unsigned char>& byteVector, int start){
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

		//diffie-hellman 키교환
		std::random_device rd;
		std::mt19937 gen(rd());
		//recv p, g
		recv(sock, buf, 4, 0);
		auto p = parseuint(buf);
		recv(sock, buf, 4, 0);
		auto g = parseuint(buf);
		std::cout<<"recv {p, g} : { " << p << " , " << g <<" }\n";

		//select b
		std::uniform_int_distribution<int> dis(1, 100);
		auto b = dis(gen);
		std::cout<<"b selected : " << b <<"\n";

		//g^a%p 수신
		recv(sock, buf, 4, 0);
		auto key_a = parseuint(buf);
		//g^b%p 계산 & 송신
		unsigned int key_b = 1;
		for(int i =0;i<b;++i){
				key_b = (key_b * g) % p;
		}
		uint_to_bytes(buf, key_b);
		send(sock, buf, 4, 0);

		//key 계산
		unsigned int key = 1;
		for(int i =0;i<b;++i){
			key = (key * key_a) % p;
		}
		std::cout << "key : " << key << "\n";

		//암호화 엔진 초기화
		std::vector<unsigned char> keyvec(16, 0);	//key값을 aes 클래스에서 요구하는 형태로 변환
		keyvec[15] = key;
		aes engine("ecb", keyvec);


		//암호화 수행
		int pad_userId, pad_userPw, pad_unused;
		std::string unhashed_userId(argv[2]), unhashed_userPw(argv[3]);
		std::string hashed_userId = std::to_string(std::hash<std::string>{}(unhashed_userId + "5"));
		std::string hashed_userPw = std::to_string(std::hash<std::string>{}(unhashed_userPw + "5"));
		std::vector<unsigned char> userId(hashed_userId.begin(), hashed_userId.end());
		auto enc_userId = engine.encrypto(userId, pad_userId);
		std::vector<unsigned char> userPw(hashed_userPw.begin(), hashed_userPw.end());
		auto enc_userPw = engine.encrypto(userPw, pad_userPw);
		auto val_vec = uint_to_byteVector(val);
		auto enc_val = engine.encrypto(val_vec, pad_unused);
		std::cout << "encrypto complete\n\n";
		std::cout << "- E(userId) size : " << enc_userId.size() << " / hex : " << engine.bytes_to_hexstr(enc_userId) << "\n";
		std::cout << "- E(userPw) size : " << enc_userPw.size() << " / hex : " << engine.bytes_to_hexstr(enc_userPw) << "\n";
		std::cout << "- E(val) size : " << enc_val.size() << " / hex : " << engine.bytes_to_hexstr(enc_val) << "\n";

		//암호화 결과에 따른 길이 정보 전송
		char header[16];
		uint_to_bytes(header, enc_userId.size());
		uint_to_bytes(header+4, pad_userId);
		uint_to_bytes(header+8, enc_userPw.size());
		uint_to_bytes(header+12, pad_userPw);
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
		std::cout << "recv result : " << parseuint(result, 0) << "\n";

		close(sock);
	}
	catch (std::exception err) {
		std::cout << err.what();
		exit(1);
	}
}