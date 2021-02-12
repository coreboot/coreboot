/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 WHEA initial Data
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "cpuLateInit.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_CPUF16WHEAINITDATATABLES_FILECODE


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
GetF16WheaInitData (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **F16WheaInitDataPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AMD_HEST_BANK_INIT_DATA F16HestBankInitData[] = {
  {0xFFFFFFFF,0xFFFFFFFF,0x400,0x401,0x402,0x403},
  {0xFFFFFFFF,0xFFFFFFFF,0x404,0x405,0x406,0x407},
  {0xFFFFFFFF,0xFFFFFFFF,0x408,0x409,0x40A,0x40B},
  {0xFFFFFFFF,0xFFFFFFFF,0x410,0x411,0x412,0x413},
  {0xFFFFFFFF,0xFFFFFFFF,0x414,0x415,0x416,0x417},
  {0xFFFFFFFF,0xFFFFFFFF,0x418,0x419,0x41A,0x41B},
};

AMD_WHEA_INIT_DATA F16WheaInitData = {
  0x000000000,              // AmdGlobCapInitDataLsd
  0x000000000,              // AmdGlobCapInitDataMsd
  0x000000077,              // AmdGlobCtrlInitDataLsd
  0x000000000,              // AmdGlobCtrlInitDataMsd
  0x00,                     // AmdMcbClrStatusOnInit
  0x02,                     // AmdMcbStatusDataFormat
  0x00,                     // AmdMcbConfWriteEn
  ARRAY_SIZE(F16HestBankInitData), // HestBankNum
  &F16HestBankInitData[0]   // Pointer to Initial data of HEST Bank
};


/*---------------------------------------------------------------------------------------*/
/**
 *  Returns the family specific WHEA table properties.
 *
 *  @CpuServiceMethod{::F_CPU_GET_FAMILY_SPECIFIC_ARRAY}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[out]  F16WheaInitDataPtr       Points to the family 16h WHEA properties.
 *  @param[out]  NumberOfElements         Will be one to indicate one structure.
 *  @param[in]   StdHeader                Header for library and services.
 *
 */
VOID
GetF16WheaInitData (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **F16WheaInitDataPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  *NumberOfElements = 1;
  *F16WheaInitDataPtr = &F16WheaInitData;
}
