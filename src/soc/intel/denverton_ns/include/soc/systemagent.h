/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_SYSTEMAGENT_H_
#define _DENVERTON_NS_SYSTEMAGENT_H_

#include <soc/iomap.h>

/* Device 0:0.0 PCI configuration space (Host Bridge) */
#define PCH_SA_DEV PCI_DEV(0, SA_DEV, SA_FUNC)

#define MCHBAR 0x48   /* MCH space. */
#define PCIEXBAR 0x60 /* PCI express space. */
#define MASK_PCIEXBAR_256M 0xF0000000
#define MASK_PCIEXBAR_128M 0xF8000000
#define MASK_PCIEXBAR_64M 0xFC000000
#define MASK_PCIEXBAR_LENGTH 0x6
#define SHIFT_PCIEXBAR_LENGTH 0x1
#define MASK_PCIEXBAR_LENGTH_256M (0x0 << SHIFT_PCIEXBAR_LENGTH)
#define MASK_PCIEXBAR_LENGTH_128M (0x1 << SHIFT_PCIEXBAR_LENGTH)
#define MASK_PCIEXBAR_LENGTH_64M (0x2 << SHIFT_PCIEXBAR_LENGTH)

#define TOUUD_LO 0xa8 /* Top of Upper Usable DRAM - Low */
#define MASK_TOUUD_LO 0xFFF00000
#define TOUUD_HI 0xac /* Top of Upper Usable DRAM - High */
#define MASK_TOUUD_HI 0x0000007F
#define TOUUD TOUUD_LO /* Top of Upper Usable DRAM */
#define MASK_TOUUD 0x7FFFF00000

#define TSEGMB 0xb8 /* TSEG base */
#define MASK_TSEGMB 0xFFF00000
#define TOLUD 0xbc /* Top of Low Used Memory */
#define MASK_TOLUD 0xFFF00000

#define CAPID0_A	0xe4
#define  VTD_DISABLE	(1 << 23)

/* SideBand B-UNIT */
#define B_UNIT 3

/* SideBand C-UNIT */
#define C_UNIT 8

/* SideBand D-UNIT */
#define D_UNIT 1

/* SideBand P-UNIT */
#define P_UNIT 4

/*
 * MCHBAR
 */
#define MCH_BASE_SIZE 0x8000
#define MCH_BMISC 0x6800
#define MCH_BMISC_SBVDRAM \
	0x08 /* Bit 3: 1 - reads targeting boot vector are routed to DRAM. */
#define MCH_BMISC_ABSEGINDRAM \
	0x04 /* Bit 2: 1 - reads targeting A/B-segment are routed to DRAM. */
#define MCH_BMISC_RFSDRAM \
	0x02 /* Bit 1: 1 - reads targeting E-segment are routed to DRAM. */
#define MCH_BMISC_RESDRAM \
	0x01 /* Bit 0: 1 - reads targeting E-segment are routed to DRAM. */

#define MCH_VTBAR_OFFSET		0x6c80
#define  MCH_VTBAR_ENABLE_MASK		0x1
#define  MCH_VTBAR_MASK			0x7ffffff000

#define MCH_BAR_BIOS_RESET_CPL 0x7078
#define RST_CPL_BIT (1 << 0)
#define PCODE_INIT_DONE (1 << 8)
#define MCH_BAR_CORE_EXISTS_MASK 0x7164
#define MCH_BAR_CORE_DISABLE_MASK 0x7168

/* Device 0:4.0 PCI configuration space (RAS) */

/* Device 0:5.0 PCI configuration space (RCEC) */

/* Top of 32bit usable memory */
u32 top_of_32bit_ram(void);

#endif //_DENVERTON_NS_SYSTEMAGENT_H_
