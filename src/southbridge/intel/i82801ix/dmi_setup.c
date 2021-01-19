/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_def.h>
#include <console/console.h>
#include "i82801ix.h"

/* VC1 Port Arbitration Table */
static const u8 vc1_pat[] = {
	0x0f, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0f, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xf0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0f,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0xf0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x0f, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0f, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xf0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0f,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0xf0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};
void i82801ix_dmi_setup(void)
{
	int i;
	u32 reg32;

	RCBA32(RCBA_V1CAP) = (RCBA32(RCBA_V1CAP) & ~(0x7f<<16)) | (0x12<<16);

	/* NB: other CIRs are handled in i82801ix_early_settings(). */
	RCBA32(RCBA_CIR1) = 0x00109000;
	RCBA16(RCBA_CIR3) = 0x060b;
	RCBA32(RCBA_CIR2) = 0x86000040;
	RCBA8(RCBA_BCR)   = 0x45;
	RCBA32(RCBA_CIR6) &= ~(1 << 7);

	/* VC1 setup for isochronous transfers: */

	/* Set VC1 virtual channel id to 1. */
	RCBA32(RCBA_V1CTL) = (RCBA32(RCBA_V1CTL) & ~(0x7 << 24)) | (0x1 << 24);
	/* Enable TC7 traffic on VC1. */
	RCBA32(RCBA_V1CTL) = (RCBA32(RCBA_V1CTL) & ~(0x7f << 1)) | (1 << 7);
	/* Disable TC7-TC1 traffic on VC0. */
	RCBA32(RCBA_V0CTL) &= ~(0x7f << 1);
	/* TC7-TC1 traffic on PCIe root ports will be disabled in pci driver. */

	/* Set table type to time-based WRR. */
	RCBA32(RCBA_V1CTL) = (RCBA32(RCBA_V1CTL) & ~(0x7 << 17)) | (0x4 << 17);
	/* Program port arbitration table. */
	for (i = 0; i < sizeof(vc1_pat); ++i)
		RCBA8(RCBA_PAT + i) = vc1_pat[i];
	/* Load port arbitration table. */
	RCBA32(RCBA_V1CTL) |= (1 << 16);

	/* Enable VC1. */
	RCBA32(RCBA_V1CTL) |= (1 << 31);

	/* Setup RCRB: */

	/* Set component id to 2 for southbridge, northbridge has id 1. */
	RCBA8(RCBA_ESD + 2) = 2;
	/* Set target port number and target component id of the northbridge. */
	RCBA8(RCBA_ULD + 3) = 1;
	RCBA8(RCBA_ULD + 2) = 1;
	/* Set target rcrb base address, i.e. DMIBAR. */
	RCBA32(RCBA_ULBA) = (uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE;

	/* Enable ASPM. */
	if (LPC_IS_MOBILE(PCI_DEV(0, 0x1f, 0))) {
		reg32 = RCBA32(RCBA_DMC);
		/* Enable mobile specific power saving (set this first). */
		reg32 = (reg32 & ~(3 << 10)) | (1 << 10);
		RCBA32(RCBA_DMC) = reg32;
		/* Enable DMI power savings. */
		reg32 |= (1 << 19);
		RCBA32(RCBA_DMC) = reg32;
		/* Advertise L0s and L1. */
		RCBA32(RCBA_LCAP) |= (3 << 10);
		/* Enable L0s and L1. */
		RCBA32(RCBA_LCTL) |= (3 <<  0);
	} else {
		/* Enable DMI power savings. */
		RCBA32(RCBA_DMC) |= (1 << 19);
		/* Advertise L0s only. */
		RCBA32(RCBA_LCAP) = (RCBA32(RCBA_LCAP) & ~(3<<10)) | (1<<10);
		/* Enable L0s only. */
		RCBA32(RCBA_LCTL) = (RCBA32(RCBA_LCTL) & ~(3<< 0)) | (1<< 0);
	}
}

/* Should be called after VC1 has been enabled on both sides. */
void i82801ix_dmi_poll_vc1(void)
{
	int timeout;

	timeout = 0x7ffff;
	printk(BIOS_DEBUG, "ICH9 waits for VC1 negotiation... ");
	while ((RCBA32(RCBA_V1STS) & (1 << 1)) && --timeout) {}
	if (!timeout)
		printk(BIOS_DEBUG, "timeout!\n");
	else
		printk(BIOS_DEBUG, "done.\n");

	/* Check for x2 DMI link. */
	if (((RCBA16(RCBA_LSTS) >> 4) & 0x3f) == 2) {
		printk(BIOS_DEBUG, "x2 DMI link detected.\n");
		RCBA32(0x2024) = (RCBA32(0x2024) & ~(7 << 21)) | (3 << 21);
		RCBA16(0x20c4) |= (1 << 15);
		RCBA16(0x20e4) |= (1 << 15);
		/* TODO: Maybe we have to save and
		         restore these settings across S3. */
	}

	timeout = 0x7ffff;
	printk(BIOS_DEBUG, "ICH9 waits for port arbitration table update... ");
	while ((RCBA32(RCBA_V1STS) & (1 << 0)) && --timeout) {}
	if (!timeout)
		printk(BIOS_DEBUG, "timeout!\n");
	else
		printk(BIOS_DEBUG, "done.\n");
}
