/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef NORTHBRIDGE_INTEL_PINEVIEW_H
#define NORTHBRIDGE_INTEL_PINEVIEW_H

#include <northbridge/intel/pineview/memmap.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

#define BOOT_PATH_NORMAL	0
#define BOOT_PATH_RESET		1
#define BOOT_PATH_RESUME	2

/* Device 0:0.0 PCI configuration space (Host Bridge) */
#define HOST_BRIDGE	PCI_DEV(0, 0, 0)

#include "hostbridge_regs.h"

/* Device 0:1.0 PCI configuration space (PCI Express) */

#define PEGSTS		0x214	/* 32 bits */

/* Device 0:2.0 PCI configuration space (Integrated Graphics Device) */
#define GMCH_IGD	PCI_DEV(0, 2, 0)

#define GMADR		0x18
#define GTTADR		0x1c
#define BSM		0x5c

#define GPIO32(x) *((volatile u32 *)(DEFAULT_GPIOBASE + x))

/*
 * MCHBAR
 */

#define MCHBAR8(x)  (*((volatile u8  *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR16(x) (*((volatile u16 *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR32(x) (*((volatile u32 *)(DEFAULT_MCHBAR +  x)))	/* FIXME: causes changes */
#define MCHBAR8_AND(x,  and) (MCHBAR8(x)  = MCHBAR8(x)  & (and))
#define MCHBAR16_AND(x, and) (MCHBAR16(x) = MCHBAR16(x) & (and))
#define MCHBAR32_AND(x, and) (MCHBAR32(x) = MCHBAR32(x) & (and))
#define MCHBAR8_OR(x,   or)  (MCHBAR8(x)  = MCHBAR8(x)  | (or))
#define MCHBAR16_OR(x,  or)  (MCHBAR16(x) = MCHBAR16(x) | (or))
#define MCHBAR32_OR(x,  or)  (MCHBAR32(x) = MCHBAR32(x) | (or))
#define MCHBAR8_AND_OR(x,  and, or) (MCHBAR8(x)  = (MCHBAR8(x)  & (and)) | (or))
#define MCHBAR16_AND_OR(x, and, or) (MCHBAR16(x) = (MCHBAR16(x) & (and)) | (or))
#define MCHBAR32_AND_OR(x, and, or) (MCHBAR32(x) = (MCHBAR32(x) & (and)) | (or))

/* As there are many registers, define them on a separate file */

#include "mchbar_regs.h"

/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#define EPBAR8(x)  *((volatile u8  *)(DEFAULT_EPBAR + (x)))
#define EPBAR16(x) *((volatile u16 *)(DEFAULT_EPBAR + (x)))
#define EPBAR32(x) *((volatile u32 *)(DEFAULT_EPBAR + (x)))

/*
 * DMIBAR
 */

#define DMIBAR8(x)  *((volatile u8  *)(DEFAULT_DMIBAR + (x)))
#define DMIBAR16(x) *((volatile u16 *)(DEFAULT_DMIBAR + (x)))
#define DMIBAR32(x) *((volatile u32 *)(DEFAULT_DMIBAR + (x)))

void pineview_early_init(void);
u32 decode_igd_memory_size(const u32 gms);
u32 decode_igd_gtt_size(const u32 gsm);
int decode_pcie_bar(u32 *const base, u32 *const len);

/* Mainboard romstage callback functions */
void get_mb_spd_addrmap(u8 *spd_addr_map);
void mb_pirq_setup(void); /* optional */

#endif /* NORTHBRIDGE_INTEL_PINEVIEW_H */
