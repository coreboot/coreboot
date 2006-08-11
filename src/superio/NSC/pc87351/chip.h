struct chip_operations;
extern struct chip_operations superio_NSC_pc87351_ops;

#include <pc80/keyboard.h>
#include <uart8250.h>

struct superio_NSC_pc87351_config {
	struct uart8250 com1, com2;
	struct pc_keyboard keyboard;
};
