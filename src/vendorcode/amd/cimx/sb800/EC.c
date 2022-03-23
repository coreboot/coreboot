
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
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***************************************************************************
 *
 */

#include "SBPLATFORM.h"
#include "cbtypes.h"

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

  if ( pConfig->BuildParameters.EcKbd == CIMX_OPTION_ENABLED) {
    //Enable KBRST#, IRQ1 & IRQ12, GateA20 Function signal from IMC
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD6, AccWidthUint8, ~BIT8, BIT0 + BIT1 + BIT2 + BIT3);

    //Disable LPC Decoding of port 60/64
    RWPCI (((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG47), AccWidthUint8 | S3_SAVE, ~BIT5, 0);

    //Enable logical device 0x07 (Keyboard controller)
    RWEC8 (0x07, 0x00, 0x07);
    RWEC8 (0x30, 0x00, 0x01);
  }

  if ( pConfig->BuildParameters.EcChannel0 == CIMX_OPTION_ENABLED) {
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
  /* AMDSBCFG*     pTmp;                                    // dummy code */
  /* pTmp = pConfig; */
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
  /* AMDSBCFG*     pTmp;                                    // dummy code */
  /* pTmp = pConfig; */
}
#endif
