/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 Rev E PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10/RevE
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
#include "F10PackageType.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X10_REVE_F10REVEPCITABLES_FILECODE

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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10RevEPciRegisters[] =
{
// F0x68 -
// BufRelPri for rev E
// bits[14:13]  BufRelPri = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Ex                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO(0, 0, 24, FUNC_0, 0x68),   // Address
      0x00002000,                           // regData
      0x00006000,                           // regMask
    }
  },

// F0x16C - Link Global Extended Control Register
// bit[7:6] InLnSt = 0x01
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Ex                          // CpuRevision
    },
    AMD_PF_SINGLE_LINK,                // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x0000C026,                           // regData
      0x0000E03F,                           // regMask
    }
  },
// F0x16C - Link Global Extended Control Register
// bit[15:13] ForceFullT0 = 6
// bit[9] RXCalEn = 1
// bit[5:0] T0Time = 0x26
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Ex                      // CpuRevision
    },
    AMD_PF_SINGLE_LINK,                   // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x0000C226,                           // regData
      0x0000E23F,                           // regMask
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
// bits[7:5] ClkDivisor = 7
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Ex                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0x0000E681,                           // regData
      0x0000FFFF,                           // regMask
    }
  },
// F3xDC - Clock Power Timing Control 2
// bits[14:12] NbsynPtrAdj = 6
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Ex                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xDC),  // Address
      0x00006000,                           // regData
      0x00007000,                           // regMask
    }
  },
// F3x1C4 - L3 Power Control Register
// bits[8] L3PwrSavEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Ex                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1C4), // Address
      0x00000100,                           // regData
      0x00000100,                           // regMask
    }
  },
// F3x188 - NB Extended Configuration Low Register
// bit[4] = EnStpGntOnFlushMaskWakeup
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Ex                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x188), // Address
      0x00000010,                           // regData
      0x00000010,                           // regMask
    }
  },
// F4x15C - Core Performance Boost Control
// bits[1:0]   BoostSrc = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Ex                          // CpuRevision
    },
    AMD_PF_ALL,                           // platformFeatures
    {
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x15C), // Address
      0x00000000,                           // regData
      0x00000003,                           // regMask
    }
  },
};

CONST REGISTER_TABLE ROMDATA F10RevEPciRegisterTable = {
  PrimaryCores,
  (sizeof (F10RevEPciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10RevEPciRegisters,
};
