#include <arch/romcc_io.h>
#include "lpc47b397.h"

static inline void pnp_enter_conf_state(device_t dev) {
	unsigned port = dev>>8;
	outb(0x55, port);
}

static void pnp_exit_conf_state(device_t dev) {
	unsigned port = dev>>8;
	outb(0xaa, port);
}

static void lpc47b397_enable_serial(device_t dev, unsigned iobase)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}
