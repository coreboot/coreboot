/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 Hydra PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10
 * @e \$Revision: 35136 $   @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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
#include "AGESA.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X10_REVD_HY_F10HYPCITABLES_FILECODE

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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10HyPciRegisters[] =
{
// F0x68 -
  // BufRelPri for rev D
  // bits[14:13]  BufRelPri = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Dx                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO(0, 0, 24, FUNC_0, 0x68),   // Address
      0x00002000,                           // regData
      0x00006000,                           // regMask
    }
  },
// F0x150 - Link Global Retry Control Register
// bit[18:16] TotalRetryAttempts = 7
// bit[13] HtRetryCrcDatInsDynEn = 1
// bit[12]HtRetryCrcCmdPackDynEn = 1
// bit[11:9] HtRetryCrcDatIns = 0
// bit[8] HtRetryCrcCmdPack = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x150), // Address
      0x00073100,                           // regData
      0x00073F00,                           // regMask
    }
  },
// F0x16C - Link Global Extended Control Register
// bit[15:13] ForceFullT0 = 6
// bit[5:0] T0Time = 0x26
  {
    PciRegister,
    {
      AMD_FAMILY_10_HY,                   // CpuFamily
      AMD_F10_D1                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x0000C026,                           // regData
      0x0000E03F,                           // regMask
    }
  },
// F0x16C - Link Global Extended Control Register
// bit[9] RXCalEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10_HY,                   // CpuFamily
      AMD_F10_Dx                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x00000200,                           // regData
      0x00000200,                           // regMask
    }
  },
// F0x16C - Link Global Extended Control Register
// bit[7:6] InLnSt = 01b (PHY_OFF)
  {
    PciRegister,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_Dx                            // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x00000040,                           // regData
      0x000000C0,                           // regMask
    }
  },
// F0x[18C:170] - Link Extended Control Register - All connected links.
// bit[8] LS2En = 1
  {
    HtLinkPciRegister,
    {
      AMD_FAMILY_10_HY,                     // CpuFamily
      AMD_F10_D1                            // CpuRevision
    },
    AMD_PF_ALL,                             // platform Features
    {
      HT_HOST_FEATURES_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x170), // Address
      0x00000100,                           // regData
      0x00000100,                           // regMask
    }
  },
// F2x1B0 - Extended Memory Controller Configuration Low
// bits[10:8], CohPrefPrbLmt = 0
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_Dx                            // CpuRevision
    },
    AMD_PF_ALL,                             // platformFeatures
    {
      PERFORMANCE_PROBEFILTER,              // Features
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x1B0), // Address
      0x00000000,                           // regData
      0x00000700,                           // regMask
    }
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
    AMD_PF_UMA,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x158), // Address
      0x00000003,                           // regData
      0x0000000F,                           // regMask
    }
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
// bits[7:5] ClkDivisor = 5
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_GT_Ax                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0x0000A681,                           // regData
      0x0000FFFF,                           // regMask
    }
  },

// F3x80 - ACPI Power State Control
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
      AMD_F10_GT_D0                       // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0x0000E600,                           // regData
      0x0000FF00,                           // regMask
    }
  },

// F3xA0 - Power Control Miscellaneous
// bit[14] BpPinsTriEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_GT_D0                       // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x00004000,                           // regData
      0x00004000,                           // regMask
    }
  },

// F3x188 - NB Extended Configuration Low Register
// bit[27] = DisCpuWrSzDw64ReOrd
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x188), // Address
      0x08000000,                           // regData
      0x08000000,                           // regMask
    }
  },

// F3x1B8 - L3 Control
// bit[18] L3RdBufBypDis = 1, Erratum 374
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1B8), // Address
      0x00040000,                           // regData
      0x00040000,                           // regMask
    }
  },

// F3x1B8 - L3 Control
// bit[23] L3BankSwapDis = 1, Erratum 385
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Dx                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1B8), // Address
      0x00800000,                           // regData
      0x00800000,                           // regMask
    }
  },

// F3x1D4 - Probe Filter Control Register
// bits[21:20] PFPreferedSORepl = 2
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Dx                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1D4), // Address
      0x00200000,                           // regData
      0x00300000,                           // regMask
    }
  }
};

CONST REGISTER_TABLE ROMDATA F10HyPciRegisterTable = {
  PrimaryCores,
  (sizeof (F10HyPciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10HyPciRegisters,
};

