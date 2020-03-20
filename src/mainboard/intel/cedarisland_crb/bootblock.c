/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <superio/aspeed/common/aspeed.h>

void bootblock_mainboard_early_init(void)
{
	/* Enable COM1 only */
	pcr_write32(PID_DMI, 0x2770, 0);
	pcr_write32(PID_DMI, 0x2774, 1);

	/* Decode for SuperIO (0x2e) and COM1 (0x3f8) */
	pci_mmio_write_config32(PCH_DEV_LPC, 0x80, (1 << 28) | (1 << 16));

	const pnp_devfn_t serial_dev = PNP_DEV(0x2e, AST2400_SUART1);
	aspeed_enable_serial(serial_dev, CONFIG_TTYS0_BASE);
}
