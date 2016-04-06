
#include "SceneSharingMaster.h"

int main(void)
{

	SceneSharingMaster_initialize();
	SceneSharingMaster_send_loop();
	SceneSharingMaster_finish();

}