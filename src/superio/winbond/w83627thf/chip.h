#include <pc80/keyboard.h>
#include <uart8250.h>

extern struct chip_operations superio_winbond_w83627thf_ops;

struct superio_winbond_w83627thf_config {
	struct uart8250 com1, com2;
	struct pc_keyboard keyboard;
};
