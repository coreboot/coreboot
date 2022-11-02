/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <southbridge/intel/i82801ix/i82801ix.h>

#include "gm45.h"

static void init_egress(void)
{
	/* VC0: TC0 only */
	epbar_clrbits8(EPVC0RCTL, ~1);
	epbar_clrsetbits8(EPPVCCAP1, 7, 1);

	/* VC1: isoch */
	epbar_write32(EPVC1MTS, 0x0a0a0a0a);
	epbar_clrsetbits32(EPVC1RCAP, 127 << 16, 0x0a << 16);

	/* VC1: ID1, TC7 */
	epbar_clrsetbits32(EPVC1RCTL, 7 << 24, 1 << 24);
	epbar_clrsetbits8(EPVC1RCTL, ~1, 1 << 7);

	/* VC1 ARB table: setup and enable */
	epbar_write32(EP_PORTARB(0), 0x55555555);
	epbar_write32(EP_PORTARB(1), 0x55555555);
	epbar_write32(EP_PORTARB(2), 0x55555555);
	epbar_write32(EP_PORTARB(3), 0x55555555);
	epbar_write32(EP_PORTARB(4), 0x55555555);
	epbar_write32(EP_PORTARB(5), 0x55555555);
	epbar_write32(EP_PORTARB(6), 0x55555555);
	epbar_write32(EP_PORTARB(7), 0x00005555);
	epbar_setbits32(EPVC1RCTL, 1 << 16);

	do {} while ((epbar_read8(EPVC1RSTS) & 1) != 0);

	/* VC1: enable */
	epbar_setbits32(EPVC1RCTL, 1 << 31);

	do {} while ((epbar_read8(EPVC1RSTS) & 2) != 0);
}

/* MCH side */
/* b2step: b2 stepping or higher */
static void init_dmi(int b2step)
{
	/* VC0: TC0 only */
	dmibar_clrbits8(DMIVC0RCTL, ~1);
	dmibar_clrsetbits8(DMIPVCCAP1, 7, 1);

	/* VC1: ID1, TC7 */
	dmibar_clrsetbits32(DMIVC1RCTL, 7 << 24, 1 << 24);
	dmibar_clrsetbits8(DMIVC1RCTL, ~1, 1 << 7);

	/* VC1: enable */
	dmibar_setbits32(DMIVC1RCTL, 1 << 31);

	do {} while ((dmibar_read8(DMIVC1RSTS) & VC1NP) != 0);

	/* additional configuration. */
	dmibar_setbits32(0x200, 3 << 13);
	dmibar_clrbits32(0x200, 1 << 21);
	dmibar_clrsetbits32(0x200, 3 << 26, 2 << 26);
	dmibar_write32(0x2c, 0x86000040);
	dmibar_setbits32(0xfc, 1 << 0);
	dmibar_setbits32(0xfc, 1 << 1);
	dmibar_setbits32(0xfc, 1 << 4);
	if (!b2step) {
		dmibar_setbits32(0xfc, 1 << 11);
	} else {
		dmibar_clrbits32(0xfc, 1 << 11);
	}
	dmibar_clrbits32(0x204, 3 << 10);
	dmibar_clrbits32(0xf4, 1 << 4);
	dmibar_setbits32(0xf0, 3 << 24);
	dmibar_write32(0xf04, 0x07050880);
	dmibar_write32(0xf44, 0x07050880);
	dmibar_write32(0xf84, 0x07050880);
	dmibar_write32(0xfc4, 0x07050880);

	/* lock down write-once registers
	   DMIBAR32(0x84) will be set in setup_aspm(). */
	dmibar_setbits32(0x308, 0);
	dmibar_setbits32(0x314, 0);
	dmibar_setbits32(0x324, 0);
	dmibar_setbits32(0x328, 0);
	dmibar_setbits32(0x334, 0);
	dmibar_setbits32(0x338, 0);
}

