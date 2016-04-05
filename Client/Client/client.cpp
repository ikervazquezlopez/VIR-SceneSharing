/*
Simple UDP Server
Silver Moon ( m00n.silv3r@gmail.com )
*/

#include<stdio.h>
#include<iostream>

#include "SceneSharingClient.h"


int main()
{
	SceneSharingClient_initialize();
	SceneSharingClient_receive_loop();
	SceneSharingClient_finish();

	return 0;
}