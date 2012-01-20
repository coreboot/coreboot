/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 RB PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 ******************************************************************************
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
RDATA_GROUP (G2_PEI)


#define FILECODE PROC_CPU_FAMILY_0X10_REVC_RB_F10RBHTPHYTABLES_FILECODE

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

//  HT   Phy   T a b l e s
// -------------------------
STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10RbHtPhyRegisters[] =
{
// Erratum 354
// 0x40:0x48
  {
    HtPhyRangeRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      (AMD_F10_RB_C1)                     // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT3,               //
      0x40, 0x48,                           // Address
      0x00000040,                           // regData
      0x00000040,                           // regMask
    }}
  },
// 0x50:0x58
  {
    HtPhyRangeRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      (AMD_F10_RB_C1)                     // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT3,               //
      0x50, 0x58,                           // Address
      0x00000040,                           // regData
      0x00000040,                           // regMask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F10RbHtPhyRegisterTable = {
  PrimaryCores,
  (sizeof (F10RbHtPhyRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10RbHtPhyRegisters
};

