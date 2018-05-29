/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <reg_script.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/rcba.h>
#include <soc/romstage.h>
#include <soc/smbus.h>
#include <soc/intel/broadwell/chip.h>

const struct reg_script pch_early_init_script[] = {
	/* Setup southbridge BARs */
	REG_PCI_WRITE32(RCBA, RCBA_BASE_ADDRESS | 1),
	REG_PCI_WRITE32(PMBASE, ACPI_BASE_ADDRESS | 1),
	REG_PCI_WRITE8(ACPI_CNTL, ACPI_EN),
	REG_PCI_WRITE32(GPIO_BASE, GPIO_BASE_ADDRESS | 1),
	REG_PCI_WRITE8(GPIO_CNTL, GPIO_EN),

	/* Set COM1/COM2 decode range */
	REG_PCI_WRITE16(LPC_IO_DEC, 0x0010),
	/* Enable legacy decode ranges */
	REG_PCI_WRITE16(LPC_EN, CNF1_LPC_EN | CNF2_LPC_EN | GAMEL_LPC_EN |
			COMA_LPC_EN | KBC_LPC_EN | MC_LPC_EN),

	/* Enable IOAPIC */
	REG_MMIO_WRITE16(RCBA_BASE_ADDRESS + OIC, 0x0100),
	/* Read back for posted write */
	REG_MMIO_READ16(RCBA_BASE_ADDRESS + OIC),

	/* Set HPET address and enable it */
	REG_MMIO_RMW32(RCBA_BASE_ADDRESS + HPTC, ~3, (1 << 7)),
	/* Read back for posted write */
	REG_MMIO_READ32(RCBA_BASE_ADDRESS + HPTC),
	/* Enable HPET to start counter */
	REG_MMIO_OR32(HPET_BASE_ADDRESS + 0x10, (1 << 0)),

	/* Disable reset */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + GCS, (1 << 5)),
	/* TCO timer halt */
	REG_IO_OR16(ACPI_BASE_ADDRESS + TCO1_CNT, TCO_TMR_HLT),

	/* Enable upper 128 bytes of CMOS */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + RC, (1 << 2)),

	/* Disable unused device (always) */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + FD, PCH_DISABLE_ALWAYS),

	REG_SCRIPT_END
};

const struct reg_script pch_interrupt_init_script[] = {
	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P1IP  PCIE   INTA -> PIRQA
	 * D29IP_E1P   EHCI   INTA -> PIRQD
	 * D20IP_XHCI  XHCI   INTA -> PIRQC (MSI)
	 * D31IP_SIP   SATA   INTA -> PIRQF (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQG
	 * D31IP_TTIP  THRT   INTC -> PIRQA
	 * D27IP_ZIP   HDA    INTA -> PIRQG (MSI)
	 */

	/* Device interrupt pin register (board specific) */
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D31IP,
			 (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
			 (INTB << D31IP_SMIP) | (INTA << D31IP_SIP)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D29IP, (INTA << D29IP_E1P)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D28IP,
			 (INTA << D28IP_P1IP) | (INTC << D28IP_P3IP) |
			 (INTB << D28IP_P4IP)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D27IP, (INTA << D27IP_ZIP)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D26IP, (INTA << D26IP_E2P)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D22IP, (NOINT << D22IP_MEI1IP)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D20IP, (INTA << D20IP_XHCI)),

	/* Device interrupt route registers */
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D31IR, /* LPC */
			 DIR_ROUTE(PIRQG, PIRQC, PIRQB, PIRQA)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D29IR, /* EHCI */
			 DIR_ROUTE(PIRQD, PIRQD, PIRQD, PIRQD)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D28IR, /* PCIE */
			 DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D27IR, /* HDA */
			 DIR_ROUTE(PIRQG, PIRQG, PIRQG, PIRQG)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D22IR, /* ME */
			 DIR_ROUTE(PIRQA, PIRQA, PIRQA, PIRQA)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D21IR, /* SIO */
			 DIR_ROUTE(PIRQE, PIRQF, PIRQF, PIRQF)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D20IR, /* XHCI */
			 DIR_ROUTE(PIRQC, PIRQC, PIRQC, PIRQC)),
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + D23IR, /* SDIO */
			 DIR_ROUTE(PIRQH, PIRQH, PIRQH, PIRQH)),

	REG_SCRIPT_END
};

static void pch_enable_lpc(void)
{
	/* Lookup device tree in romstage */
	const struct device *dev;
	const config_t *config;

	dev = dev_find_slot(0, PCI_DEVFN(PCH_DEV_SLOT_LPC, 0));
	if (!dev || !dev->chip_info)
		return;
	config = dev->chip_info;

	pci_write_config32(PCH_DEV_LPC, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(PCH_DEV_LPC, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(PCH_DEV_LPC, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(PCH_DEV_LPC, LPC_GEN4_DEC, config->gen4_dec);
}

void pch_early_init(void)
{
	reg_script_run_on_dev(PCH_DEV_LPC, pch_early_init_script);
	reg_script_run_on_dev(PCH_DEV_LPC, pch_interrupt_init_script);

	pch_enable_lpc();

	enable_smbus();

	/* 8.14 Additional PCI Express Programming Steps, step #1 */
	pci_update_config32(_PCH_DEV(PCIE, 0), 0xf4, ~0x60, 0);
	pci_update_config32(_PCH_DEV(PCIE, 0), 0xf4, ~0x80, 0x80);
	pci_update_config32(_PCH_DEV(PCIE, 0), 0xe2, ~0x30, 0x30);
}
