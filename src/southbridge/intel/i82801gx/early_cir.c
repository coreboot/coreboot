/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/pci_def.h>
#include <device/pci_ops.h>
#include "i82801gx.h"

/* Chipset Initialization Registers magic */
void ich7_setup_cir(void)
{
	uint32_t reg32;
	uint8_t revision = pci_read_config8(PCI_DEV(0, 31, 0), PCI_REVISION_ID);
	uint16_t pci_id = pci_read_config16(PCI_DEV(0, 31, 0), PCI_DEVICE_ID);

	RCBA32(0x0088) = 0x0011d000;
	RCBA16(0x01fc) = 0x060f;
	RCBA32(0x01f4) = 0x86000040;
	/* Although bit 6 is set, it is not read back */
	RCBA32(0x0214) = 0x10030549;
	RCBA32(0x0218) = 0x00020504;
	RCBA8(0x0220) = 0xc5;
	RCBA32_AND_OR(0x3430, ~(3 << 0), 1 << 0);
	RCBA16(0x0200) = 0x2008;
	RCBA8(0x2027) = 0x0d;
	RCBA16(0x3e08) |= (1 << 7);
	RCBA16(0x3e48) |= (1 << 7);
	RCBA32(0x3e0e) |= (1 << 7);
	RCBA32(0x3e4e) |= (1 << 7);

	/* Only on mobile variants of revision b0 or later */
	switch (pci_id) {
	case 0x27b9:
	case 0x27bc:
	case 0x27bd:
		if (revision >= 2) {
			reg32 = RCBA32(0x2034);
			reg32 &= ~(0x0f << 16);
			reg32 |= (5 << 16);
			RCBA32(0x2034) = reg32;
		}
		/* FERR# MUX Enable (FME) */
		reg32 = RCBA32(GCS);
		reg32 |= (1 << 6);
		RCBA32(GCS) = reg32;
	}
}
