/* $NoKeywords:$ */
/**
 * @file
 *
 * NB Power gate Gfx/Uvd/Gmc
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 41777 $   @e \$Date: 2010-11-10 22:29:39 +0800 (Wed, 10 Nov 2010) $
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
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbFuseTable.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  "GnbRegistersON.h"
#include  "GfxLib.h"
#include  "NbSmuLib.h"
#include  "NbConfigData.h"
#include  "NbFamilyServices.h"
#include  "F14NbPowerGate.h"
#include  "GfxLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_NB_FAMILY_0x14_F14NBPOWERGATE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define POWER_GATE_GMC_PSO_CONTROL_VALID_NUM  1
#define POWER_GATE_GMC_MOTH_PSO_PWRUP         153
#define POWER_GATE_GMC_MOTH_PSO_PWRDN         50
#define POWER_GATE_GMC_DAUG_PSO_PWRUP         50
#define POWER_GATE_GMC_DAUG_PSO_PWRDN         0
#define POWER_GATE_GMC_RESET_TIMER            10
#define POWER_GATE_GMC_ISO_TIMER              10
#define POWER_GATE_GMC_SAVE_RESTORE_WIDTH     2
#define POWER_GATE_GMC_RSO_RESTORE_TIMER      10
#define POWER_GATE_GMCPSO_CONTROL_PERIOD_7to4 7
#define POWER_GATE_GMCPSO_CONTROL_PERIOD_3to0 7


#define POWER_GATE_UVD_MOTH_PSO_PWRUP         113
#define POWER_GATE_UVD_MOTH_PSO_PWRDN         50
#define POWER_GATE_UVD_DAUG_PSO_PWRUP         50
#define POWER_GATE_UVD_DAUG_PSO_PWRDN         50
#define POWER_GATE_UVD_RESET_TIMER            50
#define POWER_GATE_UVD_ISO_TIMER              50
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

POWER_GATE_DATA F14NbGmcPowerGatingData = {
  POWER_GATE_GMC_MOTH_PSO_PWRUP,
  POWER_GATE_GMC_MOTH_PSO_PWRDN,
  POWER_GATE_GMC_DAUG_PSO_PWRUP,
  POWER_GATE_GMC_DAUG_PSO_PWRDN,
  POWER_GATE_GMC_RESET_TIMER,
  POWER_GATE_GMC_ISO_TIMER
};

/// GMC power gating
UINT32  F14GmcPowerGatingTable_1[] = {
//  SMUx0B_x8408_ADDRESS
  0,
//  SMUx0B_x840C_ADDRESS
  0,
//  SMUx0B_x8410_ADDRESS
  (0x1 << SMUx0B_x8410_PwrGatingEn_OFFSET) |
  (0x0 << SMUx0B_x8410_Reserved_2_1_OFFSET) |
  (POWER_GATE_GMC_PSO_CONTROL_VALID_NUM << SMUx0B_x8410_PsoControlValidNum_OFFSET) |
  (((POWER_GATE_GMCPSO_CONTROL_PERIOD_7to4 << 4) | POWER_GATE_GMCPSO_CONTROL_PERIOD_3to0) << SMUx0B_x8410_SavePsoDelay_OFFSET) |
  (0x0 << SMUx0B_x8410_PwrGaterSel_OFFSET)
};

/*----------------------------------------------------------------------------------------*/
/**
 * GMC Power Gating
 *
 *
 * @param[in] StdHeader      Standard Configuration Header
 * @param[in] PowerGateData  Pointer power gate data
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
STATIC
F14NbSmuGmcPowerGatingInit (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      POWER_GATE_DATA     *PowerGateData
  )
{
  SMUx0B_x8504_STRUCT SMUx0B_x8504;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuGmcPowerGatingInit Enter\n");
  NbSmuRcuRegisterWrite (
    SMUx0B_x8408_ADDRESS,
    &F14GmcPowerGatingTable_1[0],
    sizeof (POWER_GATE_DATA) / sizeof (UINT32),
    TRUE,
    StdHeader
    );

  NbSmuRcuRegisterWrite (
    SMUx0B_x84A0_ADDRESS,
    (UINT32 *) PowerGateData,
    sizeof (POWER_GATE_DATA) / sizeof (UINT32),
    TRUE,
    StdHeader
    );

  SMUx0B_x8504.Value = 0;
  SMUx0B_x8504.Field.SaveRestoreWidth = POWER_GATE_GMC_SAVE_RESTORE_WIDTH;
  SMUx0B_x8504.Field.PsoRestoreTimer = POWER_GATE_GMC_RSO_RESTORE_TIMER;
  NbSmuRcuRegisterWrite (
    SMUx0B_x8504_ADDRESS,
    &SMUx0B_x8504.Value,
    1,
    TRUE,
    StdHeader
    );

  NbSmuServiceRequest (0x01, TRUE, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuGmcPowerGatingInit Exit\n");
  return AGESA_SUCCESS;
}


POWER_GATE_DATA F14NbUvdPowerGatingData = {
  POWER_GATE_UVD_MOTH_PSO_PWRUP,
  POWER_GATE_UVD_MOTH_PSO_PWRDN,
  POWER_GATE_UVD_DAUG_PSO_PWRUP,
  POWER_GATE_UVD_DAUG_PSO_PWRDN,
  POWER_GATE_UVD_RESET_TIMER,
  POWER_GATE_UVD_ISO_TIMER
};

/// UVD power gating
UINT32  F14UvdPowerGatingTable_1[] = {
//  SMUx0B_x8408_ADDRESS
  0,
//  SMUx0B_x840C_ADDRESS
  0,
//  SMUx0B_x8410_ADDRESS
  (0x0 << SMUx0B_x8410_PwrGatingEn_OFFSET) |
  (0x0 << SMUx0B_x8410_Reserved_2_1_OFFSET) |
  (0x1 << SMUx0B_x8410_PsoControlValidNum_OFFSET) |
  (0x77 << SMUx0B_x8410_SavePsoDelay_OFFSET) |
  (0x2 << SMUx0B_x8410_PwrGaterSel_OFFSET)
};


/*----------------------------------------------------------------------------------------*/
/**
 * UVD Power Gating
 *
 *
 *
 * @param[in] StdHeader      Standard Configuration Header
 * @param[in] PowerGateData  Pointer power gate data
 *
 */


