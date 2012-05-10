/* $NoKeywords:$ */
/**
 * @file
 *
 * Fch Gpp Library
 *
 * Gpp Library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 44909 $   @e \$Date: 2011-01-10 18:45:45 +0800 (Mon, 10 Jan 2011) $
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
#include "FchDef.h"
#define FILECODE PROC_FCH_PCIE_GPPLIB_FILECODE

/**
 * FchGppForceGen2 - Set GPP to Gen2
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 * @param[in] ActivePorts Activate Ports.
 *
 */
VOID
FchGppForceGen2 (
  IN  VOID            *FchDataPtr,
  IN  CONST UINT8     ActivePorts
  )
{
  UINT32       PortId;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    if (ActivePorts & (1 << PortId)) {
      RwAlink (FCH_RCINDXP_REGA4 | PortId << 24, 0xFFFFFFFF, BIT29 + BIT0, StdHeader);
      RwAlink ((FCH_ABCFG_REG340 + PortId * 4) | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT21, StdHeader);
      RwAlink (FCH_RCINDXP_REGA2 | PortId << 24, ~BIT13, 0, StdHeader);
      RwAlink (FCH_RCINDXP_REGC0 | PortId << 24, ~BIT15, 0, StdHeader);
      RwPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x88), AccessWidth8, 0xf0, 0x02, StdHeader);

      (&LocalCfgPtr->Gpp.PortCfg[PortId])->PortIsGen2 = 2;
    }
  }
}

/**
 * FchGppForceGen1 - Set GPP to Gen1
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 * @param[in] ActivePorts Activate Ports.
 *
 */
VOID
FchGppForceGen1 (
  IN  VOID           *FchDataPtr,
  IN  CONST UINT8     ActivePorts
  )
{
  UINT32       PortId;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    if (ActivePorts & (1 << PortId) && LocalCfgPtr->Gpp.GppHardwareDownGrade != PortId + 1) {
      RwAlink ((FCH_ABCFG_REG340 + PortId * 4) | (UINT32) (ABCFG << 29), ~BIT21, 0, StdHeader);
      RwAlink (FCH_RCINDXP_REGA4 | PortId << 24, ~BIT0, BIT29, StdHeader);
      RwAlink (FCH_RCINDXP_REGA2 | PortId << 24, 0xFFFFFFFF, BIT13, StdHeader);
      RwAlink (FCH_RCINDXP_REGC0 | PortId << 24, ~BIT15, 0, StdHeader);
      RwPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x88), AccessWidth8, 0xf0, 0x01, StdHeader);

      (&LocalCfgPtr->Gpp.PortCfg[PortId])->PortIsGen2 = 1;
    }
  }
}

/**
 * GppPortPollingLtssm - Loop polling the LTSSM for each GPP port marked in PortMap
 *
 *
 * @param[in]    FchDataPtr      Fch configuration structure pointer.
 * @param[in]    ActivePorts     A bitmap of ports which should be polled
 * @param[in]    IsGen2          TRUE if the polling is in Gen2 mode
 *
 * @retval       FailedPorts     A bitmap of ports which failed to train
 *
 */
UINT8
GppPortPollingLtssm (
  IN  VOID        *FchDataPtr,
  IN  UINT8       ActivePorts,
  IN  BOOLEAN     IsGen2
  )
{
  UINT32              RetryCounter;
  UINT8               PortId;
  UINT8               FailedPorts;
  FCH_GPP_PORT_CONFIG  *PortCfg;
  UINT32              AbIndex;
  UINT32              GppData32;
  UINT8               EmptyPorts;
  UINT8               Index;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  FailedPorts = 0;
  RetryCounter = MAX_LT_POLLINGS;
  EmptyPorts = ActivePorts;

  while (RetryCounter-- && ActivePorts) {
    for (PortId = 0; PortId < MAX_GPP_PORTS; PortId++) {
      if (ActivePorts & (1 << PortId)) {
        PortCfg = &LocalCfgPtr->Gpp.PortCfg[PortId];
        AbIndex = FCH_RCINDXP_REGA5 | (UINT32) (RCINDXP << 29) | (PortId << 24);
        GppData32 = ReadAlink (AbIndex, StdHeader) & 0x3F3F3F3F;

        if ((UINT8) (GppData32) > 0x04) {
          EmptyPorts &= ~(1 << PortId);
        }

        if ((UINT8) (GppData32) == 0x10) {
          ActivePorts &= ~(1 << PortId);
          PortCfg->PortDetected = TRUE;
          break;
        }

        if (IsGen2) {
          for (Index = 0; Index < 4; Index++) {
            if ((UINT8) (GppData32) == 0x29 || (UINT8) (GppData32) == 0x2A ) {
              ActivePorts &= ~(1 << PortId);
              FailedPorts |= (1 << PortId);
              break;
            }
            GppData32 >>= 8;
          }
        }
      }
    }

    if (EmptyPorts && RetryCounter < (MAX_LT_POLLINGS - 200)) {
      ActivePorts &= ~EmptyPorts;
    }

    FchStall (1000, StdHeader);
  }

  FailedPorts |= ActivePorts;
  return FailedPorts;
}

