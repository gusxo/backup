#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<random>

//client1에서 RSA, hash 적용

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
	if(argc < 5){
		printf("usage : %s 'server-ip' 'id' 'password' 'unsigned int'\n", argv[0]);
		return 0;
	}
	unsigned int val = std::stoul(argv[4]);
	auto idsize = strlen(argv[2]);
	auto pwsize = strlen(argv[3]);
	std::string unhashed_userId(argv[2]), unhashed_userPw(argv[3]);
	std::string hashed_userId = std::to_string(std::hash<std::string>{}(unhashed_userId + "5"));
	std::string hashed_userPw = std::to_string(std::hash<std::string>{}(unhashed_userPw + "5"));
	if(idsize > 100 || pwsize > 100){
		std::cout<< "error : arguments max length : 100\n";
		return 0;
	}

	

	//작업 수행
	try {
		char buf1[500];
		char buf2[500];

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

		//RSA N, e 수신
		recv(sock, buf1, 8, 0);
		auto sN = parseuint(buf1);
		auto se = parseuint(buf1+4);
		std::cout<<"{N, e} 수신 : { " << sN << " , " << se << " }\n";

		int ce, cd, cN;
		rsa_genkey(100, 10000, cN, ce, cd);

		//send N,e
		uint_to_bytes(buf1, cN);
		uint_to_bytes(buf1+4, ce);
		send(sock, buf1, 8, 0);
		std::cout<<"N, e send\n";

		//id-password 송신
		strcpy(buf1, hashed_userId.c_str());
		enc(buf1, buf2, hashed_userId.size(), sN, se);
		send(sock, buf2, 500, 0);
		strcpy(buf1, hashed_userPw.c_str());
		enc(buf1, buf2, hashed_userPw.size(), sN, se);
		send(sock, buf2, 500, 0);
		std::cout<<"id-password 송신\n";

		//인증 결과 수신
		recv(sock, buf2, 4, 0);
		dec(buf2, buf1, 4, cN, cd);
		if(buf1[0] != 1){
			std::cout<<"auth failed.\n";
			close(sock);
			return 0;
		}
		std::cout<<"auth success\n";

		
		uint_to_bytes(buf1, val);
		enc(buf1, buf2, 4, sN, se);
		send(sock, buf2, 16, 0);
		std::cout<<"send value\n";

		recv(sock, buf2, 16, 0);
		dec(buf2, buf1, 16, cN, cd);
		auto result = parseuint(buf1);
		std::cout<<"recv result : " <<result <<"\n";

		close(sock);
	}
	catch (std::exception err) {
		std::cout << err.what();
		exit(1);
	}
}