/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/intel/common/block/lpc/lpc_def.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <superio/aspeed/common/aspeed.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6791d/nct6791d.h>
#include <device/pnp_ops.h>

#define PCR_DMI_LPCIOD	0x2770
#define PCR_DMI_LPCIOE	0x2774

void bootblock_mainboard_early_init(void)
{
	/*
	 * Set up decoding windows on PCH over PCR. The CPU uses two of AST2600 SIO ports,
	 * one is connected to debug header (SUART1) and another is used as SOL (SUART2).
	 */
	const uint16_t lpciod = (LPC_IOD_COMB_RANGE | LPC_IOD_COMA_RANGE);
	const uint16_t lpcioe = (LPC_IOE_EC_4E_4F | LPC_IOE_COMB_EN | LPC_IOE_COMA_EN);

	/* Open IO windows: 0x3f8 for com1 and 02f8 for com2 */
	pcr_or32(PID_DMI, PCR_DMI_LPCIOD, lpciod);
	/* LPC I/O enable: com1 and com2 */
	pcr_or32(PID_DMI, PCR_DMI_LPCIOE, lpcioe);

	/* Enable com1 (0x3f8), com2 (0x2f8) and superio (0x4e) */
	pci_write_config16(PCH_DEV_LPC, LPC_IO_DECODE, lpciod);
	pci_write_config16(PCH_DEV_LPC, LPC_IO_ENABLES, lpcioe);

	/*
	 * Disable the Nuvoton NCT6791D SuperIO UART1.  It is enabled by
	 * default, but the AST2600's is connected to the serial port.
	 */
	const pnp_devfn_t nvt_serial_dev = PNP_DEV(0x2E, NCT6791D_SP1);
	nuvoton_pnp_enter_conf_state(nvt_serial_dev);
	pnp_set_logical_device(nvt_serial_dev);
	pnp_set_enable(nvt_serial_dev, 0);
	nuvoton_pnp_exit_conf_state(nvt_serial_dev);

	/* Enable AST2600 SuperIO UART1 */
	const pnp_devfn_t ast_serial_dev = PNP_DEV(0x4E, AST2400_SUART1);
	aspeed_enable_serial(ast_serial_dev, CONFIG_TTYS0_BASE);
}
