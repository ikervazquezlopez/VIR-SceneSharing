/*
Simple udp client
Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h>
#include <iostream>
#include<winsock2.h>
#include<ws2ipdef.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library


#define BUFLEN 512  // Max length of buffer
#define MULTICAST_GROUP "224.198.87.101"  // Multicast gropu to send the data to
#define MASTER_PORT 8887   //The port on which to send the data to the server

struct sockaddr_in si_other;
int s, slen = sizeof(si_other);
char buf[BUFLEN];
char * message;
WSADATA wsa;

int initialize() {
	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(MASTER_PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(MULTICAST_GROUP);

	return 0;
}

int main(void)
{
	
	initialize();
	

	//start communication
	while (1)
	{
		message = "300;450";

		//send the message
		if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}