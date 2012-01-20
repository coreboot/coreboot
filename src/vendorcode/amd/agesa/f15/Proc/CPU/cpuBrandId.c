/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU BrandId related functions.
 *
 * Contains code that provides CPU BrandId information
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*****************************************************************************
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
#include "amdlib.h"
#include "OptionPstate.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuEarlyInit.h"
#include "cpuRegisters.h"
#include "heapManager.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_CPUBRANDID_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
CONST CHAR8 ROMDATA strEngSample[] = "AMD Engineering Sample";
CONST CHAR8 ROMDATA strTtkSample[] = "AMD Thermal Test Kit";
CONST CHAR8 ROMDATA strUnknown[] = "AMD Processor Model Unknown";

CONST AMD_CPU_BRAND ROMDATA EngSample_Str = {0, 0, 0, SOCKET_IGNORE, strEngSample, sizeof (strEngSample)};
CONST AMD_CPU_BRAND ROMDATA TtkSample_Str = {0, 1, 0, SOCKET_IGNORE, strTtkSample, sizeof (strTtkSample)};
CONST AMD_CPU_BRAND ROMDATA Dflt_Str1 = {0, 0, 0, SOCKET_IGNORE, strUnknown, sizeof (strUnknown)};
CONST AMD_CPU_BRAND ROMDATA Dflt_Str2 = {0, 0, 0, SOCKET_IGNORE, DR_NO_STRING, DR_NO_STRING};


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
SetBrandIdRegistersAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Program BrandID registers (CPUIDNameStringPtr[0-5])
 *
 * This function determines the appropriate brand string for the executing
 * core, and programs the namestring MSRs.
 *
 * @param[in,out] StdHeader   Config handle for library and services.
 *
 */
