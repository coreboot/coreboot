/*
 * This file is part of the coreboot project.
 *
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
#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/pcr_ids.h>

#include <chip.h>

static const struct lpc_mmio_range xeon_lpc_fixed_mmio_ranges[] = {
	{ 0, 0 }
};

const struct lpc_mmio_range *soc_get_fixed_mmio_ranges(void)
{
	return xeon_lpc_fixed_mmio_ranges;
}

void soc_get_gen_io_dec_range(const struct device *dev, uint32_t *gen_io_dec)
{
	const config_t *config = config_of(dev);

	gen_io_dec[0] = config->gen1_dec;
	gen_io_dec[1] = config->gen2_dec;
	gen_io_dec[2] = config->gen3_dec;
	gen_io_dec[3] = config->gen4_dec;
}

void soc_setup_dmi_pcr_io_dec(uint32_t *gen_io_dec)
{
	/* Mirror these same settings in DMI PCR */
	pcr_write32(PID_DMI, PCR_DMI_LPCLGIR1, gen_io_dec[0]);
	pcr_write32(PID_DMI, PCR_DMI_LPCLGIR2, gen_io_dec[1]);
	pcr_write32(PID_DMI, PCR_DMI_LPCLGIR3, gen_io_dec[2]);
	pcr_write32(PID_DMI, PCR_DMI_LPCLGIR4, gen_io_dec[3]);
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
