extern struct chip_operations superio_NSC_pc87366_ops;

#include <pc80/keyboard.h>
#include <uart8250.h>

struct superio_NSC_pc87366_config {
	struct uart8250 com1, com2;
	struct pc_keyboard keyboard;
};
