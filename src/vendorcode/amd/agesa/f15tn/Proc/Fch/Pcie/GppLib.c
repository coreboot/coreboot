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
#include "FchDef.h"
#define FILECODE PROC_FCH_PCIE_GPPLIB_FILECODE

/**
 * FchGppForceGen2 - Set GPP to Gen2
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] ActivePorts Activate Ports
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
VOID
FchGppForceGen2 (
  IN       FCH_GPP             *FchGpp,
  IN       CONST UINT8         ActivePorts,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32            PortId;

  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    if (ActivePorts & (1 << PortId)) {
      RwAlink (FCH_RCINDXP_REGA4 | PortId << 24, 0xFFFFFFFF, BIT29 + BIT0, StdHeader);
      RwAlink ((FCH_ABCFG_REG340 + PortId * 4) | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT21, StdHeader);
      RwAlink (FCH_RCINDXP_REGA2 | PortId << 24, (UINT32)~BIT13, 0, StdHeader);
      RwAlink (FCH_RCINDXP_REGC0 | PortId << 24, (UINT32)~BIT15, 0, StdHeader);
      RwPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x88), AccessWidth8, 0xf0, 0x02, StdHeader);

      (&FchGpp->PortCfg[PortId])->PortIsGen2 = TRUE;
    }
  }
}

/**
 * FchGppForceGen1 - Set GPP to Gen1
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] ActivePorts Activate Ports
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
VOID
FchGppForceGen1 (
  IN       FCH_GPP             *FchGpp,
  IN       CONST UINT8         ActivePorts,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32            PortId;

  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    if (ActivePorts & (1 << PortId) && FchGpp->GppHardwareDownGrade != PortId + 1) {
      RwAlink ((FCH_ABCFG_REG340 + PortId * 4) | (UINT32) (ABCFG << 29), (UINT32)~BIT21, 0, StdHeader);
      RwAlink (FCH_RCINDXP_REGA4 | PortId << 24, (UINT32)~BIT0, BIT29, StdHeader);
      RwAlink (FCH_RCINDXP_REGA2 | PortId << 24, 0xFFFFFFFF, BIT13, StdHeader);
      RwAlink (FCH_RCINDXP_REGC0 | PortId << 24, (UINT32)~BIT15, 0, StdHeader);
      RwPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x88), AccessWidth8, 0xf0, 0x01, StdHeader);

      (&FchGpp->PortCfg[PortId])->PortIsGen2 = FALSE;
    }
  }
}

/**
 * GppPortPollingLtssm - Loop polling the LTSSM for each GPP port marked in PortMap
 *
 *
 * @param[in] FchGpp        Pointer to Fch GPP configuration structure
 * @param[in] ActivePorts   A bitmap of ports which should be polled
 * @param[in] IsGen2        TRUE if the polling is in Gen2 mode
 * @param[in] StdHeader     Pointer to AMD_CONFIG_PARAMS
 *
 * @retval       FailedPorts     A bitmap of ports which failed to train
 *
 */
