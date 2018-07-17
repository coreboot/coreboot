/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017-2018 Intel Corporation.
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

#include "chip.h"
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <reg_script.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

/*
* As per the BWG, Chapter 5.9.1. "PCH BIOS component will reserve
* certain memory range as reserved range for BIOS usage.
* For this SOC, the range will be from 0FC800000h till FE7FFFFFh"
*/
static const struct lpc_mmio_range cnl_lpc_fixed_mmio_ranges[] = {
	{ PCH_PRESERVED_BASE_ADDRESS, PCH_PRESERVED_BASE_SIZE },
	{ 0, 0 }
};

const struct lpc_mmio_range *soc_get_fixed_mmio_ranges()
{
	return cnl_lpc_fixed_mmio_ranges;
}

void soc_get_gen_io_dec_range(const struct device *dev, uint32_t *gen_io_dec)
{
	const config_t *config = dev->chip_info;

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

uint8_t get_pch_series(void)
{
	uint16_t lpc_did_hi_byte;

	/*
	 * Fetch upper 8 bits on LPC device ID to determine PCH type
	 * Adding 1 to the offset to fetch upper 8 bits
	 */
	lpc_did_hi_byte = pci_read_config8(PCH_DEV_LPC, PCI_DEVICE_ID + 1);

	if (lpc_did_hi_byte == 0x9D)
		return PCH_LP;
	else if (lpc_did_hi_byte == 0xA3)
		return PCH_H;
	else
		return PCH_UNKNOWN_SERIES;
}

#if ENV_RAMSTAGE
static void soc_mirror_dmi_pcr_io_dec(void)
{
	uint32_t io_dec_arr[] = {
		pci_read_config32(PCH_DEV_LPC, LPC_GEN1_DEC),
		pci_read_config32(PCH_DEV_LPC, LPC_GEN2_DEC),
		pci_read_config32(PCH_DEV_LPC, LPC_GEN3_DEC),
		pci_read_config32(PCH_DEV_LPC, LPC_GEN4_DEC),
	};
	/* Mirror these same settings in DMI PCR */
	soc_setup_dmi_pcr_io_dec(&io_dec_arr[0]);
}

static void pch_enable_ioapic(const struct device *dev)
{
	u32 reg32;
	/* PCH-LP has 120 redirection entries */
	const int redir_entries = 120;

	set_ioapic_id((void *)IO_APIC_ADDR, 0x02);

	/* affirm full set of redirection table entries ("write once") */
	reg32 = io_apic_read((void *)IO_APIC_ADDR, 0x01);

	reg32 &= ~0x00ff0000;
	reg32 |= (redir_entries - 1) << 16;

	io_apic_write((void *)IO_APIC_ADDR, 0x01, reg32);

	/*
	 * Select Boot Configuration register (0x03) and
	 * use Processor System Bus (0x01) to deliver interrupts.
	 */
	io_apic_write((void *)IO_APIC_ADDR, 0x03, 0x01);
}
/*
 * PIRQ[n]_ROUT[3:0] - PIRQ Routing Control
 * 0x00 - 0000 = Reserved
 * 0x01 - 0001 = Reserved
 * 0x02 - 0010 = Reserved
 * 0x03 - 0011 = IRQ3
 * 0x04 - 0100 = IRQ4
 * 0x05 - 0101 = IRQ5
 * 0x06 - 0110 = IRQ6
 * 0x07 - 0111 = IRQ7
 * 0x08 - 1000 = Reserved
 * 0x09 - 1001 = IRQ9
 * 0x0A - 1010 = IRQ10
 * 0x0B - 1011 = IRQ11
 * 0x0C - 1100 = IRQ12
 * 0x0D - 1101 = Reserved
 * 0x0E - 1110 = IRQ14
 * 0x0F - 1111 = IRQ15
 * PIRQ[n]_ROUT[7] - PIRQ Routing Control
 * 0x80 - The PIRQ is not routed.
 */

void soc_pch_pirq_init(const struct device *dev)
{
	const config_t *config = dev->chip_info;
	uint8_t pch_interrupt_routing[MAX_PXRC_CONFIG];

	pch_interrupt_routing[0] = config->pirqa_routing;
	pch_interrupt_routing[1] = config->pirqb_routing;
	pch_interrupt_routing[2] = config->pirqc_routing;
	pch_interrupt_routing[3] = config->pirqd_routing;
	pch_interrupt_routing[4] = config->pirqe_routing;
	pch_interrupt_routing[5] = config->pirqf_routing;
	pch_interrupt_routing[6] = config->pirqg_routing;
	pch_interrupt_routing[7] = config->pirqh_routing;

	itss_irq_init(pch_interrupt_routing);
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t irq_dev;
#else
	struct device *irq_dev;
#endif
	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin = 0, int_line = 0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		switch (int_pin) {
		case 1: /* INTA# */
			int_line = config->pirqa_routing;
			break;
		case 2: /* INTB# */
			int_line = config->pirqb_routing;
			break;
		case 3: /* INTC# */
			int_line = config->pirqc_routing;
			break;
		case 4: /* INTD# */
			int_line = config->pirqd_routing;
			break;
		}

		if (!int_line)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
	}
}

static void pch_misc_init(void)
{
	uint8_t reg8;

	/* Setup NMI on errors, disable SERR */
	reg8 = (inb(0x61)) & 0xf0;
	outb(0x61, (reg8 | (1 << 2)));

	/* Disable NMI sources */
	outb(0x70, (1 << 7));
};

static void clock_gate_8254(const struct device *dev)
{
	const config_t *config = dev->chip_info;

	if (!config->clock_gate_8254)
		return;

	itss_clock_gate_8254();
}

void lpc_soc_init(struct device *dev)
{
	/* Legacy initialization */
	isa_dma_init();
	pch_misc_init();

	/* Enable CLKRUN_EN for power gating LPC */
	lpc_enable_pci_clk_cntl();

	/* Set LPC Serial IRQ mode */
	if (IS_ENABLED(CONFIG_SERIRQ_CONTINUOUS_MODE))
		lpc_set_serirq_mode(SERIRQ_CONTINUOUS);
	else
		lpc_set_serirq_mode(SERIRQ_QUIET);

	/* Interrupt configuration */
	pch_enable_ioapic(dev);
	soc_pch_pirq_init(dev);
	setup_i8259();
	i8259_configure_irq_trigger(9, 1);
	clock_gate_8254(dev);
	soc_mirror_dmi_pcr_io_dec();
}

/* Fill up LPC IO resource structure inside SoC directory */
void pch_lpc_soc_fill_io_resources(struct device *dev)
{
	/*
	 * PMC pci device gets hidden from PCI bus due to Silicon
	 * policy hence bind ACPI BASE aka ABASE (offset 0x20) with
	 * LPC IO resources to ensure that ABASE falls under PCI reserved
	 * IO memory range.
	 *
	 * Note: Don't add any more resource with same offset 0x20
	 * under this device space.
	 */
	pch_lpc_add_new_resource(dev, PCI_BASE_ADDRESS_4,
			ACPI_BASE_ADDRESS, ACPI_BASE_SIZE, IORESOURCE_IO |
			IORESOURCE_ASSIGNED | IORESOURCE_FIXED);
}

#endif
