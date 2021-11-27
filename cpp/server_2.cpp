#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<fstream>
#include<unordered_map>
#include"aes.h"

// client1에 symmetric key crypto 적용
// 적용 알고리즘 : AES (ECB mode)

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
	std::cout << "usage : " << argv[0] << " 'id-password file' \n";
	std::cout<<"id-password 파일은 한 줄 마다 1개의 id-password 쌍을 저장하며 공백으로 구분한다.\n\n";

	//id-password 파일 불러오기
	std::ifstream fin;
	std::unordered_map<std::string, std::string> pair;	//id-password pair
	std::string str;
	try{
		std::string filename(argv[1]);
		fin.open(filename);
		while(std::getline(fin,str)){
			int blankpos = str.find(' ');
			auto id = str.substr(0,blankpos);
			auto password = str.substr(blankpos+1);
			pair[id] = password;
			std::cout<< "load : " << id << " / " << password<<"\n";
		}
	}
	catch(std::exception err){
		if(fin.is_open()){
			fin.close();
		}
		std::cout<<"error : id-password 파일을 불러오는 것에 실패하였습니다.\n";
		exit(1);
	}
	std::cout<<"id-password load complete.\n";
	

	//서버
	try {
		char buf[120];

		int server_sock;
		int client_sock;
		int recvSize;
		int unused;
		struct sockaddr_in server_addr;
		struct sockaddr_in client_addr;
		socklen_t client_addr_size;

		server_sock=socket(PF_INET, SOCK_STREAM, 0);
				
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family=AF_INET;
		server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
		server_addr.sin_port=htons(5000);
		
		if(bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr))==-1 )
			throw ("bind() fail"); 
		
		if(listen(server_sock, 10)==-1)
			throw ("listen() fail");
		while(1){
			std::cout<<"\nwaiting for connection....\n";
			client_addr_size=sizeof(client_addr);  
			client_sock=accept(server_sock, (struct sockaddr*)&client_addr,&client_addr_size);
			if(client_sock==-1)
				throw ("accept() fail");  
			
			std::cout<<"accepted\n";

			//key 생성
			aes engine("ecb");
			auto key = engine.getkey();

			//key 교환
			byteVector_to_bytes(buf, key);
			send(client_sock, buf, 16, 0);
			std::cout<< "send key\n";

			//길이 정보 수신
			recvSize = recv(client_sock, buf, 16, 0);
			if(recvSize < 0){
				std::cout<<"connect error\n";
				close(client_sock);	
				continue;
			}
			else if(recvSize == 0){
				std::cout<<"connect closed\n";
				close(client_sock);	
				continue;
			}
			auto enc_header = bytes_to_byteVector(buf, 16);
			auto header = engine.decrypto(enc_header, 0);	//원본이 16byte이므로 padding 0
			auto size_userId = parseuint(header, 0);
			auto pad_userId = parseuint(header, 4);
			auto size_userPw = parseuint(header, 8);
			auto pad_userPw = parseuint(header, 12);

			if(size_userId - pad_userId > 100 || size_userPw - pad_userPw > 100 || pad_userId > 15 || pad_userPw > 15){
				std::cout<<"recv header : Invalid value\nconnect close...\n";
				close(client_sock);	
				continue;
			}
			std::cout<<"recv header\n";


			//id recv
			recvSize = recv(client_sock, buf, size_userId, 0);
			if(recvSize < 0){
				std::cout<<"connect error\n";
				close(client_sock);	
				continue;
			}
			else if(recvSize == 0){
				std::cout<<"connect closed\n";
				close(client_sock);	
				continue;
			}
			auto enc_userId = bytes_to_byteVector(buf, size_userId);
			auto userId_bytes = engine.decrypto(enc_userId, pad_userId);
			std::string userId(userId_bytes.begin(), userId_bytes.end());
			std::cout<<"recv id : " << userId << "\n";

			//password recv
			recvSize = recv(client_sock, buf, size_userPw, 0);
			if(recvSize < 0){
				std::cout<<"connect error\n";
				close(client_sock);	
				continue;
			}
			else if(recvSize == 0){
				std::cout<<"connect closed\n";
				close(client_sock);	
				continue;
			}
			auto enc_userPw = bytes_to_byteVector(buf, size_userPw);
			auto userPw_bytes = engine.decrypto(enc_userPw, pad_userPw);
			std::string userPw(userPw_bytes.begin(), userPw_bytes.end());
			std::cout << "recv password : " << userPw << "\n";

			//auth
			std::vector<unsigned char> auth_result(1, 0);
			if(pair.find(userId) != pair.end()){
				if(userPw == pair[userId]){
					auth_result[0] = 1;
				}
			}
			auto enc_auth_result = engine.encrypto(auth_result, unused);
			byteVector_to_bytes(buf, enc_auth_result);
			send(client_sock, buf, 16, 0);
			if(auth_result[0] == 0){
				std::cout<<"auth failed. connect close.\n";
				close(client_sock);	
				continue;
			}
			std::cout<<"auth success\n";

			//데이터 처리
			recvSize = recv(client_sock, buf, 16, 0);
			if(recvSize < 0){
				std::cout<<"connect error\n";
				close(client_sock);	
				continue;
			}
			else if(recvSize == 0){
				std::cout<<"connect closed\n";
				close(client_sock);	
				continue;
			}
			auto enc_val = bytes_to_byteVector(buf, 16);
			auto val_vec = engine.decrypto(enc_val, 12);
			auto val = parseuint(val_vec, 0);
			unsigned int result = val*val;		//오버플로우 허용
			std::cout<< "recv value : " << val << "\n";
			std::cout<<"result value : "<< result << "\n";
			
			auto result_vec = uint_to_byteVector(result);
			auto enc_result = engine.encrypto(result_vec, unused);
			byteVector_to_bytes(buf, enc_result);
			send(client_sock, buf, 16, 0);
			std::cout<<"send result, connect close...\n";

			close(client_sock);	
		}
		close(server_sock);
	}
	catch (std::exception err) {
		std::cout << err.what();
		exit(1);
	}
}