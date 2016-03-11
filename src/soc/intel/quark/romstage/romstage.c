/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#define __SIMPLE_DEVICE__

#include <arch/early_variables.h>
#include <console/console.h>
#include <cbfs.h>
#include "../chip.h"
#include <device/pci_def.h>
#include <fsp/car.h>
#include <fsp/util.h>
#include <lib.h>
#include <soc/intel/common/util.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>

void car_soc_pre_console_init(void)
{
	if (IS_ENABLED(CONFIG_ENABLE_BUILTIN_HSUART1))
		set_base_address_and_enable_uart(0, HSUART1_DEV, HSUART1_FUNC,
			UART_BASE_ADDRESS);
}

void car_soc_post_console_init(void)
{
	report_platform_info();
};

static struct chipset_power_state power_state CAR_GLOBAL;

struct chipset_power_state *fill_power_state(void)
{
	struct chipset_power_state *ps = car_get_var_ptr(&power_state);

	ps->prev_sleep_state = 0;
	printk(BIOS_DEBUG, "prev_sleep_state %d\n", ps->prev_sleep_state);
	return ps;
}

/* Initialize the UPD parameters for MemoryInit */
void soc_memory_init_params(struct romstage_params *params,
			    MEMORY_INIT_UPD *upd)
{
	const struct device *dev;
	char *pdat_file;
	size_t pdat_file_len;
	const struct soc_intel_quark_config *config;

	/* Locate the pdat.bin file */
	pdat_file = cbfs_boot_map_with_leak("pdat.bin", CBFS_TYPE_RAW,
		&pdat_file_len);
	if (!pdat_file) {
		printk(BIOS_DEBUG,
			"Platform configuration file (pdat.bin) not found.");
		pdat_file_len = 0;
	}

	/* Locate the configuration data from devicetree.cb */
	dev = dev_find_slot(0, LPC_DEV_FUNC);
	if (!dev) {
		printk(BIOS_ERR,
			"Error! Device (PCI:0:%02x.%01x) not found, "
			"soc_memory_init_params!\n", PCI_DEVICE_NUMBER_QNC_LPC,
			PCI_FUNCTION_NUMBER_QNC_LPC);
		return;
	}
	config = dev->chip_info;

	/* Display the ROM shadow data */
	hexdump((void *)0x000ffff0, 0x10);
}

void soc_after_ram_init(struct romstage_params *params)
{
	uint32_t data;

	/* Determine if the shadow ROM is enabled */
	data = port_reg_read(QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
				QNC_MSG_FSBIC_REG_HMISC);
	printk(BIOS_DEBUG, "0x%08x: HMISC\n", data);
	if ((data & (ESEG_RD_DRAM | FSEG_RD_DRAM))
		!= (ESEG_RD_DRAM | FSEG_RD_DRAM)) {

		/* Disable the ROM shadow 0x000e0000 - 0x000fffff */
		data |= ESEG_RD_DRAM | FSEG_RD_DRAM;
		port_reg_write(QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
			QNC_MSG_FSBIC_REG_HMISC, data);
	}

	/* Display the DRAM data */
	hexdump((void *)0x000ffff0, 0x10);
}

void soc_display_memory_init_params(const MEMORY_INIT_UPD *old,
	MEMORY_INIT_UPD *new)
{
}
