#include <arch/romcc_io.h>
#include "w83977tf.h"


static void w83977tf_enable_serial(device_t dev, unsigned iobase)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
}
