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
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*****************************************************************************
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
#include "cpuC6State.h"
#define FILECODE PROC_CPU_FEATURE_CPUPSTATETABLES_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
extern OPTION_PSTATE_LATE_CONFIGURATION    OptionPstateLateConfiguration;  // global user config record
extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;
extern CPU_FAMILY_SUPPORT_TABLE            C6FamilyServiceTable;

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

/*----------------------------------------------------------------------------
 *                           EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
AGESA_STATUS
PStateLevelingMain (
  IN OUT   S_CPU_AMD_PSTATE   *PStateStrucPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );
AGESA_STATUS
CreateAcpiTablesMain (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SsdtPtr
  );
AGESA_STATUS
CreateAcpiTablesStub (
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
STATIC UINT32
CalAcpiTablesSize (
  IN       S_CPU_AMD_PSTATE       *AmdPstatePtr,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32               ScopeSize;
  UINT32               CoreCount;
  UINT32               SocketCount;
  UINT32               MaxCoreNumberInCurrentSocket;
  UINT32               MaxSocketNumberInSystem;
  UINT32               MaxPstateNumberInCurrentCore;
  UINT32               CstObjSize;
  PSTATE_LEVELING      *PStateLevelingBufferStructPtr;
  C6_FAMILY_SERVICES   *C6FamilyServices;

  ScopeSize = sizeof (ACPI_TABLE_HEADER);
  CstObjSize = 0;
  C6FamilyServices = NULL;

  PStateLevelingBufferStructPtr = AmdPstatePtr->PStateLevelingStruc;
  MaxSocketNumberInSystem = AmdPstatePtr->TotalSocketInSystem;

  if (IsFeatureEnabled (C6Cstate, PlatformConfig, StdHeader)) {
    GetFeatureServicesOfCurrentCore (&C6FamilyServiceTable, (CONST VOID **)&C6FamilyServices, StdHeader);
    CstObjSize = C6FamilyServices->GetAcpiCstObj ();
  }

  for (SocketCount = 0; SocketCount < MaxSocketNumberInSystem; SocketCount++) {
    MaxCoreNumberInCurrentSocket = PStateLevelingBufferStructPtr->TotalCoresInNode;
    for (CoreCount = 0; CoreCount < MaxCoreNumberInCurrentSocket; CoreCount++) {
      MaxPstateNumberInCurrentCore = PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateMaxValue + 1;
      ScopeSize += (PCT_STRUCT_SIZE +
                    PSS_HEADER_STRUCT_SIZE +
                    (MaxPstateNumberInCurrentCore * PSS_BODY_STRUCT_SIZE) +
                    XPSS_HEADER_STRUCT_SIZE +
                    (MaxPstateNumberInCurrentCore * XPSS_BODY_STRUCT_SIZE) +
                    PSD_HEADER_STRUCT_SIZE +
                    PSD_BODY_STRUCT_SIZE +
                    PPC_HEADER_BODY_STRUCT_SIZE) + (SCOPE_STRUCT_SIZE - 1) +
                    CstObjSize;

    }
    ScopeSize += MaxCoreNumberInCurrentSocket;
    PStateLevelingBufferStructPtr = (PSTATE_LEVELING *) ((UINT8 *) PStateLevelingBufferStructPtr + (UINTN) sizeof (PSTATE_LEVELING) + (UINTN) (PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateMaxValue * sizeof (S_PSTATE_VALUES)));
  }

  AmdPstatePtr->SizeOfBytes = ScopeSize;

  return ScopeSize;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  CreateAcpiTables
 *
 *  Description:
 *    This function will populate the ACPI PState tables
 *    This function should be called only from BSP
 *
 *  Parameters:
 *    @param[in]       StdHeader          Handle to config for library and services
 *    @param[in]       PlatformConfig     Contains the power cap parameter
 *    @param[in,out]   SsdtPtr            ACPI SSDT table pointer
 *
 *    @retval      AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
CreateAcpiTables (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SsdtPtr
  )
{
  AGESA_TESTPOINT (TpProcCpuEntryPstate, StdHeader);
  return ((*(OptionPstateLateConfiguration.PstateFeature)) (StdHeader, PlatformConfig, SsdtPtr));
  // Note: Split config struct into PEI/DXE halves. This one is DXE.
}

/**--------------------------------------------------------------------------------------
 *
 *  CreateAcpiTablesStub
 *
 *  Description:
 *     This is the default routine for use when the PState option is NOT requested.
 *      The option install process will create and fill the transfer vector with
 *      the address of the proper routine (Main or Stub). The link optimizer will
 *      strip out of the .DLL the routine that is not used.
 *
 *  Parameters:
 *    @param[in]       StdHeader          Handle to config for library and services
 *    @param[in]       PlatformConfig     Contains the power cap parameter
 *    @param[in,out]   SsdtPtr            ACPI SSDT table pointer
 *
 *    @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
CreateAcpiTablesStub (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SsdtPtr
  )
{
  return  AGESA_UNSUPPORTED;
}

/**--------------------------------------------------------------------------------------
 *
 *  CreateAcpiTablesMain
 *
 *  Description:
 *     This is the common routine for creating the ACPI information tables.
 *
 *  Parameters:
 *    @param[in]       StdHeader          Handle to config for library and services
 *    @param[in]       PlatformConfig     Contains the power cap parameter
 *    @param[in,out]   SsdtPtr            ACPI SSDT table pointer
 *
 *    @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
CreateAcpiTablesMain (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SsdtPtr
  )
{
  UINT32                 i;
  UINT32                 j;
  UINT32                 k;
  UINT32                 TempVar8_a;
  UINT32                 PStateCount;
  UINT32                 CoreCount;
  UINT32                 CoreCount1;
  UINT32                 SocketCount;
  UINT32                 ScopeSize;
  UINT32                 TempVar_a;
  UINT32                 TempVar_b;
  UINT32                 TempVar_c;
  UINT32                 TempVar_d;
  PCI_ADDR               PciAddress;
  UINT32                 TransAndBusMastLatency;
  UINT32                 MaxCorePerNode;
  UINT8                  PStateMaxValueOnCurrentCore;
  UINT8                  *IntermediatePtr;
  PSTATE_LEVELING        *PStateLevelingBufferStructPtr;
  AMD_CONFIG_PARAMS      *AmdConfigParamsStructPtr;
  SCOPE                  *ScopeAcpiTablesStructPtr;
  SCOPE                  *ScopeAcpiTablesStructPtrTemp;
  PCT_HEADER_BODY        *pPctAcpiTables;
  PSS_HEADER             *pPssHeaderAcpiTables;
  PSS_BODY               *pPssBodyAcpiTables;
  XPSS_HEADER            *pXpssHeaderAcpiTables;
  XPSS_BODY              *pXpssBodyAcpiTables;
  PSD_HEADER             *pPsdHeaderAcpiTables;
  PSD_BODY               *pPsdBodyAcpiTables;
  PPC_HEADER_BODY        *pPpcHeaderBodyAcpiTables;
  AGESA_BUFFER_PARAMS    AgesaBuffer;
  LOCATE_HEAP_PTR        LocateHeapParams;
  S_CPU_AMD_PSTATE       *AmdPstatePtr;
  BOOLEAN                PstateCapEnable;
  UINT32                 PstateCapInputMilliWatts;
  UINT8                  PstateCapLevelSupport;
  BOOLEAN                PstateCapLevelSupportDetermined;
  UINT8                  LocalApicId;
  AGESA_STATUS           AgesaStatus;
  AGESA_STATUS           IgnoredStatus;
  CPU_SPECIFIC_SERVICES  *FamilySpecificServices;
  PSTATE_CPU_FAMILY_SERVICES *FamilyServices;
  BOOLEAN                IsPsdDependent;
  C6_FAMILY_SERVICES     *C6FamilyServices;
  //
  //Initialize local variables
  //
  AmdConfigParamsStructPtr = StdHeader;
  PstateCapEnable = FALSE;
  PstateCapInputMilliWatts = PlatformConfig->PowerCeiling;
  PstateCapLevelSupport = DEFAULT_PERF_PRESENT_CAP;
  PstateCapLevelSupportDetermined = TRUE;
  LocalApicId = 0;
  AgesaStatus = AGESA_SUCCESS;
  TempVar_c = 0;
  ScopeAcpiTablesStructPtrTemp = NULL;
  TransAndBusMastLatency = 0;
  PStateCount = 0;
  IsPsdDependent = !(PlatformConfig->ForcePstateIndependent);
  FamilyServices = NULL;
  C6FamilyServices = NULL;

  ASSERT (IsBsp (StdHeader, &IgnoredStatus));

  //
  //Locate P-state gathered data heap.
  //
  LocateHeapParams.BufferHandle = AMD_PSTATE_DATA_BUFFER_HANDLE;

  AGESA_TESTPOINT (TpProcCpuBeforeLocateSsdtBuffer, StdHeader);
  if (HeapLocateBuffer (&LocateHeapParams, StdHeader) != AGESA_SUCCESS) {
    return AGESA_ERROR;
  }
  AGESA_TESTPOINT (TpProcCpuAfterLocateSsdtBuffer, StdHeader);

  AmdPstatePtr = (S_CPU_AMD_PSTATE *) LocateHeapParams.BufferPtr;
  PStateLevelingBufferStructPtr = AmdPstatePtr->PStateLevelingStruc;

  //
  // Check if we need to create ACPI tables
  //
  if (PStateLevelingBufferStructPtr[0].CreateAcpiTables == 0) {
    return AGESA_UNSUPPORTED;
  }

  //
  //Allocate rough buffer for AcpiTable, if AcpiPstateBufferPtr is NULL
  //
  if (*SsdtPtr == NULL) {
    AgesaBuffer.StdHeader = *StdHeader;
    //
    //Do not know the actual size.. pre-calculate it.
    //
    AgesaBuffer.BufferLength = CalAcpiTablesSize (AmdPstatePtr, PlatformConfig, StdHeader);
    AgesaBuffer.BufferHandle = AMD_PSTATE_ACPI_BUFFER_HANDLE;

    AGESA_TESTPOINT (TpProcCpuBeforeAllocateSsdtBuffer, StdHeader);
    if (AgesaAllocateBuffer (0, &AgesaBuffer) != AGESA_SUCCESS) {
      return AGESA_ERROR;
    }
    AGESA_TESTPOINT (TpProcCpuAfterAllocateSsdtBuffer, StdHeader);
    *SsdtPtr = AgesaBuffer.BufferPointer;
  }

  //SSDT header
  LibAmdMemCopy (*SsdtPtr, (VOID *) &CpuSsdtHdrStruct, (UINTN) (sizeof (ACPI_TABLE_HEADER)), StdHeader);

  IntermediatePtr = (UINT8 *) *SsdtPtr;
  ScopeAcpiTablesStructPtr = (SCOPE *) &IntermediatePtr[sizeof (ACPI_TABLE_HEADER)];

  SocketCount = AmdPstatePtr->TotalSocketInSystem;

  // Initialize data variables
  ScopeSize = 0;
  CoreCount = 0;
  for (i = 0; i < SocketCount; i++) {

    MaxCorePerNode = PStateLevelingBufferStructPtr->TotalCoresInNode;

    for (j = 0; j < MaxCorePerNode; j++) {

      //
      //Check Pstate Capability enable
      //
      if (PstateCapInputMilliWatts != 0) {
        PstateCapEnable = TRUE;
        PstateCapLevelSupportDetermined = FALSE;
      }

      PStateMaxValueOnCurrentCore = PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateMaxValue;

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

      //
      // Increment and typecast the pointer
      //
      ScopeAcpiTablesStructPtr++;
      pPctAcpiTables = (PCT_HEADER_BODY *) ScopeAcpiTablesStructPtr;
      ScopeAcpiTablesStructPtr--;
      ScopeAcpiTablesStructPtrTemp = ScopeAcpiTablesStructPtr;
      ScopeAcpiTablesStructPtrTemp++;

      if (OptionPstateLateConfiguration.CfgPstatePct) {
        //
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

        // Set _PSS Table - START
        //------------------------

        // Increment and then typecast the pointer
        pPctAcpiTables++;
        TempVar_c += PCT_STRUCT_SIZE;

        ScopeAcpiTablesStructPtrTemp = (SCOPE *) pPctAcpiTables;
      } // end of OptionPstateLateConfiguration.CfgPstatePct

      pPssHeaderAcpiTables = (PSS_HEADER *) pPctAcpiTables;

      if (OptionPstateLateConfiguration.CfgPstatePss) {
        //
        // Set _PSS Header
        // Note: Set the pssLength and numOfItemsInPss later
        //---------------------------------------------------
        pPssHeaderAcpiTables->NameOpcode      = NAME_OPCODE;
        pPssHeaderAcpiTables->PssName_a__     = PSS_NAME__;
        pPssHeaderAcpiTables->PssName_a_P     = PSS_NAME_P;
        pPssHeaderAcpiTables->PssName_a_S     = PSS_NAME_S;
        pPssHeaderAcpiTables->PssName_b_S     = PSS_NAME_S;
        pPssHeaderAcpiTables->PkgOpcode       = PACKAGE_OPCODE;

        pPssHeaderAcpiTables++;
      }// end of PSS Header if OptionPstateLateConfiguration.CfgPstatePss

      pPssBodyAcpiTables = (PSS_BODY *) pPssHeaderAcpiTables;

      if (OptionPstateLateConfiguration.CfgPstatePss) {
        // Restore the pPssHeaderAcpiTables
        pPssHeaderAcpiTables--;

        // Set _PSS Body
        //---------------
        PStateCount = 0;

        //
        //Calculate pci address for socket only
        //
        GetPciAddress (StdHeader, (UINT32) PStateLevelingBufferStructPtr->SocketNumber, 0, &PciAddress, &IgnoredStatus);
        TransAndBusMastLatency = 0;
        GetCpuServicesOfSocket ((UINT32) PStateLevelingBufferStructPtr->SocketNumber, &FamilySpecificServices, StdHeader);
        ASSERT (FamilySpecificServices != NULL);
        FamilySpecificServices->GetPstateLatency (FamilySpecificServices,
                                                  PStateLevelingBufferStructPtr,
                                                  &PciAddress,
                                                  &TransAndBusMastLatency,
                                                  StdHeader);

        for (k = 0; k <= PStateMaxValueOnCurrentCore; k++) {
          if (PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateStruct[k].PStateEnable != 0) {
            pPssBodyAcpiTables->PkgOpcode           = PACKAGE_OPCODE;
            pPssBodyAcpiTables->PkgLength           = PSS_PKG_LENGTH;
            pPssBodyAcpiTables->NumOfElements       = PSS_NUM_OF_ELEMENTS;
            pPssBodyAcpiTables->DwordPrefixOpcode1  = DWORD_PREFIX_OPCODE;

            pPssBodyAcpiTables->Frequency           =
            PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateStruct[k].CoreFreq;

            pPssBodyAcpiTables->DwordPrefixOpcode2  = DWORD_PREFIX_OPCODE;

            pPssBodyAcpiTables->Power               =
            PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateStruct[k].Power;

            if (PstateCapEnable && (!PstateCapLevelSupportDetermined) && (PstateCapInputMilliWatts >= pPssBodyAcpiTables->Power)) {
              PstateCapLevelSupport = (UINT8) k;
              PstateCapLevelSupportDetermined = TRUE;
            }

            pPssBodyAcpiTables->DwordPrefixOpcode3  = DWORD_PREFIX_OPCODE;

            pPssBodyAcpiTables->TransitionLatency   = TransAndBusMastLatency;
            pPssBodyAcpiTables->DwordPrefixOpcode4  = DWORD_PREFIX_OPCODE;
            pPssBodyAcpiTables->BusMasterLatency    = TransAndBusMastLatency;
            pPssBodyAcpiTables->DwordPrefixOpcode5  = DWORD_PREFIX_OPCODE;
            pPssBodyAcpiTables->Control             = k;
            pPssBodyAcpiTables->DwordPrefixOpcode6  = DWORD_PREFIX_OPCODE;
            pPssBodyAcpiTables->Status              = k;

            pPssBodyAcpiTables++;
            PStateCount++;
          }

        } // for (k = 0; k < MPPSTATE_MAXIMUM_STATES; k++)

        if (PstateCapEnable && (!PstateCapLevelSupportDetermined)) {
          PstateCapLevelSupport = PStateMaxValueOnCurrentCore;
        }

        // Set _PSS Header again
        // Now Set pssLength and numOfItemsInPss
        //---------------------------------------
        TempVar_a = (PStateCount * PSS_BODY_STRUCT_SIZE) + 3;
        if (TempVar_a > 63) {
          TempVar_b = TempVar_a;
          TempVar_d = ((TempVar_b << 4) & 0x0000FF00);
          TempVar_d = TempVar_d | ((TempVar_b & 0x0000000F) | 0x00000040);
          TempVar_a = (UINT16) TempVar_d;
        }

        pPssHeaderAcpiTables->PssLength = (UINT16) TempVar_a;
        pPssHeaderAcpiTables->NumOfItemsInPss = (UINT8) PStateCount;
        TempVar_c += (PSS_HEADER_STRUCT_SIZE + (PStateCount * PSS_BODY_STRUCT_SIZE));

        ScopeAcpiTablesStructPtrTemp = (SCOPE *) pPssBodyAcpiTables;
      } // end of PSS Body if OptionPstateLateConfiguration.CfgPstatePss

      //
      // Set XPSS Table - START
      //------------------------
      // Typecast the pointer
      pXpssHeaderAcpiTables = (XPSS_HEADER *) pPssBodyAcpiTables;

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
      }//end of XPSS header if OptionPstateLateConfiguration.CfgPstateXpss

      pXpssBodyAcpiTables = (XPSS_BODY *) pXpssHeaderAcpiTables;

      if (OptionPstateLateConfiguration.CfgPstateXpss) {
        // Restore the pXpssHeaderAcpiTables
        pXpssHeaderAcpiTables--;

        // Set XPSS Body
        //---------------
        for (k = 0; k <= PStateMaxValueOnCurrentCore; k++) {
          if (PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateStruct[k].PStateEnable != 0) {
            pXpssBodyAcpiTables->PkgOpcode          = PACKAGE_OPCODE;
            pXpssBodyAcpiTables->PkgLength          = XPSS_PKG_LENGTH;
            pXpssBodyAcpiTables->NumOfElements      = XPSS_NUM_OF_ELEMENTS;
            pXpssBodyAcpiTables->XpssValueTbd       = 04;
            pXpssBodyAcpiTables->DwordPrefixOpcode1 = DWORD_PREFIX_OPCODE;

            pXpssBodyAcpiTables->Frequency          =
            PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateStruct[k].CoreFreq;

            pXpssBodyAcpiTables->DwordPrefixOpcode2 = DWORD_PREFIX_OPCODE;

            pXpssBodyAcpiTables->Power              =
            PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateStruct[k].Power;

            pXpssBodyAcpiTables->DwordPrefixOpcode3 = DWORD_PREFIX_OPCODE;

            pXpssBodyAcpiTables->TransitionLatency  = TransAndBusMastLatency;
            pXpssBodyAcpiTables->DwordPrefixOpcode4 = DWORD_PREFIX_OPCODE;
            pXpssBodyAcpiTables->BusMasterLatency   = TransAndBusMastLatency;
            pXpssBodyAcpiTables->ControlBuffer      = XPSS_ACPI_BUFFER;
            pXpssBodyAcpiTables->ControlLo          = k;
            pXpssBodyAcpiTables->ControlHi          = 0;
            pXpssBodyAcpiTables->StatusBuffer       = XPSS_ACPI_BUFFER;
            pXpssBodyAcpiTables->StatusLo           = k;
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
        // Now Set pssLength and numOfItemsInPss
        //---------------------------------------
        TempVar_a = (PStateCount * XPSS_BODY_STRUCT_SIZE) + 3;
        if (TempVar_a > 63) {
          TempVar_b = TempVar_a;
          TempVar_d = ((TempVar_b << 4) & 0x0000FF00);
          TempVar_d = TempVar_d | ((TempVar_b & 0x0000000F) | 0x00000040);
          TempVar_a = (UINT16) TempVar_d;
        }

        pXpssHeaderAcpiTables->XpssLength = (UINT16) TempVar_a;
        pXpssHeaderAcpiTables->NumOfItemsInXpss = (UINT8) PStateCount;
        TempVar_c += (XPSS_HEADER_STRUCT_SIZE + (PStateCount * XPSS_BODY_STRUCT_SIZE));

        ScopeAcpiTablesStructPtrTemp = (SCOPE *) pXpssBodyAcpiTables;
      } //end of XPSS Body OptionPstateLateConfiguration.CfgPstateXpss

      //
      // Set _PSD Table - START
      //------------------------
      // Typecast the pointer
      pPsdHeaderAcpiTables = (PSD_HEADER *) pXpssBodyAcpiTables;

      //
      // Get Total Cores Per Node
      /// @todo Maybe this should ask for single core Sockets not single core nodes?
      if (GetActiveCoresInGivenSocket ((UINT32) PStateLevelingBufferStructPtr->SocketNumber, &CoreCount1, AmdConfigParamsStructPtr)) {
        if ((CoreCount1 != 1) && (OptionPstateLateConfiguration.CfgPstatePsd)) {
          //
          // Set _PSD Header
          //-----------------
          pPsdHeaderAcpiTables->NameOpcode    = NAME_OPCODE;
          pPsdHeaderAcpiTables->PkgOpcode     = PACKAGE_OPCODE;
          pPsdHeaderAcpiTables->PsdLength     = PSD_HEADER_LENGTH;
          pPsdHeaderAcpiTables->Value1        = PSD_VALUE1;

          // Set _PSD Header
          pPsdHeaderAcpiTables->PsdName_a__   = PSD_NAME__;
          pPsdHeaderAcpiTables->PsdName_a_P   = PSD_NAME_P;
          pPsdHeaderAcpiTables->PsdName_a_S   = PSD_NAME_S;
          pPsdHeaderAcpiTables->PsdName_a_D   = PSD_NAME_D;

          // Typecast the pointer
          pPsdHeaderAcpiTables++;
          TempVar_c += PSD_HEADER_STRUCT_SIZE;
        }  // end of PSD Header if (CoreCount1 != 1) && (OptionPstateLateConfiguration.CfgPstatePsd)
     }
      pPsdBodyAcpiTables = (PSD_BODY *) pPsdHeaderAcpiTables;

      if ((CoreCount1 != 1) && (OptionPstateLateConfiguration.CfgPstatePsd)) {
        pPsdHeaderAcpiTables--;
        //
        // Set _PSD Body
        //----------------
        pPsdBodyAcpiTables->PkgOpcode           = PACKAGE_OPCODE;
        pPsdBodyAcpiTables->PkgLength           = PSD_PKG_LENGTH;
        pPsdBodyAcpiTables->NumOfEntries        = NUM_OF_ENTRIES;
        pPsdBodyAcpiTables->BytePrefixOpcode1   = BYTE_PREFIX_OPCODE;
        pPsdBodyAcpiTables->PsdNumOfEntries     = PSD_NUM_OF_ENTRIES;
        pPsdBodyAcpiTables->BytePrefixOpcode2   = BYTE_PREFIX_OPCODE;
        pPsdBodyAcpiTables->PsdRevision         = PSD_REVISION;
        pPsdBodyAcpiTables->DwordPrefixOpcode1  = DWORD_PREFIX_OPCODE;
        //
        //Get ApicId from AMD CPU general service.
        //Basically, DependencyDomain should be LocalApicId for each core PSD independent.
        //
        GetApicId (StdHeader, (UINT32) PStateLevelingBufferStructPtr->SocketNumber, j, &LocalApicId, &AgesaStatus);

        GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
        if (FamilyServices != NULL) {
          IsPsdDependent = FamilyServices->IsPstatePsdDependent (FamilyServices, PlatformConfig, StdHeader);
        }

        if (IsPsdDependent) {
          pPsdBodyAcpiTables->DependencyDomain    = PSD_DEPENDENCY_DOMAIN;
          pPsdBodyAcpiTables->CoordinationType    = PSD_COORDINATION_TYPE_SW_ALL;
          pPsdBodyAcpiTables->NumOfProcessors     = CoreCount1;
        } else {
          pPsdBodyAcpiTables->DependencyDomain    = LocalApicId;
          pPsdBodyAcpiTables->CoordinationType    = PSD_COORDINATION_TYPE_SW_ANY;
          pPsdBodyAcpiTables->NumOfProcessors     = PSD_NUM_OF_PROCESSORS;
        }

        pPsdBodyAcpiTables->DwordPrefixOpcode2  = DWORD_PREFIX_OPCODE;
        pPsdBodyAcpiTables->DwordPrefixOpcode3  = DWORD_PREFIX_OPCODE;

        pPsdBodyAcpiTables++;
        ScopeAcpiTablesStructPtrTemp = (SCOPE *) pPsdBodyAcpiTables;
        TempVar_c += PSD_BODY_STRUCT_SIZE;
      }// end of PSD Body if (CoreCount1 != 1) || (OptionPstateLateConfiguration.CfgPstatePsd)

      //
      // Set _PPC Table - START
      //------------------------

      // Typecast the pointer

      pPpcHeaderBodyAcpiTables = (PPC_HEADER_BODY *) pPsdBodyAcpiTables;

      // Set _PPC Header and Body
      //--------------------------
      if (OptionPstateLateConfiguration.CfgPstatePpc) {
        pPpcHeaderBodyAcpiTables->NameOpcode            = NAME_OPCODE;
        pPpcHeaderBodyAcpiTables->PpcName_a__           = PPC_NAME__;
        pPpcHeaderBodyAcpiTables->PpcName_a_P           = PPC_NAME_P;
        pPpcHeaderBodyAcpiTables->PpcName_b_P           = PPC_NAME_P;
        pPpcHeaderBodyAcpiTables->PpcName_a_C           = PPC_NAME_C;
        pPpcHeaderBodyAcpiTables->Value1                = PPC_VALUE1;

        pPpcHeaderBodyAcpiTables->DefaultPerfPresentCap = PstateCapLevelSupport;
        TempVar_c += PPC_HEADER_BODY_STRUCT_SIZE;
        // Increment and typecast the pointer
        pPpcHeaderBodyAcpiTables++;
        ScopeAcpiTablesStructPtrTemp = (SCOPE *) pPpcHeaderBodyAcpiTables;
      }// end of OptionPstateLateConfiguration.CfgPstatePpc

      // If C6 is enabled, generate the corresponding ACPI object for it
      if (IsFeatureEnabled (C6Cstate, PlatformConfig, StdHeader)) {
        GetFeatureServicesOfCurrentCore (&C6FamilyServiceTable, (CONST VOID **)&C6FamilyServices, StdHeader);
        C6FamilyServices->CreateAcpiCstObj ((VOID *) &ScopeAcpiTablesStructPtrTemp, StdHeader);
        TempVar_c += C6FamilyServices->GetAcpiCstObj ();
      }

      //
      // Now update the SCOPE Length field
      //
      {
        TempVar_c += (SCOPE_STRUCT_SIZE - 1);
        ScopeSize += TempVar_c;

        TempVar_d = ((TempVar_c << 4) & 0x0000FF00);
        TempVar_d |= ((TempVar_c & 0x0000000F) | 0x00000040);
        TempVar_a = TempVar_d;
        ScopeAcpiTablesStructPtr->ScopeLength = (UINT16) TempVar_a;
        TempVar_c = 0;
      }

      // Increment and typecast the pointer
      ScopeAcpiTablesStructPtr = ScopeAcpiTablesStructPtrTemp;

    } // for (j = 0; j < MPPSTATE_MAX_CORES_PER_NODE; j++)

    //
    //Calculate next node buffer address
    //
    PStateLevelingBufferStructPtr = (PSTATE_LEVELING *) ((UINT8 *) PStateLevelingBufferStructPtr + (UINTN) sizeof (PSTATE_LEVELING) + (UINTN) (PStateLevelingBufferStructPtr->PStateCoreStruct[0].PStateMaxValue * sizeof (S_PSTATE_VALUES)));

  } // for (i = 0; i < NodeCount; i++)

  //Update SSDT header Checksum
  ((ACPI_TABLE_HEADER *) *SsdtPtr)->TableLength = (ScopeSize + CoreCount + sizeof (ACPI_TABLE_HEADER));
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) *SsdtPtr, StdHeader);

  return AGESA_SUCCESS;
}



