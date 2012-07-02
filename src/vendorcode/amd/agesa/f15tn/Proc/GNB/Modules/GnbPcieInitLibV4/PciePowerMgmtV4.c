/* $NoKeywords:$ */
/**
 * @file
 *
 * Power saving features/services
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
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV4.h"
#include  "GnbRegistersTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV4_PCIEPOWERMGMTV4_FILECODE

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
 * Clock gating
 *
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */

VOID
PciePwrClockGatingV4 (
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  D0F0xE4_WRAP_8011_STRUCT  D0F0xE4_WRAP_8011;
  D0F0xE4_WRAP_8012_STRUCT  D0F0xE4_WRAP_8012;
  D0F0xE4_WRAP_8014_STRUCT  D0F0xE4_WRAP_8014;
  D0F0xE4_WRAP_8015_STRUCT  D0F0xE4_WRAP_8015;
  D0F0xE4_WRAP_8016_STRUCT  D0F0xE4_WRAP_8016;
  UINT8                     CoreId;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGatingV4 Enter\n");
  D0F0xE4_WRAP_8014.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8014_ADDRESS),
                              Pcie
                              );
  D0F0xE4_WRAP_8015.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8015_ADDRESS),
                              Pcie
                              );

  D0F0xE4_WRAP_8012.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8012_ADDRESS),
                              Pcie
                              );

  D0F0xE4_WRAP_8011.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8011_ADDRESS),
                              Pcie
                              );

  if (Wrapper->Features.ClkGating == 0x1) {
    D0F0xE4_WRAP_8014.Field.TxclkPermGateEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.TxclkPrbsGateEnable = 0x1;

    D0F0xE4_WRAP_8014.Field.PcieGatePifA1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifB1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifC1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifD1xEnable = 0x1;

    D0F0xE4_WRAP_8014.Field.PcieGatePifA2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifB2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifC2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifD2p5xEnable = 0x1;


    D0F0xE4_WRAP_8011.Field.TxclkDynGateEnable = 0x1;
    D0F0xE4_WRAP_8011.Field.TxclkRegsGateEnable = 0x1;
    D0F0xE4_WRAP_8011.Field.TxclkLcntGateEnable = 0x1;
    D0F0xE4_WRAP_8011.Field.RcvrDetClkEnable = 0x1;
    D0F0xE4_WRAP_8011.Field.TxclkPermGateEven = 0x1;
    D0F0xE4_WRAP_8011.Field.TxclkDynGateLatency = 0x3f;
    D0F0xE4_WRAP_8011.Field.TxclkRegsGateLatency = 0x3f;
    D0F0xE4_WRAP_8011.Field.TxclkPermGateLatency = 0x3f;

    D0F0xE4_WRAP_8012.Field.Pif2p5xIdleResumeLatency = 0x7;
    D0F0xE4_WRAP_8012.Field.Pif2p5xIdleGateEnable = 0x1;
    D0F0xE4_WRAP_8012.Field.Pif2p5xIdleGateLatency = 0x1;
    D0F0xE4_WRAP_8012.Field.Pif1xIdleResumeLatency = 0x7;
    D0F0xE4_WRAP_8012.Field.Pif1xIdleGateEnable = 0x1;
    D0F0xE4_WRAP_8012.Field.Pif1xIdleGateLatency = 0x1;

    D0F0xE4_WRAP_8015.Field.RefclkBphyGateEnable = 0x1;
    D0F0xE4_WRAP_8015.Field.RefclkBphyGateLatency = 0x0;
    D0F0xE4_WRAP_8015.Field.RefclkRegsGateEnable = 0x1;
    D0F0xE4_WRAP_8015.Field.RefclkRegsGateLatency = 0x3f;
    D0F0xE4_WRAP_8015.Field.line477  = 0x0;
    D0F0xE4_WRAP_8015.Field.line478  = 0x0;
    D0F0xE4_WRAP_8015.Field.line479  = 0x3;
    D0F0xE4_WRAP_8015.Field.line480  = 0x1;
    D0F0xE4_WRAP_8015.Field.line482  = 0x0;
    D0F0xE4_WRAP_8015.Field.line483  = 0x0;
    D0F0xE4_WRAP_8015.Field.line484  = 0x0;
    D0F0xE4_WRAP_8015.Field.line485  = 0x1;
    D0F0xE4_WRAP_8015.Field.line486  = 0x1;

    D0F0xE4_WRAP_8014.Field.DdiGateDigAEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGateDigBEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGateDigCEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGateDigDEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifA1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifB1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifC1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifD1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifA2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifB2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifC2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifD2p5xEnable = 0x1;
  }
  if (Wrapper->Features.TxclkGatingPllPowerDown == 0x1) {
    D0F0xE4_WRAP_8014.Field.TxclkPermGateOnlyWhenPllPwrDn = 0x1;
  }
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8014_ADDRESS),
    D0F0xE4_WRAP_8014.Value,
    TRUE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8015_ADDRESS),
    D0F0xE4_WRAP_8015.Value,
    TRUE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8012_ADDRESS),
    D0F0xE4_WRAP_8012.Value,
    TRUE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8011_ADDRESS),
    D0F0xE4_WRAP_8011.Value,
    TRUE,
    Pcie
    );
  for (CoreId = Wrapper->StartPcieCoreId; CoreId <= Wrapper->EndPcieCoreId; CoreId++) {
    PcieRegisterWriteField (
      Wrapper,
      CORE_SPACE (CoreId, D0F0xE4_CORE_0011_ADDRESS),
      D0F0xE4_CORE_0011_DynClkLatency_OFFSET,
      D0F0xE4_CORE_0011_DynClkLatency_WIDTH,
      0xf,
      TRUE,
      Pcie
      );
  }
  if (Wrapper->Features.LclkGating == 0x1) {
    D0F0xE4_WRAP_8016.Value = PcieRegisterRead (
                                Wrapper,
                                WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8016_ADDRESS),
                                Pcie
                                );
    D0F0xE4_WRAP_8016.Field.LclkDynGateEnable = 0x1;
    D0F0xE4_WRAP_8016.Field.LclkGateFree = 0x1;
    PcieRegisterWrite (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8016_ADDRESS),
      D0F0xE4_WRAP_8016.Value,
      TRUE,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGatingV4 Exit\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * Power down DDI plls
 *
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */

