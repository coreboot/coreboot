/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD DMI Record Creation API, and related functions.
 *
 * Contains code that produce the DMI related information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 39742 $   @e \$Date: 2010-10-15 02:11:58 +0800 (Fri, 15 Oct 2010) $
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
#include "OptionDmi.h"
#include "cpuLateInit.h"
#include "cpuServices.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "heapManager.h"
#include "Ids.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FEATURE_CPUDMI_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_DMI_CONFIGURATION    OptionDmiConfiguration;  // global user config record

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT16
STATIC
AdjustGranularity (
  IN UINT32 *CacheSizePtr
  );

VOID
STATIC
IntToString (
  IN OUT   CHAR8 *String,
  IN       UINT8 *Integer,
  IN       UINT8 SizeInByte
);

AGESA_STATUS
GetDmiInfoStub (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   DMI_INFO              **DmiTable
  );

AGESA_STATUS
GetDmiInfoMain (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   DMI_INFO              **DmiTable
  );

AGESA_STATUS
ReleaseDmiBufferStub (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
ReleaseDmiBuffer (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CreateDmiRecords
 *
 *  Description:
 *     This function creates DMI/SMBios records pertinent to the processor
 *     SMBIOS type 4, type 7, and type 40.
 *
 *  Parameters:
 *    @param[in, out]  *StdHeader
 *    @param[in, out]  **DmiTable
 *
 *    @retval         AGESA_STATUS
 *
 */

AGESA_STATUS
CreateDmiRecords (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   DMI_INFO              **DmiTable
  )
{
  AGESA_TESTPOINT (TpProcCpuEntryDmi, StdHeader);
  return ((*(OptionDmiConfiguration.DmiFeature)) (StdHeader, DmiTable));
}

/* -----------------------------------------------------------------------------*/
/**
 *  GetDmiInfoStub
 *
 *  Description:
 *     This is the default routine for use when the DMI option is NOT requested.
 *      The option install process will create and fill the transfer vector with
 *      the address of the proper routine (Main or Stub). The link optimizer will
 *      strip out of the .DLL the routine that is not used.
 *
 *  Parameters:
 *    @param[in, out]    *StdHeader
 *    @param[in, out]    **DmiTable
 *
 *    @retval         AGESA_STATUS
 *
 */
AGESA_STATUS
GetDmiInfoStub (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   DMI_INFO              **DmiTable
  )
{
  return  AGESA_UNSUPPORTED;
}

/* -----------------------------------------------------------------------------*/
/**
 *  GetDmiInfoMain
 *
 *  Description:
 *     This is the common routine for getting Dmi type4 and type7 CPU related information.
 *
 *  Parameters:
 *    @param[in, out]  *StdHeader
 *    @param[in, out]  **DmiTable
 *
 *    @retval         AGESA_STATUS
 *
 */
AGESA_STATUS
GetDmiInfoMain (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   DMI_INFO              **DmiTable
  )
{
  UINT8  Socket;
  UINT8  Channel;
  UINT8  Dimm;
  UINT16 Index;
  UINT16 DimmIndex;
  UINT16 NumberOfDimm;
  UINT32 SocketNum;
  UINT64 MsrData;
  UINT64 MsrReg;
  BOOLEAN FamilyNotFound;
  AGESA_STATUS Flag;
  AGESA_STATUS CalledStatus;
  AP_EXE_PARAMS ApParams;
  MEM_DMI_INFO *MemInfo;
  DMI_T17_MEMORY_TYPE MemType;
  DMI_INFO *DmiBufferPtr;
  ALLOCATE_HEAP_PARAMS AllocateHeapParams;
  LOCATE_HEAP_PTR LocateHeapParams;
  CPU_LOGICAL_ID LogicalId;
  PROC_FAMILY_TABLE *ProcData;
  CPU_GET_MEM_INFO CpuGetMemInfo;

  MsrData = 0;
  Flag = TRUE;
  ProcData = NULL;
  MemInfo = NULL;
  DmiBufferPtr = *DmiTable;
  FamilyNotFound = TRUE;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  for (Index = 0; Index < OptionDmiConfiguration.NumEntries; Index++) {
    ProcData = (PROC_FAMILY_TABLE *) ((*OptionDmiConfiguration.FamilyList)[Index]);
    if ((ProcData->ProcessorFamily & LogicalId.Family) != 0) {
      FamilyNotFound = FALSE;
      break;
    }
  }

  if (FamilyNotFound) {
    return AGESA_ERROR;
  }

  if (DmiBufferPtr == NULL) {
    //
    // Allocate a buffer by heap function
    //
    AllocateHeapParams.BufferHandle = AMD_DMI_INFO_BUFFER_HANDLE;
    AllocateHeapParams.RequestedBufferSize = sizeof (DMI_INFO);
    AllocateHeapParams.Persist = HEAP_SYSTEM_MEM;

    if (HeapAllocateBuffer (&AllocateHeapParams, StdHeader) != AGESA_SUCCESS) {
      return AGESA_ERROR;
    }

    DmiBufferPtr = (DMI_INFO *) AllocateHeapParams.BufferPtr;
    *DmiTable = DmiBufferPtr;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  DMI is enabled\n");

  // Fill with 0x00
  LibAmdMemFill (DmiBufferPtr, 0x00, sizeof (DMI_INFO), StdHeader);
  // Set type 4 offset 18h Status to
  //  bit[6] to 1 - CPU Socket Populated
  //  bit[2:0] to 0 - Unknown
  for (SocketNum = 0; SocketNum < MAX_SOCKETS_SUPPORTED; SocketNum++) {
    DmiBufferPtr->T4[SocketNum].T4Status = SOCKET_POPULATED | CPU_STATUS_UNKNOWN;
  }
  //
  // Get CPU information
  //

  // Run GetType4Type7Info on all core0s.
  ApParams.StdHeader = *StdHeader;
  ApParams.FunctionNumber = AP_LATE_TASK_GET_TYPE4_TYPE7;
  ApParams.RelatedDataBlock = (VOID *) DmiBufferPtr;
  ApParams.RelatedBlockLength = sizeof (DMI_INFO);
  CalledStatus = RunLateApTaskOnAllCore0s (&ApParams, StdHeader);
  if (CalledStatus > Flag) {
    Flag = CalledStatus;
  }
  CalledStatus = GetType4Type7Info (&ApParams);
  if (CalledStatus > Flag) {
    Flag = CalledStatus;
  }

  //------------------------------
  // T Y P E 16 17 19 20
  //------------------------------

  LocateHeapParams.BufferHandle = AMD_DMI_MEM_DEV_INFO_HANDLE;
  if (HeapLocateBuffer (&LocateHeapParams, StdHeader) != AGESA_SUCCESS) {
    if (Flag < AGESA_ERROR) {
      Flag = AGESA_ERROR;
    }
  } else {
    NumberOfDimm = *((UINT16 *) (LocateHeapParams.BufferPtr));
    MemType = *((DMI_T17_MEMORY_TYPE *) ((UINT8 *) (LocateHeapParams.BufferPtr) + 2));
    MemInfo = (MEM_DMI_INFO *) ((UINT8 *) (LocateHeapParams.BufferPtr) + 2 + sizeof (DMI_T17_MEMORY_TYPE));
    // TYPE 16
    DmiBufferPtr->T16.Location = 0x03;
    DmiBufferPtr->T16.Use = 0x03;

    // Gather memory information
    ProcData->DmiGetMemInfo (&CpuGetMemInfo, StdHeader);

    if (CpuGetMemInfo.EccCapable) {
      DmiBufferPtr->T16.MemoryErrorCorrection = 0x04;
    } else {
      DmiBufferPtr->T16.MemoryErrorCorrection = 0x03;
    }

    LibAmdMsrRead (TOP_MEM2, &MsrData, StdHeader);
    if (MsrData == 0) {
      LibAmdMsrRead (TOP_MEM, &MsrData, StdHeader);
      DmiBufferPtr->T16.MaximumCapacity = (UINT32) (MsrData >> 10);
    } else {
      DmiBufferPtr->T16.MaximumCapacity = (UINT32) (MsrData >> 10);
    }

    DmiBufferPtr->T16.NumberOfMemoryDevices = NumberOfDimm;

    // TYPE 17
    for (DimmIndex = 0; DimmIndex < NumberOfDimm; DimmIndex++) {
      Socket = (MemInfo + DimmIndex)->Socket;
      Channel = (MemInfo + DimmIndex)->Channel;
      Dimm = (MemInfo + DimmIndex)->Dimm;

      DmiBufferPtr->T17[Socket][Channel][Dimm].TotalWidth = (MemInfo + DimmIndex)->TotalWidth;
      DmiBufferPtr->T17[Socket][Channel][Dimm].DataWidth = (MemInfo + DimmIndex)->DataWidth;
      DmiBufferPtr->T17[Socket][Channel][Dimm].MemorySize = (MemInfo + DimmIndex)->MemorySize;
      DmiBufferPtr->T17[Socket][Channel][Dimm].FormFactor = (MemInfo + DimmIndex)->FormFactor;
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceSet = 0;

      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[0] = 'D';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[1] = 'I';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[2] = 'M';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[3] = 'M';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[4] = ' ';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[5] = Dimm + 0x30;
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[6] = '\0';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[7] = '\0';

      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[0] = 'C';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[1] = 'H';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[2] = 'A';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[3] = 'N';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[4] = 'N';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[5] = 'E';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[6] = 'L';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[7] = ' ';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[8] = Channel + 0x41;
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[9] = '\0';

      DmiBufferPtr->T17[Socket][Channel][Dimm].MemoryType = MemType;
      DmiBufferPtr->T17[Socket][Channel][Dimm].TypeDetail.Synchronous = 1;
      DmiBufferPtr->T17[Socket][Channel][Dimm].Speed = (MemInfo + DimmIndex)->Speed;

      DmiBufferPtr->T17[Socket][Channel][Dimm].ManufacturerIdCode = (MemInfo + DimmIndex)->ManufacturerIdCode;

      IntToString (DmiBufferPtr->T17[Socket][Channel][Dimm].SerialNumber, (MemInfo + DimmIndex)->SerialNumber, (sizeof DmiBufferPtr->T17[Socket][Channel][Dimm].SerialNumber - 1) / 2);

      LibAmdMemCopy (&DmiBufferPtr->T17[Socket][Channel][Dimm].PartNumber, &(MemInfo + DimmIndex)->PartNumber, sizeof (DmiBufferPtr->T17[Socket][Channel][Dimm].PartNumber), StdHeader);
      DmiBufferPtr->T17[Socket][Channel][Dimm].PartNumber[18] = 0;

      DmiBufferPtr->T17[Socket][Channel][Dimm].Attributes = (MemInfo + DimmIndex)->Attributes;
      DmiBufferPtr->T17[Socket][Channel][Dimm].ExtSize = (MemInfo + DimmIndex)->ExtSize;
      DmiBufferPtr->T17[Socket][Channel][Dimm].ConfigSpeed = (MemInfo + DimmIndex)->ConfigSpeed;

      //TYPE 20
      DmiBufferPtr->T20[Socket][Channel][Dimm].StartingAddr = (MemInfo + DimmIndex)->StartingAddr;
      DmiBufferPtr->T20[Socket][Channel][Dimm].EndingAddr = (MemInfo + DimmIndex)->EndingAddr;
      // Partition Row Position - 2 for single channel memory
      //                          0 for dual channel memory
      DmiBufferPtr->T20[Socket][Channel][Dimm].PartitionRowPosition = CpuGetMemInfo.PartitionRowPosition;
      DmiBufferPtr->T20[Socket][Channel][Dimm].InterleavePosition = 0xFF;
      DmiBufferPtr->T20[Socket][Channel][Dimm].InterleavedDataDepth = 0xFF;
    }

    // TYPE 19
    DmiBufferPtr->T19.StartingAddr = 0;

    LibAmdMsrRead (TOP_MEM2, &MsrReg, StdHeader);
    if (MsrReg == 0) {
      LibAmdMsrRead (TOP_MEM, &MsrReg, StdHeader);
      DmiBufferPtr->T19.EndingAddr = (UINT32) (MsrReg >> 10);
    } else if (MsrReg != 0) {
      DmiBufferPtr->T19.EndingAddr = (UINT32) (MsrReg >> 10);
    }

    DmiBufferPtr->T19.PartitionWidth = 0xFF;
  }
  return (Flag);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  GetType4Type7Info
 *
 *  Description:
 *    This routine should be run on core 0 of every socket. It creates DMI type 4 and type 7 tables.
 *
 *  Parameters:
 *    @param[in] ApExeParams   Handle to config for library and services.
 *
 *    @retval         AGESA_STATUS
 *
 *  Processing:
 *
 */
AGESA_STATUS
GetType4Type7Info (
  IN       AP_EXE_PARAMS *ApExeParams
  )
{
  UINT8  ByteIndexInUint64;
  UINT16 Index;
  UINT32 SocketNum;
  UINT32 CacheSize;
  UINT32 IgnoredModule;
  UINT32 IgnoredCore;
  UINT64 MsrData;
  DMI_INFO *DmiBufferPtr;
  AGESA_STATUS IgnoredSts;
  AGESA_STATUS Flag;
  BOOLEAN FamilyNotFound;
  CPUID_DATA CpuId;
  CPU_TYPE_INFO CpuInfo;
  PROC_FAMILY_TABLE   *ProcData;
  CPU_LOGICAL_ID LogicalID;

  Flag = TRUE;
  DmiBufferPtr = (DMI_INFO *) ApExeParams->RelatedDataBlock;
  GetLogicalIdOfCurrentCore (&LogicalID, &ApExeParams->StdHeader);

  ProcData = NULL;
  FamilyNotFound = TRUE;
  for (Index = 0; Index < OptionDmiConfiguration.NumEntries; Index++) {
    ProcData = (PROC_FAMILY_TABLE *) ((*OptionDmiConfiguration.FamilyList)[Index]);
    if ((ProcData->ProcessorFamily & LogicalID.Family) != 0) {
      FamilyNotFound = FALSE;
      break;
    }
  }

  if (FamilyNotFound) {
    return AGESA_ERROR;
  }

  ProcData->DmiGetCpuInfo (&CpuInfo, &ApExeParams->StdHeader);

  // ------------------------------
  // T Y P E 4
  // ------------------------------

  IdentifyCore (&ApExeParams->StdHeader, &SocketNum, &IgnoredModule, &IgnoredCore, &IgnoredSts);

  // Type 4 Offset 0x05, Processor Type
  DmiBufferPtr->T4[SocketNum].T4ProcType = CENTRAL_PROCESSOR;

  // Type 4 Offset 0x06, Processor Family
  for (Index = 0; Index < ProcData->LenBrandList; Index++) {
    if ((ProcData->DmiBrandList[Index].PackageType == 'x' || ProcData->DmiBrandList[Index].PackageType == CpuInfo.PackageType) &&
        (ProcData->DmiBrandList[Index].PgOfBrandId == 'x' || ProcData->DmiBrandList[Index].PgOfBrandId == CpuInfo.BrandId.Pg) &&
        (ProcData->DmiBrandList[Index].NumberOfCores == 'x' || ProcData->DmiBrandList[Index].NumberOfCores == CpuInfo.TotalCoreNumber) &&
        (ProcData->DmiBrandList[Index].String1ofBrandId == 'x' || ProcData->DmiBrandList[Index].String1ofBrandId == CpuInfo.BrandId.String1)) {
      DmiBufferPtr->T4[SocketNum].T4ProcFamily = ProcData->DmiBrandList[Index].ValueSetToDmiTable;
      break;
    }
  }

  if (DmiBufferPtr->T4[SocketNum].T4ProcFamily == P_UPGRADE_UNKNOWN) {
    Flag = AGESA_ERROR;
  }

  // Type4 Offset 0x08, Processor ID
  LibAmdCpuidRead (AMD_CPUID_APICID_LPC_BID, &CpuId, &ApExeParams->StdHeader);
  DmiBufferPtr->T4[SocketNum].T4ProcId.ProcIdLsd = CpuId.EAX_Reg;
  DmiBufferPtr->T4[SocketNum].T4ProcId.ProcIdMsd = CpuId.EDX_Reg;

  // Type4 Offset 0x11, Voltage
  DmiBufferPtr->T4[SocketNum].T4Voltage = ProcData->DmiGetVoltage (&ApExeParams->StdHeader);

  // Type4 Offset 0x12, External Clock
  DmiBufferPtr->T4[SocketNum].T4ExternalClock = ProcData->DmiGetExtClock (&ApExeParams->StdHeader);

  // Type4 Offset 0x14, Max Speed
  DmiBufferPtr->T4[SocketNum].T4MaxSpeed = ProcData->DmiGetMaxSpeed (&ApExeParams->StdHeader);

  // Type4 Offset 0x16, Current Speed
  DmiBufferPtr->T4[SocketNum].T4CurrentSpeed = DmiBufferPtr->T4[SocketNum].T4MaxSpeed;

  // Type4 Offset 0x18, Status
  DmiBufferPtr->T4[SocketNum].T4Status = SOCKET_POPULATED | CPU_STATUS_ENABLED;

  // Type4 Offset 0x19, Processor Upgrade
  DmiBufferPtr->T4[SocketNum].T4ProcUpgrade = CpuInfo.ProcUpgrade;

  // Type4 Offset 0x23, 0x24 and 0x25, Core Count, Core Enabled and Thread Count
  DmiBufferPtr->T4[SocketNum].T4CoreCount   = CpuInfo.TotalCoreNumber + 1;
  DmiBufferPtr->T4[SocketNum].T4CoreEnabled = CpuInfo.EnabledCoreNumber + 1;
  DmiBufferPtr->T4[SocketNum].T4ThreadCount = CpuInfo.EnabledCoreNumber + 1;

  // Type4 Offset 0x26, Processor Characteristics
  DmiBufferPtr->T4[SocketNum].T4ProcCharacteristics = P_CHARACTERISTICS;

  // Type4 Offset 0x28, Processor Family 2
  DmiBufferPtr->T4[SocketNum].T4ProcFamily2 = DmiBufferPtr->T4[SocketNum].T4ProcFamily;

  // Type4 ProcVersion
  for (Index = 0; Index <= 5; Index++) {
    LibAmdMsrRead ((MSR_CPUID_NAME_STRING0 + Index), &MsrData, &ApExeParams->StdHeader);
    for (ByteIndexInUint64 = 0; ByteIndexInUint64 <= 7; ByteIndexInUint64++) {
      DmiBufferPtr->T4[SocketNum].T4ProcVersion[Index * 8 + ByteIndexInUint64] = (UINT8) (MsrData >> (8 * ByteIndexInUint64));
    }
  }

  //------------------------------
  // T Y P E 7
  //------------------------------

  // Type7 Offset 0x05, Cache Configuration
  DmiBufferPtr->T7L1[SocketNum].T7CacheCfg = CACHE_CFG_L1;
  DmiBufferPtr->T7L2[SocketNum].T7CacheCfg = CACHE_CFG_L2;
  DmiBufferPtr->T7L3[SocketNum].T7CacheCfg = CACHE_CFG_L3;

  // Type7 Offset 0x07 and 09, Maximum Cache Size and Installed Size
  LibAmdCpuidRead (AMD_CPUID_TLB_L1Cache, &CpuId, &ApExeParams->StdHeader);

  // Maximum L1 cache size
  CacheSize = (UINT32) (((UINT8) (CpuId.ECX_Reg >> 24) + (UINT8) (CpuId.EDX_Reg >> 24)) * (CpuInfo.EnabledCoreNumber + 1));
  DmiBufferPtr->T7L1[SocketNum].T7MaxCacheSize = AdjustGranularity (&CacheSize);

  // Installed L1 cache size
  DmiBufferPtr->T7L1[SocketNum].T7InstallSize = DmiBufferPtr->T7L1[SocketNum].T7MaxCacheSize;

  // Maximum L2 cache size
  LibAmdCpuidRead (AMD_CPUID_L2L3Cache_L2TLB, &CpuId, &ApExeParams->StdHeader);
  CacheSize = (UINT32) ((UINT16) (CpuId.ECX_Reg >> 16) * (CpuInfo.EnabledCoreNumber + 1));
  DmiBufferPtr->T7L2[SocketNum].T7MaxCacheSize = AdjustGranularity (&CacheSize);

  // Installed L2 cache size
  DmiBufferPtr->T7L2[SocketNum].T7InstallSize = DmiBufferPtr->T7L2[SocketNum].T7MaxCacheSize;

  // Maximum L3 cache size
  CacheSize = ((CpuId.EDX_Reg >> 18) & 0x3FFF) * 512;
  DmiBufferPtr->T7L3[SocketNum].T7MaxCacheSize = AdjustGranularity (&CacheSize);

  // Installed L3 cache size
  DmiBufferPtr->T7L3[SocketNum].T7InstallSize = DmiBufferPtr->T7L3[SocketNum].T7MaxCacheSize;

  // Type7 Offset 0x0B and 0D, Supported SRAM Type and Current SRAM Type
  DmiBufferPtr->T7L1[SocketNum].T7SupportedSramType = SRAM_TYPE;
  DmiBufferPtr->T7L1[SocketNum].T7CurrentSramType = SRAM_TYPE;
  DmiBufferPtr->T7L2[SocketNum].T7SupportedSramType = SRAM_TYPE;
  DmiBufferPtr->T7L2[SocketNum].T7CurrentSramType = SRAM_TYPE;
  DmiBufferPtr->T7L3[SocketNum].T7SupportedSramType = SRAM_TYPE;
  DmiBufferPtr->T7L3[SocketNum].T7CurrentSramType = SRAM_TYPE;

  // Type7 Offset 0x0F, Cache Speed
  DmiBufferPtr->T7L1[SocketNum].T7CacheSpeed = 1;
  DmiBufferPtr->T7L2[SocketNum].T7CacheSpeed = 1;
  DmiBufferPtr->T7L3[SocketNum].T7CacheSpeed = 1;

  // Type7 Offset 0x10, Error Correction Type
  DmiBufferPtr->T7L1[SocketNum].T7ErrorCorrectionType = ERR_CORRECT_TYPE;
  DmiBufferPtr->T7L2[SocketNum].T7ErrorCorrectionType = ERR_CORRECT_TYPE;
  DmiBufferPtr->T7L3[SocketNum].T7ErrorCorrectionType = ERR_CORRECT_TYPE;

  // Type7 Offset 0x11, System Cache Type
  DmiBufferPtr->T7L1[SocketNum].T7SystemCacheType = CACHE_TYPE;
  DmiBufferPtr->T7L2[SocketNum].T7SystemCacheType = CACHE_TYPE;
  DmiBufferPtr->T7L3[SocketNum].T7SystemCacheType = CACHE_TYPE;

  // Type7 Offset 0x12, Associativity
  DmiBufferPtr->T7L1[SocketNum].T7Associativity = ASSOCIATIVE_2_WAY;
  DmiBufferPtr->T7L2[SocketNum].T7Associativity = ASSOCIATIVE_16_WAY;
  if (((CpuId.EDX_Reg >> 12) & 0x0F) == ASSOCIATIVE_16_WAY) {
    DmiBufferPtr->T7L3[SocketNum].T7Associativity = ASSOCIATIVE_16_WAY;
  } else {
    DmiBufferPtr->T7L3[SocketNum].T7Associativity = ASSOCIATIVE_OTHER;
  }
  return (Flag);
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *  AdjustGranularity
 *
 *  Description:
 *    If cache size is greater than or equal to 32M, then set granularity
 *    to 64K. otherwise, set granularity to 1K
 *
 *  Parameters:
 *    @param[in]    *CacheSizePtr
 *
 *    @retval       CacheSize
 *
 *  Processing:
 *
 */
UINT16
STATIC
AdjustGranularity (
  IN UINT32 *CacheSizePtr
  )
{
  UINT16 CacheSize;

  if (*CacheSizePtr >= 0x8000) {
    CacheSize = (UINT16) (*CacheSizePtr / 64);
    CacheSize |= 0x8000;
  } else {
    CacheSize = (UINT16) *CacheSizePtr;
  }

  return (CacheSize);
}

/* -----------------------------------------------------------------------------*/
/**
 *  ReleaseDmiBufferStub
 *
 *  Description:
 *     This is the default routine for use when the DMI option is NOT requested.
 *
 *  Parameters:
 *    @param[in, out]    *StdHeader
 *
 *    @retval         AGESA_STATUS
 *
 */
AGESA_STATUS
ReleaseDmiBufferStub (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader
  )
{
  return  AGESA_UNSUPPORTED;
}

/* -----------------------------------------------------------------------------*/
/**
 *  ReleaseDmiBuffer
 *
 *  Description:
 *     Deallocate DMI buffer
 *
 *  Parameters:
 *    @param[in, out]    *StdHeader
 *
 *    @retval         AGESA_STATUS
 *
 */
AGESA_STATUS
ReleaseDmiBuffer (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader
  )
{
  HeapDeallocateBuffer ((UINT32) AMD_DMI_MEM_DEV_INFO_HANDLE, StdHeader);

  return  AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  IntToString
 *
 *  Description:
 *    Translate UINT array to CHAR array.
 *
 *  Parameters:
 *    @param[in, out]    *String       Pointer to CHAR array
 *    @param[in]         *Integer      Pointer to UINT array
 *    @param[in]         SizeInByte    The size of UINT array
 *
 *  Processing:
 *
 */
VOID
STATIC
IntToString (
  IN OUT   CHAR8 *String,
  IN       UINT8 *Integer,
  IN       UINT8 SizeInByte
  )
{
  UINT8 Index;

  for (Index = 0; Index < SizeInByte; Index++) {
    *(String + Index * 2) = (*(Integer + Index) >> 4) & 0x0F;
    *(String + Index * 2 + 1) = *(Integer + Index) & 0x0F;
  }
  for (Index = 0; Index < (SizeInByte * 2); Index++) {
    if (*(String + Index) >= 0x0A) {
      *(String + Index) += 0x37;
    } else {
      *(String + Index) += 0x30;
    }
  }
  *(String + SizeInByte * 2) = 0x0;
}

