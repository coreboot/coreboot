/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe wrapper services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbNbInitLibV4.h"
#include  "PcieWrapperServicesV4.h"
#include  "GnbRegistersTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV4_PCIEWRAPPERSERVICESV4_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */



/*----------------------------------------------------------------------------------------*/
/**
 * Relinquish control to DDI for specific lanes
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
VOID
PcieSetDdiOwnPhyV4 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{

  UINT32  LaneBitmap;
  UINT8   Slice;
  if (PcieLibIsDdiWrapper (Wrapper)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieSetDdiOwnPhyV4 Enter\n");
    LaneBitmap = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_DDI_PHY_NATIVE, 0, Wrapper);
    for (Slice = 0; Slice < 4; Slice++) {
      if ((LaneBitmap & (1 << (Slice * 4))) != 0) {
        PcieRegisterRMW (
          Wrapper,
          WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8040_ADDRESS + Slice),
          D0F0xE4_WRAP_8040_OwnSlice_MASK,
          1 << D0F0xE4_WRAP_8040_OwnSlice_OFFSET,
          FALSE,
          Pcie
          );
      }
    }
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieSetDdiOwnPhyV4 Exit\n");
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Execute/clean up reconfiguration
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieTopologyExecuteReconfigV4 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  D0F0xE4_WRAP_8062_STRUCT  D0F0xE4_WRAP_8062;
  PCIe_SILICON_CONFIG       *Silicon;

  if (PcieLibIsPcieWrapper (Wrapper)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologyExecuteReconfigV4 Enter\n");

    PcieTopologyInitSrbmReset (FALSE, Wrapper, Pcie);

    D0F0xE4_WRAP_8062.Value = PcieRegisterRead (
                                Wrapper,
                                WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8062_ADDRESS),
                                Pcie
                                );
    D0F0xE4_WRAP_8062.Field.ReconfigureEn = 0x1;
    PcieRegisterWrite (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8062_ADDRESS),
      D0F0xE4_WRAP_8062.Value,
      FALSE,
      Pcie
      );

    Silicon = PcieConfigGetParentSilicon (Wrapper);

    GnbLibPciIndirectRMW (
      Silicon->Address.AddressValue | D0F0xB8_ADDRESS,
      D0F0xBC_x1F630_ADDRESS,
      AccessWidth32,
      (UINT32) ~D0F0xBC_x1F630_RECONF_WRAPPER_MASK,
      Wrapper->WrapId << D0F0xBC_x1F630_RECONF_WRAPPER_OFFSET,
      GnbLibGetHeader (Pcie)
      );

    GnbSmuServiceRequestV4 (
      Silicon->Address,
      SMC_MSG_RECONFIGURE,
      0,
      GnbLibGetHeader (Pcie)
      );

    D0F0xE4_WRAP_8062.Field.ConfigXferMode = 0x1;
    D0F0xE4_WRAP_8062.Field.ReconfigureEn = 0x0;
    PcieRegisterWrite (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8062_ADDRESS),
      D0F0xE4_WRAP_8062.Value,
      FALSE,
      Pcie
      );
    PcieTopologyInitSrbmReset (TRUE, Wrapper, Pcie);
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologyExecuteReconfigV4 Exit\n");
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set SSID
 *
 *
 * @param[in]  Ssid            SSID
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
VOID
PcieSetSsidV4 (
  IN       UINT32                        Ssid,
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  if (PcieLibIsPcieWrapper (Wrapper)) {
    PcieRegisterWrite (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_0046_ADDRESS),
      Ssid,
      FALSE,
      Pcie
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable lane reversal
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieTopologySetLinkReversalV4 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_ENGINE_CONFIG  *EngineList;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologySetLinkReversal Enter\n");
  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if (PcieLibIsEngineAllocated (EngineList)) {
      if (PcieLibIsPcieEngine (EngineList)) {
        if (EngineList->EngineData.StartLane > EngineList->EngineData.EndLane) {
            PciePortRegisterWriteField (
              EngineList,
              DxF0xE4_xC1_ADDRESS,
              DxF0xE4_xC1_StrapReverseLanes_OFFSET,
              DxF0xE4_xC1_StrapReverseLanes_WIDTH,
              0x1,
              FALSE,
              Pcie
              );
        }
      }
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologySetLinkReversal Exit\n");
}
