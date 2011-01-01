
/**
 * @file
 *
 * Config Southbridge EC Controller
 *
 * Init EC features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */

#include "SBPLATFORM.h"

#ifndef NO_EC_SUPPORT

/**
 * Config EC controller during power-on
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
ecPowerOnInit (
  IN       AMDSBCFG* pConfig
  )
{
  //Enable config mode
  EnterEcConfig ();

  //Do settings for mailbox - logical device 0x09
  RWEC8 (0x07, 0x00, 0x09);               //switch to device 9 (Mailbox)
  RWEC8 (0x60, 0x00, (MailBoxPort >> 8));    //set MSB of Mailbox port
  RWEC8 (0x61, 0x00, (MailBoxPort & 0xFF));  //set LSB of Mailbox port
  RWEC8 (0x30, 0x00, 0x01);               //;Enable Mailbox Registers Interface, bit0=1

  if ( pConfig->BuildParameters.EcKbd == ENABLED) {
    //Enable KBRST#, IRQ1 & IRQ12, GateA20 Function signal from IMC
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD6, AccWidthUint8, ~BIT8, BIT0 + BIT1 + BIT2 + BIT3);

    //Disable LPC Decoding of port 60/64
    RWPCI (((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG47), AccWidthUint8 | S3_SAVE, ~BIT5, 0);

    //Enable logical device 0x07 (Keyboard controller)
    RWEC8 (0x07, 0x00, 0x07);
    RWEC8 (0x30, 0x00, 0x01);
  }

  if ( pConfig->BuildParameters.EcChannel0 == ENABLED) {
    //Logical device 0x03
    RWEC8 (0x07, 0x00, 0x03);
    RWEC8 (0x60, 0x00, 0x00);
    RWEC8 (0x61, 0x00, 0x62);
    RWEC8 (0x30, 0x00, 0x01);            //;Enable Device 8
  }

  //Enable EC (IMC) to generate SMI to BIOS
  RWMEM (ACPI_MMIO_BASE + SMI_BASE + SB_SMI_REGB3, AccWidthUint8, ~BIT6, BIT6);
  ExitEcConfig ();
}

/**
 * Config EC controller before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
ecInitBeforePciEnum (
  IN       AMDSBCFG* pConfig
  )
{
  AMDSBCFG*     pTmp;                                    // dummy code
  pTmp = pConfig;
}

/**
 * Prepare EC controller to boot to OS.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
ecInitLatePost (
  IN       AMDSBCFG* pConfig
  )
{
  AMDSBCFG*     pTmp;                                    // dummy code
  pTmp = pConfig;
}
#endif
