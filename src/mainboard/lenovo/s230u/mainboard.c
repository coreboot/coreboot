/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <console/console.h>
#include <drivers/intel/gma/int15.h>
#include <ec/acpi/ec.h>
#include <southbridge/intel/common/gpio.h>
#include <string.h>
#include <smbios.h>
#include "ec.h"

#include <acpi/acpi.h>

static u8 mainboard_fill_ec_version(char *buf, u8 buf_len)
{
	u8 i, c;
	char str[16 + 1]; /* 16 ASCII chars + \0 */

	/* Build ID */
	for (i = 0; i < 8; i++) {
		c = ec_mm_read(0xf0 + i);
		if (c < 0x20 || c > 0x7f) {
			i = snprintf(str, sizeof(str), "*INVALID");
			break;
		}
		str[i] = c;
	}

	i = MIN(buf_len, i);
	memcpy(buf, str, i);

	return i;
}

static void mainboard_smbios_strings(
	struct device *dev, struct smbios_type11 *t)
{
	char tpec[] = "IBM ThinkPad Embedded Controller -[                 ]-";
	u16 fwvh, fwvl;

	mainboard_fill_ec_version(tpec + 35, 17);
	t->count = smbios_add_string(t->eos, tpec);

	/* Apparently byteswapped compared to H8 */
	fwvh = ec_mm_read(0xe8);
	fwvl = ec_mm_read(0xe9);

	printk(BIOS_INFO, "EC Firmware ID %.54s, Version %d.%d%d%c\n", tpec,
	       fwvh >> 4, fwvh & 0x0f, fwvl >> 4, 0x41 + (fwvl & 0xf));
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->get_smbios_strings = mainboard_smbios_strings;

	install_intel_vga_int15_handler(
		GMA_INT15_ACTIVE_LFP_INT_LVDS,
		GMA_INT15_PANEL_FIT_DEFAULT,
		GMA_INT15_BOOT_DISPLAY_DEFAULT,
		0);

	if (!acpi_is_wakeup_s3())
		lenovo_s230u_ec_init();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
