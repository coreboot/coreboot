#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <stdlib.h>

#include "pmh7.h"

void pmh7_register_set_bit(int reg, int bit)
{
	char val;

	outb(reg, EC_LENOVO_PMH7_ADDR);
	val = inb(EC_LENOVO_PMH7_DATA);
	outb(reg, EC_LENOVO_PMH7_ADDR);
	outb(val | (1 << bit), EC_LENOVO_PMH7_DATA);
}

void pmh7_register_clear_bit(int reg, int bit)
{
	char val;

	outb(reg, EC_LENOVO_PMH7_ADDR);
	val = inb(EC_LENOVO_PMH7_DATA);
	outb(reg, EC_LENOVO_PMH7_ADDR);
	outb(val &= ~(1 << bit), EC_LENOVO_PMH7_DATA);
}

char pmh7_register_read(int reg)
{
	outb(reg, EC_LENOVO_PMH7_ADDR);
	return inb(EC_LENOVO_PMH7_DATA);
}

void pmh7_register_write(int reg, int val)
{
	outb(reg, EC_LENOVO_PMH7_ADDR);
	outb(val, EC_LENOVO_PMH7_DATA);
}

static void enable_dev(device_t dev)
{
	struct resource *resource;
	resource = new_resource(dev, EC_LENOVO_PMH7_INDEX);
	resource->flags = IORESOURCE_IO | IORESOURCE_FIXED;
	resource->base = EC_LENOVO_PMH7_BASE;
	resource->size = 16;
	resource->align = 5;
	resource->gran = 5;

}

struct chip_operations ec_lenovo_pmh7_ops = {
	CHIP_NAME("Lenovo Power Management Hardware Hub 7")
	.enable_dev = enable_dev,
};
