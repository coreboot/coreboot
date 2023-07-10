/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/intel/common/block/lpc/lpc_def.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <superio/aspeed/common/aspeed.h>

#define ASPEED_SIO_PORT 0x2E
#define PCR_DMI_LPCIOD  0x2770
#define PCR_DMI_LPCIOE  0x2774

void bootblock_mainboard_early_init(void)
{
	uint16_t lpciod = LPC_IOD_COMA_RANGE;
	uint16_t lpcioe = (LPC_IOE_SUPERIO_2E_2F | LPC_IOE_COMA_EN);

	/* Open IO windows: 0x3f8 for com1 */
	pcr_or32(PID_DMI, PCR_DMI_LPCIOD, lpciod);
	/* LPC I/O enable: com1 */
	pcr_or32(PID_DMI, PCR_DMI_LPCIOE, lpcioe);

	/* Enable com1 (0x3f8) and superio (0x2e) */
	pci_write_config16(PCH_DEV_LPC, LPC_IO_DECODE, lpciod);
	pci_write_config16(PCH_DEV_LPC, LPC_IO_ENABLES, lpcioe);
}