static void init_pcie(const int peg_enabled,
		      const int sdvo_enabled,
		      const int peg_x16)
{
	const pci_devfn_t mch = PCI_DEV(0, 0, 0);
	const pci_devfn_t pciex = PCI_DEV(0, 1, 0);

	printk(BIOS_DEBUG, "PEG x%d %s, SDVO %s\n", peg_x16?16:1,
		peg_enabled?"enabled":"disabled",
		sdvo_enabled?"enabled":"disabled");

	if (peg_enabled) {
		pci_or_config8(mch, D0F0_DEVEN, 1 << 1);

		pci_write_config8(pciex, 0x224,
			(pci_read_config8(pciex, 0x224) & ~31) | (peg_x16 ? 16 : 0) | 1);

		pci_and_config16(pciex, 0x224, ~(1 << 8));

		/* FIXME: fill in: slot or fixed? -> devicetree */
		int peg_is_slot = 0;
		if (peg_is_slot) {
			pci_or_config16(pciex, PEG_CAP, 1 << 8);
		}

		/* FIXME: fill in: slot number, slot power -> devicetree */
		/* Use slot number 0 by now, slots on sb count from 1. */
		int peg_slot = 0; /* unique within chassis */
		/* peg_power := val * 10^-exp */
		int peg_power_val = 75;
		int peg_power_exp = 0; /* 0..3 */
		const u32 tmp = (peg_slot << 17) | (peg_power_exp << 15) | (peg_power_val << 7);
		pci_write_config32(pciex, SLOTCAP, tmp);

		/* GPEs */
		pci_or_config8(pciex, PEGLC, 7);

		/* VC0: TC0 only, VC0 only */
		pci_and_config8(pciex, D1F0_VC0RCTL, 1);

		pci_and_config8(pciex, D1F0_VCCAP, ~7);
	}
}

static void setup_aspm(const stepping_t stepping, const int peg_enabled)
{
	u32 tmp32;
	const pci_devfn_t pciex = PCI_DEV(0, 1, 0);

	/* Prerequisites for ASPM: */
	if (peg_enabled) {
		pci_or_config32(pciex, 0x200, 3 << 13);

		pci_and_config32(pciex, 0x0f0, ~((1 << 27) | (1 << 26)));

		pci_or_config32(pciex, 0x0f0, 3 << 24);

		pci_and_config32(pciex, 0x0f4, ~(1 << 4));

		pci_or_config32(pciex, 0x0fc, 1 << 0);

		pci_or_config32(pciex, 0x0fc, 1 << 1);

		pci_or_config32(pciex, 0x0fc, 1 << 4);

		pci_and_config32(pciex, 0x0fc, ~(7 << 5));

		/* Set L0s, L1 supported in LCTL? */
		pci_or_config32(pciex, 0x0b0, 3 << 0);

		pci_or_config32(pciex, 0x0f0, 3 << 24);

		tmp32 = pci_read_config32(pciex, 0x0f0);
		if ((stepping >= STEPPING_B0) && (stepping <= STEPPING_B1))
			tmp32 |= (1 << 31);
		else if (stepping >= STEPPING_B2)
			tmp32 &= ~(1 << 31);
		pci_write_config32(pciex, 0x0f0, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0fc);
		if ((stepping >= STEPPING_B0) && (stepping <= STEPPING_B1))
			tmp32 |= (1 << 10);
		else if (stepping >= STEPPING_B2)
			tmp32 &= ~(1 << 10);
		pci_write_config32(pciex, 0x0fc, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0fc);
		if (stepping >= STEPPING_B2)
			tmp32 |= (1 << 14);
		pci_write_config32(pciex, 0x0fc, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0fc);
		if (stepping >= STEPPING_B1)
			tmp32 &= ~(1 << 13);
		pci_write_config32(pciex, 0x0fc, tmp32);
	}
	dmibar_setbits8(0x0e1c, 1 <<  0);
	dmibar_setbits16(0x0f00, 3 <<  8);
	dmibar_setbits16(0x0f00, 7 <<  3);
	dmibar_clrbits32(0x0f14, 1 << 17);
	dmibar_clrbits16(0x0e1c, 1 <<  8);
	if (stepping >= STEPPING_B0) {
		dmibar_clrsetbits32(0x0e28 + 4, 0xf << (52 - 32), 0xd << (52 - 32));
		dmibar_write32(0x0e2c, 0x88d07333);
	}
	if (peg_enabled) {
		pci_and_config32(pciex, 0xa08, ~(1 << 15));

		pci_or_config32(pciex, 0xa84, 1 << 8);

		pci_and_config32(pciex, 0xb14, ~(1 << 17));

		pci_or_config32(pciex, 0xb00, 3 << 8);

		pci_or_config32(pciex, 0xb00, 7 << 3);

		pci_and_config32(pciex, 0xa84, ~(1 << 8));

		pci_or_config32(pciex, 0xa84, 1 << 8);

		pci_update_config32(pciex, 0xb04, ~(0x1f << 23), 0x0e << 23);

		pci_or_config32(pciex, 0xb04, 1 << 31);

		pci_update_config32(pciex, 0xb04, ~(0x03 << 29), 0x01 << 29);
	}

	/*\ Setup ASPM on DMI \*/

	/* Exit latencies should be checked to be supported by
	   the endpoint (ICH), but ICH doesn't give any limits. */

	if (LPC_IS_MOBILE(PCI_DEV(0, 0x1f, 0)))
		dmibar_setbits8(DMILCTL, 3 << 0); // enable ASPM L0s, L1 (write-once)
	else
		dmibar_setbits8(DMILCTL, 1 << 0); // enable ASPM L0s (write-once)
	/* timing */
	dmibar_clrsetbits32(DMILCAP, 63 << 12, 2 << 12 | 2 << 15);
	dmibar_write8(0x208 + 3, 0);
	dmibar_clrbits32(0x208, 3 << 20);

	/*\ Setup ASPM on PEG \*/
	/*
	 * Maybe we just have to advertise ASPM through LCAP[11:10]
	 * (LCAP[17:15] == 010b is the default, will be locked, as it's R/WO),
	 * set 0x208[31:24,23:22] to zero, 0x224[24:21] = 1 and let the
	 * generic ASPM code do the rest? - Nico
	 */
	/* TODO: Prepare PEG for ASPM. */
}