VOID
SetBrandIdRegisters (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8   SocketIndex;
  UINT8   SuffixStatus;
  UINT8   TableElements;
  UINT8   TableEntryCount;
  UINT8   TableEntryIndex;
  CHAR8   TempChar;
  CHAR8   *NameStringPtr;
  CHAR8   *SuffixStringPtr;
  CHAR8   *BrandStringPtr;
  CHAR8   *TempNameCharPtr;
  UINT32  MsrIndex;
  UINT32  Quotient;
  UINT32  Remainder;
  UINT64  *MsrNameStringPtrPtr;
  CPUID_DATA    CpuId;
  CPU_LOGICAL_ID CpuLogicalId;
  CPU_BRAND_TABLE *SocketTableEntry;
  CPU_BRAND_TABLE **SocketTableEntry1;
  AMD_CPU_BRAND *SocketTablePtr;
  AMD_CPU_BRAND_DATA Data;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  SuffixStatus = 0;
  FamilySpecificServices = NULL;
  SocketTablePtr = NULL;
  SocketTableEntry = NULL;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  // Step1: Allocate 48 bytes from Heap space
  AllocHeapParams.RequestedBufferSize = CPU_BRAND_ID_LENGTH;
  AllocHeapParams.BufferHandle = AMD_BRAND_ID_BUFFER_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
    // Clear NameBuffer
    BrandStringPtr = (CHAR8 *) AllocHeapParams.BufferPtr;
    LibAmdMemFill (BrandStringPtr, 0, CPU_BRAND_ID_LENGTH, StdHeader);
  } else {
    PutEventLog (
      AGESA_ERROR,
      CPU_ERROR_BRANDID_HEAP_NOT_AVAILABLE,
      0, 0, 0, 0, StdHeader
      );
    return;
  }

  // Step2: Get brandid from model number and model string
  LibAmdCpuidRead (AMD_CPUID_FMF, &CpuId, StdHeader);

  // Step3: Figure out Socket/Page/Model/String1/String2/Core Number
  Data.String2 = (UINT8) (CpuId.EBX_Reg & 0x0f);
  Data.Model   = (UINT8) ((CpuId.EBX_Reg >> 4) & 0x7f);
  Data.String1 = (UINT8) ((CpuId.EBX_Reg >> 11) & 0x0f);
  Data.Page    = (UINT8) ((CpuId.EBX_Reg >> 15) & 0x01);
  Data.Socket  = (UINT8) ((CpuId.EBX_Reg >> 28) & 0x0f);
  Data.Cores = FamilySpecificServices->GetNumberOfPhysicalCores (FamilySpecificServices, StdHeader);

  // Step4: If NN = 0, we have an engineering sample, no suffix; then jump to Step6
  if (Data.Model == 0) {
    if (Data.Page == 0) {
      SocketTablePtr = (AMD_CPU_BRAND *)&EngSample_Str;
    } else {
      SocketTablePtr = (AMD_CPU_BRAND *)&TtkSample_Str;
    }
  } else {

    // Model is not equal to zero, so decrement it
    // For family 10 if PkgType[3:0] is greater than or equal to 2h and families >= 12h
    GetLogicalIdOfCurrentCore (&CpuLogicalId, StdHeader);
    if ((((CpuLogicalId.Family & AMD_FAMILY_10) != 0) && (Data.Socket >= DR_SOCKET_S1G3)) ||
        ((CpuLogicalId.Family & AMD_FAMILY_GE_12) != 0)) {
      Data.Model--;
    }

    // Step5: Search for String1 (there can be only 1)
    FamilySpecificServices->GetBrandString1 (FamilySpecificServices, (const VOID **) &SocketTableEntry, &TableEntryCount, StdHeader);
    SocketTableEntry1 = (CPU_BRAND_TABLE **) SocketTableEntry;
    for (TableEntryIndex = 0; ((TableEntryIndex < TableEntryCount)
         && (SuffixStatus == 0)); TableEntryIndex++, SocketTableEntry1++) {
      if (*SocketTableEntry1 == NULL) {
        break;
      }
      SocketTablePtr = (AMD_CPU_BRAND *) (*SocketTableEntry1)->Table;
      TableElements = (*SocketTableEntry1)->NumberOfEntries;
      for (SocketIndex = 0; (SocketIndex < TableElements)
           && SuffixStatus == 0; SocketIndex++) {
        if ((SocketTablePtr->Page == Data.Page) &&
            (SocketTablePtr->Index == Data.String1) &&
            (SocketTablePtr->Socket == Data.Socket) &&
            (SocketTablePtr->Cores == Data.Cores)) {
          SuffixStatus = 1;
        } else {
          SocketTablePtr++;
        }
      }
    }
    if (SuffixStatus == 0) {
      SocketTablePtr = (AMD_CPU_BRAND *)&Dflt_Str1;  // We did not find one, make 'Unknown'
    }
  }

  // Step6: Copy String into NameBuffer
  // We now have data structure pointing to correct type in (*SocketTablePtr)
  LibAmdMemCopy  (BrandStringPtr,
                  (CHAR8 *)SocketTablePtr->Stringstart,
                  SocketTablePtr->Stringlength,
                  StdHeader);

  // Step7: Get suffix, determine addition to BRANDSPEED
  if (SuffixStatus != 0) {
    // Turn our value into a decimal string
    // We have a value like 37d which we need to turn into '3' '7'
    // Divide by 10, store remainder as an ASCII char on stack, repeat until Quotient is 0
    NameStringPtr = BrandStringPtr + SocketTablePtr->Stringlength - 1;
    TempNameCharPtr = NameStringPtr;
    Quotient = Data.Model;
    do {
      Remainder = Quotient % 10;
      Quotient = Quotient / 10;
      *TempNameCharPtr++ = (CHAR8) (Remainder + '0');   // Put suffix into our NameBuffer
    } while (Quotient != 0);
    if (Data.Model < 10) {
      *TempNameCharPtr++ = '0';
    }

    // Step8: Reverse the string sequence and copy into NameBuffer
    SuffixStringPtr = TempNameCharPtr--;
    while (NameStringPtr < TempNameCharPtr) {
      TempChar = *NameStringPtr;
      *NameStringPtr = *TempNameCharPtr;
      *TempNameCharPtr = TempChar;
      NameStringPtr++;
      TempNameCharPtr--;
    }

    // Step9: Search for String2
    SuffixStatus = 0;
    FamilySpecificServices->GetBrandString2 (FamilySpecificServices, (const VOID **) &SocketTableEntry, &TableEntryCount, StdHeader);
    SocketTableEntry1 = (CPU_BRAND_TABLE **) SocketTableEntry;
    for (TableEntryIndex = 0; ((TableEntryIndex < TableEntryCount)
         && (SuffixStatus == 0)); TableEntryIndex++, SocketTableEntry1++) {
      if (*SocketTableEntry1 == NULL) {
        break;
      }
      SocketTablePtr = (AMD_CPU_BRAND *) (*SocketTableEntry1)->Table;
      TableElements = (*SocketTableEntry1)->NumberOfEntries;
      for (SocketIndex = 0; (SocketIndex < TableElements)
           && SuffixStatus == 0; SocketIndex++) {
        if ((SocketTablePtr->Page == Data.Page) &&
            (SocketTablePtr->Index == Data.String2) &&
            (SocketTablePtr->Socket == Data.Socket) &&
            (SocketTablePtr->Cores == Data.Cores)) {
          SuffixStatus = 1;
        } else {
          SocketTablePtr++;
        }
      }
    }
    if (SuffixStatus == 0) {
      SocketTablePtr = (AMD_CPU_BRAND *)&Dflt_Str2;
    }

    // Step10: Copy String2 into our NameBuffer
    if (SocketTablePtr->Stringlength != 0) {
      LibAmdMemCopy (SuffixStringPtr,
                      (CHAR8 *)SocketTablePtr->Stringstart,
                      SocketTablePtr->Stringlength,
                      StdHeader);
    }
  }

  // Step11: Put values into name MSRs,  Always write the full 48 bytes
  MsrNameStringPtrPtr = (UINT64 *) BrandStringPtr;
  for (MsrIndex = MSR_CPUID_NAME_STRING0; MsrIndex <= MSR_CPUID_NAME_STRING5; MsrIndex++) {
    LibAmdMsrWrite (MsrIndex, MsrNameStringPtrPtr, StdHeader);
    MsrNameStringPtrPtr++;
  }
  HeapDeallocateBuffer (AMD_BRAND_ID_BUFFER_HANDLE, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Program BrandID registers (CPUIDNameStringPtr[0-5])
 *
 * This function acts as a wrapper for calling the SetBrandIdRegisters
 * routine at AmdInitEarly.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
SetBrandIdRegistersAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AGESA_TESTPOINT (TpProcCpuSetBrandID, StdHeader);
  SetBrandIdRegisters (StdHeader);
}
