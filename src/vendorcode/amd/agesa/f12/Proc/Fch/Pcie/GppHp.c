/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch GPP controller
 *
 * Init GPP features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 49456 $   @e \$Date: 2011-03-24 04:13:38 +0800 (Thu, 24 Mar 2011) $
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
#include "Filecode.h"
#define FILECODE PROC_FCH_PCIE_GPPHP_FILECODE
/**
 * GPP hot plug handler
 *
 *
 * @param[in] FchDataPtr   Fch configuration structure pointer.
 * @param[in] HpPort    The hot plug port number.
 *
 */
VOID
FchGppHotPlugSmiProcess (
  IN  AMD_FCH_CFG     *FchDataPtr,
  IN  UINT32         HpPort
  )
{
  UINT8   FailedPort;

  //
  // First restore GPP pads if needed
  //
  if (FchDataPtr->Gpp->GppDynamicPowerSaving && FchDataPtr->Ab->AlinkPhyPllPowerDown && FchDataPtr->Gpp->GppPhyPllPowerDown) {
    RwAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), ~(UINT32) (1 << (12 + HpPort)), 0, StdHeader);
    RwAlink (RC_INDXC_REG65, ~(UINT32) (0x101 << HpPort), 0, StdHeader);
    FchStall (1000);
  }

  FailedPort = (UINT8) (1 << HpPort);
  if (FchDataPtr->Gpp->GppGen2 && FchDataPtr->Gpp->GppGen2Strap) {
    if (GppPortPollingLtssm (FchDataPtr, FailedPort, TRUE)) {
      FchGppForceGen1 (FchDataPtr, FailedPort);
      FailedPort = GppPortPollingLtssm (FchDataPtr, FailedPort, FALSE);
    }
  } else {
    FchGppForceGen1 (FchDataPtr, FailedPort);
    FailedPort = GppPortPollingLtssm (FchDataPtr, FailedPort, FALSE);
  }
}


/**
 * GPP hot-unplug handler
 *
 *
 * @param[in] FchDataPtr   Fch configuration structure pointer.
 * @param[in] HpPort    The hot plug port number.
 *
 */
VOID
FchGppHotUnplugSmiProcess (
  IN  AMD_FCH_CFG     *FchDataPtr,
  IN  UINT32         HpPort
  )
{
  FchDataPtr->Gpp->PortCfg[HpPort].PortDetected = FALSE;

  if (FchDataPtr->Gpp->GppGen2 && FchDataPtr->Gpp->GppGen2Strap) {
    FchGppForceGen2 (FchDataPtr, (UINT8) (1 << HpPort));
  }

  if (FchDataPtr->Gpp->GppDynamicPowerSaving && FchDataPtr->Ab->AlinkPhyPllPowerDown && FchDataPtr->Gpp->GppPhyPllPowerDown) {
    RwAlink (FCH_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT17), BIT17, StdHeader);
    RwAlink (FCH_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT8), BIT8, StdHeader);
    RwAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), ~(UINT32) (1 << (12 + HpPort)), (1 << (12 + HpPort)), StdHeader);
    RwAlink (FCH_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT17), 0, StdHeader);
  }

  //
  // Finally re-configure GPP pads if needed
  //
  FchGppDynamicPowerSaving (FchDataPtr);
}


/**
 * SMI handler for GPP hot-plug
 *
 *
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 * @param[in] IsPlugged   Is a card currently plugged in the GPP port?
 *
 */
VOID
FchGppHotplugSmiCallback (
  IN  AMD_FCH_CFG     *FchDataPtr,
  IN  BOOLEAN        IsPlugged
  )
{
  UINT32   PortNum;
  UINT32   HpPort;

  if (!FchDataPtr->Gpp->GppFunctionEnable) {
    return;
  }

  HpPort = 0xff;
  for (PortNum = 0; PortNum < MAX_GPP_PORTS; PortNum++) {
    if (FchDataPtr->Gpp->PortCfg[PortNum].PortHotPlug == TRUE) {
      HpPort = PortNum;
      break;
    }
  }

  if (HpPort == 0xff) {
    return;
  }

  if (IsPlugged) {
    OutPort80 (0x9C);
    FchGppHotPlugSmiProcess (FchDataPtr, HpPort);
  } else {
    OutPort80 (0x9D);
    FchGppHotUnplugSmiProcess (FchDataPtr, HpPort);
  }
}
