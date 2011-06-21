/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 RB, MSR tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
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


#define FILECODE PROC_CPU_FAMILY_0X10_REVC_RB_F10RBMSRTABLES_FILECODE

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
STATIC CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F10RbMsrRegisters[] =
{
//  M S R    T a b l e s
// ----------------------

// MSR_DC_CFG (0xC0011022)
// bits[43:42] = 0
// Errata #326
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                       // CpuFamily
      AMD_F10_RB_C0                        // CpuRevision
    },
    AMD_PF_MULTI_LINK,                     // platformFeatures
    {
      MSR_DC_CFG,                            // MSR Address
      0x0000000000000000,                    // OR Mask
      0x00000C0000000000,                    // NAND Mask
    }
  },

// MSR_BU_CFG (0xC0011023)
// Erratum #309 BU_CFG[23]=1
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_RB_C0                       // CpuRevision
    },
    AMD_PF_ALL,                            // platformFeatures
    {
      MSR_BU_CFG,                            // MSR Address
      (1 << 23),                             // OR Mask
      (1 << 23),                             // NAND Mask
    }
  }
};

CONST REGISTER_TABLE ROMDATA F10RbMsrRegisterTable = {
  AllCores,
  (sizeof (F10RbMsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F10RbMsrRegisters,
};