static void setup_rcrb(const int peg_enabled)
{
	/*\ RCRB setup: Egress Port \*/

	/* Set component ID of MCH (1). */
	epbar_write8(EPESD + 2, 1);

	/* Link1: component ID 1, link valid. */
	epbar_clrsetbits32(EPLE1D, 0xffffff, 1 << 16 | 1 << 0);
	epbar_write32(EPLE1A, CONFIG_FIXED_DMIBAR_MMIO_BASE);

	if (peg_enabled)
		/* Link2: link_valid. */
		epbar_setbits8(EPLE2D, 1 << 0); /* link valid */

	/*\ RCRB setup: DMI Port \*/

	/* Set component ID of MCH (1). */
	dmibar_write8(DMIESD + 2, 1);

	/* Link1: target port 0, component id 2 (ICH), link valid. */
	dmibar_write32(DMILE1D, 0 << 24 | 2 << 16 | 1 << 0);
	dmibar_write32(DMILE1A, CONFIG_FIXED_RCBA_MMIO_BASE);

	/* Link2: component ID 1 (MCH), link valid */
	dmibar_clrsetbits32(DMILE2D, 0xffffff, 1 << 16 | 1 << 0);
	dmibar_write32(DMILE2A, CONFIG_FIXED_MCHBAR_MMIO_BASE);
}

void gm45_late_init(const stepping_t stepping)
{
	const pci_devfn_t mch = PCI_DEV(0, 0, 0);
	const int peg_enabled = (pci_read_config8(mch, D0F0_DEVEN) >> 1) & 1;
	const int sdvo_enabled = mchbar_read16(0x40) >> 8 & 1;
	const int peg_x16 = (peg_enabled && !sdvo_enabled);

	init_egress();
	init_dmi(stepping >= STEPPING_B2);
	init_pcie(peg_enabled, sdvo_enabled, peg_x16);

	setup_aspm(stepping, peg_enabled);
	setup_rcrb(peg_enabled);
}
