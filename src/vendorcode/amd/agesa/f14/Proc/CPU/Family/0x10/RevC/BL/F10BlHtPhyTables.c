/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 BL PCI tables with values as defined in BKDG
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


#define FILECODE PROC_CPU_FAMILY_0X10_REVC_BL_F10BLHTPHYTABLES_FILECODE

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

//  HT  Phy   T a b l e s
// -------------------------
STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10BlHtPhyRegisters[] =
{

//
// NOTE: This entry is here for making this array not to be empty.
//       This entry should be removed after adding another.
//
//
// Deemphasis Settings
//

// For BL-C3, also set [7]TxLs23ClkGateEn.
//deemphasis level        DL1[20:16], DL2[12:8], DP1[4:0] PostCur1En[31] PostCur2En[30] PreCur1En[29] MapPostCur2En[6]
// No deemphasis            00h        00h          00h     0                0             0             0
// -3dB postcursor          12h        00h          00h     1                0             0             0
// -6dB postcursor          1Fh        00h          00h     1                0             0             0
// -8dB postcursor          1Fh        06h          00h     1                1             0             1
// -11dB postcursor         1Fh        0Dh          00h     1                1             0             1
// -11dB postcursor with
// -8dB precursor           1Fh        06h          07h     1                1             1             1

  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_BL_C3                         // CpuRevision
    },
    AMD_PF_ALL,                             // platformFeatures
    {
      DEEMPHASIS_LEVEL_NONE,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0x00000080,                           // regData
      0xE01F1FDF,                           // regMask
    }
  },
};

CONST REGISTER_TABLE ROMDATA F10BlHtPhyRegisterTable = {
  PrimaryCores,
  (sizeof (F10BlHtPhyRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10BlHtPhyRegisters
};
