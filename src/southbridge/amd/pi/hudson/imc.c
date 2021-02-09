/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include "imc.h"
#include <amdblocks/acpimmio.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <Porting.h>
#include <AGESA.h>
#include <Lib/amdlib.h>
#include <Proc/Fch/Common/FchCommonCfg.h>
#include <Proc/Fch/Fch.h>
#include <Proc/Fch/FchPlatform.h>

void imc_reg_init(void)
{
	u8 reg8;
	/* Init Power Management Block 2 (PM2) Registers.
	 * Check BKDG for AMD Family 16h for details. */
	pm2_write8(0, 0x06);
	pm2_write8(1, 0x06);
	pm2_write8(2, 0xf7);
	pm2_write8(3, 0xff);
	pm2_write8(4, 0xff);

	pm2_write8(0x10, 0x06);
	pm2_write8(0x11, 0x06);
	pm2_write8(0x12, 0xf7);
	pm2_write8(0x13, 0xff);
	pm2_write8(0x14, 0xff);

	reg8 = pci_read_config8(PCI_DEV(0, 0x18, 0x3), 0x1E4);
	reg8 &= 0x8F;
	reg8 |= 0x10;
	pci_write_config8(PCI_DEV(0, 0x18, 0x3), 0x1E4, reg8);
}

void enable_imc_thermal_zone(void)
{
	AMD_CONFIG_PARAMS StdHeader;
	UINT8 FunNum;
	UINT8 regs[10];
	int i;

	regs[0] = 0;
	regs[1] = 0;
	FunNum = Fun_80;
	for (i = 0; i <= 1; i++)
		WriteECmsg(MSG_REG0 + i, AccessWidth8, &regs[i], &StdHeader);
	WriteECmsg(MSG_SYS_TO_IMC, AccessWidth8, &FunNum, &StdHeader);
	WaitForEcLDN9MailboxCmdAck(&StdHeader);

	for (i = 2; i < ARRAY_SIZE(regs); i++)
		ReadECmsg(MSG_REG0 + i, AccessWidth8, &regs[i], &StdHeader);

	/* enable thermal zone 0 */
	regs[2] |= 1;
	regs[0] = 0;
	regs[1] = 0;
	FunNum = Fun_81;
	for (i = 0; i < ARRAY_SIZE(regs); i++)
		WriteECmsg(MSG_REG0 + i, AccessWidth8, &regs[i], &StdHeader);
	WriteECmsg(MSG_SYS_TO_IMC, AccessWidth8, &FunNum, &StdHeader);
	WaitForEcLDN9MailboxCmdAck(&StdHeader);
}
