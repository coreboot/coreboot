/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <arch/acpigen.h>
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
#include <arch/acpi.h>
#include <cpu/cpu.h>
#include <cpu/x86/smm.h>
#include <cbmem.h>
#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <reg_script.h>
#include <string.h>
#include <soc/acpi.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/nvs.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/pmc.h>
#include <soc/ramstage.h>
#include <soc/pcr_ids.h>
#if IS_ENABLED(CONFIG_CHROMEOS)
#include <vendorcode/google/chromeos/chromeos.h>
#endif

static void pch_enable_ioapic(struct device *dev)
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

static void pch_pirq_init(device_t dev)
{
	device_t irq_dev;
	config_t *config = dev->chip_info;
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


static const struct reg_script pch_misc_init_script[] = {
	/* Setup NMI on errors, disable SERR */
	REG_IO_RMW8(0x61, ~0xf0, (1 << 2)),
	/* Disable NMI sources */
	REG_IO_OR8(0x70, (1 << 7)),
	/* Enable BIOS updates outside of SMM */
	REG_PCI_RMW8(0xdc, ~(1 << 5), 0),
	/* Setup SERIRQ, enable continuous mode */
	REG_PCI_OR8(SERIRQ_CNTL, (1 << 7) | (1 << 6)),
#if !IS_ENABLED(CONFIG_SERIRQ_CONTINUOUS_MODE)
	REG_PCI_RMW8(SERIRQ_CNTL, ~(1 << 6), 0),
#endif
	/* Enable CLKRUN_EN for power gating LPC */
	REG_PCI_OR8(PCCTL, (CLKRUN_EN)),
	REG_SCRIPT_END
};

static void clock_gate_8254(struct device *dev)
{
	config_t *config = dev->chip_info;

	if (!config->clock_gate_8254)
		return;

	itss_clock_gate_8254();
}

static void lpc_init(struct device *dev)
{
	/* Legacy initialization */
	isa_dma_init();
	reg_script_run_on_dev(dev, pch_misc_init_script);

	/* Interrupt configuration */
	pch_enable_ioapic(dev);
	pch_pirq_init(dev);
	setup_i8259();
	i8259_configure_irq_trigger(9, 1);
	clock_gate_8254(dev);
}

static void pch_lpc_add_mmio_resources(device_t dev)
{
	u32 reg;
	struct resource *res;
	/*
	 * As per the BWG, Chapter 5.9.1. "PCH BIOS component will reserve
	 * certain memory range as reserved range for BIOS usage.
	 * For this SOC, the range will be from 0FD000000h till FE7FFFFFh"
	 * Hence, use FD000000h as PCR_BASE
	 */
	const u32 default_decode_base = CONFIG_PCR_BASE_ADDRESS;

	res = new_resource(dev, PCI_BASE_ADDRESS_0);
	res->base = default_decode_base;
	res->size = 0 - default_decode_base;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED |
		     IORESOURCE_FIXED | IORESOURCE_RESERVE;

	/* Check LPC Memory Decode register. */
	reg = pci_read_config32(dev, LGMR);
	if (reg & 1) {
		reg &= ~0xffff;
		if (reg < default_decode_base) {
			res = new_resource(dev, LGMR);
			res->base = reg;
			res->size = 16 * 1024;
			res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED |
				     IORESOURCE_FIXED | IORESOURCE_RESERVE;
		}
	}
}

/* Default IO range claimed by the LPC device. The upper bound is exclusive. */
#define LPC_DEFAULT_IO_RANGE_LOWER 0
#define LPC_DEFAULT_IO_RANGE_UPPER 0x1000

static inline int pch_io_range_in_default(u16 base, u16 size)
{
	/* Does it start above the range? */
	if (base >= LPC_DEFAULT_IO_RANGE_UPPER)
		return 0;

	/*
	 * Is it entirely contained?
	 * Since LPC_DEFAULT_IO_RANGE_LOWER is Zero,
	 * it need not be checked against lower base.
	 */
	if ((base + size) < LPC_DEFAULT_IO_RANGE_UPPER)
		return 1;

	/* This will return not in range for partial overlaps. */
	return 0;
}

/*
 * Note: this function assumes there is no overlap with the default LPC device's
 * claimed range: LPC_DEFAULT_IO_RANGE_LOWER -> LPC_DEFAULT_IO_RANGE_UPPER.
 */
static void pch_lpc_add_io_resource(device_t dev, u16 base, u16 size, int index)
{
	struct resource *res;

	if (pch_io_range_in_default(base, size))
		return;

	res = new_resource(dev, index);
	res->base = base;
	res->size = size;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void pch_lpc_add_gen_io_resources(device_t dev, int reg_value, int index)
{
	/*
	 * Check if the register is enabled. If so and the base exceeds the
	 * device's deafult claim range add the resoure.
	 */
	if (reg_value & 1) {
		u16 base = reg_value & 0xfffc;
		u16 size = (0x3 | ((reg_value >> 16) & 0xfc)) + 1;
		pch_lpc_add_io_resource(dev, base, size, index);
	}
}

static void pch_lpc_add_io_resources(device_t dev)
{
	struct resource *res;
	config_t *config = dev->chip_info;

	/* Add the default claimed IO range for the LPC device. */
	res = new_resource(dev, 0);
	res->base = LPC_DEFAULT_IO_RANGE_LOWER;
	res->size = LPC_DEFAULT_IO_RANGE_UPPER - LPC_DEFAULT_IO_RANGE_LOWER;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* LPC Generic IO Decode range. */
	pch_lpc_add_gen_io_resources(dev, config->gen1_dec, LPC_GEN1_DEC);
	pch_lpc_add_gen_io_resources(dev, config->gen2_dec, LPC_GEN2_DEC);
	pch_lpc_add_gen_io_resources(dev, config->gen3_dec, LPC_GEN3_DEC);
	pch_lpc_add_gen_io_resources(dev, config->gen4_dec, LPC_GEN4_DEC);
}

static void pch_lpc_read_resources(device_t dev)
{
	global_nvs_t *gnvs;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add non-standard MMIO resources. */
	pch_lpc_add_mmio_resources(dev);

	/* Add IO resources. */
	pch_lpc_add_io_resources(dev);

	/* Allocate ACPI NVS in CBMEM */
	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(global_nvs_t));
	if (!acpi_is_wakeup_s3() && gnvs)
		memset(gnvs, 0, sizeof(global_nvs_t));
}

static struct device_operations device_ops = {
	.read_resources		= &pch_lpc_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.acpi_inject_dsdt_generator = southcluster_inject_dsdt,
	.write_acpi_tables	= southcluster_write_acpi_tables,
	.init			= &lpc_init,
	.scan_bus		= &scan_lpc_bus,
	.ops_pci		= &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_SPT_LP_SAMPLE,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_BASE,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_PREMIUM,
	PCI_DEVICE_ID_INTEL_SPT_LP_Y_PREMIUM,
	PCI_DEVICE_ID_INTEL_KBP_H_QM170,
	PCI_DEVICE_ID_INTEL_KBP_H_PREMIUM,
	PCI_DEVICE_ID_INTEL_KBP_H_C236,
	PCI_DEVICE_ID_INTEL_KBP_LP_SUPER_SKU,
	PCI_DEVICE_ID_INTEL_KBP_LP_U_PREMIUM,
	PCI_DEVICE_ID_INTEL_KBP_LP_Y_PREMIUM,
	PCI_DEVICE_ID_INTEL_SPT_LP_Y_PREMIUM_HDCP22,
	PCI_DEVICE_ID_INTEL_SPT_LP_U_PREMIUM_HDCP22,
	0
};

static const struct pci_driver pch_lpc __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
