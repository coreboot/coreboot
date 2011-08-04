/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_14 Ontario PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x14/ON
 * @e \$Revision: 46389 $   @e \$Date: 2011-01-31 22:22:49 -0500 (Mon, 31 Jan 2011) $
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
#define FILECODE PROC_CPU_FAMILY_0X14_ON_F14ONPCITABLES_FILECODE

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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F14OnPciRegisters[] =
{
// Function 4

// D18F4x104 - TDP Lock Accumulator
// bits[1:0]   TdpLockDivVal = 1
// bits[13:2]  TdpLockDivRate = 0x190
// bits[16:15] TdpLockDivValCpu = 1
// bits[28:17] TdpLockDivRateCpu = 0x190
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ON_Cx                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x104),  // Address
      0x03208641,                           // regData
      0x1FFFBFFF,                           // regMask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F14OnPciRegisterTable = {
  PrimaryCores,
  (sizeof (F14OnPciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F14OnPciRegisters,
};
