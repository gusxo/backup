#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<fstream>
#include<unordered_map>

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
		char buf[100];

		int server_sock;
		int client_sock;
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
			
			int recvSize;
			//id recv
			recvSize = recv(client_sock, buf, 100, 0);
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
			std::string userId(buf);
			std::cout<<"recv id : " << userId << "\n";

			//password recv
			recvSize = recv(client_sock, buf, 100, 0);
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
			std::string userPw(buf);
			std::cout<<"recv password : " << userPw << "\n";

			//auth
			buf[0] = 0;
			if(pair.find(userId) != pair.end()){
				if(userPw == pair[userId]){
					buf[0] = 1;
				}
			}
			send(client_sock, buf, 1, 0);
			if(buf[0] == 0){
				std::cout<<"auth failed. connect close.\n";
				close(client_sock);	
				continue;
			}
			std::cout<<"auth success\n";

			//데이터 처리
			recvSize = recv(client_sock, buf, 4, 0);
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
			unsigned int val = parseuint8(buf);
			unsigned int result = val*val;		//오버플로우 허용
			uint8_to_bytes(buf, result);
			std::cout<< "recv value : " << val << "\n";
			std::cout<<"result value : "<< result << "\n";
			send(client_sock, buf, 4, 0);
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