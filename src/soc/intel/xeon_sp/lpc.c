/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <arch/ioapic.h>
#include <intelblocks/lpc_lib.h>
#include <soc/iomap.h>
#include <soc/pcr_ids.h>

#include <chip.h>

void soc_get_gen_io_dec_range(uint32_t *gen_io_dec)
{
	const config_t *config = config_of_soc();

	gen_io_dec[0] = config->gen1_dec;
	gen_io_dec[1] = config->gen2_dec;
	gen_io_dec[2] = config->gen3_dec;
	gen_io_dec[3] = config->gen4_dec;
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
