extern struct chip_operations superio_winbond_w83977tf_ops;

#include <pc80/keyboard.h>
#include <uart8250.h>

struct superio_winbond_w83977tf_config {
	struct uart8250 com1, com2;
	struct pc_keyboard keyboard;
};
