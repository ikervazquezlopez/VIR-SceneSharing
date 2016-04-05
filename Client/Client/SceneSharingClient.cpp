#include<stdio.h>
#include<iostream>
#include<winsock2.h>
#include<ws2ipdef.h>

#include "SceneSharingClient.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library


SOCKET s;
WSADATA wsa;
struct sockaddr_in server, si_other;

int slen, recv_len;
char buf[BUFLEN];

int * coords;



int SceneSharingClient_initialize() {

	slen = sizeof(si_other);

	// Initialise winsock
	std::cout << "Initialising Winsock..." << std::endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "Failed. Error Code : " << WSAGetLastError() << std::endl;
		return -1;
	}
	std::cout << "WSA initialised." << std::endl;

	// Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Could not create socket : " << WSAGetLastError() << std::endl;
		return -1;
	}
	std::cout << "Socket created." << std::endl;

	// Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(CLIENT_PORT);

	// Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "Bind failed with error code : " << WSAGetLastError() << std::endl;
		return -1;
	}
	std::cout << "Bind done." << std::endl;

	// Join multicast group
	struct ip_mreq mreq;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
	if (setsockopt(s, IPPROTO_IP, 12, (const char *)&mreq, sizeof(mreq)) == SOCKET_ERROR) { // 12 = IP_ADD_MEMBERSHIP
		std::cout << "Multicast group joining failed with error code : " << WSAGetLastError() << std::endl;
		return -1;
	}
	std::cout << "Joined to multicast group at " << MULTICAST_GROUP << ":" << CLIENT_PORT << std::endl;

	return 0;
}




int SceneSharingClient_look_at(int x, int y) {
	std::cout << "(" << x << "," << y << ")" << std::endl;
	return 0;
}


void SceneSharingClient_receive_loop() {
	while (1)
	{
		// Clear the buffer
		memset(buf, '\0', BUFLEN);

		// Receive data from server
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			std::cout << "recvfrom() failed with error code : " << WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}

		int x = atoi(strtok(buf, ";"));
		int y = atoi(strtok(NULL, ";"));

		// Set the look at the coordinates received from the server
		SceneSharingClient_look_at(x, y);
	}
}




int SceneSharingClient_finish() {
	if (closesocket(s) == SOCKET_ERROR) {
		std::cout << "Socket close failed with error code : " << WSAGetLastError() << std::endl;
		return -1;
	}
	WSACleanup();

	return 0;
}