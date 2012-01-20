/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Bulldozer Equivalence Table related data
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 55600 $   @e \$Date: 2011-06-23 12:39:18 -0600 (Thu, 23 Jun 2011) $
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
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
#include "amdlib.h"
#include "cpuRegisters.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15OREQUIVALENCETABLE_FILECODE


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
VOID
GetF15OrMicrocodeEquivalenceTable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **OrEquivalenceTablePtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
STATIC CONST UINT16 ROMDATA CpuF15OrMicrocodeEquivalenceTable[] =
{
  0x6012, 0x6012,
  0x6011, 0x6011,
  0x6010, 0x6010,
  0x6001, 0x6001,
  0x6000, 0x6000
};

// Unencrypted equivalent
STATIC CONST UINT16 ROMDATA CpuF15OrUnEncryptedMicrocodeEquivalenceTable[] =
{
  0x6012, 0x6812,
  0x6011, 0x6811,
  0x6010, 0x6810,
  0x6001, 0x6801,
  0x6000, 0x6800
};

/*---------------------------------------------------------------------------------------*/
/**
 *  Returns the appropriate microcode patch equivalent ID table.
 *
 *  @CpuServiceMethod{::F_CPU_GET_FAMILY_SPECIFIC_ARRAY}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[out]  OrEquivalenceTablePtr    Points to the first entry in the table.
 *  @param[out]  NumberOfElements         Number of valid entries in the table.
 *  @param[in]   StdHeader                Header for library and services.
 *
 */
VOID
GetF15OrMicrocodeEquivalenceTable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **OrEquivalenceTablePtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 MsrDeCfg;

  LibAmdMsrRead (MSR_DE_CFG, &MsrDeCfg, StdHeader);
  if ((MsrDeCfg & 0x80000) == 0) {
    *NumberOfElements = ((sizeof (CpuF15OrUnEncryptedMicrocodeEquivalenceTable) / sizeof (UINT16)) / 2);
    *OrEquivalenceTablePtr = CpuF15OrUnEncryptedMicrocodeEquivalenceTable;
  } else {
    *NumberOfElements = ((sizeof (CpuF15OrMicrocodeEquivalenceTable) / sizeof (UINT16)) / 2);
    *OrEquivalenceTablePtr = CpuF15OrMicrocodeEquivalenceTable;
  }
}

