/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "imc.h"
#include <arch/io.h>
#include <delay.h>
#include "Porting.h"
#include "AGESA.h"
#include <Lib/amdlib.h>
#include <Proc/Fch/Fch.h>
#include <Proc/Fch/Common/FchCommonCfg.h>
#include <Proc/Fch/FchPlatform.h>

#define VACPI_MMIO_VBASE ((u8 *)ACPI_MMIO_BASE)

void imc_reg_init(void)
{
	/* Init Power Management Block 2 (PM2) Registers.
	 * Check BKDG for AMD Family 16h for details. */
	write8(VACPI_MMIO_VBASE + PMIO2_BASE + 0x00, 0x06);
	write8(VACPI_MMIO_VBASE + PMIO2_BASE + 0x01, 0x06);
	write8(VACPI_MMIO_VBASE + PMIO2_BASE + 0x02, 0xf7);
	write8(VACPI_MMIO_VBASE + PMIO2_BASE + 0x03, 0xff);
	write8(VACPI_MMIO_VBASE + PMIO2_BASE + 0x04, 0xff);

#if !CONFIG_SOUTHBRIDGE_AMD_AGESA_YANGTZE
	write8(VACPI_MMIO_VBASE + PMIO2_BASE + 0x10, 0x06);
	write8(VACPI_MMIO_VBASE + PMIO2_BASE + 0x11, 0x06);
	write8(VACPI_MMIO_VBASE + PMIO2_BASE + 0x12, 0xf7);
	write8(VACPI_MMIO_VBASE + PMIO2_BASE + 0x13, 0xff);
	write8(VACPI_MMIO_VBASE + PMIO2_BASE + 0x14, 0xff);
#endif

#if CONFIG_SOUTHBRIDGE_AMD_AGESA_YANGTZE
	UINT8 PciData;
	PCI_ADDR PciAddress;
	AMD_CONFIG_PARAMS StdHeader;
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 0x3, 0x1E4);
	LibAmdPciRead(AccessWidth8, PciAddress, &PciData, &StdHeader);
	PciData &= (UINT8)0x8F;
	PciData |= 0x10;
	LibAmdPciWrite(AccessWidth8, PciAddress, &PciData, &StdHeader);
#endif
}

#ifndef __PRE_RAM__
void enable_imc_thermal_zone(void)
{
	AMD_CONFIG_PARAMS StdHeader;
	UINT8 FunNum;
	UINT8 regs[9];
	int i;

	regs[0] = 0;
	regs[1] = 0;
	FunNum = Fun_80;
	for (i=0; i<=1; i++)
		WriteECmsg(MSG_REG0 + i, AccessWidth8, &regs[i], &StdHeader);
	WriteECmsg(MSG_SYS_TO_IMC, AccessWidth8, &FunNum, &StdHeader);     // function number
	WaitForEcLDN9MailboxCmdAck(&StdHeader);

	for (i=2; i<=9; i++)
		ReadECmsg(MSG_REG0 + i, AccessWidth8, &regs[i], &StdHeader);

	/* enable thermal zone 0 */
	regs[2] |= 1;
	regs[0] = 0;
	regs[1] = 0;
	FunNum = Fun_81;
	for (i=0; i<=9; i++)
		WriteECmsg(MSG_REG0 + i, AccessWidth8, &regs[i], &StdHeader);
	WriteECmsg(MSG_SYS_TO_IMC, AccessWidth8, &FunNum, &StdHeader);     // function number
	WaitForEcLDN9MailboxCmdAck(&StdHeader);
}
#endif