VOID
STATIC
F14NbSmuUvdPowerGatingInit (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      POWER_GATE_DATA     *PowerGateData
  )
{
  SMUx0B_x8504_STRUCT SMUx0B_x8504;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuUvdPowerGatingInit Enter\n");
  NbSmuRcuRegisterWrite (
    SMUx0B_x8408_ADDRESS,
    &F14UvdPowerGatingTable_1[0],
    sizeof (F14UvdPowerGatingTable_1) / sizeof (UINT32),
    TRUE,
    StdHeader
    );

  NbSmuRcuRegisterWrite (
    SMUx0B_x84A0_ADDRESS,
    (UINT32 *) PowerGateData,
    sizeof (POWER_GATE_DATA) / sizeof (UINT32),
    TRUE,
    StdHeader
    );

  SMUx0B_x8504.Value = 0;
  SMUx0B_x8504.Field.SaveRestoreWidth = 0x02;
  SMUx0B_x8504.Field.PsoRestoreTimer = 0x0A;
  NbSmuRcuRegisterWrite (
    SMUx0B_x8504_ADDRESS,
    &SMUx0B_x8504.Value,
    1,
    TRUE,
    StdHeader
    );

  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuUvdPowerGatingInit Exit\n");
  NbSmuServiceRequest (0x01, TRUE, StdHeader);
}



/*----------------------------------------------------------------------------------------*/
/**
 * UVD Power Shutdown
 *
 *
 *
 * @param[in] StdHeader     Standard Configuration Header
 */


