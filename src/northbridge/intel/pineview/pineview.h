/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef NORTHBRIDGE_INTEL_PINEVIEW_H
#define NORTHBRIDGE_INTEL_PINEVIEW_H

#include <southbridge/intel/i82801gx/i82801gx.h>

#define DEFAULT_PMIOBAR		0x00000400

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

/*
 * MCHBAR
 */

#include <northbridge/intel/common/fixed_bars.h>

/* As there are many registers, define them on a separate file */

#include "mchbar_regs.h"

void pineview_early_init(void);
u32 decode_igd_memory_size(const u32 gms);
u32 decode_igd_gtt_size(const u32 gsm);

/* Mainboard romstage callback functions */
void get_mb_spd_addrmap(u8 *spd_addr_map);
void mb_pirq_setup(void); /* optional */

#endif /* NORTHBRIDGE_INTEL_PINEVIEW_H */
