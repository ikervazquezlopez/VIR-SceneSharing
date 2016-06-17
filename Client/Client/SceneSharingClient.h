#pragma once

#include<stdio.h>
#include<iostream>
#include<winsock2.h>
#include<ws2ipdef.h>



#define BUFLEN 512  // Max length of buffer
#define MULTICAST_GROUP "224.198.87.101" // Multicast gropu to receive the data from
#define CLIENT_PORT 8888   //The port on which to listen for incoming data

#define INSTANT 0 // Transition mode instant
#define BEZIER 1  // Transition mode using a Bezier curve
#define STEPS 2 // Transition mode using Steps approach

#define SNAP_THRESHOLD 2 // Threshold to snap the current coordinates with the master


#define BEZIER_STEP 0.05  // Step size for the bezier curve


#define STEPS_MAX_STEP 40 // Max steps for step approach






int SceneSharingClient_initialize();

int SceneSharingClient_look_at(double x, double y);

void SceneSharingClient_receive_loop();

int SceneSharingClient_finish();


double bezier_value(double t, double * control_points);
void bezier_transition(double * control_points_x, double * control_points_y, double m_x, double m_y);

float step_value(int step, float s_coord, float m_coord);
void step_trainsition(float s_x, float s_y, float m_x, float m_y);