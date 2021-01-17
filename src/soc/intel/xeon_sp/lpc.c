/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <arch/ioapic.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
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
