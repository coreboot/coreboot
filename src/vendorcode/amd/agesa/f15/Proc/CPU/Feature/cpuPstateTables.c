/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD PSTATE, ACPI table related API functions.
 *
 * Contains code that generates the _PSS, _PCT, and other ACPI tables.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 55600 $   @e \$Date: 2011-06-23 12:39:18 -0600 (Thu, 23 Jun 2011) $
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


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "OptionPstate.h"
#include "cpuLateInit.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "heapManager.h"
#include "Ids.h"
#include "Filecode.h"
#include "GeneralServices.h"
#include "cpuPstateTables.h"
#include "cpuFeatures.h"
#include "cpuIoCstate.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FEATURE_CPUPSTATETABLES_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
extern OPTION_PSTATE_LATE_CONFIGURATION    OptionPstateLateConfiguration;  // global user config record
extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;
extern CPU_FAMILY_SUPPORT_TABLE            IoCstateFamilyServiceTable;

STATIC ACPI_TABLE_HEADER  ROMDATA CpuSsdtHdrStruct =
{
  {'S','S','D','T'},
  0,
  1,
  0,
  {'A','M','D',' ',' ',' '},
  {'P','O','W','E','R','N','O','W'},
  1,
  {'A','M','D',' '},
  1
};


/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
UINT32
STATIC
CalAcpiTablesSize (
  IN       S_CPU_AMD_PSTATE       *AmdPstatePtr,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

AGESA_STATUS
GenerateSsdtStub (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SsdtPtr
  );

UINT32
CreateAcpiTablesStub (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PSTATE_LEVELING        *PStateLevelingBuffer,
  IN OUT   VOID                   **SsdtPtr,
  IN       UINT8                  LocalApicId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

UINT32
CreatePStateAcpiTables (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PSTATE_LEVELING        *PStateLevelingBuffer,
  IN OUT   VOID                   **SsdtPtr,
  IN       UINT8                  LocalApicId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

UINT32
CreateCStateAcpiTables (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PSTATE_LEVELING        *PStateLevelingBuffer,
  IN OUT   VOID                   **SsdtPtr,
  IN       UINT8                  LocalApicId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

AGESA_STATUS
GenerateSsdt (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SsdtPtr
  );

/**
 *---------------------------------------------------------------------------------------
 *
 *  CalAcpiTablesSize
 *
 *  Description:
 *    This function will calculate the size of ACPI PState tables
 *
 *  Parameters:
 *    @param[in]     *AmdPstatePtr
 *    @param[in]     *PlatformConfig
 *    @param[in]     *StdHeader
 *
 *    @retval      UINT32
 *
 *---------------------------------------------------------------------------------------
 */
UINT32
STATIC
CalAcpiTablesSize (
  IN       S_CPU_AMD_PSTATE       *AmdPstatePtr,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                    ScopeSize;
  UINT32                    CoreCount;
  UINT32                    SocketCount;
  UINT32                    MaxCoreNumberInCurrentSocket;
  UINT32                    MaxSocketNumberInSystem;
  UINT32                    MaxPstateNumberInCurrentCore;
  UINT32                    CstateAcpiObjSize;
  PSTATE_LEVELING           *PStateLevelingBufferStructPtr;
  IO_CSTATE_FAMILY_SERVICES *IoCstateFamilyServices;

  ScopeSize = sizeof (ACPI_TABLE_HEADER);
  CstateAcpiObjSize = 0;
  IoCstateFamilyServices = NULL;

  PStateLevelingBufferStructPtr = AmdPstatePtr->PStateLevelingStruc;
  MaxSocketNumberInSystem = AmdPstatePtr->TotalSocketInSystem;

  if (IsFeatureEnabled (IoCstate, PlatformConfig, StdHeader)) {
    GetFeatureServicesOfCurrentCore (&IoCstateFamilyServiceTable, (CONST VOID **)&IoCstateFamilyServices, StdHeader);
    // If we're supporting multiple families, only proceed when IO Cstate family services are available
    if (IoCstateFamilyServices != NULL) {
      CstateAcpiObjSize = IoCstateFamilyServices->GetAcpiCstObj (IoCstateFamilyServices, PlatformConfig, StdHeader);
    }
  }

  for (SocketCount = 0; SocketCount < MaxSocketNumberInSystem; SocketCount++) {
    MaxCoreNumberInCurrentSocket = PStateLevelingBufferStructPtr->TotalCoresInNode;
    for (CoreCount = 0; CoreCount < MaxCoreNumberInCurrentSocket; CoreCount++) {
      MaxPstateNumberInCurrentCore = PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateMaxValue + 1;

      ScopeSize += (SCOPE_STRUCT_SIZE - 1); // Scope size per core
      ScopeSize += CstateAcpiObjSize;       // C-State ACPI objects size per core

      // Add P-State ACPI Objects size per core
      if ((PStateLevelingBufferStructPtr[0].CreateAcpiTables != 0) && (PlatformConfig->UserOptionPState)) {
        ScopeSize += (PCT_STRUCT_SIZE +
                     PSS_HEADER_STRUCT_SIZE +
                     (MaxPstateNumberInCurrentCore * PSS_BODY_STRUCT_SIZE) +
                     XPSS_HEADER_STRUCT_SIZE +
                     (MaxPstateNumberInCurrentCore * XPSS_BODY_STRUCT_SIZE) +
                     PSD_HEADER_STRUCT_SIZE +
                     PSD_BODY_STRUCT_SIZE +
                     PPC_HEADER_BODY_STRUCT_SIZE);
      }
    }
    ScopeSize += MaxCoreNumberInCurrentSocket;
    PStateLevelingBufferStructPtr = (PSTATE_LEVELING *) ((UINT8 *) PStateLevelingBufferStructPtr + (UINTN) sizeof (PSTATE_LEVELING) + (UINTN) (PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateMaxValue * sizeof (S_PSTATE_VALUES)));
  }
  AmdPstatePtr->SizeOfBytes = ScopeSize;

  return ScopeSize;
}

/**--------------------------------------------------------------------------------------
 *
 *  GenerateSsdtStub
 *
 *  Description:
 *     This is the default routine for use when both PState and CState option is NOT
 *     requested. The option install process will create and fill the transfer vector
 *     with the address of the proper routine (Main or Stub). The link optimizer will
 *     strip out of the .DLL the routine that is not used.
 *
 *  Parameters:
 *    @param[in]       StdHeader          Handle to config for library and services
 *    @param[in]       PlatformConfig     Contains the power cap parameter
 *    @param[in,out]   SsdtPtr            ACPI SSDT table pointer
 *
 *    @retval          AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
GenerateSsdtStub (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SsdtPtr
  )
{
  return  AGESA_UNSUPPORTED;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  GenerateSsdt
 *
 *  Description:
 *    This function will populate the SSDT with ACPI P-States and C-States Objects, whenever
 *    necessary
 *    This function should be called only from BSP
 *
 *  Parameters:
 *    @param[in]       StdHeader          Handle to config for library and services
 *    @param[in]       PlatformConfig     Contains the power cap parameter
 *    @param[in,out]   SsdtPtr            ACPI SSDT pointer
 *
 *    @retval          AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
GenerateSsdt (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SsdtPtr
  )
{
  UINT32                 i;
  UINT32                 j;
  UINT32                 TempVar8_a;
  UINT32                 CurrSize;
  UINT32                 TempVar_a;
  UINT32                 TempVar_b;
  UINT32                 ScopeSize;
  UINT32                 CoreCount;
  UINT32                 SocketCount;
  UINT32                 MaxCorePerNode;
  UINT8                  LocalApicId;
  UINT8                  *IntermediatePtr;
  AGESA_STATUS           AgesaStatus;
  LOCATE_HEAP_PTR        LocateHeapParams;
  ALLOCATE_HEAP_PARAMS   AllocateHeapParams;
  S_CPU_AMD_PSTATE       *AmdPstatePtr;
  PSTATE_LEVELING        *PStateLevelingBufferStructPtr;
  SCOPE                  *ScopeAcpiTablesStructPtr;
  SCOPE                  *ScopeAcpiTablesStructPtrTemp;

  AGESA_TESTPOINT (TpProcCpuEntryPstate, StdHeader);

  ASSERT (IsBsp (StdHeader, &AgesaStatus));

  // If P-State and C-State ACPI tables do not need to be generated, exit this routine
  if ((!PlatformConfig->UserOptionPState) && (!IsFeatureEnabled (IoCstate, PlatformConfig, StdHeader))) {
    AgesaStatus = AGESA_UNSUPPORTED;
    return AgesaStatus;
  }

  // Initialize data variables
  ScopeSize   = 0;
  CoreCount   = 0;
  LocalApicId = 0;
  CurrSize    = 0;

  // Locate P-State data buffer
  LocateHeapParams.BufferHandle = AMD_PSTATE_DATA_BUFFER_HANDLE;
  AGESA_TESTPOINT (TpProcCpuBeforeLocateSsdtBuffer, StdHeader);
  if (HeapLocateBuffer (&LocateHeapParams, StdHeader) != AGESA_SUCCESS) {
    return AGESA_ERROR;
  }
  AGESA_TESTPOINT (TpProcCpuAfterLocateSsdtBuffer, StdHeader);

  AmdPstatePtr = (S_CPU_AMD_PSTATE *) LocateHeapParams.BufferPtr;
  PStateLevelingBufferStructPtr = AmdPstatePtr->PStateLevelingStruc;

  // Allocate rough buffer for AcpiTable, if SsdtPtr is NULL
  if (*SsdtPtr == NULL) {
    //Do not know the actual size.. pre-calculate it.
    AllocateHeapParams.RequestedBufferSize = CalAcpiTablesSize (AmdPstatePtr, PlatformConfig, StdHeader);
    AllocateHeapParams.BufferHandle = AMD_PSTATE_ACPI_BUFFER_HANDLE;
    AllocateHeapParams.Persist = HEAP_SYSTEM_MEM;

    AGESA_TESTPOINT (TpProcCpuBeforeAllocateSsdtBuffer, StdHeader);
    if (HeapAllocateBuffer (&AllocateHeapParams, StdHeader) != AGESA_SUCCESS) {
      return AGESA_ERROR;
    }
    AGESA_TESTPOINT (TpProcCpuAfterAllocateSsdtBuffer, StdHeader);
    *SsdtPtr = AllocateHeapParams.BufferPtr;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  SSDT is created\n");

  // Copy SSDT header into allocated buffer
  LibAmdMemCopy (*SsdtPtr, (VOID *) &CpuSsdtHdrStruct, (UINTN) (sizeof (ACPI_TABLE_HEADER)), StdHeader);
  IntermediatePtr = (UINT8 *) *SsdtPtr;
  ScopeAcpiTablesStructPtr = (SCOPE *) &IntermediatePtr[sizeof (ACPI_TABLE_HEADER)];

  SocketCount = AmdPstatePtr->TotalSocketInSystem;

  // Generate name scope and ACPI objects for every core in the system
  for (i = 0; i < SocketCount; i++) {
    MaxCorePerNode = PStateLevelingBufferStructPtr->TotalCoresInNode;
    for (j = 0; j < MaxCorePerNode; j++) {
      CoreCount++;
      // Set Name Scope for CPU0, 1, 2, ..... n
      // CPU0 to CPUn will name  as  C000 to Cnnn
      // -----------------------------------------
      ScopeAcpiTablesStructPtr->ScopeOpcode      = SCOPE_OPCODE;
      // This value will be filled at the end of this function
      // Since at this time, we don't know how many  Pstates we
      // would have
      ScopeAcpiTablesStructPtr->ScopeLength      = 0;
      ScopeAcpiTablesStructPtr->ScopeValue1      = SCOPE_VALUE1;
      ScopeAcpiTablesStructPtr->ScopeValue2      = SCOPE_VALUE2;
      ScopeAcpiTablesStructPtr->ScopeNamePt1a__  = SCOPE_NAME__;
      if (PlatformConfig->ProcessorScopeInSb) {
        ScopeAcpiTablesStructPtr->ScopeNamePt1a_P  = SCOPE_NAME_S;
        ScopeAcpiTablesStructPtr->ScopeNamePt1a_R  = SCOPE_NAME_B;
      } else {
        ScopeAcpiTablesStructPtr->ScopeNamePt1a_P  = SCOPE_NAME_P;
        ScopeAcpiTablesStructPtr->ScopeNamePt1a_R  = SCOPE_NAME_R;
      }
      ScopeAcpiTablesStructPtr->ScopeNamePt1b__  = SCOPE_NAME__;
      ASSERT ((PlatformConfig->ProcessorScopeName0 >= 'A') && (PlatformConfig->ProcessorScopeName0 <= 'Z'))
      ASSERT (((PlatformConfig->ProcessorScopeName1 >= 'A') && (PlatformConfig->ProcessorScopeName1 <= 'Z')) || \
              ((PlatformConfig->ProcessorScopeName1 >= '0') && (PlatformConfig->ProcessorScopeName1 <= '9')) || \
              (PlatformConfig->ProcessorScopeName1 == '_'))

      ScopeAcpiTablesStructPtr->ScopeNamePt2a_C  = PlatformConfig->ProcessorScopeName0;
      ScopeAcpiTablesStructPtr->ScopeNamePt2a_P  = PlatformConfig->ProcessorScopeName1;

      TempVar8_a = ((CoreCount - 1) >> 4) & 0x0F;
      ScopeAcpiTablesStructPtr->ScopeNamePt2a_U  = (UINT8) (SCOPE_NAME_0 + TempVar8_a);

      TempVar8_a = (CoreCount - 1) & 0x0F;
      if (TempVar8_a < 0xA) {
        ScopeAcpiTablesStructPtr->ScopeNamePt2a_0  = (UINT8) (SCOPE_NAME_0 + TempVar8_a);
      } else {
        ScopeAcpiTablesStructPtr->ScopeNamePt2a_0  = (UINT8) (SCOPE_NAME_A + TempVar8_a - 0xA);
      }
      // Increment and typecast the pointer
      ScopeAcpiTablesStructPtrTemp = ScopeAcpiTablesStructPtr;
      ScopeAcpiTablesStructPtrTemp++;

      // Get the Local Apic Id for each core
      LocalApicId = PStateLevelingBufferStructPtr->PStateCoreStruct[0].LocalApicId + (UINT8) j;

      // Create P-State ACPI Objects
      CurrSize += ((*(OptionPstateLateConfiguration.PstateFeature)) (PlatformConfig, PStateLevelingBufferStructPtr, (VOID *) &ScopeAcpiTablesStructPtrTemp, LocalApicId, StdHeader));

      // Create C-State ACPI Objects
      CurrSize += ((*(OptionPstateLateConfiguration.CstateFeature)) (PlatformConfig, PStateLevelingBufferStructPtr, (VOID *) &ScopeAcpiTablesStructPtrTemp, LocalApicId, StdHeader));

      // Now update the SCOPE Length field
      {
        CurrSize += (SCOPE_STRUCT_SIZE - 1);
        ScopeSize += CurrSize;

        TempVar_b = ((CurrSize << 4) & 0x0000FF00);
        TempVar_b |= ((CurrSize & 0x0000000F) | 0x00000040);
        TempVar_a = TempVar_b;
        ScopeAcpiTablesStructPtr->ScopeLength = (UINT16) TempVar_a;
        CurrSize = 0;
      }

      ScopeAcpiTablesStructPtr = ScopeAcpiTablesStructPtrTemp;
    }
    //Calculate next node buffer address
    PStateLevelingBufferStructPtr = (PSTATE_LEVELING *) ((UINT8 *) PStateLevelingBufferStructPtr + (UINTN) sizeof (PSTATE_LEVELING) + (UINTN) (PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateMaxValue * sizeof (S_PSTATE_VALUES)));
  }
  //Update SSDT header Checksum
  ((ACPI_TABLE_HEADER *) *SsdtPtr)->TableLength = (ScopeSize + CoreCount + sizeof (ACPI_TABLE_HEADER));
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) *SsdtPtr, StdHeader);

  return AGESA_SUCCESS;
}

/**--------------------------------------------------------------------------------------
 *
 *  CreateAcpiTablesStub
 *
 *  Description:
 *     This is the default routine for use when the P-State or C-State option is NOT
 *     requested. The option install process will create and fill the transfer vector
 *     with the address of the proper routine (Main or Stub). The link optimizer will
 *     strip out of the .DLL the routine that is not used.
 *
 *  Parameters:
 *    @param[in]     PlatformConfig       Platform operational characteristics; power cap
 *    @param[in]     PStateLevelingBuffer Buffer that contains P-State Leveling information
 *    @param[in,out] SsdtPtr              ACPI SSDT table pointer
 *    @param[in]     LocalApicId          Local Apic Id
 *    @param[in]     StdHeader            Handle to config for library and services
 *
 *    @retval        Size of generated ACPI objects
 *
 *---------------------------------------------------------------------------------------
 **/
UINT32
CreateAcpiTablesStub (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PSTATE_LEVELING        *PStateLevelingBuffer,
  IN OUT   VOID                   **SsdtPtr,
  IN       UINT8                  LocalApicId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  return  0;
}


/**--------------------------------------------------------------------------------------
 *
 *  CreatePStateAcpiTables
 *
 *  Description:
 *     This is the common routine for creating ACPI P-State objects
 *
 *  Parameters:
 *    @param[in]     PlatformConfig       Platform operational characteristics; power cap
 *    @param[in]     PStateLevelingBuffer Buffer that contains P-State Leveling information
 *    @param[in,out] SsdtPtr              ACPI SSDT table pointer
 *    @param[in]     LocalApicId          Local Apic Id
 *    @param[in]     StdHeader            Handle to config for library and services
 *
 *    @retval          Size of generated ACPI P-States objects
 *
 *---------------------------------------------------------------------------------------
 **/
UINT32
CreatePStateAcpiTables (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PSTATE_LEVELING        *PStateLevelingBuffer,
  IN OUT   VOID                   **SsdtPtr,
  IN       UINT8                  LocalApicId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT8                      PstateCapLevelSupport;
  UINT8                      PStateMaxValueOnCurrentCore;
  BOOLEAN                    PstateCapEnable;
  BOOLEAN                    PstateCapLevelSupportDetermined;
  BOOLEAN                    IsPsdDependent;
  UINT32                     k;
  UINT32                     TempVar_a;
  UINT32                     TempVar_b;
  UINT32                     TempVar_c;
  UINT32                     PstateCapInputMilliWatts;
  UINT32                     CurrSize;
  UINT32                     PstateCount;
  UINT32                     CoreCount1;
  UINT32                     TransAndBusMastLatency;
  AGESA_STATUS               IgnoredStatus;
  PCI_ADDR                   PciAddress;
  PCT_HEADER_BODY            *pPctAcpiTables;
  PSS_HEADER                 *pPssHeaderAcpiTables;
  PSS_BODY                   *pPssBodyAcpiTables;
  XPSS_HEADER                *pXpssHeaderAcpiTables;
  XPSS_BODY                  *pXpssBodyAcpiTables;
  PSD_HEADER                 *pPsdHeaderAcpiTables;
  PSD_BODY                   *pPsdBodyAcpiTables;
  PPC_HEADER_BODY            *pPpcAcpiTables;
  PSTATE_CPU_FAMILY_SERVICES *FamilyServices;

  CurrSize = 0;
  PstateCount = 0;
  PstateCapEnable = FALSE;
  PstateCapLevelSupport = DEFAULT_PERF_PRESENT_CAP;
  PstateCapLevelSupportDetermined = TRUE;
  PstateCapInputMilliWatts = PlatformConfig->PowerCeiling;
  IsPsdDependent = !(PlatformConfig->ForcePstateIndependent);
  TransAndBusMastLatency = 0;

  if ((PStateLevelingBuffer[0].CreateAcpiTables != 0) && (PlatformConfig->UserOptionPState)) {
    pPctAcpiTables = (PCT_HEADER_BODY *) *SsdtPtr;

    //Check Pstate Capability
    if (PstateCapInputMilliWatts != 0) {
      PstateCapEnable = TRUE;
      PstateCapLevelSupportDetermined = FALSE;
    }

    PStateMaxValueOnCurrentCore = PStateLevelingBuffer->PStateCoreStruct[0].PStateMaxValue;
    if (OptionPstateLateConfiguration.CfgPstatePct) {
      // Set _PCT Table
      // --------------
      pPctAcpiTables->NameOpcode              = NAME_OPCODE;
      pPctAcpiTables->PctName_a__             = PCT_NAME__;
      pPctAcpiTables->PctName_a_P             = PCT_NAME_P;
      pPctAcpiTables->PctName_a_C             = PCT_NAME_C;
      pPctAcpiTables->PctName_a_T             = PCT_NAME_T;
      pPctAcpiTables->Value1                  = PCT_VALUE1;
      pPctAcpiTables->Value2                  = PCT_VALUE2;
      pPctAcpiTables->Value3                  = PCT_VALUE3;
      pPctAcpiTables->GenericRegDescription1  = GENERIC_REG_DESCRIPTION;
      pPctAcpiTables->Length1                 = PCT_LENGTH;
      pPctAcpiTables->AddressSpaceId1         = PCT_ADDRESS_SPACE_ID;
      pPctAcpiTables->RegisterBitWidth1       = PCT_REGISTER_BIT_WIDTH;
      pPctAcpiTables->RegisterBitOffset1      = PCT_REGISTER_BIT_OFFSET;
      pPctAcpiTables->Reserved1               = PCT_RESERVED;
      pPctAcpiTables->ControlRegAddressLo     = PCT_CONTROL_REG_LO;
      pPctAcpiTables->ControlRegAddressHi     = PCT_CONTROL_REG_HI;
      pPctAcpiTables->Value4                  = PCT_VALUE4;
      pPctAcpiTables->Value5                  = PCT_VALUE5;
      pPctAcpiTables->GenericRegDescription2  = GENERIC_REG_DESCRIPTION;
      pPctAcpiTables->Length2                 = PCT_LENGTH;
      pPctAcpiTables->AddressSpaceId2         = PCT_ADDRESS_SPACE_ID;
      pPctAcpiTables->RegisterBitWidth2       = PCT_REGISTER_BIT_WIDTH;
      pPctAcpiTables->RegisterBitOffset2      = PCT_REGISTER_BIT_OFFSET;
      pPctAcpiTables->Reserved2               = PCT_RESERVED;
      pPctAcpiTables->StatusRegAddressLo      = PCT_STATUS_REG_LO;
      pPctAcpiTables->StatusRegAddressHi      = PCT_STATUS_REG_HI;
      pPctAcpiTables->Value6                  = PCT_VALUE6;

      // Increment and then typecast the pointer
      pPctAcpiTables++;
      CurrSize += PCT_STRUCT_SIZE;

      *SsdtPtr = pPctAcpiTables;
    } // end of OptionPstateLateConfiguration.CfgPstatePct

    pPssHeaderAcpiTables = (PSS_HEADER *) pPctAcpiTables;
    pPssBodyAcpiTables   = (PSS_BODY *)   pPctAcpiTables;
    if (OptionPstateLateConfiguration.CfgPstatePss) {
      // Set _PSS Header
      // Note: Set pssLength and numOfItemsInPss later
      //---------------------------------------------------
      pPssHeaderAcpiTables->NameOpcode      = NAME_OPCODE;
      pPssHeaderAcpiTables->PssName_a__     = PSS_NAME__;
      pPssHeaderAcpiTables->PssName_a_P     = PSS_NAME_P;
      pPssHeaderAcpiTables->PssName_a_S     = PSS_NAME_S;
      pPssHeaderAcpiTables->PssName_b_S     = PSS_NAME_S;
      pPssHeaderAcpiTables->PkgOpcode       = PACKAGE_OPCODE;

      pPssHeaderAcpiTables++;
      pPssBodyAcpiTables = (PSS_BODY *) pPssHeaderAcpiTables;
      // Restore the pPssHeaderAcpiTables
      pPssHeaderAcpiTables--;

      // Set _PSS Body
      //---------------
      PstateCount = 0;

      // Calculate PCI address for socket only
      GetPciAddress (StdHeader, (UINT32) PStateLevelingBuffer->SocketNumber, 0, &PciAddress, &IgnoredStatus);
      TransAndBusMastLatency = 0;
      GetFeatureServicesOfSocket (&PstateFamilyServiceTable, (UINT32) PStateLevelingBuffer->SocketNumber, (CONST VOID **)&FamilyServices, StdHeader);
      ASSERT (FamilyServices != NULL)
      FamilyServices->GetPstateLatency (  FamilyServices,
                                          PStateLevelingBuffer,
                                          &PciAddress,
                                          &TransAndBusMastLatency,
                                          StdHeader);

      for (k = 0; k <= PStateMaxValueOnCurrentCore; k++) {
        if (PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].PStateEnable != 0) {
          pPssBodyAcpiTables->PkgOpcode           = PACKAGE_OPCODE;
          pPssBodyAcpiTables->PkgLength           = PSS_PKG_LENGTH;
          pPssBodyAcpiTables->NumOfElements       = PSS_NUM_OF_ELEMENTS;
          pPssBodyAcpiTables->DwordPrefixOpcode1  = DWORD_PREFIX_OPCODE;
          pPssBodyAcpiTables->Frequency           =
          PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].CoreFreq;
          pPssBodyAcpiTables->DwordPrefixOpcode2  = DWORD_PREFIX_OPCODE;
          pPssBodyAcpiTables->Power               =
          PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].Power;

          if (PstateCapEnable && (!PstateCapLevelSupportDetermined) && (PstateCapInputMilliWatts >= pPssBodyAcpiTables->Power)) {
            PstateCapLevelSupport = (UINT8) PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].SwPstateNumber;
            PstateCapLevelSupportDetermined = TRUE;
          }

          pPssBodyAcpiTables->DwordPrefixOpcode3  = DWORD_PREFIX_OPCODE;
          pPssBodyAcpiTables->TransitionLatency   = TransAndBusMastLatency;
          pPssBodyAcpiTables->DwordPrefixOpcode4  = DWORD_PREFIX_OPCODE;
          pPssBodyAcpiTables->BusMasterLatency    = TransAndBusMastLatency;
          pPssBodyAcpiTables->DwordPrefixOpcode5  = DWORD_PREFIX_OPCODE;
          pPssBodyAcpiTables->Control             =
          PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].SwPstateNumber;
          pPssBodyAcpiTables->DwordPrefixOpcode6  = DWORD_PREFIX_OPCODE;
          pPssBodyAcpiTables->Status              =
          PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].SwPstateNumber;

          pPssBodyAcpiTables++;
          PstateCount++;
        }
      } // for (k = 0; k < MPPSTATE_MAXIMUM_STATES; k++)

      if (PstateCapEnable && (!PstateCapLevelSupportDetermined)) {
        PstateCapLevelSupport = PStateMaxValueOnCurrentCore;
      }

      // Set _PSS Header again
      // Now Set pssLength and numOfItemsInPss
      //---------------------------------------
      TempVar_a = (PstateCount * PSS_BODY_STRUCT_SIZE) + 3;
      TempVar_b = TempVar_a;
      TempVar_c = ((TempVar_b << 4) & 0x0000FF00);
      TempVar_c = TempVar_c | ((TempVar_b & 0x0000000F) | 0x00000040);
      TempVar_a = (UINT16) TempVar_c;

      pPssHeaderAcpiTables->PssLength = (UINT16) TempVar_a;
      pPssHeaderAcpiTables->NumOfItemsInPss = (UINT8) PstateCount;
      CurrSize += (PSS_HEADER_STRUCT_SIZE + (PstateCount * PSS_BODY_STRUCT_SIZE));

      *SsdtPtr = pPssBodyAcpiTables;
    } // end of PSS Body if OptionPstateLateConfiguration.CfgPstatePss

    // Set XPSS Table
    //---------------
    // Typecast the pointer
    pXpssHeaderAcpiTables = (XPSS_HEADER *) pPssBodyAcpiTables;
    pXpssBodyAcpiTables   = (XPSS_BODY *)   pPssBodyAcpiTables;
    if (OptionPstateLateConfiguration.CfgPstateXpss) {
      // Set XPSS Header
      // Note: Set the pssLength and numOfItemsInPss later
      //---------------------------------------------------
      pXpssHeaderAcpiTables->NameOpcode       = NAME_OPCODE;
      pXpssHeaderAcpiTables->XpssName_a_X     = PSS_NAME_X;
      pXpssHeaderAcpiTables->XpssName_a_P     = PSS_NAME_P;
      pXpssHeaderAcpiTables->XpssName_a_S     = PSS_NAME_S;
      pXpssHeaderAcpiTables->XpssName_b_S     = PSS_NAME_S;
      pXpssHeaderAcpiTables->PkgOpcode        = PACKAGE_OPCODE;

      // Increment and then typecast the pointer
      pXpssHeaderAcpiTables++;
      pXpssBodyAcpiTables = (XPSS_BODY *) pXpssHeaderAcpiTables;
      // Restore the pXpssHeaderAcpiTables
      pXpssHeaderAcpiTables--;

      // Set XPSS Body
      //---------------
      for (k = 0; k <= PStateMaxValueOnCurrentCore; k++) {
        if (PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].PStateEnable != 0) {
          pXpssBodyAcpiTables->PkgOpcode          = PACKAGE_OPCODE;
          pXpssBodyAcpiTables->PkgLength          = XPSS_PKG_LENGTH;
          pXpssBodyAcpiTables->NumOfElements      = XPSS_NUM_OF_ELEMENTS;
          pXpssBodyAcpiTables->XpssValueTbd       = 04;
          pXpssBodyAcpiTables->DwordPrefixOpcode1 = DWORD_PREFIX_OPCODE;
          pXpssBodyAcpiTables->Frequency          =
          PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].CoreFreq;
          pXpssBodyAcpiTables->DwordPrefixOpcode2 = DWORD_PREFIX_OPCODE;
          pXpssBodyAcpiTables->Power              =
          PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].Power;
          pXpssBodyAcpiTables->DwordPrefixOpcode3 = DWORD_PREFIX_OPCODE;
          pXpssBodyAcpiTables->TransitionLatency  = TransAndBusMastLatency;
          pXpssBodyAcpiTables->DwordPrefixOpcode4 = DWORD_PREFIX_OPCODE;
          pXpssBodyAcpiTables->BusMasterLatency   = TransAndBusMastLatency;
          pXpssBodyAcpiTables->ControlBuffer      = XPSS_ACPI_BUFFER;
          pXpssBodyAcpiTables->ControlLo          =
          PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].SwPstateNumber;
          pXpssBodyAcpiTables->ControlHi          = 0;
          pXpssBodyAcpiTables->StatusBuffer       = XPSS_ACPI_BUFFER;
          pXpssBodyAcpiTables->StatusLo           =
          PStateLevelingBuffer->PStateCoreStruct[0].PStateStruct[k].SwPstateNumber;
          pXpssBodyAcpiTables->StatusHi           = 0;
          pXpssBodyAcpiTables->ControlMaskBuffer  = XPSS_ACPI_BUFFER;
          pXpssBodyAcpiTables->ControlMaskLo      = 0;
          pXpssBodyAcpiTables->ControlMaskHi      = 0;
          pXpssBodyAcpiTables->StatusMaskBuffer   = XPSS_ACPI_BUFFER;
          pXpssBodyAcpiTables->StatusMaskLo       = 0;
          pXpssBodyAcpiTables->StatusMaskHi       = 0;

          pXpssBodyAcpiTables++;
        }
      } // for (k = 0; k < MPPSTATE_MAXIMUM_STATES; k++)

      // Set XPSS Header again
      // Now set pssLength and numOfItemsInPss
      //---------------------------------------
      TempVar_a = (PstateCount * XPSS_BODY_STRUCT_SIZE) + 3;
      TempVar_b = TempVar_a;
      TempVar_c = ((TempVar_b << 4) & 0x0000FF00);
      TempVar_c = TempVar_c | ((TempVar_b & 0x0000000F) | 0x00000040);
      TempVar_a = (UINT16) TempVar_c;

      pXpssHeaderAcpiTables->XpssLength = (UINT16) TempVar_a;
      pXpssHeaderAcpiTables->NumOfItemsInXpss = (UINT8) PstateCount;
      CurrSize += (XPSS_HEADER_STRUCT_SIZE + (PstateCount * XPSS_BODY_STRUCT_SIZE));

      *SsdtPtr = pXpssBodyAcpiTables;
    } //end of XPSS Body OptionPstateLateConfiguration.CfgPstateXpss

    // Set _PSD Table
    //---------------
    // Typecast the pointer
    pPsdHeaderAcpiTables = (PSD_HEADER *) pXpssBodyAcpiTables;
    pPsdBodyAcpiTables = (PSD_BODY *)     pXpssBodyAcpiTables;
    // Get Total Cores Per Node
    if (GetActiveCoresInGivenSocket ((UINT32) PStateLevelingBuffer->SocketNumber, &CoreCount1, StdHeader)) {
      GetFeatureServicesOfSocket (&PstateFamilyServiceTable, (UINT32) PStateLevelingBuffer->SocketNumber, (CONST VOID **)&FamilyServices, StdHeader);
      ASSERT (FamilyServices != NULL)
      if ((CoreCount1 != 1) && (OptionPstateLateConfiguration.CfgPstatePsd) &&
          FamilyServices->IsPstatePsdNeeded (FamilyServices, PlatformConfig, StdHeader)) {
        // Set _PSD Header
        //----------------
        pPsdHeaderAcpiTables->NameOpcode    = NAME_OPCODE;
        pPsdHeaderAcpiTables->PkgOpcode     = PACKAGE_OPCODE;
        pPsdHeaderAcpiTables->PsdLength     = PSD_HEADER_LENGTH;
        pPsdHeaderAcpiTables->Value1        = PSD_VALUE1;
        pPsdHeaderAcpiTables->PsdName_a__   = PSD_NAME__;
        pPsdHeaderAcpiTables->PsdName_a_P   = PSD_NAME_P;
        pPsdHeaderAcpiTables->PsdName_a_S   = PSD_NAME_S;
        pPsdHeaderAcpiTables->PsdName_a_D   = PSD_NAME_D;

        // Typecast the pointer
        pPsdHeaderAcpiTables++;
        CurrSize += PSD_HEADER_STRUCT_SIZE;
        pPsdBodyAcpiTables = (PSD_BODY *) pPsdHeaderAcpiTables;

        pPsdHeaderAcpiTables--;
        // Set _PSD Body
        //--------------
        pPsdBodyAcpiTables->PkgOpcode           = PACKAGE_OPCODE;
        pPsdBodyAcpiTables->PkgLength           = PSD_PKG_LENGTH;
        pPsdBodyAcpiTables->NumOfEntries        = NUM_OF_ENTRIES;
        pPsdBodyAcpiTables->BytePrefixOpcode1   = BYTE_PREFIX_OPCODE;
        pPsdBodyAcpiTables->PsdNumOfEntries     = PSD_NUM_OF_ENTRIES;
        pPsdBodyAcpiTables->BytePrefixOpcode2   = BYTE_PREFIX_OPCODE;
        pPsdBodyAcpiTables->PsdRevision         = PSD_REVISION;
        pPsdBodyAcpiTables->DwordPrefixOpcode1  = DWORD_PREFIX_OPCODE;

        IsPsdDependent = FamilyServices->IsPstatePsdDependent (FamilyServices, PlatformConfig, StdHeader);

        if (IsPsdDependent) {
          pPsdBodyAcpiTables->DependencyDomain    = PSD_DEPENDENCY_DOMAIN;
          pPsdBodyAcpiTables->CoordinationType    = PSD_COORDINATION_TYPE_SW_ALL;
          pPsdBodyAcpiTables->NumOfProcessors     = CoreCount1;
        } else {
          switch (GetComputeUnitMapping (StdHeader)) {
          case AllCoresMapping:
            // All cores are in their own compute unit.
            pPsdBodyAcpiTables->DependencyDomain    = LocalApicId;
            pPsdBodyAcpiTables->CoordinationType    = PSD_COORDINATION_TYPE_SW_ANY;
            pPsdBodyAcpiTables->NumOfProcessors     = PSD_NUM_OF_PROCESSORS;
            break;
          case EvenCoresMapping:
            // Cores are paired in compute units.
            pPsdBodyAcpiTables->DependencyDomain    = (LocalApicId >> 1) & PSD_DOMAIN_COMPUTE_UNIT_MASK;
            pPsdBodyAcpiTables->CoordinationType    = PSD_COORDINATION_TYPE_HW_ALL;
            pPsdBodyAcpiTables->NumOfProcessors     = PSD_CORE_NUM_PER_COMPUTE_UNIT;
            break;
          default:
            ASSERT (FALSE);
          }
        }
        pPsdBodyAcpiTables->DwordPrefixOpcode2  = DWORD_PREFIX_OPCODE;
        pPsdBodyAcpiTables->DwordPrefixOpcode3  = DWORD_PREFIX_OPCODE;

        pPsdBodyAcpiTables++;
        *SsdtPtr = pPsdBodyAcpiTables;
        CurrSize += PSD_BODY_STRUCT_SIZE;
      }
    }// end of PSD Body if (CoreCount1 != 1) || (OptionPstateLateConfiguration.CfgPstatePsd)
    // Typecast the pointer

    pPpcAcpiTables = (PPC_HEADER_BODY *) pPsdBodyAcpiTables;

    // Set _PPC Table
    //---------------
    if (OptionPstateLateConfiguration.CfgPstatePpc) {
      // Name (PPCV, value)
      pPpcAcpiTables->NameOpcode            = NAME_OPCODE;
      pPpcAcpiTables->PpcName_a_P           = PPC_NAME_P;
      pPpcAcpiTables->PpcName_b_P           = PPC_NAME_P;
      pPpcAcpiTables->PpcName_a_C           = PPC_NAME_C;
      pPpcAcpiTables->PpcName_a_V           = PPC_NAME_V;
      pPpcAcpiTables->Value1                = PPC_VALUE1;
      pPpcAcpiTables->DefaultPerfPresentCap = PstateCapLevelSupport;
      // Method (_PPC) { return (PPCV) }
      pPpcAcpiTables->MethodOpcode          = METHOD_OPCODE;
      pPpcAcpiTables->PpcLength             = PPC_METHOD_LENGTH;
      pPpcAcpiTables->PpcName_a__           = PPC_NAME__;
      pPpcAcpiTables->PpcName_c_P           = PPC_NAME_P;
      pPpcAcpiTables->PpcName_d_P           = PPC_NAME_P;
      pPpcAcpiTables->PpcName_b_C           = PPC_NAME_C;
      pPpcAcpiTables->MethodFlags           = PPC_METHOD_FLAGS;
      pPpcAcpiTables->ReturnOpcode          = RETURN_OPCODE;
      pPpcAcpiTables->PpcName_e_P           = PPC_NAME_P;
      pPpcAcpiTables->PpcName_f_P           = PPC_NAME_P;
      pPpcAcpiTables->PpcName_c_C           = PPC_NAME_C;
      pPpcAcpiTables->PpcName_b_V           = PPC_NAME_V;

      CurrSize += PPC_HEADER_BODY_STRUCT_SIZE;
      // Increment and typecast the pointer
      pPpcAcpiTables++;
      *SsdtPtr =  pPpcAcpiTables;
    }// end of OptionPstateLateConfiguration.CfgPstatePpc
  }
  return CurrSize;
}

