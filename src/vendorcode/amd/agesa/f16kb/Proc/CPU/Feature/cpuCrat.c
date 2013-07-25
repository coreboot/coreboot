/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CRAT, ACPI table related API functions.
 *
 * Contains code that Create the APCI CRAT Table after early reset.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FEATURE
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
 ***************************************************************************/


/*----------------------------------------------------------------------------
 * This file provides functions, that will generate CRAT tables
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuServices.h"
#include "OptionCrat.h"
#include "cpuCrat.h"
#include "mfCrat.h"
#include "heapManager.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuLateInit.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FEATURE_CPUCRAT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_CRAT_CONFIGURATION OptionCratConfiguration;  // global user config record
extern CPU_FAMILY_SUPPORT_TABLE CratFamilyServiceTable;
extern S_MAKE_CRAT_ENTRY MakeCratEntryTable[];

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GetAcpiCratStub (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
     OUT   VOID                  **CratPtr
  );

AGESA_STATUS
GetAcpiCratMain (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
     OUT   VOID                  **CratPtr
  );

/*----------------------------------------------------------------------------
 *  All of the DATA should be defined in _CODE segment.
 *  Use ROMDATA to specify that it belongs to _CODE.
 *----------------------------------------------------------------------------
 */
CONST UINT8 ROMDATA L2L3Associativity[] =
{
  0,
  1,
  2,
  0,
  4,
  0,
  8,
  0,
  16,
  0,
  32,
  48,
  64,
  96,
  128,
  0xFF
};

