#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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

int main(int argc, char** argv) {
	if(argc < 5){
		printf("usage : %s 'server-ip' 'id' 'password' 'unsigned int'\n", argv[0]);
		return 0;
	}
	unsigned int val = std::stoul(argv[4]);
	auto idsize = strlen(argv[2]);
	auto pwsize = strlen(argv[3]);

	//작업 수행
	try {
		char buf[100];

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

		strcpy(buf, argv[2]);
		buf[(idsize > 99 ? 99 : idsize)] = '\0';
		send(sock, buf, 100, 0);
		std::cout<<"send id\n";

		strcpy(buf, argv[3]);
		buf[(pwsize > 99 ? 99 : pwsize)] = '\0';
		send(sock, buf, 100, 0);
		std::cout<<"send password\n";

		recv(sock, buf, 1, 0);
		if(buf[0] != 1){
			std::cout<<"auth failed.\n";
			close(sock);
			return 0;
		}
		std::cout<<"auth success\n";

		uint_to_bytes(buf, val);
		send(sock, buf, 4, 0);
		std::cout<<"send value\n";

		recv(sock, buf, 4, 0);
		auto result = parseuint(buf);
		std::cout<<"recv result : " <<result <<"\n";

		close(sock);
	}
	catch (std::exception err) {
		std::cout << err.what();
		exit(1);
	}
}