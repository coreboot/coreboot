struct chip_operations;
extern struct chip_operations superio_smsc_lpc47b272_ops;

#include <pc80/keyboard.h>
#include <uart8250.h>

struct superio_smsc_lpc47b272_config {
	struct uart8250 com1, com2;
	struct pc_keyboard keyboard;
};
