#ifndef SIO_COM1
#define SIO_COM1_BASE   0x3F8
#endif
#ifndef SIO_COM2
#define SIO_COM2_BASE   0x2F8
#endif

struct chip_operations;
extern struct chip_operations superio_smsc_lpc47n217_ops;

#include <uart8250.h>

struct superio_smsc_lpc47n217_config {
	struct uart8250 com1, com2;
};