/**--------------------------------------------------------------------------------------
 *
 *  CreateCStateAcpiTables
 *
 *  Description:
 *     This is the common routine for creating ACPI C-State objects
 *
 *  Parameters:
 *    @param[in]     PlatformConfig       Platform operational characteristics; power cap
 *    @param[in]     PStateLevelingBuffer Buffer that contains P-State Leveling information
 *    @param[in,out] SsdtPtr              ACPI SSDT table pointer
 *    @param[in]     LocalApicId          Local Apic Id
 *    @param[in]     StdHeader            Handle to config for library and services
 *
 *    @retval        Size of ACPI C-States objects generated
 *
 *---------------------------------------------------------------------------------------
 **/
UINT32
CreateCStateAcpiTables (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PSTATE_LEVELING        *PStateLevelingBuffer,
  IN OUT   VOID                   **SsdtPtr,
  IN       UINT8                  LocalApicId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                    ObjSize;
  IO_CSTATE_FAMILY_SERVICES *IoCstateFamilyServices;

  ObjSize = 0;

  if (IsFeatureEnabled (IoCstate, PlatformConfig, StdHeader)) {
    GetFeatureServicesOfCurrentCore (&IoCstateFamilyServiceTable, (CONST VOID **)&IoCstateFamilyServices, StdHeader);
    // If we're supporting multiple families, only proceed when IO Cstate family services are available
    if (IoCstateFamilyServices != NULL) {
      IoCstateFamilyServices->CreateAcpiCstObj (IoCstateFamilyServices, LocalApicId, SsdtPtr, StdHeader);
      ObjSize = IoCstateFamilyServices->GetAcpiCstObj (IoCstateFamilyServices, PlatformConfig, StdHeader);
    }
  }
  return ObjSize;
}