UINT8
GppPortPollingLtssm (
  IN       FCH_GPP             *FchGpp,
  IN       UINT8               ActivePorts,
  IN       BOOLEAN             IsGen2,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32                 RetryCounter;
  UINT8                  PortId;
  UINT8                  FailedPorts;
  UINT8                  HotPlugPorts;
  FCH_GPP_PORT_CONFIG    *PortCfg;
  UINT32                 AbIndex;
  UINT32                 GppData32;
  UINT8                  EmptyPorts;
  UINT8                  Index;
  UINT8                  FixedPolling;

  FailedPorts = 0;
  HotPlugPorts = 0;
  RetryCounter = MAX_LT_POLLINGS;
  EmptyPorts = ActivePorts;
  FixedPolling = 200;
  if ( FchGpp->NewGppAlgorithm == TRUE ) {
    FixedPolling = FchGpp->GppPortMinPollingTime;
  }
  while (RetryCounter-- && ActivePorts) {
    for (PortId = 0; PortId < MAX_GPP_PORTS; PortId++) {
      if (ActivePorts & (1 << PortId)) {
        PortCfg = &FchGpp->PortCfg[PortId];
        if ( PortCfg->PortHotPlug == TRUE ) {
          HotPlugPorts |= ( 1 << PortId);
        }
        AbIndex = FCH_RCINDXP_REGA5 | (UINT32) (PortId << 24);
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
    if (EmptyPorts && RetryCounter < (MAX_LT_POLLINGS - (UINT32) FixedPolling)) {
      ActivePorts &= ~EmptyPorts;
    }
    FchStall (1000, StdHeader);
  }
  FchGpp->HotPlugPortsStatus = HotPlugPorts;

  FailedPorts |= ActivePorts;
  return FailedPorts;
}


/**
 * FchFindPciCap - Find PCI Cap
 *
 *
 * @param[in] PciAddress     PCI Address.
 * @param[in] TargetCapId    Target Cap ID.
 * @param[in] StdHeader
 *
 */
UINT8
FchFindPciCap (
  IN  UINT32                PciAddress,
  IN  UINT8                 TargetCapId,
  IN  AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8   NextCapPtr;
  UINT8   CapId;

  NextCapPtr = 0x34;
  while (NextCapPtr != 0) {
    ReadPci (PciAddress + NextCapPtr, AccessWidth8, &NextCapPtr, StdHeader);

    if (NextCapPtr == 0xff) {
      return 0;
    }

    if (NextCapPtr != 0) {
      ReadPci (PciAddress + NextCapPtr, AccessWidth8, &CapId, StdHeader);
      if (CapId == TargetCapId) {
        break;
      } else {
        NextCapPtr++;
      }
    }
  }
  return NextCapPtr;
}

STATIC
BOOLEAN
IsDeviceGen2Capable (
  IN       UINT32              pciAddress,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8      pcieCapOffset;
  UINT8      value8;
  UINT16     value16;

  pcieCapOffset = FchFindPciCap (pciAddress, PCIE_CAP_ID, StdHeader);
  if (pcieCapOffset) {
    ReadPci (pciAddress + pcieCapOffset + 0x0C, AccessWidth8, &value8, StdHeader);
    if (value8 & BIT1) {
      return TRUE;
    } else {
      ReadPci (pciAddress, AccessWidth16, &value16, StdHeader);
      if ((value16 == AMD_FCH_VID) || (value16 == ATI_VID)) {
        return TRUE;
      }
    }
  }
  return FALSE;
}


/**
 *
 * 5/10/2011 - BIOS workaround for PLLPD hangup issue (applied for both POST and hotplug phases):
 *
 * if (GppPhyPllPowerDown == TRUE) {
 *   if (GppGen2 == TRUE && GppGen2Strap == TRUE) {
 *     if ((Any EP is GEN2 capable) || (Any EP is AMD/ATI GFX card)) {
 *       INDXC_REG40[3] = 0;
 *     } else {
 *       INDXC_REG40[3] = 1;
 *     }
 *   } else {
 *     INDXC_REG40[3] = 1;
 *   }
 * }
 *
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
VOID
GppGen2Workaround (
  IN       FCH_GPP             *FchGpp,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8            portId;
  UINT8            busNum;
  UINT32           reg32Value;
  BOOLEAN          DisablePllPdInL1;

  if (FchGpp->GppPhyPllPowerDown == TRUE) {
    DisablePllPdInL1 = FALSE;
    if (FchGpp->GppGen2 && FchGpp->GppGen2Strap) {
      // Search all EP for max link speed capability
      for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
        ReadPci (PCI_ADDRESS (0, FCH_GPP_DEV, portId, 0x19), AccessWidth8, &busNum, StdHeader);
        if (busNum != 0xFF) {
          ReadPci (PCI_ADDRESS (busNum, 0, 0, 0x00), AccessWidth32, &reg32Value, StdHeader);
          if (reg32Value != 0xffffffff) {
            DisablePllPdInL1 = IsDeviceGen2Capable (PCI_ADDRESS (busNum, 0, 0, 0), StdHeader);
            if (DisablePllPdInL1 == TRUE) {
              break;
            }
          }
        }
      }
    }

    if (DisablePllPdInL1 == TRUE) {
      RwAlink (FCH_RCINDXC_REG40, (UINT32)~BIT3, 0, StdHeader);
    } else {
      RwAlink (FCH_RCINDXC_REG40, (UINT32)~BIT3, BIT3, StdHeader);
    }
  }
}

UINT32
GppGetFchTempBus (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32                FchTempBus;
  UINT8                 TempValue;
  UINT64                MmioMsr;

  LibAmdMsrRead (0xC0010058, &MmioMsr, StdHeader);
  TempValue = (UINT8) ((MmioMsr & 0x03C) >> 2);
  FchTempBus = ( 0x01 << TempValue);
  FchTempBus--;
  FchTempBus--;
  return ( FchTempBus);
}
