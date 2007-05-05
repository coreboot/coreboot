#include "w83627hf.h"

static inline void pnp_enter_ext_func_mode(struct device *dev) 
{
	unsigned port = dev>>8;
        outb(0x87, port);
        outb(0x87, port);
}
static void pnp_exit_ext_func_mode(struct device *dev) 
{
	unsigned port = dev>>8;
        outb(0xaa, port);
}
static void w83627hf_enable_serial(struct device *dev, unsigned iobase)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_ext_func_mode(dev);
}
