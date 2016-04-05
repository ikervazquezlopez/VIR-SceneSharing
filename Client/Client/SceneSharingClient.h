#pragma once

#include<stdio.h>
#include<iostream>
#include<winsock2.h>
#include<ws2ipdef.h>



#define BUFLEN 512  // Max length of buffer
#define MULTICAST_GROUP "224.198.87.101" // Multicast gropu to receive the data from
#define CLIENT_PORT 8888   //The port on which to listen for incoming data






int SceneSharingClient_initialize();

int SceneSharingClient_look_at(int x, int y);

void SceneSharingClient_receive_loop();

int SceneSharingClient_finish();