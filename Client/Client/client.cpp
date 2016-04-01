/*
Simple UDP Server
Silver Moon ( m00n.silv3r@gmail.com )
*/

#include<stdio.h>
#include<iostream>
#include<winsock2.h>
#include<ws2ipdef.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

SOCKET s;
struct sockaddr_in server, si_other;
int slen, recv_len;
char buf[BUFLEN];
WSADATA wsa;

int initialize() {
	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("WSA initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Bind done.\n");

	struct ip_mreq mreq;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	mreq.imr_multiaddr.s_addr = inet_addr("224.198.87.101");
	setsockopt(s, IPPROTO_IP, 12, (const char *)&mreq, sizeof(mreq)); // 12 = IP_ADD_MEMBERSHIP
	int err = WSAGetLastError();

	return 0;
}

int look_at(int x, int y) {
	std::cout << "(" << x << "," << y << ")" << std::endl;
	return 0;
}

int main()
{
	initialize();

	slen = sizeof(si_other);
	while (1)
	{
		// Clear the buffer
		memset(buf, '\0', BUFLEN);

		// Receive data
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			std::cout << "recvfrom() failed with error code : " << WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}

		int x = atoi(strtok(buf, ";"));
		int y = atoi(strtok(NULL, ";"));

		look_at(x, y);
	}

	closesocket(s);
	WSACleanup();

	return 0;
}