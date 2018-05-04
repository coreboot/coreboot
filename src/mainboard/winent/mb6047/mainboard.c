#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp_def.h>
#include <stdlib.h>
#include <superio/winbond/w83627thg/w83627thg.h>

static void hwm_write(u16 base, u8 bank, u8 reg, u8 value)
{
	outb(0x4e, base + 0x05);
	outb(bank, base + 0x06);
	outb(reg, base + 0x05);
	outb(value, base + 0x06);
}

static void mb6047_hwm_init(void)
{
	struct device *dev;
	struct resource *res;
	size_t i;

	static const struct { u8 bnk; u8 idx; u8 dat; } hwmtab[] = {
		{ 0x00, 0x5d, 0x05 }, /* CPUTIN diode */
		{ 0x04, 0x55, 0x32 }, /* CPUTIN offset */
		{ 0x00, 0x4e, 0x00 }, /* reset bank */
	};

	dev = dev_find_slot_pnp(0x2e, W83627THG_HWM);
	if (dev == NULL)
		return;

	res = find_resource(dev, PNP_IDX_IO0);
	if (res == NULL)
		return;

	printk(BIOS_INFO, "setting up hardware monitor at 0x%04x\n", (unsigned int)res->base);

	/* Init hardware monitor. */
	for (i = 0; i < ARRAY_SIZE(hwmtab); i++)
		hwm_write(res->base, hwmtab[i].bnk, hwmtab[i].idx, hwmtab[i].dat);
}

static void mb6047_mainboard_init(struct device *dev)
{
	mb6047_hwm_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mb6047_mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
