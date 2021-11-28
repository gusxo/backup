#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<fstream>
#include<random>
#include<unordered_map>

//server1에서 RSA적용

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


void enc(char* in, char* out, int len, unsigned int N, unsigned int e){
	for(int i =0;i<len;++i){
		unsigned long long result = 1;
		for(int j =0;j<e;++j){
			result = (result * in[i]) % N;
		}
		uint_to_bytes(out+(i*4), result);
	}
}
void dec(char* in, char* out, int len, unsigned int N, unsigned int d){
	for(int i =0;i<len;i+=4){
		auto val = parseuint(in + i);
		unsigned long long result = 1;
		for(int j =0;j<d;++j){
			result = (result * val) % N;
		}
		out[i/4] = result;
	}
}
unsigned int gcd(unsigned int a, unsigned int b){
	if(b==0){
		return a;
	}
	return gcd(b, a%b);
}
void rsa_genkey(int min, int max, int& N, int& e, int& d){
	//RSA key 생성을위한 초기화
	std::mt19937 gen(std::random_device{}());
	std::vector<bool> eratos(max + 1, true);
	for(int i = 2; i<eratos.size(); ++i){
		if(!eratos[i]) continue;
		for(int j = i*i; j < eratos.size(); j += i){
			eratos[j] = false;
		}
	}
	std::vector<int> primes;
	//100~10000 범위의 소수
	for(int i = min; i<eratos.size(); ++i){
		if(eratos[i]){
			primes.push_back(i);
		}
	}

	//select p,q,e,d, N
	std::uniform_int_distribution<unsigned int> dis(1, primes.size());
	auto p = primes[dis(gen)-1];
	auto q = primes[dis(gen)-1];
	while(p == q){
		q = primes[dis(gen)-1];
	}
	auto phi = (p-1)*(q-1);
	e = 2;
	while (gcd(e, phi) != 1)
	{
		e++;
	}
	
	d = 1;
	while (((unsigned long long)d*e)%phi != 1)
	{
		d++;
	}
	
	N = p*q;
	std::cout<<"rsa_genkey() : {p, q, phi, e, d, N} : { " << p << " , " << q << " , " << phi << " , " << e << " , " << d << " , " << N <<" }\n";
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
		char buf1[500];
		char buf2[500];

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
			
			int se, sd, sN;
			rsa_genkey(100, 10000, sN, se, sd);
			std::cout<<"e selected : " << se <<"\n";
			std::cout<<"d selected : " << sd <<"\n";
			std::cout<<"N selected : " << sN <<"\n";

			//send N,e
			uint_to_bytes(buf1, sN);
			uint_to_bytes(buf1+4, se);
			send(client_sock, buf1, 8, 0);
			std::cout<<"N, e send\n";

			//client측 N, e 수신
			recv(client_sock, buf1, 8, 0);
			auto cN = parseuint(buf1);
			auto ce = parseuint(buf1+4);
			std::cout<<"{N, e} 수신 : { " << cN << " , " << ce << " }\n";

			//id-password recv
			recv(client_sock, buf2, 500, 0);
			dec(buf2, buf1, 500, sN, sd);
			std::string userId(buf1);
			recv(client_sock, buf2, 500, 0);
			dec(buf2, buf1, 500, sN, sd);
			std::string userPw(buf1);

			//auth
			buf1[0] = 0;
			if(pair.find(userId) != pair.end()){
				if(userPw == pair[userId]){
					buf1[0] = 1;
				}
			}
			enc(buf1, buf2, 1, cN, ce);
			send(client_sock, buf2, 4, 0);
			if(buf1[0] == 0){
				std::cout<<"auth failed. connect close.\n";
				close(client_sock);	
				continue;
			}
			std::cout<<"auth success\n";

			//데이터 처리
			recv(client_sock, buf2, 16, 0);
			dec(buf2, buf1, 16, sN, sd);
			unsigned int val = parseuint(buf1);
			unsigned int result = val*val;		//오버플로우 허용
			uint_to_bytes(buf1, result);
			std::cout<< "recv value : " << val << "\n";
			std::cout<<"result value : "<< result << "\n";
			enc(buf1, buf2, 4, cN, ce);
			send(client_sock, buf2, 16, 0);
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