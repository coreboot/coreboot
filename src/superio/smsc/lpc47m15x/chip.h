struct chip_operations;
extern struct chip_operations superio_smsc_lpc47m15x_ops;

#include <pc80/keyboard.h>
#include <uart8250.h>

struct superio_smsc_lpc47m15x_config {
	struct uart8250 com1, com2;
	struct pc_keyboard keyboard;
};
