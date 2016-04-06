
#include "SceneSharingServer.h"

int main(void)
{

	SceneSharingServer_initialize();
	SceneSharingServer_server_loop();
	SceneSharingServer_finish();

	return 0;
}