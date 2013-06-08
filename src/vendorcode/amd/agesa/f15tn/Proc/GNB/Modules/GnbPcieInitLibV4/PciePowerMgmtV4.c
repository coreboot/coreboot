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