VOID
PciePwrPowerDownDdiPllsV4 (
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrPowerDownDdiPllsV4 Enter\n");
  if (PcieConfigIsDdiWrapper (Wrapper) && !PcieConfigIsPcieWrapper (Wrapper)) {
    PcieRegisterRMW (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8020_ADDRESS),
      D0F0xE4_WRAP_8020_PrbsPcieLbSelect_MASK,
      0x1 << D0F0xE4_WRAP_8020_PrbsPcieLbSelect_OFFSET,
      FALSE,
      Pcie
      );
    PciePollPifForCompeletion (Wrapper, Pcie);
    PcieRegisterRMW (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8025_ADDRESS),
      D0F0xE4_WRAP_8025_LMTxPhyCmd0_MASK | D0F0xE4_WRAP_8025_LMTxPhyCmd1_MASK,
      (0x1 << D0F0xE4_WRAP_8025_LMTxPhyCmd0_OFFSET) | (0x1 << D0F0xE4_WRAP_8025_LMTxPhyCmd1_OFFSET),
      FALSE,
      Pcie
      );
    PciePollPifForCompeletion (Wrapper, Pcie);
    PcieRegisterRMW (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8025_ADDRESS),
      D0F0xE4_WRAP_8025_LMTxPhyCmd0_MASK | D0F0xE4_WRAP_8025_LMTxPhyCmd1_MASK,
      (0x7 << D0F0xE4_WRAP_8025_LMTxPhyCmd0_OFFSET) | (0x7 << D0F0xE4_WRAP_8025_LMTxPhyCmd1_OFFSET),
      FALSE,
      Pcie
      );
    PcieRegisterRMW (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8020_ADDRESS),
      D0F0xE4_WRAP_8020_PrbsPcieLbSelect_MASK,
      0x0 << D0F0xE4_WRAP_8020_PrbsPcieLbSelect_OFFSET,
      FALSE,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrPowerDownDdiPllsV4 Exit\n");
}
