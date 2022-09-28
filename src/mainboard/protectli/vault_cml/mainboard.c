
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/x86/name.h>
#include <device/device.h>
#include <pc80/i8254.h>
#include <smbios.h>
#include <string.h>

const char *smbios_mainboard_product_name(void)
{
	char processor_name[49];

	fill_processor_name(processor_name);

	if (strstr(processor_name, "i3-10110U") != NULL)
		return "VP4630";
	else if (strstr(processor_name, "i5-10210U") != NULL)
		return "VP4650";
	else if (strstr(processor_name, "i7-10810U") != NULL)
		return "VP4670";
	else
		return CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME;
}

static void mainboard_final(void *unused)
{
	if (CONFIG(BEEP_ON_BOOT))
		beep(1500, 100);
}

struct chip_operations mainboard_ops = {
	.final = mainboard_final,
};
