
#include<stdio.h>
#include <iostream>
#include<winsock2.h>
#include<ws2ipdef.h>
#include <Windows.h>

#include "SceneSharingMaster.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

WSADATA wsa;
SOCKET s;
struct sockaddr_in si_other;
int slen = sizeof(si_other);
char buf[BUFLEN];
char * message;

int x = 100;
int y = 100;

int SceneSharingMaster_initialize() {
	//Initialise winsock
	std::cout << "Initialising Winsock..." << std::endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "Failed. Error Code : " << WSAGetLastError() << std::endl;
		return -1;
	}
	std::cout << "Initialised." << std::endl;

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		std::cout << "socket() failed with error code : " << WSAGetLastError() << std::endl;
		return -1;
	}

	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(MASTER_PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(MULTICAST_GROUP);

	return 0;
}


void SceneSharingMaster_send_loop() {
	while (1)
	{
		if (GetAsyncKeyState(VK_RIGHT)) {
			x += 10;
		}
		if (GetAsyncKeyState(VK_LEFT)) {
			x -= 10;
		}
		if (GetAsyncKeyState(VK_UP)) {
			y += 10;
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			y -= 10;
		}

		memset(buf, '\0', BUFLEN);
		sprintf(buf, "%d;%d", x, y);
		message = buf;

		//send the message
		if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
			std::cout << "sendto() failed with error code : " << WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}
		Sleep(100);
	}
}


int SceneSharingMaster_finish() {
	if (closesocket(s) == SOCKET_ERROR) {
		std::cout << "Socket close failed with error code : " << WSAGetLastError() << std::endl;
		return -1;
	}
	WSACleanup();

	return 0;
}