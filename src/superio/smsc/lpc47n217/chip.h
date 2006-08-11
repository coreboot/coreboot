struct chip_operations;
extern struct chip_operations superio_smsc_lpc47n217_ops;

#include <uart8250.h>

struct superio_smsc_lpc47n217_config {
	struct uart8250 com1, com2;
};