STATIC CRAT_HEADER  ROMDATA CratHeaderStruct =
{
  {'C','R','A','T'},
  0,
  1,
  0,
  {0},
  {0},
  1,
  {'A','M','D',' '},
  1,
  0,
  0,
  {0, 0, 0, 0, 0, 0}
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function will generate a complete Component Resource Affinity Table
 * i.e. CRAT into a memory buffer. After completion, this table must be set
 * by the system BIOS into its internal ACPI name space.
 *
 *    @param[in, out] StdHeader         Standard Head Pointer
 *    @param[out]     CratPtr           Point to Crat Struct including buffer address and length
 *
 *    @retval         AGESA_STATUS
 */

AGESA_STATUS
CreateAcpiCrat (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
     OUT   VOID                  **CratPtr
  )
{
  AGESA_TESTPOINT (TpProcCpuEntryCrat, StdHeader);
  return ((*(OptionCratConfiguration.CratFeature)) (StdHeader, CratPtr));
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This is the default routine for use when the CRAT option is NOT requested.
 *
 * The option install process will create and fill the transfer vector with
 * the address of the proper routine (Main or Stub). The link optimizer will
 * strip out of the .DLL the routine that is not used.
 *
 *    @param[in, out] StdHeader      Standard Head Pointer
 *    @param[out]     CratPtr        Point to Crat Struct including buffer address and length
 *
 *    @retval         AGESA_STATUS
 */

AGESA_STATUS
GetAcpiCratStub (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
     OUT   VOID                  **CratPtr
  )
{
  return  AGESA_UNSUPPORTED;
}
/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function will generate a complete Component Resource Affinity Table
 * i.e. CRAT into a memory buffer. After completion, this table must be set
 * by the system BIOS into its internal ACPI name space.
 *
 *    @param[in, out] StdHeader         Standard Head Pointer
 *    @param[out]     CratPtr           Point to Crat Struct including buffer address and length
 *
 *    @retval         AGESA_STATUS
 */
AGESA_STATUS
GetAcpiCratMain (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
     OUT   VOID                  **CratPtr
  )
{
  UINT8     i;
  UINT8     *TableEnd;
  CRAT_HEADER  *CratHeaderStructPtr;
  ALLOCATE_HEAP_PARAMS AllocParams;

  // Allocate a buffer
  AllocParams.RequestedBufferSize = CRAT_MAX_LENGTH;
  AllocParams.BufferHandle = AMD_CRAT_INFO_BUFFER_HANDLE;
  AllocParams.Persist = HEAP_SYSTEM_MEM;

  if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
    return AGESA_ERROR;
  }
  *CratPtr = AllocParams.BufferPtr;


  CratHeaderStructPtr = (CRAT_HEADER *) *CratPtr;
  TableEnd = (UINT8 *) *CratPtr;

  // Copy CratHeaderStruct -> data buffer
  LibAmdMemCopy ((VOID *) CratHeaderStructPtr, (VOID *) &CratHeaderStruct, (UINTN) (sizeof (CRAT_HEADER)), StdHeader);
  // Update table OEM fields.
  LibAmdMemCopy ((VOID *) &CratHeaderStructPtr->OemId, (VOID *) &OptionCratConfiguration.OemIdString,
                 sizeof (OptionCratConfiguration.OemIdString), StdHeader);
  LibAmdMemCopy ((VOID *) &CratHeaderStructPtr->OemTableId, (VOID *) &OptionCratConfiguration.OemTableIdString,
                 sizeof (OptionCratConfiguration.OemTableIdString), StdHeader);

  TableEnd += sizeof (CRAT_HEADER);

  // Make all CRAT entries.
  for (i = 0; MakeCratEntryTable[i].MakeCratEntry != NULL; i++) {
    MakeCratEntryTable[i].MakeCratEntry (CratHeaderStructPtr, &TableEnd, StdHeader);

  }

  // Store size in table (current buffer offset - buffer start offset)
  CratHeaderStructPtr->Length = (UINT32) (TableEnd - (UINT8 *) CratHeaderStructPtr);

  //Update SSDT header Checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) CratHeaderStructPtr, StdHeader);
  IDS_HDT_CONSOLE (CPU_TRACE, "  CRAT is created\n");
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------
 *                            L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------*/
/**
 * This function will add HSA Processing Unit entry.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
MakeHSAProcUnitEntry (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8     NodeNum;
  UINT8     NodeCount;
  UINT32    Socket;
  UINT32    Module;
  UINT32    LowCore;
  UINT32    HighCore;
  UINT32    RegVal;
  CRAT_HSA_PROCESSING_UNIT *EntryPtr;
  AMD_APIC_PARAMS ApicParams;
  PCI_ADDR  PciAddress;

  // Get Node count
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, LOW_NODE_DEVICEID, FUNC_0, 0x60);
  LibAmdPciRead (AccessWidth32 , PciAddress, &RegVal, StdHeader);
  NodeCount = (UINT8) (((RegVal >> 4) & 0x7) + 1);

  NodeNum = 0;
  ApicParams.StdHeader = *StdHeader;
  while (NodeNum < NodeCount) {
    GetSocketModuleOfNode ((UINT32) NodeNum, &Socket, &Module, StdHeader);
    GetGivenModuleCoreRange (Socket, Module, &LowCore, &HighCore, StdHeader);
    ApicParams.Socket = (UINT8) Socket;
    ApicParams.Core = 0;
    AmdGetApicId (&ApicParams);
    if (ApicParams.IsPresent) {
      // Adding one CRAT entry for every node
      EntryPtr = (CRAT_HSA_PROCESSING_UNIT *) AddOneCratEntry (CRAT_TYPE_HSA_PROC_UNIT, CratHeaderStructPtr, TableEnd, StdHeader);

      EntryPtr->ProximityDomain = NodeNum;
      EntryPtr->ProcessorIdLow = ApicParams.ApicAddress;
      EntryPtr->NumCPUCores = (UINT16) (HighCore - LowCore + 1);
      EntryPtr->Flags.Enabled = 1;
      EntryPtr->Flags.CpuPresent = 1;
    }
    /// @todo SimdCount SimdWidth IoCount
    CratHeaderStructPtr->NumDomains++;
    NodeNum++;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * This function will add memory entry.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
MakeMemoryEntry (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8     EntryNum;
  UINT8     NumOfMemAffinityInfoEntries;
  UINT32    Width;
  AGESA_STATUS  AgesaStatus;
  CRAT_MEMORY *EntryPtr;
  LOCATE_HEAP_PTR LocateHeapParams;
  CRAT_MEMORY_AFFINITY_INFO_HEADER *MemAffinityInfoHeaderPtr;
  CRAT_MEMORY_AFFINITY_INFO_ENTRY *MemAffinityInfoEntryPtr;

  EntryNum = 0;

  // Get the CRAT memory affinity info from heap
  LocateHeapParams.BufferHandle = AMD_MEM_CRAT_INFO_BUFFER_HANDLE;
  AgesaStatus = HeapLocateBuffer (&LocateHeapParams, StdHeader);
  if (AgesaStatus != AGESA_SUCCESS) {
    ASSERT (FALSE);
  } else {
    MemAffinityInfoHeaderPtr = (CRAT_MEMORY_AFFINITY_INFO_HEADER *) (LocateHeapParams.BufferPtr);
    MemAffinityInfoHeaderPtr ++;
    MemAffinityInfoEntryPtr  = (CRAT_MEMORY_AFFINITY_INFO_ENTRY *) MemAffinityInfoHeaderPtr;
    MemAffinityInfoHeaderPtr --;

    // Get the number of CRAT memory affinity entries
    NumOfMemAffinityInfoEntries = MemAffinityInfoHeaderPtr->NumOfMemAffinityInfoEntries;
    Width = MemAffinityInfoHeaderPtr->MemoryWidth;

    // Create CRAT memory affinity entries
    IDS_HDT_CONSOLE (MAIN_FLOW, "  CRAT memory affinity entries\n");
    while (EntryNum < NumOfMemAffinityInfoEntries) {
      // Add one CRAT memory entry
      EntryPtr = (CRAT_MEMORY *) AddOneCratEntry (CRAT_TYPE_MEMORY, CratHeaderStructPtr, TableEnd, StdHeader);
      EntryPtr->Flags.Enabled = 1;
      EntryPtr->ProximityDomain = MemAffinityInfoEntryPtr->Domain;
      EntryPtr->BaseAddressLow = MemAffinityInfoEntryPtr->BaseAddressLow;
      EntryPtr->BaseAddressHigh = MemAffinityInfoEntryPtr->BaseAddressHigh;
      EntryPtr->LengthLow = MemAffinityInfoEntryPtr->LengthLow;
      EntryPtr->LengthHigh = MemAffinityInfoEntryPtr->LengthHigh;
      EntryPtr->Width = Width;
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Entry #%d\n", EntryNum);
      IDS_HDT_CONSOLE (MAIN_FLOW, "      Type:              0x%08x\n", EntryPtr->Type);
      IDS_HDT_CONSOLE (MAIN_FLOW, "      Length:            0x%08x\n", EntryPtr->Length);
      IDS_HDT_CONSOLE (MAIN_FLOW, "      Flags:             %s %s %s\n", (EntryPtr->Flags.Enabled == 1) ? "Enabled" : "",
                                                                         (EntryPtr->Flags.HotPluggable == 1) ? "EnHotPluggableabled" : "",
                                                                         (EntryPtr->Flags.NonVolatile == 1) ? "NonVolatile" : ""
                                                                         );
      IDS_HDT_CONSOLE (MAIN_FLOW, "      Proximity Domain:  0x%08x\n", EntryPtr->ProximityDomain);
      IDS_HDT_CONSOLE (MAIN_FLOW, "      Base Address Low:  0x%08x\n", EntryPtr->BaseAddressLow);
      IDS_HDT_CONSOLE (MAIN_FLOW, "      Base Address High: 0x%08x\n", EntryPtr->BaseAddressHigh);
      IDS_HDT_CONSOLE (MAIN_FLOW, "      Length Low:        0x%08x\n", EntryPtr->LengthLow);
      IDS_HDT_CONSOLE (MAIN_FLOW, "      Length High:       0x%08x\n", EntryPtr->LengthHigh);
      IDS_HDT_CONSOLE (MAIN_FLOW, "      Width:             0x%08x\n", EntryPtr->Width);
      MemAffinityInfoEntryPtr ++;
      EntryNum ++;
    }
    HeapDeallocateBuffer ((UINT32) AMD_MEM_CRAT_INFO_BUFFER_HANDLE, StdHeader);
  }

  return;
}

/*---------------------------------------------------------------------------------------*/
/**
 * This function will add cache entry.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
MakeCacheEntry (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  CRAT_FAMILY_SERVICES  *CratFamilyServices;

  GetFeatureServicesOfSocket (&CratFamilyServiceTable, 0, (CONST VOID **)&CratFamilyServices, StdHeader);
  CratFamilyServices->generateCratCacheEntry (CratHeaderStructPtr, TableEnd, StdHeader);
  return;
}

/*---------------------------------------------------------------------------------------*/
/**
 * This function will add TLB entry.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
MakeTLBEntry (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  CRAT_FAMILY_SERVICES  *CratFamilyServices;

  GetFeatureServicesOfSocket (&CratFamilyServiceTable, 0, (CONST VOID **)&CratFamilyServices, StdHeader);
  CratFamilyServices->generateCratTLBEntry (CratHeaderStructPtr, TableEnd, StdHeader);
  return;
}

/*---------------------------------------------------------------------------------------*/
/**
 * This function will add CRAT entry.
 *
 *    @param[in]      CratEntryType        CRAT entry type
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
UINT8 *
AddOneCratEntry (
  IN       CRAT_ENTRY_TYPE CratEntryType,
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 *CurrentEntry;

  CurrentEntry = *TableEnd;
  CratHeaderStructPtr->TotalEntries++;
  switch (CratEntryType) {
  case CRAT_TYPE_HSA_PROC_UNIT:
    *TableEnd += sizeof (CRAT_HSA_PROCESSING_UNIT);
    ASSERT ((*TableEnd - (UINT8 *) CratHeaderStructPtr) <= CRAT_MAX_LENGTH);
    CratHeaderStructPtr->Length += sizeof (CRAT_HSA_PROCESSING_UNIT);
    ((CRAT_HSA_PROCESSING_UNIT *) CurrentEntry)->Type = (UINT8) CratEntryType;
    ((CRAT_HSA_PROCESSING_UNIT *) CurrentEntry)->Length = sizeof (CRAT_HSA_PROCESSING_UNIT);
    break;
  case CRAT_TYPE_MEMORY:
    *TableEnd += sizeof (CRAT_MEMORY);
    ASSERT ((*TableEnd - (UINT8 *) CratHeaderStructPtr) <= CRAT_MAX_LENGTH);
    CratHeaderStructPtr->Length += sizeof (CRAT_MEMORY);
    ((CRAT_MEMORY *) CurrentEntry)->Type = (UINT8) CratEntryType;
    ((CRAT_MEMORY *) CurrentEntry)->Length = sizeof (CRAT_MEMORY);
    break;
  case CRAT_TYPE_CACHE:
    *TableEnd += sizeof (CRAT_CACHE);
    ASSERT ((*TableEnd - (UINT8 *) CratHeaderStructPtr) <= CRAT_MAX_LENGTH);
    CratHeaderStructPtr->Length += sizeof (CRAT_CACHE);
    ((CRAT_CACHE *) CurrentEntry)->Type = (UINT8) CratEntryType;
    ((CRAT_CACHE *) CurrentEntry)->Length = sizeof (CRAT_CACHE);
    break;
  case CRAT_TYPE_TLB:
    *TableEnd += sizeof (CRAT_TLB);
    ASSERT ((*TableEnd - (UINT8 *) CratHeaderStructPtr) <= CRAT_MAX_LENGTH);
    CratHeaderStructPtr->Length += sizeof (CRAT_TLB);
    ((CRAT_TLB *) CurrentEntry)->Type = (UINT8) CratEntryType;
    ((CRAT_TLB *) CurrentEntry)->Length = sizeof (CRAT_TLB);
    break;
  case CRAT_TYPE_FPU:
    *TableEnd += sizeof (CRAT_FPU);
    ASSERT ((*TableEnd - (UINT8 *) CratHeaderStructPtr) <= CRAT_MAX_LENGTH);
    CratHeaderStructPtr->Length += sizeof (CRAT_FPU);
    ((CRAT_FPU *) CurrentEntry)->Type = (UINT8) CratEntryType;
    ((CRAT_FPU *) CurrentEntry)->Length = sizeof (CRAT_FPU);
    break;
  case CRAT_TYPE_IO:
    *TableEnd += sizeof (CRAT_IO);
    ASSERT ((*TableEnd - (UINT8 *) CratHeaderStructPtr) <= CRAT_MAX_LENGTH);
    CratHeaderStructPtr->Length += sizeof (CRAT_IO);
    ((CRAT_IO *) CurrentEntry)->Type = (UINT8) CratEntryType;
    ((CRAT_IO *) CurrentEntry)->Length = sizeof (CRAT_IO);
    break;
  default:
    ASSERT (FALSE);
    break;
  }
  return CurrentEntry;
}

