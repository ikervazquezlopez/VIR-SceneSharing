
#include<stdio.h>
#include <iostream>
#include<winsock2.h>
#include<ws2ipdef.h>
#include <Windows.h>

#include <vector>
#include <fstream>

#include "SceneSharingMaster.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

WSADATA wsa;
SOCKET s;
struct sockaddr_in si_other;
int slen = sizeof(si_other);
char buf[BUFLEN];
char * message;

double x = 100;
double y = 100;

double fov_x = 30;
double fov_y = 30;

std::vector<int> coordinates_x = {100,100,100,100,100,100,100,100,100,100,100,110,120,130,140,150,160,170,180,190,200,210,220,230,230,240,250,260,270,280,290,300,310,320,330,340,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,340,330,320,310,300,290,280,270,260,250,250,250,250,250,240,230,220,210,200,190,180,170,160,150,140,130,120,110,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,110,120,130,140,150,160,170,180,190,200,210,220,230,230,230,230,230,230,230,230,230,230,230,220,210,200,190,180,170,160,150,150,150,150,140,130,120,110,100,90,80,70,60,50,40,30,20,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};//std::vector<int>();
std::vector<int> coordinates_y = {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,90,80,70,60,50,40,30,20,10,0,-10,-20,-20,-20,-20,-30,-40,-50,-60,-70,-80,-90,-100,-110,-120,-130,-140,-150,-150,-150,-150,-150,-150,-160,-170,-180,-190,-200,-210,-220,-230,-240,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-250,-240,-230,-220,-210,-200,-190,-180,-170,-160,-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,-140,-130,-120,-110,-100,-90,-90,-90,-80,-70,-60,-50,-40,-30,-20,-10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-10,-20,-30,-40,-50,-60,-70,-80,-90,-100,-110,-120,-130,-140,-150,-160,-170,-180,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190,-190 };//std::vector<int>();

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
			x += 2;
		}
		if (GetAsyncKeyState(VK_LEFT)) {
			x -= 2;
		}
		if (GetAsyncKeyState(VK_UP)) {
			y -= 2;
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			y += 2;
		}

		message = SceneSharingMaster_getMessage(x, y, fov_x, fov_y);

		//coordinates_x.push_back(x);
		//coordinates_y.push_back(y);

		//send the message
		if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
			std::cout << "sendto() failed with error code : " << WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}
		Sleep(100);
	}


	/*
	std::fstream f;
	f.open("coordinates_x.txt", std::fstream::trunc | std::fstream::out);
	for (int i = 0; i < coordinates_x.size(); i++) {
		f << coordinates_x.at(i) << ",";
	}
	f << "\n";
	f.close();

	f.open("coordinates_y.txt", std::fstream::trunc | std::fstream::out);
	for (int i = 0; i < coordinates_y.size(); i++) {
		f << coordinates_y.at(i) << ",";
	}
	f << "\n";
	f.close();
	*/
}


char * SceneSharingMaster_getMessage(double m_x, double m_y, double fov_x, double fov_y) {
	char buff[BUFLEN];
	char message[BUFLEN];

	//Master coordinates
	memset(buff, '\0', BUFLEN);
	sprintf(buff, "%lf", m_x);
	strcpy(message, buff);

	memset(buff, '\0', BUFLEN);
	sprintf(buff, "%lf", m_y);
	strcat(message, ";");
	strcat(message, buff);

	//Master FOV
	memset(buff, '\0', BUFLEN);
	sprintf(buff, "%lf", fov_x);
	strcat(message, ";");
	strcat(message, buff);

	memset(buff, '\0', BUFLEN);
	sprintf(buff, "%lf", fov_y);
	strcat(message, ";");
	strcat(message, buff);

	return message;
}



int SceneSharingMaster_finish() {
	if (closesocket(s) == SOCKET_ERROR) {
		std::cout << "Socket close failed with error code : " << WSAGetLastError() << std::endl;
		return -1;
	}
	WSACleanup();

	return 0;
}