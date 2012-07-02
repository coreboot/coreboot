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
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
