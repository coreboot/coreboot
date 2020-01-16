/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 - 2020 Intel Corporation
 * Copyright (C) 2019 - 2020 Facebook Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <arch/ioapic.h>
#include <intelblocks/lpc_lib.h>
#include <soc/soc_util.h>
#include <soc/iomap.h>

static const struct lpc_mmio_range xeon_lpc_fixed_mmio_ranges[] = {
	{ 0, 0 }
};

const struct lpc_mmio_range *soc_get_fixed_mmio_ranges(void)
{
	return xeon_lpc_fixed_mmio_ranges;
}

void lpc_soc_init(struct device *dev)
{
	printk(BIOS_SPEW, "pch: lpc_init\n");

	/* FSP configures IOAPIC and PCHInterrupt Config */
	printk(BIOS_SPEW, "IOAPICID 0x%x, 0x%x\n",
		io_apic_read((void *)IO_APIC_ADDR, 0x00),
		((io_apic_read((void *)IO_APIC_ADDR, 0x00) & 0x0f000000) >> 24));
}

void pch_lpc_soc_fill_io_resources(struct device *dev)
{
}
