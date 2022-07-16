/* SPDX-License-Identifier: GPL-2.0-only */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <device/pci_ops.h>
#include <cf9_reset.h>
#include <reset.h>

#define HT_INIT_CONTROL		0x6c
#define HTIC_ColdR_Detect	(1 << 4)
#define HTIC_BIOSR_Detect	(1 << 5)
#define HTIC_INIT_Detect	(1 << 6)

void cf9_reset_prepare(void)
{
	u32 htic;
	htic = pci_s_read_config32(PCI_DEV(0, 0x18, 0), HT_INIT_CONTROL);
	htic &= ~HTIC_BIOSR_Detect;
	pci_s_write_config32(PCI_DEV(0, 0x18, 0), HT_INIT_CONTROL, htic);
}

void do_board_reset(void)
{
	system_reset();
}
