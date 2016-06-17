#pragma once


#define BUFLEN 512  // Max length of buffer
#define MULTICAST_GROUP "224.198.87.101"  // Multicast gropu to send the data to
#define MASTER_PORT 8887   //The port on which to send the data to the server


int SceneSharingMaster_initialize();

char * SceneSharingMaster_getMessage(double m_x, double m_y, double fov_x, double fov_y);

void SceneSharingMaster_send_loop();

int SceneSharingMaster_finish();
