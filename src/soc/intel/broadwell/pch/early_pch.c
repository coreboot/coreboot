/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/rcba.h>
#include <soc/romstage.h>
#include <soc/smbus.h>
#include <soc/intel/broadwell/pch/chip.h>

static void pch_route_interrupts(void)
{
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
	RCBA32(D31IP) = (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
			(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (INTA << D28IP_P1IP) | (INTC << D28IP_P3IP) |
			(INTB << D28IP_P4IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);
	RCBA32(D20IP) = (INTA << D20IP_XHCI);

	/* Device interrupt route registers */
	RCBA32(D31IR) = DIR_ROUTE(PIRQG, PIRQC, PIRQB, PIRQA);	/* LPC */
	RCBA32(D29IR) = DIR_ROUTE(PIRQD, PIRQD, PIRQD, PIRQD);	/* EHCI */
	RCBA32(D28IR) = DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD);	/* PCIE */
	RCBA32(D27IR) = DIR_ROUTE(PIRQG, PIRQG, PIRQG, PIRQG);	/* HDA */
	RCBA32(D23IR) = DIR_ROUTE(PIRQH, PIRQH, PIRQH, PIRQH);	/* SDIO */
	RCBA32(D22IR) = DIR_ROUTE(PIRQA, PIRQA, PIRQA, PIRQA);	/* ME */
	RCBA32(D21IR) = DIR_ROUTE(PIRQE, PIRQF, PIRQF, PIRQF);	/* SIO */
	RCBA32(D20IR) = DIR_ROUTE(PIRQC, PIRQC, PIRQC, PIRQC);	/* XHCI */
}

static void pch_enable_lpc(void)
{
	/* Lookup device tree in romstage */
	const struct device *const dev = pcidev_on_root(0x1f, 0);

	if (!dev || !dev->chip_info)
		return;

	const struct soc_intel_broadwell_pch_config *config = dev->chip_info;

	pci_write_config32(PCH_DEV_LPC, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(PCH_DEV_LPC, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(PCH_DEV_LPC, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(PCH_DEV_LPC, LPC_GEN4_DEC, config->gen4_dec);
}

void pch_early_init(void)
{
	pch_route_interrupts();

	pch_enable_lpc();

	enable_smbus();

	/* 8.14 Additional PCI Express Programming Steps, step #1 */
	pci_update_config32(_PCH_DEV(PCIE, 0), 0xf4, ~0x60, 0);
	pci_update_config32(_PCH_DEV(PCIE, 0), 0xf4, ~0x80, 0x80);
	pci_update_config32(_PCH_DEV(PCIE, 0), 0xe2, ~0x30, 0x30);
}
