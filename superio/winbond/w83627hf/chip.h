#include <keyboard.h>
#include <uart8250.h>

struct superio_winbond_w83627hf_config {
	int com1_baud, com2_baud;
	struct pc_keyboard keyboard;
};
