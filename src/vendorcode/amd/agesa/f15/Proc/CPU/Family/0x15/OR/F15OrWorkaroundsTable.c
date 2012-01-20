/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Models 0x00 - 0x0F Specific Workaround table
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x15/OR
 * @e \$Revision: 57155 $   @e \$Date: 2011-07-28 02:27:47 -0600 (Thu, 28 Jul 2011) $
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuF15Utilities.h"
#include "F15OrUtilities.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORWORKAROUNDSTABLE_FILECODE

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

//  F a m i l y  S p e c i f i c  W o r k a r o u n d    T a b l e s
// -----------------------------------------------------------------

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F15OrWorkarounds[] =
{
// F0x6C - Link Initialization Control Register
// Request for warm reset in AmdInitEarly
// [5, BiosRstDet] = 1b
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_ALL                           // CpuRevision
    },
	{AMD_PF_ALL},                             // platformFeatures
    {{
      SetWarmResetAtEarly,                  // function call
      0x00000000,                           // data
    }}
  },
  // HT PHY DLL Compensation setting for rev B and later
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_15,
      AMD_F15_OR_GT_Ax
    },
    {AMD_PF_ALL},
    {{
      F15HtPhyOverrideDllCompensation,
      0x00000001
    }}
  },
  // Internal Node 1 SbiAddr sync for OR
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_15_OR,
      AMD_F15_OR_ALL
    },
    {AMD_PF_ALL},
    {{
      F15OrSyncInternalNode1SbiAddr,
      0x00000000
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F15OrWorkaroundsTable = {
  PrimaryCores,
  (sizeof (F15OrWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *)F15OrWorkarounds,
};
