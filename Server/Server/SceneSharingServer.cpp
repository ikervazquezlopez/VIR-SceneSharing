#include<stdio.h>
#include <iostream>
#include<winsock2.h>
#include <ws2ipdef.h>
#include <Windows.h>

#include <time.h>
#include <fstream>

#include "SceneSharingServer.h"
#include "Smoothing.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

WSADATA wsa;
SOCKET s;
struct sockaddr_in si_other, master;

int slen;
int mlen;
int rec_len;
char buf[BUFLEN];
char * message;




int SceneSharingServer_initialize() {

	
	slen = sizeof(si_other);

	// Initialise winsock
	std::cout << "Initialising Winsock..." << std::endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "Failed. Error Code : " << WSAGetLastError() << std::endl;
		return -1;
	}
	std::cout << "Winsock initialised." << std::endl;

	// Create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		std::cout << "socket() failed with error code : " << WSAGetLastError() << std::endl;
		return -1;
	}

	// Prepare the sockaddr_in structure
	master.sin_family = AF_INET;
	master.sin_addr.s_addr = INADDR_ANY;
	master.sin_port = htons(MASTER_PORT);

	// Bind the socket
	if (bind(s, (struct sockaddr *) &master, sizeof(master)) == SOCKET_ERROR)
	{
		std::cout << "Bind failed with error code : " << WSAGetLastError() << std::endl;
		return -1;
	}
	std::cout << "Bind done" << std::endl;

	// Join to multicast address
	struct ip_mreq mreq;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
	if (setsockopt(s, IPPROTO_IP, 12, (const char *)&mreq, sizeof(mreq)) == SOCKET_ERROR) { // 12 = IP_ADD_MEMBERSHIP
		std::cout << "Multicast join failed with error code : " << WSAGetLastError() << std::endl;
		return -1;
	}



	// Setup client address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(CLIENT_PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(MULTICAST_GROUP);

	return 0;
}



char * SceneSharingServer_getMessage(double * coords) {
	char buff[BUFLEN];
	char message[BUFLEN];

	memset(buff, '\0', BUFLEN);
	sprintf(buff, "%d", coords[0]);
	strcpy(message, buff);

	memset(buff, '\0', BUFLEN);
	sprintf(buff, "%lf", coords[1]);

	strcat(message, ";");
	strcat(message, buff);

	return message;
}



void SceneSharingServer_server_loop() {
	Smoothing smooth = Smoothing("averaged", 20);

	while (1)
	{
		if(GetAsyncKeyState(0x41)){ //A key
			smooth.setMode("averaged");
		}
		if (GetAsyncKeyState(0x57)) { //W key
			smooth.setMode("weighted");
		}
		if (GetAsyncKeyState(0x42)) { //B key
			smooth.setMode("bspline");
		}
		memset(buf, '\0', BUFLEN);

		int mlen = sizeof(master);
		// Receive from master
		if ((rec_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &master, &mlen) == SOCKET_ERROR))
		{
			std::cout << "recvfrom() failed with error code : " << WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}

		char * x = strtok(buf, ";");
		char * y = strtok(NULL, ";");

		

		double * coords = smooth.getNewCoordinates(atoi(x), atoi(y));
		std::cout << coords[0] << "|" << coords[1] << std::endl;
		message = SceneSharingServer_getMessage(coords);

		// Send the message
		if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
			std::cout << "sendto() failed with error code : " << WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	/* Write the coordinates into file */
	std::fstream fs;
	/*
	fs.open("MasterCoordinates.txt", std::fstream::out | std::fstream::trunc);
	for (int i = 0; i < smooth.oriCoords.size(); i++) {
		fs << smooth.oriCoords.at(i) << ";";
	}
	fs.close();
	*/ 

	fs.open("AverageCoordinates_x.txt", std::fstream::out | std::fstream::trunc);
	for (int i = 0; i < smooth.aveCoords.size(); i++) {
		fs << smooth.aveCoords.at(i)[0] << ";";
	}
	fs.close();
	fs.open("AverageCoordinates_y.txt", std::fstream::out | std::fstream::trunc);
	for (int i = 0; i < smooth.aveCoords.size(); i++) {
		fs << smooth.aveCoords.at(i)[1] << ";";
	}
	fs.close();

	fs.open("WeightCoordinates_x.txt", std::fstream::out | std::fstream::trunc);
	for (int i = 0; i < smooth.weiCoords.size(); i++) {
		fs << smooth.weiCoords.at(i)[0] << ";";
	}
	fs.close();
	fs.open("WeightCoordinates_y.txt", std::fstream::out | std::fstream::trunc);
	for (int i = 0; i < smooth.weiCoords.size(); i++) {
		fs << smooth.weiCoords.at(i)[1] << ";";
	}
	fs.close();
}



int SceneSharingServer_finish() {
	if (closesocket(s) == SOCKET_ERROR) {
		std::cout << "Socket close failed with error code : " << WSAGetLastError() << std::endl;
		return -1;
	}
	WSACleanup();
	
	return 0;
}