VOID
STATIC
F14NbSmuUvdShutdown (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuUvdShutdown Enter\n");
  NbSmuServiceRequest (0x03, TRUE, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuUvdShutdown Exit\n");
}


/// GMC shutdown table
UINT32  F14SmuGmcShutdownTable_1[] = {
//    SMUx0B_x8600_ADDRESS,
  (0x3 << SMUx0B_x8600_TransactionCount_OFFSET) |
  (0x8650 << SMUx0B_x8600_MemAddr_7_0__OFFSET),
//    SMUx0B_x8604_ADDRESS,
  (0xFE << SMUx0B_x8604_Txn1MBusAddr_31_24__OFFSET) |
  (0x60 << SMUx0B_x8604_Txn1MBusAddr_23_16__OFFSET) |
  (0x14 << SMUx0B_x8604_Txn1TransferLength_7_0__OFFSET),
//    SMUx0B_x8608_ADDRESS,
  (0x03ull << SMUx0B_x8608_Txn1Tsize_OFFSET) |
  (0x01ull << SMUx0B_x8608_Txn1Overlap_OFFSET) |
  (0x01ull << SMUx0B_x8608_Txn1Mode_OFFSET) |
  (0x07ull << SMUx0B_x8608_Txn2Mbusaddr70_OFFSET),
//    SMUx0B_x860C_ADDRESS,
  (0xFE << SMUx0B_x860C_Txn2MBusAddr3124_OFFSET) |
  (0x60 << SMUx0B_x860C_Txn2MBusAddr2316_OFFSET) |
  (0x4  << SMUx0B_x860C_Txn2TransferLength70_OFFSET) |
  (0x3  << SMUx0B_x860C_Txn2Tsize_OFFSET),
//    SMUx0B_x8610_ADDRESS,
  (0x1 << SMUx0B_x8610_Txn2Overlap_OFFSET) |
  (0x1 << SMUx0B_x8610_Txn2Mode_OFFSET) |
  (0x60 << SMUx0B_x8610_Txn3MBusAddr2316_OFFSET) |
  (0x6  << SMUx0B_x8610_Txn3MBusAddr70_OFFSET),
//    SMUx0B_x8614_ADDRESS,
  (0xFEull << SMUx0B_x8614_Txn3MBusAddr3124_OFFSET) |
  (0x04ull << SMUx0B_x8614_Txn3TransferLength70_OFFSET) |
  (0x03ull << SMUx0B_x8614_Txn3Tsize_OFFSET) |
  (0x01ull << SMUx0B_x8614_Txn3Mode_OFFSET),
};

UINT32  F14SmuGmcShutdownTable_2[] = {
//    SMUx0B_x8650_ADDRESS,
  0x76543210,
//    SMUx0B_x8654_ADDRESS,
  0xFEDCBA98,
//    SMUx0B_x8658_ADDRESS,
  0x8,
//    SMUx0B_x865C_ADDRESS,
  0x00320032,
//    SMUx0B_x8660_ADDRESS,
  0x00100010,
//    SMUx0B_x8664_ADDRESS,
  0x00320032,
//    SMUx0B_x866C_ADDRESS,
  0x00
};

/*----------------------------------------------------------------------------------------*/
/**
 * Shutdown GMC
 *
 *
 *
 * @param[in] StdHeader     Standard Configuration Header
 */

VOID
STATIC
F14NbSmuGmcShutdown (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuGmcShutdown Enter\n");
  NbSmuRcuRegisterWrite (
    SMUx0B_x8600_ADDRESS,
    &F14SmuGmcShutdownTable_1[0],
    sizeof (F14SmuGmcShutdownTable_1) / sizeof (UINT32),
    TRUE,
    StdHeader
    );

  NbSmuRcuRegisterWrite (
    SMUx0B_x8650_ADDRESS,
    &F14SmuGmcShutdownTable_2[0],
    sizeof (F14SmuGmcShutdownTable_2) / sizeof (UINT32),
    TRUE,
    StdHeader
    );

  NbSmuServiceRequest (0x0B, TRUE, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuGmcShutdown Exit\n");
}

/// GFX shutdown table
UINT32  F14SmuGfxShutdownTable_1[] = {
//    SMUx0B_x8600_ADDRESS,
  (0x09ull   << SMUx0B_x8600_TransactionCount_OFFSET) |
  (0x8650ull << SMUx0B_x8600_MemAddr_7_0__OFFSET) |
  (0x00ull   << SMUx0B_x8600_Txn1MBusAddr_7_0__OFFSET),
//    SMUx0B_x8604_ADDRESS,
  (0xFEull << SMUx0B_x8604_Txn1MBusAddr_31_24__OFFSET) |
  (0x70ull << SMUx0B_x8604_Txn1MBusAddr_23_16__OFFSET) |
  (0x00ull << SMUx0B_x8604_Txn1MBusAddr_15_8__OFFSET) |
  (0x14ull << SMUx0B_x8604_Txn1TransferLength_7_0__OFFSET),
//    SMUx0B_x8608_ADDRESS,
  (0x03ull << SMUx0B_x8608_Txn1Tsize_OFFSET) |
  (0x00ull << SMUx0B_x8608_Txn1TransferLength_13_8__OFFSET) |
  (0x00ull << SMUx0B_x8608_Txn1Spare_OFFSET) |
  (0x01ull << SMUx0B_x8608_Txn1Overlap_OFFSET) |
  (0x00ull << SMUx0B_x8608_Txn1Static_OFFSET) |
  (0x01ull << SMUx0B_x8608_Txn1Mode_OFFSET) |
  (0x00ull << SMUx0B_x8608_Txn2Mbusaddr158_OFFSET) |
  (0x07ull << SMUx0B_x8608_Txn2Mbusaddr70_OFFSET),
//    SMUx0B_x860C_ADDRESS,
  (0xFEull << SMUx0B_x860C_Txn2MBusAddr3124_OFFSET) |
  (0x70ull << SMUx0B_x860C_Txn2MBusAddr2316_OFFSET) |
  (0x04ull << SMUx0B_x860C_Txn2TransferLength70_OFFSET) |
  (0x03ull << SMUx0B_x860C_Txn2Tsize_OFFSET) |
  (0x00ull << SMUx0B_x860C_Txn2TransferLength138_OFFSET),
//    SMUx0B_x8610_ADDRESS,
  (0x00ull << SMUx0B_x8610_Txn2Spare_OFFSET) |
  (0x01ull << SMUx0B_x8610_Txn2Overlap_OFFSET) |
  (0x00ull << SMUx0B_x8610_Txn2Static_OFFSET) |
  (0x01ull << SMUx0B_x8610_Txn2Mode_OFFSET) |
  (0x70ull << SMUx0B_x8610_Txn3MBusAddr2316_OFFSET) |
  (0x00ull << SMUx0B_x8610_Txn3MBusAddr158_OFFSET) |
  (0x06ull << SMUx0B_x8610_Txn3MBusAddr70_OFFSET),
//    SMUx0B_x8614_ADDRESS,
  (0xFEull << SMUx0B_x8614_Txn3MBusAddr3124_OFFSET) |
  (0x04ull << SMUx0B_x8614_Txn3TransferLength70_OFFSET) |
  (0x03ull << SMUx0B_x8614_Txn3Tsize_OFFSET) |
  (0x00ull << SMUx0B_x8614_Txn3TransferLength138_OFFSET) |
  (0x00ull << SMUx0B_x8614_Txn3Spare_OFFSET) |
  (0x00ull << SMUx0B_x8614_Txn3Overlap_OFFSET) |
  (0x00ull << SMUx0B_x8614_Txn3Static_OFFSET) |
  (0x01ull << SMUx0B_x8614_Txn3Mode_OFFSET),
//    SMUx0B_x8618_ADDRESS,
  (0xFEull << SMUx0B_x8618_Txn4MBusAddr3124_OFFSET) |
  (0xA0ull << SMUx0B_x8618_Txn4MBusAddr2316_OFFSET) |
  (0x00ull << SMUx0B_x8618_Txn4MBusAddr158_OFFSET) |
  (0x00ull << SMUx0B_x8618_Txn4MBusAddr70_OFFSET),
//    SMUx0B_x861C_ADDRESS,
  (0x07ull << SMUx0B_x861C_Txn5Mbusaddr70_OFFSET) |
  (0x14ull << SMUx0B_x861C_Txn4TransferLength70_OFFSET) |
  (0x03ull << SMUx0B_x861C_Txn4Tsize_OFFSET) |
  (0x00ull << SMUx0B_x861C_Txn4TransferLength138_OFFSET) |
  (0x00ull << SMUx0B_x861C_Txn4Spare_OFFSET) |
  (0x01ull << SMUx0B_x861C_Txn4Overlap_OFFSET) |
  (0x00ull << SMUx0B_x861C_Txn4Static_OFFSET) |
  (0x01ull << SMUx0B_x861C_Txn4Mode_OFFSET),
//    SMUx0B_x8620_ADDRESS,
  (0x00ull << SMUx0B_x8620_Txn5MBusAddr158_OFFSET) |
  (0xA0ull << SMUx0B_x8620_Txn5MBusAddr2316_OFFSET) |
  (0xFEull << SMUx0B_x8620_Txn5MBusAddr3124_OFFSET) |
  (0x04ull << SMUx0B_x8620_Txn5TransferLength70_OFFSET),
//    SMUx0B_x8624_ADDRESS,
  (0x03ull << SMUx0B_x8624_Txn5Tsize_OFFSET) |
  (0x00ull << SMUx0B_x8624_Txn5TransferLength138_OFFSET) |
  (0x00ull << SMUx0B_x8624_Txn5Spare_OFFSET) |
  (0x01ull << SMUx0B_x8624_Txn5Overlap_OFFSET) |
  (0x00ull << SMUx0B_x8624_Txn5Static_OFFSET) |
  (0x01ull << SMUx0B_x8624_Txn5Mode_OFFSET) |
  (0x00ull << SMUx0B_x8624_Txn6MBusAddr158_OFFSET) |
  (0x06ull << SMUx0B_x8624_Txn6MBusAddr70_OFFSET),
//    SMUx0B_x8628_ADDRESS,
  (0xFEull << SMUx0B_x8628_Txn6MBusAddr3124_OFFSET) |
  (0xA0ull << SMUx0B_x8628_Txn6MBusAddr2316_OFFSET) |
  (0x04ull << SMUx0B_x8628_Txn6TransferLength70_OFFSET) |
  (0x03ull << SMUx0B_x8628_Txn6Tsize_OFFSET) |
  (0x00ull << SMUx0B_x8628_Txn6TransferLength138_OFFSET),
//    SMUx0B_x862C_ADDRESS,
  (0xB0ull << SMUx0B_x862C_Txn7MBusAddr2316_OFFSET) |
  (0x00ull << SMUx0B_x862C_Txn7MBusAddr158_OFFSET) |
  (0x00ull << SMUx0B_x862C_Txn7MBusAddr70_OFFSET) |
  (0x00ull << SMUx0B_x862C_Txn6Spare_OFFSET) |
  (0x00ull << SMUx0B_x862C_Txn6Overlap_OFFSET) |
  (0x00ull << SMUx0B_x862C_Txn6Static_OFFSET) |
  (0x01ull << SMUx0B_x862C_Txn6Mode_OFFSET),
//    SMUx0B_x8630_ADDRESS,
  (0xFEull << SMUx0B_x8630_Txn7MBusAddr3124_OFFSET) |
  (0x14ull << SMUx0B_x8630_Txn7TransferLength70_OFFSET) |
  (0x03ull << SMUx0B_x8630_Txn7Tsize_OFFSET) |
  (0x00ull << SMUx0B_x8630_Txn7TransferLength138_OFFSET) |
  (0x00ull << SMUx0B_x8630_Txn7Spare_OFFSET) |
  (0x01ull << SMUx0B_x8630_Txn7Overlap_OFFSET) |
  (0x00ull << SMUx0B_x8630_Txn7Static_OFFSET) |
  (0x01ull << SMUx0B_x8630_Txn7Mode_OFFSET),
//    SMUx0B_x8634_ADDRESS,
  (0xFEull << SMUx0B_x8634_Txn8MBusAddr3124_OFFSET) |
  (0xB0ull << SMUx0B_x8634_Txn8MBusAddr2316_OFFSET) |
  (0x00ull << SMUx0B_x8634_Txn8MBusAddr158_OFFSET) |
  (0x07ull << SMUx0B_x8634_Txn8MBusAddr70_OFFSET),
//    SMUx0B_x8638_ADDRESS,
  (0x06ull << SMUx0B_x8638_Txn9MBusAddr70_OFFSET) |
  (0x04ull << SMUx0B_x8638_Txn8TransferLength70_OFFSET) |
  (0x03ull << SMUx0B_x8638_Txn8Tsize_OFFSET) |
  (0x00ull << SMUx0B_x8638_Txn8TransferLength138_OFFSET) |
  (0x00ull << SMUx0B_x8638_Txn8Spare_OFFSET) |
  (0x01ull << SMUx0B_x8638_Txn8Overlap_OFFSET) |
  (0x00ull << SMUx0B_x8638_Txn8Static_OFFSET) |
  (0x01ull << SMUx0B_x8638_Txn8Mode_OFFSET),
//    SMUx0B_x863C_ADDRESS,
  (0x00ull << SMUx0B_x863C_Txn9MBusAddr158_OFFSET) |
  (0xB0ull << SMUx0B_x863C_Txn9MBuAaddr2316_OFFSET) |
  (0xFEull << SMUx0B_x863C_Txn9MBusAddr3124_OFFSET) |
  (0x04ull << SMUx0B_x863C_Txn9TransferLength70_OFFSET),
//    SMUx0B_x8640_ADDRESS,
  (0x03ull << SMUx0B_x8640_Txn9Tsize_OFFSET) |
  (0x00ull << SMUx0B_x8640_Txn9TransferLength138_OFFSET) |
  (0x00ull << SMUx0B_x8640_Txn9Spare_OFFSET) |
  (0x00ull << SMUx0B_x8640_Txn9Overlap_OFFSET) |
  (0x00ull << SMUx0B_x8640_Txn9Static_OFFSET) |
  (0x01ull << SMUx0B_x8640_Txn9Mode_OFFSET) |
  (0x00ull << SMUx0B_x8640_Txn10MBusAddr158_OFFSET) |
  (0x00ull << SMUx0B_x8640_Txn10MBusAddr70_OFFSET)
};
UINT32  F14SmuGfxShutdownTable_2[] = {
//    SMUx0B_x8650_ADDRESS,
  0x10103210,
//    SMUx0B_x8654_ADDRESS,
  0x10101010,
//    SMUx0B_x8658_ADDRESS,
  0x20,
//    SMUx0B_x865C_ADDRESS,
  0x00320032,
//    SMUx0B_x8660_ADDRESS,
  0x00100010,
//    SMUx0B_x8664_ADDRESS,
  0x0032000A,
//    SMUx0B_x866C_ADDRESS,
  0x00,
//    SMUx0B_x8670_ADDRESS,
  0x10103210,
//    SMUx0B_x8674_ADDRESS,
  0x10101010,
//    SMUx0B_x8678_ADDRESS,
  0x20,
//    SMUx0B_x867C_ADDRESS,
  0x00320032,
//    SMUx0B_x8680_ADDRESS,
  0x00100010,
//    SMUx0B_x8684_ADDRESS,
  0x00320010,
//    SMUx0B_x868C_ADDRESS,
  0x00,
//    SMUx0B_x8690_ADDRESS,
  0x10103210,
//    SMUx0B_x8694_ADDRESS,
  0x10101010,
//    SMUx0B_x8698_ADDRESS,
  0x20,
//    SMUx0B_x869C_ADDRESS,
  0x00320032,
//    SMUx0B_x86A0_ADDRESS,
  0x00100010,
//    SMUx0B_x86A4_ADDRESS,
  0x00320016,
//    SMUx0B_x86AC_ADDRESS,
  0x00
};

/*----------------------------------------------------------------------------------------*/
/**
 * Shutdown GFX
 *
 *
 *
 * @param[in] StdHeader     Standard Configuration Header
 */



VOID
STATIC
F14NbSmuGfxShutdown (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuGfxShutdown Enter\n");
  NbSmuRcuRegisterWrite (
    SMUx0B_x8600_ADDRESS,
    &F14SmuGfxShutdownTable_1[0],
    sizeof (F14SmuGfxShutdownTable_1) / sizeof (UINT32),
    TRUE,
    StdHeader
    );

  NbSmuRcuRegisterWrite (
    SMUx0B_x8650_ADDRESS,
    &F14SmuGfxShutdownTable_2[0],
    sizeof (F14SmuGfxShutdownTable_2) / sizeof (UINT32),
    TRUE,
    StdHeader
    );

  NbSmuServiceRequest (0x0B, TRUE, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuGfxShutdown Exit\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * Power gate unused blocks
 *
 *
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
F14NbPowerGateFeature (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  NB_POWERGATE_CONFIG    NbPowerGate;
  FCRxFF30_0398_STRUCT   FCRxFF30_0398;
  IDS_HDT_CONSOLE (GNB_TRACE, "NbPowerGateFeature Enter\n");

  NbPowerGate.Services.GmcPowerGate = 0x1;
  NbPowerGate.Services.UvdPowerGate = 0x1;
  NbPowerGate.Services.GfxPowerGate = 0x1;
  LibAmdMemCopy (&NbPowerGate.Gmc, &F14NbGmcPowerGatingData, sizeof (POWER_GATE_DATA), StdHeader);
  LibAmdMemCopy (&NbPowerGate.Uvd, &F14NbUvdPowerGatingData, sizeof (POWER_GATE_DATA), StdHeader);
  IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_NB_POWERGATE_CONFIG, &NbPowerGate, StdHeader);
  F14NbSmuGmcPowerGatingInit (StdHeader, &NbPowerGate.Gmc);
  F14NbSmuUvdPowerGatingInit (StdHeader, &NbPowerGate.Uvd);
  if (!GfxLibIsControllerPresent (StdHeader)) {
    FCRxFF30_0398.Value = (1 << FCRxFF30_0398_SoftResetGrbm_OFFSET) | (1 << FCRxFF30_0398_SoftResetMc_OFFSET) |
                    (1 << FCRxFF30_0398_SoftResetDc_OFFSET) | (1 << FCRxFF30_0398_SoftResetRlc_OFFSET) |
                    (1 << FCRxFF30_0398_SoftResetUvd_OFFSET);
    NbSmuSrbmRegisterWrite (FCRxFF30_0398_ADDRESS, &FCRxFF30_0398.Value, TRUE, StdHeader);
    if (NbPowerGate.Services.GmcPowerGate == 1) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Shutdown GMC\n");
      F14NbSmuGmcShutdown (StdHeader);
    }
    if (NbPowerGate.Services.UvdPowerGate == 1) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Shutdown UVD\n");
      F14NbSmuUvdShutdown (StdHeader);
    }
    if (NbPowerGate.Services.GfxPowerGate == 1) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Shutdown GFX\n");
      F14NbSmuGfxShutdown (StdHeader);
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "NbPowerGateFeature Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get GMC restore latency
 *
 * Restore Latency = ((( DAUG_PSO_PWRUP + MOTH_PSO_PWRUP + PSO_RESTORE_TIMER + SAVE_RESTORE_WIDTH + PSO_CONTROL_PERIOD_7to4 +
 * ISO_TIMER + 10) * PSO_CONTROL_VALID_NUM) + RESET_TIMER ) * 10ns
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @retval    AGESA_STATUS
 */

UINT32
F14NbPowerGateGmcRestoreLatency (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 RestoreLatency;
  //may need dynamic calculation
  RestoreLatency = ((POWER_GATE_GMC_DAUG_PSO_PWRUP + POWER_GATE_GMC_MOTH_PSO_PWRUP +
                   POWER_GATE_GMC_SAVE_RESTORE_WIDTH + POWER_GATE_GMC_RSO_RESTORE_TIMER +
                   POWER_GATE_GMCPSO_CONTROL_PERIOD_7to4 + POWER_GATE_GMC_ISO_TIMER + 10) *
                   POWER_GATE_GMC_PSO_CONTROL_VALID_NUM + POWER_GATE_GMC_RESET_TIMER) * 10;
  return RestoreLatency;
}
