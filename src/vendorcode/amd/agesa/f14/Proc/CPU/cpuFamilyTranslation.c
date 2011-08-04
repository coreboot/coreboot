/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Family Translation functions.
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Interface
 * @e \$Revision: 37150 $   @e \$Date: 2010-08-31 23:53:37 +0800 (Tue, 31 Aug 2010) $
 *
 */
/*
 *****************************************************************************
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
 * ***************************************************************************
 *
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "CommonReturns.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_CPUFAMILYTRANSLATION_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

CONST CPU_SPECIFIC_SERVICES ROMDATA cpuNullServices =
{
  0,
  (PF_CPU_DISABLE_PSTATE) CommonReturnAgesaSuccess,
  (PF_CPU_TRANSITION_PSTATE) CommonReturnAgesaSuccess,
  (PF_CPU_GET_IDD_MAX) CommonReturnFalse,
  (PF_CPU_GET_TSC_RATE) CommonReturnAgesaSuccess,
  (PF_CPU_GET_NB_FREQ) CommonReturnAgesaSuccess,
  (PF_CPU_GET_NB_PSTATE_INFO) CommonReturnFalse,
  (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonReturnAgesaSuccess,
  (PF_CPU_AP_INITIAL_LAUNCH) CommonReturnFalse,
  (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonReturnZero8,
  (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonReturnAgesaSuccess,
  (PF_CPU_SET_AP_CORE_NUMBER) CommonVoid,
  (PF_CPU_GET_AP_CORE_NUMBER) CommonReturnZero32,
  (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonVoid,
  (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonReturnAgesaSuccess,
  (PF_CPU_SAVE_FEATURES) CommonReturnAgesaSuccess,
  (PF_CPU_WRITE_FEATURES) CommonReturnAgesaSuccess,
  (PF_CPU_SET_WARM_RESET_FLAG) CommonReturnAgesaSuccess,
  (PF_CPU_GET_WARM_RESET_FLAG) CommonReturnAgesaSuccess,
  GetEmptyArray,
  GetEmptyArray,
  GetEmptyArray,
  GetEmptyArray,
  GetEmptyArray,
  GetEmptyArray,
  GetEmptyArray,
  (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonReturnAgesaSuccess,
  (PF_IS_NB_PSTATE_ENABLED) CommonReturnFalse,
  (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
  (PF_SET_HT_PHY_REGISTER) CommonVoid,
  (PF_GET_NEXT_HT_LINK_FEATURES) CommonVoid,
  NULL,
  NULL,
  NULL,
  NULL,
  InitCacheDisabled,
  (PF_GET_EARLY_INIT_TABLE) CommonVoid
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
GetCpuServices (
  IN       CPU_FAMILY_SUPPORT_TABLE *FamilyTable,
  IN       UINT64            *MatchData,
     OUT   CONST VOID        **CpuServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE CpuSupportedFamiliesTable;
extern CPU_FAMILY_ID_XLAT_TABLE CpuSupportedFamilyIdTable;

/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Returns the logical ID of the desired processor. This will be obtained by
 *  reading the CPUID and converting it into a "logical ID" which is not package
 *  dependent.
 *
 *  @param[in]      Socket             Socket
 *  @param[out]     LogicalId          The Processor's Logical ID
 *  @param[in]      StdHeader          Handle of Header for calling lib functions and services.
 *
 */
