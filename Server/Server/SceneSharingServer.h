#pragma once

#include<stdio.h>
#include <iostream>
#include<winsock2.h>
#include <ws2ipdef.h>


#define MULTICAST_GROUP "224.198.87.101"  // IP address of multicast groupd
#define BUFLEN 512  // Max length of buffer
#define CLIENT_PORT 8888   // The port to send the data to the client
#define MASTER_PORT 8887   // The port to receive the data from the master



int SceneSharingServer_initialize();

char * SceneSharingServer_getMessage(double * coords, double m_x, double m_y, double fov_x, double fov_y);

void SceneSharingServer_server_loop();

int SceneSharingServer_finish();

