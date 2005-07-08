#include <arch/romcc_io.h>
#include "w83627hf.h"

static void w83627hf_disable_dev(device_t dev)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
}
static void w83627hf_enable_dev(device_t dev, unsigned iobase)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
}
