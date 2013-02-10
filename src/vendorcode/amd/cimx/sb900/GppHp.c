/**
 * @file
 *
 * Config Southbridge GPP controller
 *
 * Init GPP features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project
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
****************************************************************************
*/
#include "SbPlatform.h"
#include "cbtypes.h"
#include "AmdSbLib.h"

//
// Declaration of external functions
//
VOID  sbGppDynamicPowerSaving (IN AMDSBCFG* pConfig );
VOID  sbGppForceGen1 (IN AMDSBCFG* pConfig, IN CONST UINT8 ActivePorts);
VOID  sbGppForceGen2 (IN AMDSBCFG* pConfig, IN CONST UINT8 ActivePorts);
UINT8 GppPortPollingLtssm (IN AMDSBCFG* pConfig, IN UINT8 ActivePorts, IN BOOLEAN IsGen2);


/**
 * GPP hot plug handler
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 * @param[in] HpPort    The hot plug port number.
 *
 */
static VOID
sbGppHotPlugSmiProcess (
  IN       AMDSBCFG*     pConfig,
  IN       UINT32        HpPort
  )
{
  UINT8         FailedPort;

  // First restore GPP pads if needed
  if (pConfig->GppDynamicPowerSaving && pConfig->AlinkPhyPllPowerDown && pConfig->GppPhyPllPowerDown) {
    rwAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29), ~(UINT32) (1 << (12 + HpPort)), 0);
    rwAlink (RC_INDXC_REG65, ~(UINT32) (0x101 << HpPort), 0);
    SbStall (1000);
  }

  FailedPort = (UINT8) (1 << HpPort);
  if (pConfig->GppGen2 && pConfig->GppGen2Strap) {
    if (GppPortPollingLtssm (pConfig, FailedPort, TRUE)) {
      sbGppForceGen1 (pConfig, FailedPort);
      FailedPort = GppPortPollingLtssm (pConfig, FailedPort, FALSE);
    }
  } else {
    sbGppForceGen1 (pConfig, FailedPort);
    FailedPort = GppPortPollingLtssm (pConfig, FailedPort, FALSE);
  }
}


/**
 * GPP hot-unplug handler
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 * @param[in] HpPort    The hot plug port number.
 *
 */
static VOID
sbGppHotUnplugSmiProcess (
  IN       AMDSBCFG*     pConfig,
  IN       UINT32        HpPort
  )
{
  pConfig->PORTCONFIG[HpPort].PortCfg.PortDetected = FALSE;

  if (pConfig->GppGen2 && pConfig->GppGen2Strap) {
    sbGppForceGen2 (pConfig, (UINT8) (1 << HpPort));
  }

  // 5.19.1 GPP Power Saving with Hot Unplug
  if (pConfig->GppDynamicPowerSaving && pConfig->AlinkPhyPllPowerDown && pConfig->GppPhyPllPowerDown) {
    rwAlink (SB_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT17), BIT17);
    rwAlink (SB_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT8), BIT8);
    rwAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29), ~(UINT32) (1 << (12 + HpPort)), (1 << (12 + HpPort)));
    rwAlink (SB_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT17), 0);

    // Finally re-configure GPP pads if needed
    sbGppDynamicPowerSaving (pConfig);
  }
}


/**
 * SMI handler for GPP hot-plug
 *
 *
 * @param[in] pConfig     Southbridge configuration structure pointer.
 * @param[in] IsPlugged   Is a card currently plugged in the GPP port?
 *
 */
VOID
sbGppHotplugSmiCallback (
  IN       AMDSBCFG* pConfig,
  IN       BOOLEAN   IsPlugged
  )
{
  UINT32           portNum;
  UINT32           HpPort;

  if (!pConfig->GppFunctionEnable) {
    return;
  }

  HpPort = 0xff;
  for (portNum = 0; portNum < MAX_GPP_PORTS; portNum++) {
    if (pConfig->PORTCONFIG[portNum].PortCfg.PortHotPlug == TRUE) {
      HpPort = portNum;
      break;
    }
  }
  if (HpPort == 0xff) {
    return;
  }

  if (IsPlugged) {
    outPort80 (0x9C);
    sbGppHotPlugSmiProcess (pConfig, HpPort);
  } else {
    outPort80 (0x9D);
    sbGppHotUnplugSmiProcess (pConfig, HpPort);
  }
}

