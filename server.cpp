#include<iostream>
#include<WinSock2.h>
#include "SQL.h"
#include "Reader.h"

#pragma comment(lib,"ws2_32.lib")

#define bufsize 1024

//using namespace std;

int main(int argc,char* argv[]) {
	WSADATA wsadata;
	SOCKET serverSocket, clientSocket;
	int szClientAddr, fdnum, str_len;
	SOCKADDR_IN  serverAddr, clientAddr;
	fd_set reads, cpyReads;
	TIMEVAL timeout;
	char message[bufsize] = "\0";

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		//std::cout << "WSAStartup() error" << std::endl;
	}

	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		//std::cout << "socket()  error" << std::endl;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(atoi(argv[1]));

	if (::bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		//std::cout << "bind() error" << std::endl;
	}

	listen(serverSocket, 5);
	//std::cout << "connected" << std::endl;

	FD_ZERO(&reads);  //所有初始化为0
	FD_SET(serverSocket, &reads);  //将服务器套接字存入
	
	map<int, shared_ptr<SQL>> sockid2sql;		//每个客户端的套接字对应一个SQL调度器
	map<int, shared_ptr<Reader>> sockid2reader;

	while (1) {
		cpyReads = reads;
		timeout.tv_sec = 5;      //5秒
		timeout.tv_usec = 5000;   //5000毫秒

		//找出监听中发出请求的套接字
		if ((fdnum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
			break;
		if (fdnum == 0) {
			//std::cout << "time out" << std::endl;
			continue;
		}
		for (unsigned int i = 0; i < reads.fd_count; i++) {
			if (FD_ISSET(reads.fd_array[i], &cpyReads)) { //判断是否为发出请求的套接字
				if (reads.fd_array[i] == serverSocket) {  //是否为服务器套接字
					szClientAddr = sizeof(clientAddr);
					clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &szClientAddr);
					if (clientSocket == INVALID_SOCKET) {
						//cout << "accept() error" << endl;
					}
					FD_SET(clientSocket, &reads);
					//std::cout << "connected client:" << clientSocket << std::endl;
					sockid2sql[clientSocket] = make_shared<SQL>();
					sockid2reader[clientSocket] = make_shared<Reader>();
				}
				else {//否  就是客户端
					str_len = recv(reads.fd_array[i], message, bufsize - 1, 0);
					
					if (str_len == 0) {//根据接受数据的大小 判断是否是关闭
						FD_CLR(reads.fd_array[i], &reads);  //清除数组中该套接字
						closesocket(cpyReads.fd_array[i]);
						//std::cout << "closed client:" << cpyReads.fd_array[i] << std::endl;
						auto pos = sockid2sql.find(cpyReads.fd_array[i]);
						auto pos1 = sockid2reader.find(cpyReads.fd_array[i]);
						assert(pos != sockid2sql.end() && pos1!=sockid2reader.end());
						sockid2sql.erase(pos);
						sockid2reader.erase(pos1);
					}
					else {
						message[str_len] = 0;
						//std::cout << "message from client:" << message << std::endl;
						shared_ptr<Instruction> inst=sockid2reader[reads.fd_array[i]]->read(message);
						inst->exec(*(sockid2sql[reads.fd_array[i]]));
						//send(reads.fd_array[i], message, str_len, 0);
					}
				}
			}
		}
	}
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}

/*
#include<iostream>
#include<sstream>
#include <WinSock2.h>
#include<string.h>
#include<cctype>
#include <stdlib.h>
#include <stdio.h>
#include "SQL.h"
#include "Reader.h"
using namespace std;

#define BUF_SIZE 1024

const int MAX_CLIENT = 5;

void error_handling(const char* message) {
	cout << message << endl;
	exit(1);
}

int main(int argc,char* argv[]){
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;
	TIMEVAL timeout;
	fd_set reads, cpyReads;
	int szClntAddr;
	int strLen,fdNum,i;
	char buf[BUF_SIZE];

	if (argc != 2) {
		printf("Usage:%s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		error_handling("wsastartup() error!");
	}

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET) {
		error_handling("socket() error");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		error_handling("bind() error");
	}

	if (listen(hServSock, MAX_CLIENT) == SOCKET_ERROR) {
		error_handling("listen() error");
	}

	FD_ZERO(&reads);
	FD_SET(hServSock, &reads);

	while (1) {
		cpyReads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR) {
			break;
		}
		if (fdNum == 0) {		//timeout
			printf("time out!");
			continue;
		}
		for (i = 0; i < reads.fd_count; i++) {
			if (FD_ISSET(reads.fd_array[i], &cpyReads)) {
				if (reads.fd_array[i] == hServSock) {
					szClntAddr = sizeof(clntAddr);
					hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
					if (hClntSock == INVALID_SOCKET) {
						printf("accept() error");
					}
					FD_SET(hClntSock, &reads);
					printf("Connected Client:%d \n", hClntSock);
				}
			}
			else {
				strLen = recv(reads.fd_array[i], buf, BUF_SIZE - 1, 0);
				if (strLen == 0) {
					FD_CLR(reads.fd_array[i], &reads);
					closesocket(cpyReads.fd_array[i]);
					printf("Closed Client:%d \n", cpyReads.fd_array[i]);
				}
				else {
					send(reads.fd_array[i], buf, strLen, 0);	//echo
				}
			}
		}
	}

	

	for (int i = 0; i < MAX_CLIENT; i++) {
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
		if (hClntSock == INVALID_SOCKET) {
			error_handling("accept() error");
		}
		else {
			printf("Connected Client:%d\n", i+1);
		}
		while ((strLen = recv(hClntSock, message, BUF_SIZE, 0)) != 0) {
			send(hClntSock, message, strLen, 0);
		}
		closesocket(hClntSock);
	}
	closesocket(hClntSock);
	closesocket(hServSock);
	WSACleanup();

    SQL sql;
    Reader reader;
    shared_ptr<Instruction> inst;
    while(inst=reader.read()){
        inst->exec(sql);
    }
    return 0;
}

*/