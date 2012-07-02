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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
RDATA_GROUP (G1_PEICC)
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
    FamilySpecificServices->GetBrandString1 (FamilySpecificServices, (CONST VOID **) &SocketTableEntry, &TableEntryCount, StdHeader);
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
    FamilySpecificServices->GetBrandString2 (FamilySpecificServices, (CONST VOID **) &SocketTableEntry, &TableEntryCount, StdHeader);
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
