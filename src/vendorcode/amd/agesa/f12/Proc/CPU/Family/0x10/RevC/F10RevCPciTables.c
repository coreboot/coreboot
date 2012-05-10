/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 Rev C PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10/RevC
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "F10PackageType.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE PROC_CPU_FAMILY_0X10_REVC_F10REVCPCITABLES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

//  P C I    T a b l e s
// ----------------------

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10RevCPciRegisters[] =
{
// Function 2 - DRAM Controller

// F2x1B0 - Extended Memory Controller Configuration Low Register
//
// bit[5:4], AdapPrefNegativeStep = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x1B0), // Address
      0x00000000,                           // regData
      0x00000030,                           // regMask
    }}
  },
// Function 3 - Misc. Control

// F3x158 - Link to XCS Token Count
// bits[3:0] LnkToXcsDRToken = 3
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_GT_A2                       // CpuRevision
    },
    {AMD_PF_UMA},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x158), // Address
      0x00000003,                           // regData
      0x0000000F,                           // regMask
    }}
  },
// F3x80 - ACPI Power State Control
// ACPI State C2
// bits[0] CpuPrbEn = 1
// bits[1] NbLowPwrEn = 0
// bits[2] NbGateEn = 0
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 1
// ACPI State C3, C1E or Link init
// bits[0] CpuPrbEn = 0
// bits[1] NbLowPwrEn = 1
// bits[2] NbGateEn = 1
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 7
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0x0000E681,                           // regData
      0x0000FFFF,                           // regMask
    }}
  },
// F3x80 - ACPI Power State Control
// ACPI State C3, C1E or Link init
// bits[0] CpuPrbEn = 1
// bits[1] NbLowPwrEn = 1
// bits[2] NbGateEn = 1
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 4
  {
    HtFeatPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    {AMD_PF_SINGLE_LINK},                   // platformFeatures
    {{
      HT_HOST_FEAT_HT1,                     // link feats
      PACKAGE_TYPE_ASB2,                    // package type
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0x00008700,                           // regData
      0x0000FF00,                           // regMask
    }}
  },
// F3x80 - ACPI Power State Control
// ACPI State C3, C1E or Link init
// bits[0] CpuPrbEn = 0
// bits[1] NbLowPwrEn = 1
// bits[2] NbGateEn = 1
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 1
// bits[7:5] ClkDivisor = 7
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_C3                           // CpuRevision
    },
    {AMD_PF_ALL},                             // platformFeatures
    {{
      PERFORMANCE_VRM_HIGH_SPEED_ENABLE,    // PerformanceFeatures
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0x0000F600,                           // regData
      0x0000FF00,                           // regMask
    }}
  },
// F3x80 - ACPI Power State Control
// ACPI State C3, C1E or Link init
// bits[0] CpuPrbEn = 1
// bits[1] NbLowPwrEn = 1
// bits[2] NbGateEn = 1
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 4
  {
    HtFeatPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    {AMD_PF_SINGLE_LINK},                   // platformFeatures
    {{
      HT_HOST_FEAT_HT1,                     // link feats
      PACKAGE_TYPE_ALL & (~ PACKAGE_TYPE_ASB2),  // package type
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0x00008700,                           // regData
      0x0000FF00,                           // regMask
    }}
  },
// F3xDC - Clock Power Timing Control 2
// bits[14:12] NbsynPtrAdj = 5
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xDC),  // Address
      0x00005000,                           // regData
      0x00007000,                           // regMask
    }}
  },
// F3x180 - NB Extended Configuration
// bits[23] SyncFloodOnDramTempErr = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x180), // Address
      0x00800000,                           // regData
      0x00800000,                           // regMask
    }}
  },
// F3x188 - NB Extended Configuration Low Register
// bit[22] = DisHldReg2
// Errata #346
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x188), // Address
      0x00400000,                           // regData
      0x00400000,                           // regMask
    }}
  }
};

CONST REGISTER_TABLE ROMDATA F10RevCPciRegisterTable = {
  PrimaryCores,
  (sizeof (F10RevCPciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10RevCPciRegisters,
};
