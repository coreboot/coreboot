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
#define FILECODE PROC_FCH_PCIB_PCIBRESET_FILECODE
/**
 * FchInitResetPcibPciTable - Pcib device registers initial
 * during the power on stage.
 *
 *
 *
 *
 */
CONST REG8_MASK FchInitResetPcibPciTable[] =
{
  //
  // P2P Bridge (Bus 0, Dev 20, Func 4)
  //
  {0x00, PCIB_BUS_DEV_FUN, 0},
  {FCH_PCIB_REG4B, 0xFF, BIT6 + BIT7 + BIT4},
  {FCH_PCIB_REG40, 0xDF, 0x20},
  {0x50 , 0x02, 0x01},
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
    ARRAY_SIZE(FchInitResetPcibPciTable),
    StdHeader
    );
  if ( UserOptions.FchBldCfg->CfgFchPort80BehindPcib ) {
    FchInitResetPcibPort80Enable (FchDataPtr);
  }
}

/**
 * FchInitResetPcibPort80Enable - Pcib device registers initial
 * during the power on stage.
 *
 *
 *
 *
 */
CONST REG8_MASK FchInitResetPcibPort80EnableTable[] =
{
  //
  // P2P Bridge (Bus 0, Dev 20, Func 4)
  //
  {0x00, PCIB_BUS_DEV_FUN, 0},
  {0x1C , 0x00, 0xF0},
  {0x1D , 0x00, 0x00},
  {0x04 , 0x00, 0x21},
  {0xFF, 0xFF, 0xFF},
};

/**
 * FchInitResetPcibPort80Enable - Enable Port80 Behind PCIB
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetPcibPort80Enable (
  IN  VOID     *FchDataPtr
  )
{
  AMD_CONFIG_PARAMS         *StdHeader;

  StdHeader = &((AMD_RESET_PARAMS *)FchDataPtr)->StdHeader;

  ProgramPciByteTable (
    (REG8_MASK*) (&FchInitResetPcibPort80EnableTable[0]),
    ARRAY_SIZE(FchInitResetPcibPort80EnableTable),
    StdHeader
    );
}
