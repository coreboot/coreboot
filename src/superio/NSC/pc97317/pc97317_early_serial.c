#include <arch/romcc_io.h>
#include "pc97317.h"

#define PM_DEV PNP_DEV(0x2e, PC97317_PM)
#define PM_BASE 0xe8

/* The pc97317 needs clocks to be set up before the serial port will operate */

static void pc97317_enable_serial(device_t dev, unsigned iobase)
{
	/* Set base address of power management unit */

	pnp_set_logical_device(PM_DEV);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, PM_BASE);
	pnp_set_enable(dev, 1);

	/* Use on-chip clock multiplier */

	outb(0x03, PM_BASE);
	outb(inb(PM_BASE + 1) | 0x07, PM_BASE + 1);
	
	/* Wait for the clock to stabilise */
	while(!inb(PM_BASE + 1 & 0x80))
		;

	/* Set the base address of the port */

	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
}
