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

double actual_coordinates[2];

int fov_x = 100;
int fov_y = 100;

int transition_mode = BEZIER;



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




int SceneSharingClient_look_at(double x, double y) {
	std::cout << "(" << x << "," << y << ")" << std::endl;
	actual_coordinates[0] = x;
	actual_coordinates[1] = y;
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

		// Averaged coordinates for the slave
		int x = atoi(strtok(buf, ";"));
		int y = atoi(strtok(NULL, ";"));

		// Master's coordinates
		double m_x = atoi(strtok(NULL, ";"));
		double m_y = atoi(strtok(NULL, ";"));
		// Field of view of the master
		double m_fov_x = atoi(strtok(NULL, ";"));
		double m_fov_y = atoi(strtok(NULL, ";"));

		double control_points_x[] = { actual_coordinates[0], actual_coordinates[0], m_x, m_x };
		double control_points_y[] = { actual_coordinates[1], actual_coordinates[1], m_y, m_y };

		std::cout << "(" << actual_coordinates[0] << "," << actual_coordinates[1] << "|" << fov_x << "," << fov_y << ") :: (" << m_x << "," << m_y << "|" << m_fov_x << "," << m_fov_y << ")" << std::endl;
		if (actual_coordinates[0] + fov_x < m_x + m_fov_x || actual_coordinates[0] - fov_x > m_x - m_fov_x ||
			actual_coordinates[1] + fov_y < m_y + m_fov_y || actual_coordinates[1] - fov_y > m_y - m_fov_y)
		{
			switch (transition_mode) {
			case INSTANT:
				std::cout << "IN" << std::endl;
				SceneSharingClient_look_at(m_x, m_y);
				break;
			case BEZIER:
				bezier_transition(control_points_x, control_points_y, m_x, m_y);
				break;
			case STEPS:
				step_trainsition(actual_coordinates[0], actual_coordinates[1], m_x, m_y);
				break;
			}
		}
		// Set the look at the coordinates received from the server
		//SceneSharingClient_look_at(x, y);
	}
}



double bezier_value(double t, double * control_points) {
	double s = 1 - t;

	double e0_x = s * s * s * control_points[0];
	double e1_x = t * s * s * control_points[1];
	double e2_x = t * t * s * control_points[2];
	double e3_x = t * t * t * control_points[3];
	double b_x = e0_x + 3 * e1_x + 3 * e2_x + e3_x;

	return b_x;
}

void bezier_transition(double * control_points_x, double * control_points_y, double m_x, double m_y) {

	for (float t = 0; t <= 1; t += BEZIER_STEP) {
		//Get the values of the Bezier curve
		double B_x = bezier_value(t, control_points_x);
		double B_y = bezier_value(t, control_points_y);

		double d_x = abs(m_x - B_x);
		double d_y = abs(m_y - B_y);

		// Check if the new values are close to the master's
		bool snapped_x = false, snapped_y = false;
		if (d_x < SNAP_THRESHOLD)
			snapped_x = true;
		if (d_y < SNAP_THRESHOLD)
			snapped_y = true;

		if (snapped_x && snapped_y) {
			snapped_x = false, snapped_y = false;
			SceneSharingClient_look_at(m_x, m_y); // Match to the master's coordinates
			break;
		}
		else
			SceneSharingClient_look_at(B_x, B_y);
		Sleep(100);
	}
}




float step_value(int step, float s_coord, float m_coord) {
	float d = m_coord - s_coord;
	if (step > STEPS_MAX_STEP)
		step = STEPS_MAX_STEP;
	return s_coord + step*(d / STEPS_MAX_STEP);
}


void step_trainsition(float s_x, float s_y, float m_x, float m_y) {

	for (int step = 1; step <= STEPS_MAX_STEP; step++) {
		float x = step_value(step, s_x, m_x);
		float y = step_value(step, s_y, m_y);

		bool snapped_x = false, snapped_y = false;
		if (abs(x - m_x) < SNAP_THRESHOLD)
			snapped_x = true;
		if (abs(y - m_y) < SNAP_THRESHOLD)
			snapped_y = true;

		if (snapped_x && snapped_y) {
			snapped_x = false, snapped_y = false;
			SceneSharingClient_look_at(m_x, m_y); // Match to the master's coordinates
			break;
		}
		else
			SceneSharingClient_look_at(x, y);
		Sleep(100);
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