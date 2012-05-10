/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 BL PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10
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


#define FILECODE PROC_CPU_FAMILY_0X10_REVC_BL_F10BLPCITABLES_FILECODE

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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10BlPciRegisters[] =
{
  // Function 0

// F0x16C - Link Global Extended Control Register, Errata 351
// bit[15:13] ForceFullT0 = 0
// bit[5:0] T0Time = 0x14
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_BL_C2                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x00000014,                           // regData
      0x0000E03F,                           // regMask
    }}
  },
// F0x16C - Link Global Extended Control Register
// bit[7:6] InLnSt = 0x01
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_BL_C3                       // CpuRevision
    },
    {AMD_PF_SINGLE_LINK},                // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x00000040,                           // regData
      0x000000C0,                           // regMask
    }}
  },
// F0x16C - Link Global Extended Control Register
// bit[15:13] ForceFullT0 = 6
// bit[9] RXCalEn = 1
// bit[5:0] T0Time = 0x26
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_BL_C3                      // CpuRevision
    },
    {AMD_PF_SINGLE_LINK},                   // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x0000C226,                           // regData
      0x0000E23F,                           // regMask
    }}
  },
// F0x170 - Link Extended Control Register - Link 0, sublink 0
// Errata 351 (only need to override single link case.)
// bit[8] LS2En = 0,
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_BL_C2                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x170), // Address
      0x00000000,                           // regData
      0x00000100,                           // regMask
    }}
  },


// F3x80 - ACPI Power State Control
// ACPI FIDVID Change
// bits[0] CpuPrbEn = 1
// bits[1] NbLowPwrEn = 1
// bits[2] NbGateEn = 0
// bits[3] NbCofChg = 1
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 0
  {
    HtFeatPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_BL_Cx                       // CpuRevision
    },
    {AMD_PF_SINGLE_LINK},                   // platformFeatures
    {{
      HT_HOST_FEATURES_ALL,                 // link feats
      PACKAGE_TYPE_S1G3_S1G4,               // package type
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0x000B0000,                           // regData
      0x00FF0000,                           // regMask
    }}
  },
// F3xA0 - Power Control Miscellaneous
// bits[28] NbPstateForce = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_BL_C3                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x10000000,                           // regData
      0x10000000,                           // regMask
    }}
  }
};

CONST REGISTER_TABLE ROMDATA F10BlPciRegisterTable = {
  PrimaryCores,
  (sizeof (F10BlPciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10BlPciRegisters,
};
