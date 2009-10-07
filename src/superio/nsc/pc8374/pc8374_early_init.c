#include <arch/romcc_io.h>
#include "pc8374.h"

/* things that Must Be Done to get this chip working */
/* Straight from the data book */
static void pc8374_enable(unsigned iobase, u8 *init)
{
	u8 val, count;
	outb(0x29, iobase);
	val = inb(iobase+1);
	val |= 0x91;
	outb(val, iobase+1);
	for(count = 0; count < 255; count++)
		if (inb(iobase+1) == 0x91)
			break;
	for(;*init; init++) {
		outb(*init, iobase);
		val = inb(iobase+1);
		init++;
		val &= *init;
		init++;
		val |= *init;
		outb(val, iobase+1);
	}
		
	
}

static void pc8374_enable_dev(device_t dev, unsigned iobase)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	if (iobase)
		pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
}

