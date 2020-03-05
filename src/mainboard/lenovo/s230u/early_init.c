/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <string.h>
#include <cbfs.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include "ec.h"

#define SPD_LEN 256

void mainboard_pch_lpc_setup(void)
{
	/* Memory map KB9012 EC registers */
	pci_write_config32(
		PCH_LPC_DEV, LGMR,
		CONFIG_EC_BASE_ADDRESS | 1);
	pci_write_config16(PCH_LPC_DEV, BIOS_DEC_EN1, 0xffc0);

	/* Enable external USB port power. */
	if (CONFIG(USBDEBUG))
		ec_mm_set_bit(0x3b, 4);
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 },
	{ 1, 0, 0 },
	{ 1, 1, 1 },
	{ 1, 0, 1 },
	{ 1, 1, 2 },
	{ 1, 0, 2 },
	{ 0, 0, 3 },
	{ 0, 1, 3 },
	{ 1, 0, 4 },
	{ 1, 1, 4 },
	{ 1, 1, 5 },
	{ 1, 1, 5 },
	{ 1, 1, 6 },
	{ 1, 1, 6 },
};

static const char *mainboard_spd_names[9] = {
	"ELPIDA 4GB",
	"SAMSUNG 4GB",
	"HYNIX 4GB",
	"ELPIDA 8GB",
	"SAMSUNG 8GB",
	"HYNIX 8GB",
	"ELPIDA 2GB",
	"SAMSUNG 2GB",
	"HYNIX 2GB",
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	void *spd_file;
	size_t spd_file_len = 0;
	const int spd_gpios[] = {71, 70, 16, 48, -1};

	u32 spd_index = get_gpios(spd_gpios);
	if (spd_index >= ARRAY_SIZE(mainboard_spd_names)) {
		/* Fallback to pessimistic 2GB image (ELPIDA 2GB) */
		spd_index = 6;
	}

	printk(BIOS_INFO, "SPD index %d (%s)\n",
		spd_index, mainboard_spd_names[spd_index]);

	/* C0S0 is a soldered RAM with no real SPD. Use stored SPD. */
	spd_file = cbfs_map("spd.bin", &spd_file_len);

	if (!spd_file || spd_file_len < SPD_LEN * spd_index + SPD_LEN)
		die("SPD data not found.");

	memcpy(spd, spd_file + SPD_LEN * spd_index, SPD_LEN);
}
