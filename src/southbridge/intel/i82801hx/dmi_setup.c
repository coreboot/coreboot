/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel ICH8-M (82801HBM) Southbridge - DMI Link Setup
 * Reverse-engineered from ThinkPad X61 Phoenix BIOS
 *
 * Configures the DMI (Direct Media Interface) virtual channels
 * on the southbridge side. Must be paired with northbridge DMI setup.
 *
 * Reference: coreboot ICH9 (i82801ix) dmi_setup.c
 */

#include <stdint.h>
#include <stddef.h>
#include <device/pci_ops.h>
#include <device/mmio.h>
#include <console/console.h>
#include <southbridge/intel/i82801hx/i82801hx.h>

/* VC1 Port Arbitration Table - time-based weighted round robin
 * Copied from coreboot ICH9 dmi_setup.c - identical for ICH8-M */
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

/*
 * i82801hx_dmi_setup() - Configure DMI link (southbridge side)
 *
 * Sets up virtual channels VC0/VC1 in the RCBA, programs the
 * VC1 port arbitration table, configures component IDs, and
 * enables ASPM.
 *
 * BIOS equivalent: observed in bioscode_7.rom DMI init
 * coreboot equivalent: i82801ix_dmi_setup()
 */
void i82801hx_dmi_setup(void)
{
	size_t i;
	u32 reg32;

	/*
	 * VC1 capability: set WRR count.
	 * RCBA offset 0x001C, bits [22:16] = 0x12
	 */
	RCBA32(RCBA_V1CAP) = (RCBA32(RCBA_V1CAP) & ~(0x7f << 16)) | (0x12 << 16);

	/* CIR (Chipset Initialization Registers)  */
	RCBA32(RCBA_CIR1) = 0x00109000;
	RCBA16(RCBA_CIR3) = 0x060b;
	RCBA32(RCBA_CIR2) = 0x86000040;
	RCBA32(RCBA_CIR4) = 0x00002008;
	RCBA8(RCBA_BCR) = 0x45;
	/* Vendor BIOS masked writes: clear bit 7, set high-word mask 0x0d00. */
	/* TODO: different on desktop */
	RCBA32(RCBA_CIR6) = (RCBA32(RCBA_CIR6) & ~((1 << 7) | (0xff00 << 16))) |
			     (0x0d00 << 16);

	/* Set VC1 virtual channel id to 1. */
	RCBA32(RCBA_V1CTL) = (RCBA32(RCBA_V1CTL) & ~(0x7 << 24)) | (0x1 << 24);
	/* Enable TC7 traffic on VC1. */
	RCBA32(RCBA_V1CTL) = (RCBA32(RCBA_V1CTL) & ~(0x7f << 1)) | (1 << 7);
	/* Disable TC7-TC1 traffic on VC0. */
	RCBA32(RCBA_V0CTL) &= ~(0x7f << 1);

	/* Set table type to time-based WRR. */
	RCBA32(RCBA_V1CTL) = (RCBA32(RCBA_V1CTL) & ~(0x7 << 17)) | (0x4 << 17);

	/* Program port arbitration table */
	for (i = 0; i < sizeof(vc1_pat); i++)
		RCBA8(RCBA_PAT + i) = vc1_pat[i];

	/* Load port arbitration table */
	RCBA32(RCBA_V1CTL) |= (1 << 16);

	/* Enable VC1 */
	RCBA32(RCBA_V1CTL) |= (1 << 31);

	/*
	 * RCRB setup:
	 * - Set component ID to 2 (southbridge; northbridge is 1)
	 * - Set target port and component ID for northbridge
	 * - Set DMIBAR as target RCRB base
	 */
	RCBA8(RCBA_ESD + 2) = 2; /* SB component ID = 2 */
	RCBA8(RCBA_ULD + 3) = 1; /* Target port = 1 */
	RCBA8(RCBA_ULD + 2) = 1; /* Target component ID = 1 */
	RCBA32(RCBA_ULBA) = (uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE;

	/*
	 * Enable ASPM (Active State Power Management).
	 * ICH8-M is always mobile, so enable L0s + L1.
	 */
	reg32 = RCBA32(RCBA_DMC);
	/* Enable mobile specific power saving (set this first). */
	reg32 = (reg32 & ~(3 << 10)) | (1 << 10);
	RCBA32(RCBA_DMC) = reg32;
	/* Enable DMI power savings. */
	reg32 |= (1 << 19);
	RCBA32(RCBA_DMC) = reg32;

	/* Advertise L0s and L1 support */
	RCBA32(RCBA_LCAP) |= (3 << 10);
	/* Enable L0s and L1 */
	RCBA32(RCBA_LCTL) |= (3 << 0);
}

/*
 * i82801hx_dmi_poll_vc1() - Wait for VC1 negotiation to complete
 *
 * Called after both NB and SB sides have enabled VC1.
 * Polls the VC1 status register until negotiation finishes.
 *
 * coreboot equivalent: i82801ix_dmi_poll_vc1()
 */
void i82801hx_dmi_poll_vc1(void)
{
	int timeout;

	/* Wait for VC1 negotiation pending bit to clear */
	timeout = 0x7ffff;
	printk(BIOS_DEBUG, "ICH8-M waits for VC1 negotiation... ");
	while ((RCBA32(RCBA_V1STS) & (1 << 1)) && --timeout)
		;
	if (!timeout)
		printk(BIOS_DEBUG, "timeout!\n");
	else
		printk(BIOS_DEBUG, "done.\n");

	/*
	 * Check for x2 DMI link width.
	 * RCBA_LSTS bits [9:4] = negotiated link width.
	 * If x2, apply workaround settings.
	 */
	if (((RCBA16(RCBA_LSTS) >> 4) & 0x3f) == 2) {
		printk(BIOS_DEBUG, "x2 DMI link detected.\n");
		RCBA32(RCBA_CIR6) = (RCBA32(RCBA_CIR6) & ~(7 << 21)) | (3 << 21);
		RCBA16(0x20c4) |= (1 << 15);
		RCBA16(0x20e4) |= (1 << 15);
	}

	/* Wait for port arbitration table update */
	timeout = 0x7ffff;
	printk(BIOS_DEBUG, "ICH8-M waits for port arbitration table update... ");
	while ((RCBA32(RCBA_V1STS) & (1 << 0)) && --timeout)
		;
	if (!timeout)
		printk(BIOS_DEBUG, "timeout!\n");
	else
		printk(BIOS_DEBUG, "done.\n");
}
