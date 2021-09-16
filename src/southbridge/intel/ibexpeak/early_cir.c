/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/ironlake/ironlake.h>
#include "pch.h"

/* This sets up magic Chipset Initialization Registers */
void pch_setup_cir(int chipset_type)
{
	printk(BIOS_DEBUG, "Setting up Chipset Initialization Registers (CIR)\n");

	uint16_t lpc_id = pci_read_config16(PCH_LPC_DEV, PCI_DEVICE_ID);
	struct cpuinfo_x86 c;
	uint32_t cir22;

	pci_or_config8(PCH_LPC_DEV, GEN_PMCON_3, 0xfb);

	RCBA32_OR(GCS, 0x40); /* FERR# MUX enable */

	RCBA8(0x3430) |= 0xfc;

	RCBA32(CIR7) = 0xf;

	RCBA32(CIR9) = 0;

	switch (lpc_id) {
	case 0x3b01:
	case 0x3b03:
	case 0x3b05:
	case 0x3b07:
	case 0x3b09:
	case 0x3b0b:
	case 0x3b0d:
	case 0x3b0f:
		RCBA32_AND_OR(CIR6, 0xff1fff7f, 0x600000);
		break;
	}

	RCBA32_OR(0x3310, 0x31);

	/* Intel 5 Series Chipset and Intel 3400 Series Chipset
	   External Design Specification (EDS) 13.8.1.1 */
	if (chipset_type == IRONLAKE_DESKTOP)
		pci_or_config32(PCH_LPC_DEV, GEN_PMCON_1, 1 << 3);

	pci_write_config8(PCH_LPC_DEV, CIR4, 0x45);

	RCBA32(CIR8) = 0x4000000;
	pci_write_config32(PCH_LPC_DEV, PMIR, 0xc0000300);
	RCBA32(0x3318) = 0x1020000; /* undocumented */
	get_fms(&c, cpuid_eax(1));
	if (c.x86_model == 0x1e) {
		/* Lynnfield/Clarksfield */
		RCBA32(CIR13) = 0xfffff;
		RCBA32(CIR14) = 0x61080;
		RCBA32(CIR16) = 0x7f8f9f80;
		RCBA32(CIR18) = 0x3900;
		RCBA32(CIR19) = 0x20002;
		RCBA32(CIR20) = 0x44b00;
		RCBA32(CIR21) = 0x02000;
		cir22 = 0x20000;
	} else if (c.x86_model == 0x1f || c.x86_model == 0x25) {
		/* Auburndale/Havendale + Arrandale/Clarkdale */
		RCBA32(CIR10) = 0xfff80;
		RCBA32(CIR15) = 0x7f8f9fff;
		RCBA32(CIR17) = 0x2900;
		RCBA32(CIR19) = 0x10001;
		RCBA32(CIR20) = 0x1004b;
		RCBA32(CIR21) = 0x8;
		cir22 = 0x10000;
	} else {
		die("unsupported CPU model: %x!\n", c.x86_model);
	}

	/* EDS, 10.1.77: Program this register after all registers in the
	   3330-33D3 range and D31:F0:A9h are already programmed */
	RCBA32(CIR22) = cir22;
}
