#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<iostream>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#define bufsize 1024
//using namespace std;


int main(int argc,char* argv[]) {
	WSADATA wsadata;
	SOCKET clientSocket;
	SOCKADDR_IN  serverAddr;
	int  recvCnt;

	char message[bufsize] = "\0";
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
		std::cout << "WSAStartup() error" << std::endl;

	if ((clientSocket = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		std::cout << "socket()  error" << std::endl;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
	serverAddr.sin_port = htons(atoi(argv[2]));

	if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		std::cout << "connect() error" << std::endl;

	while (1) {
		std::cin.getline(message, bufsize);
		if (strlen(message) == 0) {
			std::cout << "done" << std::endl;
			break;
		}
		std::cout << "message to server:" << message << std::endl;
		send(clientSocket, message, strlen(message), 0);
		memset(message, 0, sizeof(message));
		/*recv(clientSocket, message, bufsize, 0);
		std::cout << "message from server:" << message << std::endl;*/
	}
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}