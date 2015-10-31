/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <chip.h>
#include <console/console.h>
#include <console/streams.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <stdlib.h>
#include <stdint.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/smm.h>

static void ABI_X86 send_to_console(unsigned char b)
{
	console_tx_byte(b);
}

void soc_fill_pei_data(struct pei_data *pei_data)
{
	const struct device *dev;
	const struct soc_intel_skylake_config *config;

	/* Set the parameters for MemoryInit */
	dev = dev_find_slot(0, PCH_DEVFN_LPC);
	config = dev->chip_info;

	pei_data->pei_version = PEI_VERSION;
	pei_data->tx_byte = &send_to_console;

	/* Force a full memory train if RMT is enabled */
	pei_data->disable_saved_data = config->Rmt;
}
