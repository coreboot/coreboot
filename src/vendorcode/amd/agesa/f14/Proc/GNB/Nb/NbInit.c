/* $NoKeywords:$ */
/**
 * @file
 *
 * Various NB initialization services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 48955 $   @e \$Date: 2011-03-14 18:31:17 -0600 (Mon, 14 Mar 2011) $
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
 * ***************************************************************************
 *
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  "GfxLib.h"
#include  "NbSmuLib.h"
#include  "NbConfigData.h"
#include  "NbInit.h"
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_NB_NBINIT_FILECODE
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

CONST NB_REGISTER_ENTRY NbPciInitTable [] = {
  {
   D0F0x04_ADDRESS,
    0xffffffff,
   (0x1 << D0F0x04_MemAccessEn_WIDTH) | (0x1 << D0F0x04_BusMasterEn_OFFSET)
  },
  {
   D0F0x4C_ADDRESS,
   ~(UINT32)(0x3 << D0F0x4C_CfgRdTime_OFFSET),
   0x2 << D0F0x4C_CfgRdTime_OFFSET
  },
  {
   D0F0x84_ADDRESS,
   ~(UINT32)(0x1 << D0F0x84_Ev6Mode_OFFSET),
    0x1 << D0F0x84_Ev6Mode_OFFSET
  }
};

CONST NB_REGISTER_ENTRY  NbMiscInitTable [] = {
  {
    D0F0x64_x46_ADDRESS,
    ~(UINT32)(0x3 <<  D0F0x64_x46_P2PMode_OFFSET),
    1 << D0F0x64_x46_Msi64bitEn_OFFSET
  }
};


CONST NB_REGISTER_ENTRY  NbOrbInitTable [] = {
  {
    D0F0x98_x07_ADDRESS,
    0xffffffff,
    (1 << D0F0x98_x07_IocBwOptEn_OFFSET) |
    (1 << D0F0x98_x07_MSIHTIntConversionEn_OFFSET) |
    (1 << D0F0x98_x07_DropZeroMaskWrEn_OFFSET)
  },
  {
    D0F0x98_x08_ADDRESS,
    ~(UINT32)(0xff << D0F0x98_x08_NpWrrLenC_OFFSET),
    1 << D0F0x98_x08_NpWrrLenC_OFFSET
  },
  {
    D0F0x98_x09_ADDRESS,
    ~(UINT32)(0xff << D0F0x98_x09_PWrrLenD_OFFSET),
    1 << D0F0x98_x09_PWrrLenD_OFFSET
  },
  {
    D0F0x98_x0C_ADDRESS,
    0xffffffff,
    1 << D0F0x98_x0C_StrictSelWinnerEn_OFFSET
  },
  {
    D0F0x98_x0E_ADDRESS,
    0xffffffff,
    1 << D0F0x98_x0E_MsiHtRsvIntRemapEn_OFFSET
  },
  {
    D0F0x98_x28_ADDRESS,
    0xffffffff,
    (1 << D0F0x98_x28_SmuPmInterfaceEn_OFFSET) |
    (1 << D0F0x98_x28_ForceCoherentIntr_OFFSET)
  }
};


/*----------------------------------------------------------------------------------------*/
/**
 * Init NB at Power On
 *
 *
 *
 * @param[in] Gnb             Pointer to global Gnb configuration
 * @retval    AGESA_STATUS
 */


AGESA_STATUS
NbInitOnPowerOn (
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  UINTN                   Index;
  FCRxFF30_0398_STRUCT    FCRxFF30_0398;
  UINT32                  Value;

  // Init NBCONFIG
  for (Index = 0; Index < (sizeof (NbPciInitTable) / sizeof (NB_REGISTER_ENTRY)); Index++) {
    GnbLibPciRMW (
      Gnb->GnbPciAddress.AddressValue | NbPciInitTable[Index].Reg,
      AccessWidth32,
      NbPciInitTable[Index].Mask,
      NbPciInitTable[Index].Data,
      Gnb->StdHeader
    );
  }

  // Init MISCIND
  for (Index = 0; Index < (sizeof (NbMiscInitTable) / sizeof (NB_REGISTER_ENTRY)); Index++) {
    GnbLibPciIndirectRMW (
      Gnb->GnbPciAddress.AddressValue | D0F0x60_ADDRESS,
      NbMiscInitTable[Index].Reg | IOC_WRITE_ENABLE,
      AccessWidth32,
      NbMiscInitTable[Index].Mask,
      NbMiscInitTable[Index].Data,
      Gnb->StdHeader
    );
  }

  // Init ORB
  for (Index = 0; Index < (sizeof (NbOrbInitTable) / sizeof (NB_REGISTER_ENTRY)); Index++) {
    GnbLibPciIndirectRMW (
      Gnb->GnbPciAddress.AddressValue | D0F0x94_ADDRESS,
      NbOrbInitTable[Index].Reg | (1 << D0F0x94_OrbIndWrEn_OFFSET),
      AccessWidth32,
      NbOrbInitTable[Index].Mask,
      NbOrbInitTable[Index].Data,
      Gnb->StdHeader
    );
  }
  if (!GfxLibIsControllerPresent (Gnb->StdHeader)) {
    FCRxFF30_0398.Value = (1 << FCRxFF30_0398_SoftResetGrbm_OFFSET) | (1 << FCRxFF30_0398_SoftResetMc_OFFSET) |
                          (1 << FCRxFF30_0398_SoftResetDc_OFFSET) | (1 << FCRxFF30_0398_SoftResetRlc_OFFSET) |
                          (1 << FCRxFF30_0398_SoftResetUvd_OFFSET);
    NbSmuSrbmRegisterWrite (FCRxFF30_0398_ADDRESS, &FCRxFF30_0398.Value, FALSE, Gnb->StdHeader);
  }

  Value = 0;
  for (Index = 0x8400; Index <= 0x85AC; Index = Index + 4) {
    NbSmuRcuRegisterWrite (
      (UINT16) Index,
      &Value,
      1,
      FALSE,
      Gnb->StdHeader
      );
  }

  NbSmuRcuRegisterWrite (
    0x9000,
    &Value,
    1,
    FALSE,
    Gnb->StdHeader
    );

  NbSmuRcuRegisterWrite (
    0x9004,
    &Value,
    1,
    FALSE,
    Gnb->StdHeader
    );

  return  AGESA_SUCCESS;
}

