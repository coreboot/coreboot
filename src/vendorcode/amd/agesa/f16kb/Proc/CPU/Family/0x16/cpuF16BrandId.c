/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU BrandId related functions and structures.
 *
 * Contains code that provides CPU BrandId information
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
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuEarlyInit.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_CPUF16BRANDID_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define NAME_STRING_ADDRESS_PORT 0x194
#define NAME_STRING_DATA_PORT 0x198

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// FAM16_BRAND_STRING_MSR
typedef struct _PROCESSOR_NAME_STRING {
  UINT32 lo;                 ///< lower 32-bits of 64-bit value
  UINT32 hi;                 ///< highest 32-bits of 64-bit value
} PROCESSOR_NAME_STRING;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
F16SetBrandIdRegistersAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
CONST CHAR8 ROMDATA str_Unprogrammed_Sample[48] = "AMD Unprogrammed Engineering Sample";
/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------*/
/**
 * Set the Processor Name String register based on F5x194/198
 *
 * This function copies F5x198_x[B:0] to MSR_C001_00[35:30]
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
F16SetBrandIdRegistersAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32 PciData;
  UINT32 MsrIndex;
  UINT64 MsrData;
  UINT64 *MsrNameStringPtrPtr;
  PCI_ADDR PciAddress;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, FUNC_5, 0);
  PciAddress.Address.Register = NAME_STRING_ADDRESS_PORT;
  // check if D18F5x198_x0 is 00000000h.
  PciData = 0;
  LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
  PciAddress.Address.Register = NAME_STRING_DATA_PORT;
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
  if (PciData != 0) {
    for (MsrIndex = 0; MsrIndex <= (MSR_CPUID_NAME_STRING5 - MSR_CPUID_NAME_STRING0); MsrIndex++) {
      PciAddress.Address.Register = NAME_STRING_ADDRESS_PORT;
      PciData = MsrIndex * 2;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
      PciAddress.Address.Register = NAME_STRING_DATA_PORT;
      LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
      ((PROCESSOR_NAME_STRING *) (&MsrData))->lo = PciData;

      PciAddress.Address.Register = NAME_STRING_ADDRESS_PORT;
      PciData = (MsrIndex * 2) + 1;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
      PciAddress.Address.Register = NAME_STRING_DATA_PORT;
      LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
      ((PROCESSOR_NAME_STRING *) (&MsrData))->hi = PciData;

      LibAmdMsrWrite ((MsrIndex + MSR_CPUID_NAME_STRING0), &MsrData, StdHeader);
    }
  } else {
    // It is unprogrammed (unfused) parts and use a name string of "AMD Unprogrammed Engineering Sample"
    MsrNameStringPtrPtr = (UINT64 *) str_Unprogrammed_Sample;
    // Put values into name MSRs,  Always write the full 48 bytes
    for (MsrIndex = MSR_CPUID_NAME_STRING0; MsrIndex <= MSR_CPUID_NAME_STRING5; MsrIndex++) {
      LibAmdMsrWrite (MsrIndex, MsrNameStringPtrPtr, StdHeader);
      MsrNameStringPtrPtr++;
    }
  }
  return;
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