VOID
GetLogicalIdOfSocket (
  IN       UINT32 Socket,
     OUT   CPU_LOGICAL_ID *LogicalId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32      RawCpuid;
  PCI_ADDR    PciAddress;
  AGESA_STATUS AssumedSuccess;

  RawCpuid = 0;

  if (GetPciAddress (StdHeader, (UINT8)Socket, 0, &PciAddress, &AssumedSuccess)) {
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = CPUID_FMR;
    LibAmdPciRead (AccessWidth32, PciAddress, &RawCpuid, StdHeader);
    GetLogicalIdFromCpuid (RawCpuid, LogicalId, StdHeader);
  } else {
    LogicalId->Family = 0;
    LogicalId->Revision = 0;
    // Logical ID was not found.
    IDS_ERROR_TRAP;
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Returns the logical ID of the executing core. This will be obtained by reading
 *  the CPUID and converting it into a "logical ID" which is not package dependent.
 *
 *  @param[out]     LogicalId          The Processor's Logical ID
 *  @param[in]      StdHeader          Handle of Header for calling lib functions and services.
 *
 */
VOID
GetLogicalIdOfCurrentCore (
     OUT   CPU_LOGICAL_ID *LogicalId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPUID_DATA  CpuidDataStruct;

  LibAmdCpuidRead (AMD_CPUID_APICID_LPC_BID, &CpuidDataStruct, StdHeader);
  GetLogicalIdFromCpuid (CpuidDataStruct.EAX_Reg, LogicalId, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Returns the logical ID of a processor with the given CPUID value. This
 *  will be obtained by converting it into a "logical ID" which is not package
 *  dependent.
 *
 *  @param[in]      RawCpuid           The unprocessed CPUID value to be translated
 *  @param[out]     LogicalId          The Processor's Logical ID
 *  @param[in]      StdHeader          Handle of Header for calling lib functions and services
 *
 */
VOID
GetLogicalIdFromCpuid (
  IN       UINT32 RawCpuid,
     OUT   CPU_LOGICAL_ID *LogicalId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8       i;
  UINT8       k;
  UINT8       NumberOfFamiliesSupported;
  UINT8       NumberOfLogicalSubFamilies;
  UINT8       LogicalIdEntries;
  UINT32      j;
  UINT32      RawFamily;
  UINT32      CpuModelAndExtendedModel;
  UINT64      LogicalFamily;
  BOOLEAN     IdNotFound;
  BOOLEAN     FamilyNotFound;
  CONST PF_CPU_GET_SUBFAMILY_ID_ARRAY *SubFamilyIdPtr;
  CPU_LOGICAL_ID_XLAT *CpuLogicalIdAndRevPtr;
  CONST CPU_LOGICAL_ID_FAMILY_XLAT *ImageSupportedId;

  IdNotFound = TRUE;
  FamilyNotFound = TRUE;
  CpuLogicalIdAndRevPtr = NULL;
  ImageSupportedId = CpuSupportedFamilyIdTable.FamilyIdTable;
  NumberOfFamiliesSupported = CpuSupportedFamilyIdTable.Elements;

  RawFamily = ((RawCpuid & 0xF00) >> 8) + ((RawCpuid & 0xFF00000) >> 20);
  RawCpuid &= (UINT32) CPU_FMS_MASK;
  CpuModelAndExtendedModel = (UINT16) ((RawCpuid >> 8) | RawCpuid);

  LogicalId->Family = 0;
  LogicalId->Revision = 0;

  for (i = 0; i < NumberOfFamiliesSupported && FamilyNotFound; i++) {
    if (ImageSupportedId[i].Family == RawFamily) {
      FamilyNotFound = FALSE;
      LogicalId->Family = ImageSupportedId[i].UnknownRevision.Family;
      LogicalId->Revision = ImageSupportedId[i].UnknownRevision.Revision;

      NumberOfLogicalSubFamilies = ImageSupportedId[i].Elements;
      SubFamilyIdPtr = ImageSupportedId[i].SubFamilyIdTable;
      for (j = 0; j < NumberOfLogicalSubFamilies && IdNotFound; j++) {
        SubFamilyIdPtr[j] ((const CPU_LOGICAL_ID_XLAT **)&CpuLogicalIdAndRevPtr, &LogicalIdEntries, &LogicalFamily, StdHeader);
        ASSERT (CpuLogicalIdAndRevPtr != NULL);
        for (k = 0; k < LogicalIdEntries; k++) {
          if (CpuLogicalIdAndRevPtr[k].RawId == CpuModelAndExtendedModel) {
            IdNotFound = FALSE;
            LogicalId->Family = LogicalFamily;
            LogicalId->Revision = CpuLogicalIdAndRevPtr[k].LogicalId;
            break;
          }
        }
      }
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Retrieves a pointer to the desired processor's family specific services structure.
 *
 *  @param[in]      Socket             The Processor in this Socket.
 *  @param[out]     FunctionTable      The Processor's Family Specific services.
 *  @param[in]      StdHeader          Handle of Header for calling lib functions and services.
 *
 */
VOID
GetCpuServicesOfSocket (
  IN       UINT32 Socket,
     OUT   CONST CPU_SPECIFIC_SERVICES **FunctionTable,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  GetFeatureServicesOfSocket (&CpuSupportedFamiliesTable,
                              Socket,
                              (const VOID **)FunctionTable,
                              StdHeader);
  if (*FunctionTable == NULL) {
    *FunctionTable = &cpuNullServices;
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Retrieves a pointer to the desired processor's family specific services structure.
 *
 *  @param[in]      FamilyTable        The table to search in.
 *  @param[in]      Socket             The Processor in this Socket.
 *  @param[out]     CpuServices        The Processor's Family Specific services.
 *  @param[in]      StdHeader          Handle of Header for calling lib functions and services.
 *
 */
VOID
GetFeatureServicesOfSocket (
  IN       CPU_FAMILY_SUPPORT_TABLE *FamilyTable,
  IN       UINT32            Socket,
     OUT   CONST VOID        **CpuServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPU_LOGICAL_ID CpuFamilyRevision;

  GetLogicalIdOfSocket (Socket, &CpuFamilyRevision, StdHeader);
  GetFeatureServicesFromLogicalId (FamilyTable, &CpuFamilyRevision, CpuServices, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Retrieves a pointer to the executing core's family specific services structure.
 *
 *  @param[out]     FunctionTable      The Processor's Family Specific services.
 *  @param[in]      StdHeader          Handle of Header for calling lib functions and services.
 *
 */
VOID
GetCpuServicesOfCurrentCore (
     OUT   CONST CPU_SPECIFIC_SERVICES **FunctionTable,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  GetFeatureServicesOfCurrentCore (&CpuSupportedFamiliesTable,
                                   (const VOID **)FunctionTable,
                                   StdHeader);
  if (*FunctionTable == NULL) {
    *FunctionTable = &cpuNullServices;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Retrieves a pointer to the family specific services structure for a processor
 *  with the given logical ID.
 *
 *  @param[in]      FamilyTable        The table to search in.
 *  @param[out]     CpuServices        The Processor's Family Specific services.
 *  @param[in]      StdHeader          Handle of Header for calling lib functions and services.
 *
 */
VOID
GetFeatureServicesOfCurrentCore (
  IN       CPU_FAMILY_SUPPORT_TABLE *FamilyTable,
     OUT   CONST VOID        **CpuServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPU_LOGICAL_ID CpuFamilyRevision;

  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetFeatureServicesFromLogicalId (FamilyTable, &CpuFamilyRevision, CpuServices, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Retrieves a pointer to the family specific services structure for a processor
 *  with the given logical ID.
 *
 *  @param[in]      LogicalId          The Processor's logical ID.
 *  @param[out]     FunctionTable      The Processor's Family Specific services.
 *  @param[in]      StdHeader          Handle of Header for calling lib functions and services.
 *
 */
VOID
GetCpuServicesFromLogicalId (
  IN       CPU_LOGICAL_ID *LogicalId,
     OUT   CONST CPU_SPECIFIC_SERVICES **FunctionTable,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  GetFeatureServicesFromLogicalId (&CpuSupportedFamiliesTable,
                                   LogicalId,
                                   (const VOID **)FunctionTable,
                                   StdHeader);
  if (*FunctionTable == NULL) {
    *FunctionTable = &cpuNullServices;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Retrieves a pointer to the family specific services structure for a processor
 *  with the given logical ID.
 *
 *  @param[in]      FamilyTable        The table to search in.
 *  @param[in]      LogicalId          The Processor's logical ID.
 *  @param[out]     CpuServices        The Processor's Family Specific services.
 *  @param[in]      StdHeader          Handle of Header for calling lib functions and services.
 *
 */
VOID
GetFeatureServicesFromLogicalId (
  IN       CPU_FAMILY_SUPPORT_TABLE *FamilyTable,
  IN       CPU_LOGICAL_ID    *LogicalId,
     OUT   CONST VOID        **CpuServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  GetCpuServices (FamilyTable, &LogicalId->Family, CpuServices, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Finds a family match in the given table, and returns the pointer to the
 *  appropriate table.  If no match is found in the table, NULL will be returned.
 *
 *  @param[in]      FamilyTable        The table to search in.
 *  @param[in]      MatchData          Family data that must match.
 *  @param[out]     CpuServices        The Processor's Family Specific services.
 *  @param[in]      StdHeader          Handle of Header for calling lib functions and services.
 *
 */
VOID
STATIC
GetCpuServices (
  IN       CPU_FAMILY_SUPPORT_TABLE *FamilyTable,
  IN       UINT64            *MatchData,
     OUT   CONST VOID        **CpuServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN IsFamily;
  UINT8   i;
  UINT8   NumberOfFamiliesSupported;
  CONST CPU_SPECIFIC_SERVICES_XLAT *ImageSupportedFamiliesPtr;

  ImageSupportedFamiliesPtr = FamilyTable->FamilyTable;
  NumberOfFamiliesSupported = FamilyTable->Elements;
  IsFamily = FALSE;
  for (i = 0; i < NumberOfFamiliesSupported; i++) {
    if ((ImageSupportedFamiliesPtr[i].Family & *MatchData) != 0) {
      IsFamily = TRUE;
      break;
    }
  }
  if (IsFamily) {
    *CpuServices = ImageSupportedFamiliesPtr[i].TablePtr;
  } else {
    *CpuServices = NULL;
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Used to stub out various family specific tables of information.
 *
 *  @param[in]      FamilySpecificServices  The current Family Specific Services.
 *  @param[in]      Empty                   NULL, to indicate no data.
 *  @param[out]     NumberOfElements        Zero, to indicate no data.
 *  @param[in]      StdHeader               Handle of Header for calling lib functions and services.
 *
 */
VOID
GetEmptyArray (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   CONST VOID **Empty,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  *NumberOfElements = 0;
  *Empty = NULL;
}
