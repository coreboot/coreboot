/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Ec controller
 *
 * Init Ec Controller features (PEI phase).
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_IMC_FCHECRESET_FILECODE

/**
 * FchInitResetEc - Config Ec controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetEc (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //Enable config mode
  //
  EnterEcConfig (StdHeader);

  //
  //Do settings for mailbox - logical device 0x09
  //
  RwEc8 (0x07, 0x00, 0x09, StdHeader);                      ///switch to device 9 (Mailbox)
  RwEc8 (0x60, 0x00, (MailBoxPort >> 8), StdHeader);        ///set MSB of Mailbox port
  RwEc8 (0x61, 0x00, (MailBoxPort & 0xFF), StdHeader);      ///set LSB of Mailbox port
  RwEc8 (0x30, 0x00, 0x01, StdHeader);                      ///;Enable Mailbox Registers Interface, bit0=1

  if ( LocalCfgPtr->EcKbd == ENABLED) {
    //
    //Enable KBRST#, IRQ1 & IRQ12, GateA20 Function signal from IMC
    //
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD6, AccessWidth8, (UINT32)~BIT8, BIT0 + BIT1 + BIT2 + BIT3);

    //
    //Disable LPC Decoding of port 60/64
    //
    RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG47), AccessWidth8, (UINT32)~BIT5, 0, StdHeader);

    //
    //Enable logical device 0x07 (Keyboard controller)
    //
    RwEc8 (0x07, 0x00, 0x07, StdHeader);
    RwEc8 (0x30, 0x00, 0x01, StdHeader);
  }

  if (IsImcEnabled (StdHeader) && ( LocalCfgPtr->EcChannel0 == ENABLED)) {
    //
    //Logical device 0x03
    //
    RwEc8 (0x07, 0x00, 0x03, StdHeader);
    RwEc8 (0x60, 0x00, 0x00, StdHeader);
    RwEc8 (0x61, 0x00, 0x62, StdHeader);
    RwEc8 (0x30, 0x00, 0x01, StdHeader);                    ///;Enable Device 3
  }

  //
  //Enable EC (IMC) to generate SMI to BIOS
  //
  RwMem (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB3, AccessWidth8, (UINT32)~BIT6, BIT6);
  ExitEcConfig (StdHeader);
}



