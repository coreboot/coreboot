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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#define __SIMPLE_DEVICE__

#include "imc.h"
#include <arch/io.h>
#include <device/device.h>
#include <delay.h>
#include <Porting.h>
#include <AGESA.h>
#include <Lib/amdlib.h>
#include <Proc/Fch/Common/FchCommonCfg.h>
#include <Proc/Fch/Fch.h>
#include <Proc/Fch/FchPlatform.h>

#define VACPI_MMIO_VBASE ((u8 *)ACPI_MMIO_BASE)

void imc_reg_init(void)
{
	u8 reg8;
	/* Init Power Management Block 2 (PM2) Registers.
	 * Check BKDG for AMD Family 16h for details. */
	write8((VACPI_MMIO_VBASE + PMIO2_BASE + 0x00), 0x06);
	write8((VACPI_MMIO_VBASE + PMIO2_BASE + 0x01), 0x06);
	write8((VACPI_MMIO_VBASE + PMIO2_BASE + 0x02), 0xf7);
	write8((VACPI_MMIO_VBASE + PMIO2_BASE + 0x03), 0xff);
	write8((VACPI_MMIO_VBASE + PMIO2_BASE + 0x04), 0xff);

	write8((VACPI_MMIO_VBASE + PMIO2_BASE + 0x10), 0x06);
	write8((VACPI_MMIO_VBASE + PMIO2_BASE + 0x11), 0x06);
	write8((VACPI_MMIO_VBASE + PMIO2_BASE + 0x12), 0xf7);
	write8((VACPI_MMIO_VBASE + PMIO2_BASE + 0x13), 0xff);
	write8((VACPI_MMIO_VBASE + PMIO2_BASE + 0x14), 0xff);

	reg8 = pci_read_config8(PCI_DEV(0, 0x18, 0x3), 0x1E4);
	reg8 &= 0x8F;
	reg8 |= 0x10;
	pci_write_config8(PCI_DEV(0, 0x18, 0x3), 0x1E4, reg8);
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
	for (i = 0; i <= 1; i++)
		WriteECmsg(MSG_REG0 + i, AccessWidth8, &regs[i], &StdHeader);
	WriteECmsg(MSG_SYS_TO_IMC, AccessWidth8, &FunNum, &StdHeader);
	WaitForEcLDN9MailboxCmdAck(&StdHeader);

	for (i = 2; i <= 9; i++)
		ReadECmsg(MSG_REG0 + i, AccessWidth8, &regs[i], &StdHeader);

	/* enable thermal zone 0 */
	regs[2] |= 1;
	regs[0] = 0;
	regs[1] = 0;
	FunNum = Fun_81;
	for (i = 0; i <= 9; i++)
		WriteECmsg(MSG_REG0 + i, AccessWidth8, &regs[i], &StdHeader);
	WriteECmsg(MSG_SYS_TO_IMC, AccessWidth8, &FunNum, &StdHeader);
	WaitForEcLDN9MailboxCmdAck(&StdHeader);
}
#endif

/* Bettong Hardware Monitor Fan Control
 * Hardware limitation:
 *  HWM will fail to read the input temperature via I2C if other
 *  software switches the I2C address.  AMD recommends using IMC
 *  to control fans, instead of HWM.
 */
void oem_fan_control(FCH_DATA_BLOCK *FchParams)
{
	/* Enable IMC fan control. the recommand way */
	imc_reg_init();

	FchParams->Imc.ImcEnable = TRUE;
	FchParams->Hwm.HwmControl = 1; /* 1 IMC, 0 HWM */
	FchParams->Imc.ImcEnableOverWrite = 1; /* 2 disable IMC, 1 enable IMC, 0 following hw strap setting */

	LibAmdMemFill(&(FchParams->Imc.EcStruct), 0, sizeof(FCH_EC), FchParams->StdHeader);
}
