/* SPDX-License-Identifier: GPL-2.0-only */

#include "imc.h"
#include <amdblocks/acpimmio.h>
#include <device/mmio.h>
#include <Porting.h>
#include <AGESA.h>
#include <amdlib.h>
#include <Proc/Fch/Fch.h>
#include <Proc/Fch/Common/FchCommonCfg.h>
#include <Proc/Fch/FchPlatform.h>

void imc_reg_init(void)
{
	/* Init Power Management Block 2 (PM2) Registers.
	 * Check BKDG for AMD Family 16h for details. */
	pm2_write8(0, 0x06);
	pm2_write8(1, 0x06);
	pm2_write8(2, 0xf7);
	pm2_write8(3, 0xff);
	pm2_write8(4, 0xff);

#if !CONFIG(SOUTHBRIDGE_AMD_AGESA_YANGTZE)
	pm2_write8(0x10, 0x06);
	pm2_write8(0x11, 0x06);
	pm2_write8(0x12, 0xf7);
	pm2_write8(0x13, 0xff);
	pm2_write8(0x14, 0xff);
#endif

#if CONFIG(SOUTHBRIDGE_AMD_AGESA_YANGTZE)
	UINT8 PciData;
	PCI_ADDR PciAddress;
	AMD_CONFIG_PARAMS StdHeader;
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 0x3, 0x1E4);
	LibAmdPciRead(AccessWidth8, PciAddress, &PciData, &StdHeader);
	PciData &= (UINT8)0x8F;
	PciData |= 0x10;
	LibAmdPciWrite(AccessWidth8, PciAddress, &PciData, &StdHeader);
#endif
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
	WriteECmsg(MSG_SYS_TO_IMC, AccessWidth8, &FunNum, &StdHeader);     // function number
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
	WriteECmsg(MSG_SYS_TO_IMC, AccessWidth8, &FunNum, &StdHeader);     // function number
	WaitForEcLDN9MailboxCmdAck(&StdHeader);
}
