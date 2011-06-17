/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Pcib controller
 *
 * Init Pcib Controller features (PEI phase).
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 48244 $   @e \$Date: 2011-03-05 12:39:46 +0800 (Sat, 05 Mar 2011) $
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
****************************************************************************
*/
#include "FchPlatform.h"
#define FILECODE PROC_FCH_PCIB_PCIBRESET_FILECODE
/**
 * FchInitResetPcibPciTable - Pcib device registers initial
 * during the power on stage.
 *
 *
 *
 *
 */
REG8_MASK FchInitResetPcibPciTable[] =
{
  //
  // P2P Bridge (Bus 0, Dev 20, Func 4)
  //
  {0x00, PCIB_BUS_DEV_FUN, 0},
  {FCH_PCIB_REG4B, 0xFF, BIT6 + BIT7 + BIT4},
  // ENH230012: Disable P2P bridge decoder for IO address 0x1000-0x1FFF in Reset
  // ENH261115: Add PCI port 80 support in Hudson-2/3. Platform bios define SB_PCIB_PORT_80_SUPPORT to support it
#ifdef SB_PCIB_PORT_80_SUPPORT
  {FCH_PCIB_REG1C, 0x00, 0xF0},
  {FCH_PCIB_REG1D, 0x00, 0x00},
  {FCH_PCIB_REG04, 0x00, 0x21},
#endif
  {FCH_PCIB_REG40, 0xDF, 0x20},
  {FCH_PCIB_REG50, 0x02, 0x01},
  {0xFF, 0xFF, 0xFF},
};

/**
 * FchInitResetPcib - Config Pcib controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetPcib (
  IN  VOID     *FchDataPtr
  )
{
  AMD_CONFIG_PARAMS         *StdHeader;

  StdHeader = &((AMD_RESET_PARAMS *)FchDataPtr)->StdHeader;

  ProgramPciByteTable (
    (REG8_MASK*) (&FchInitResetPcibPciTable[0]),
    sizeof (FchInitResetPcibPciTable) / sizeof (REG8_MASK),
    StdHeader
    );
}

/**
 * FchInitRecoveryPcib - Config Pcib controller during Crisis
 * Recovery
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitRecoveryPcib (
  IN  VOID     *FchDataPtr
  )
{
}

