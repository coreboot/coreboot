/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Gpp controller
 *
 * Init Gpp features (PEI phase).
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
#define FILECODE PROC_FCH_PCIE_GPPRESET_FILECODE


//
//-----------------------------------------------------------------------------------
// Early GPP initialization sequence:
//
// 1) Set port enable bit fields by current GPP link configuration mode
// 2) Deassert GPP reset and pull EP out of reset - Clear GPP_RESET (abcfg:0xC0[8] = 0)
// 3) Loop polling for the link status of all ports
// 4) Misc operations after link training:
//      - (optional) Detect GFX device
//      - Hide empty GPP configuration spaces (Disable empty GPP ports)
//      - (optional) Power down unused GPP ports
//      - (optional) Configure PCIE_P2P_Int_Map (abcfg:0xC4[7:0])
// 5) GPP init completed
//
//
// *) Gen2 vs Gen1
//                                   Gen2 mode     Gen1 mode
//  ---------------------------------------------------------------
//    STRAP_PHY_PLL_CLKF[6:0]          7'h32         7'h19
//    STRAP_BIF_GEN2_EN                  1             0
//
//    PCIE_PHY_PLL clock locks @       5GHz
//
//

/**
 * FchInitResetGpp - Config Gpp during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetGpp (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  if ( LocalCfgPtr->Gpp.NewGppAlgorithm == TRUE ) {
    if (ReadFchSleepType (StdHeader) == ACPI_SLPTYP_S3) {
      FchGppPortInitS3Phase (&LocalCfgPtr->Gpp, StdHeader);
    }
  }
}


