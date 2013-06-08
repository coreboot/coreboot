/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD DMI Record Creation API, and related functions for Family15h Trinity.
 *
 * Contains code that produce the DMI related information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/TN
 * @e \$Revision: 64351 $   @e \$Date: 2012-01-19 03:50:41 -0600 (Thu, 19 Jan 2012) $
 *
 */
/*****************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "cpuPstateTables.h"
#include "cpuLateInit.h"
#include "cpuF15Dmi.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "cpuServices.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_CPUF15TNDMI_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
CONST CHAR8 ROMDATA str_A10[] = "AMD A10-";
CONST CHAR8 ROMDATA str_A8[] = "AMD A8-";
CONST CHAR8 ROMDATA str_A6[] = "AMD A6-";
CONST CHAR8 ROMDATA str_A4[] = "AMD A4-";
CONST CHAR8 ROMDATA str_PhenomII[] = "AMD Phenom(TM) II";
CONST CHAR8 ROMDATA str_AthlonII[] = "AMD Athlon(TM) II";
CONST CHAR8 ROMDATA str_SempronII[] = "AMD Sempron(TM) II";
CONST CHAR8 ROMDATA str_Sempron[] = "AMD Sempron(TM)";
/*---------------------------------------------------------------------------------------
 * Processor Family Table
 *
 *       048h = "A-Series"
 *       0ECh = "AMD Phenom(TM) II Processor Family"
 *       0EDh = "AMD Athlon(tm) II"
 *       085h = "AMD Sempron(tm)"
 *       0E5h = "AMD Sempron(tm) II"

 *-------------------------------------------------------------------------------------*/

CONST CPU_T4_PROC_FAMILY ROMDATA F15TnFP2T4ProcFamily[] =
{
  {str_A10, 0x48},
  {str_A8, 0x48},
  {str_A6, 0x48},
  {str_A4, 0x48},
};

CONST CPU_T4_PROC_FAMILY ROMDATA F15TnFS1T4ProcFamily[] =
{
  {str_A10, 0x48},
  {str_A8, 0x48},
  {str_A6, 0x48},
  {str_A4, 0x48},
};

CONST CPU_T4_PROC_FAMILY ROMDATA F15TnFM2T4ProcFamily[] =
{
  {str_A10, 0x48},
  {str_A8, 0x48},
  {str_A6, 0x48},
  {str_A4, 0x48},
  {str_PhenomII, 0xEC},
  {str_AthlonII, 0xED},
  {str_SempronII, 0xE5},
  {str_Sempron, 0x85},
};
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
DmiF15TnGetInfo (
  IN OUT   CPU_TYPE_INFO *CpuInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
DmiF15TnGetT4ProcFamily (
  IN OUT   UINT8 *T4ProcFamily,
  IN       PROC_FAMILY_TABLE *CpuDmiProcFamilyTable,
  IN       CPU_TYPE_INFO *CpuInfo,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT8
DmiF15TnGetVoltage (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
DmiF15TnGetMemInfo (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT16
DmiF15TnGetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF15TnGetInfo
 *
 *    Get CPU type information
 *
 *    @param[in,out]  CpuInfoPtr     Pointer to CPU_TYPE_INFO struct.
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
VOID
DmiF15TnGetInfo (
  IN OUT   CPU_TYPE_INFO *CpuInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8 NumOfCoresPerCU;
  CPUID_DATA CpuId;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  LibAmdCpuidRead (AMD_CPUID_FMF, &CpuId, StdHeader);
  CpuInfoPtr->ExtendedFamily = (UINT8) (CpuId.EAX_Reg >> 20) & 0xFF; // bit 27:20
  CpuInfoPtr->ExtendedModel = (UINT8) (CpuId.EAX_Reg >> 16) & 0xF; // bit 19:16
  CpuInfoPtr->BaseFamily = (UINT8) (CpuId.EAX_Reg >> 8) & 0xF; // bit 11:8
  CpuInfoPtr->BaseModel = (UINT8) (CpuId.EAX_Reg >> 4)  & 0xF; // bit 7:4
  CpuInfoPtr->Stepping = (UINT8) (CpuId.EAX_Reg & 0xF); // bit 3:0

  CpuInfoPtr->PackageType = (UINT8) (CpuId.EBX_Reg >> 28) & 0xF; // bit 31:28

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  CpuInfoPtr->TotalCoreNumber = FamilySpecificServices->GetNumberOfPhysicalCores (FamilySpecificServices, StdHeader);
  CpuInfoPtr->TotalCoreNumber--;

  LibAmdCpuidRead (AMD_CPUID_ASIZE_PCCOUNT, &CpuId, StdHeader);
  CpuInfoPtr->EnabledCoreNumber = (UINT8) (CpuId.ECX_Reg & 0xFF); // bit 7:0

  switch (CpuInfoPtr->PackageType) {
  case TN_SOCKET_FP2:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_NONE;
    break;
  case TN_SOCKET_FS1:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_FS1;
    break;
  case TN_SOCKET_FM2:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_FM2;
    break;
  default:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_UNKNOWN;
    break;
  }

  switch (GetComputeUnitMapping (StdHeader)) {
  case AllCoresMapping:
    NumOfCoresPerCU = 1;
    break;
  case EvenCoresMapping:
    NumOfCoresPerCU = 2;
    break;
  default:
    NumOfCoresPerCU = 2;
  }
  // L1 Size & Associativity
  LibAmdCpuidRead (AMD_CPUID_TLB_L1Cache, &CpuId, StdHeader);
  CpuInfoPtr->CacheInfo.L1CacheSize = (UINT32) (((UINT8) ((CpuId.ECX_Reg >> 24) * NumOfCoresPerCU) + (UINT8) (CpuId.EDX_Reg >> 24)) * (CpuInfoPtr->EnabledCoreNumber + 1) / NumOfCoresPerCU);

  CpuInfoPtr->CacheInfo.L1CacheAssoc = DMI_ASSOCIATIVE_2_WAY; // Per the BKDG, this is hard-coded to 2-Way.

  // L2 Size & Associativity
  LibAmdCpuidRead (AMD_CPUID_L2L3Cache_L2TLB, &CpuId, StdHeader);
  CpuInfoPtr->CacheInfo.L2CacheSize = (UINT32) ((UINT16) (CpuId.ECX_Reg >> 16) * (CpuInfoPtr->EnabledCoreNumber + 1) / NumOfCoresPerCU);

  CpuInfoPtr->CacheInfo.L2CacheAssoc = DMI_ASSOCIATIVE_16_WAY; // Per the BKDG, this is hard-coded to 16-Way.

  // L3 Size & Associativity
  CpuInfoPtr->CacheInfo.L3CacheSize = 0;
  CpuInfoPtr->CacheInfo.L3CacheAssoc = DMI_ASSOCIATIVE_UNKNOWN;
  }

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF15TnGetT4ProcFamily
 *
 *    Get type 4 processor family information
 *
 *    @param[in,out]  T4ProcFamily   Pointer to type 4 processor family information.
 *    @param[in]      *CpuDmiProcFamilyTable  Pointer to DMI family special service
 *    @param[in]      *CpuInfo       Pointer to CPU_TYPE_INFO struct
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
VOID
DmiF15TnGetT4ProcFamily (
  IN OUT   UINT8 *T4ProcFamily,
  IN       PROC_FAMILY_TABLE *CpuDmiProcFamilyTable,
  IN       CPU_TYPE_INFO *CpuInfo,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  CHAR8 NameString[49];
  CONST CHAR8 *DmiString;
  CONST VOID  *DmiStringTable;
  UINT8 NumberOfDmiString;
  UINT8 i;

  // Get name string from MSR_C001_00[30:35]
  GetNameString (NameString, StdHeader);
  // Get DMI String
  DmiStringTable = NULL;
  switch (CpuInfo->PackageType) {
  case TN_SOCKET_FP2:
    DmiStringTable = (CONST VOID *) &F15TnFP2T4ProcFamily[0];
    NumberOfDmiString = sizeof (F15TnFP2T4ProcFamily) / sizeof (CPU_T4_PROC_FAMILY);
    break;
  case TN_SOCKET_FS1:
    DmiStringTable = (CONST VOID *) &F15TnFS1T4ProcFamily[0];
    NumberOfDmiString = sizeof (F15TnFS1T4ProcFamily) / sizeof (CPU_T4_PROC_FAMILY);
    break;
  case TN_SOCKET_FM2:
    DmiStringTable = (CONST VOID *) &F15TnFM2T4ProcFamily[0];
    NumberOfDmiString = sizeof (F15TnFM2T4ProcFamily) / sizeof (CPU_T4_PROC_FAMILY);
    break;
  default:
    DmiStringTable = NULL;
    NumberOfDmiString = 0;
    break;
  }

  // Find out which DMI string matches current processor's name string
  *T4ProcFamily = P_FAMILY_UNKNOWN;
  if ((DmiStringTable != NULL) && (NumberOfDmiString != 0)) {
    for (i = 0; i < NumberOfDmiString; i++) {
      DmiString = (((CPU_T4_PROC_FAMILY *) DmiStringTable)[i]).Stringstart;
      if (IsSourceStrContainTargetStr (NameString, DmiString, StdHeader)) {
        *T4ProcFamily = (((CPU_T4_PROC_FAMILY *) DmiStringTable)[i]).T4ProcFamilySetting;
        break;
      }
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF15TnGetVoltage
 *
 *    Get the voltage value according to SMBIOS SPEC's requirement.
 *
 *    @param[in]       StdHeader      Standard Head Pointer
 *
 *    @retval  Voltage   - CPU Voltage.
 *
 */
UINT8
DmiF15TnGetVoltage (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8     MaxVid;
  UINT8     Voltage;
  UINT8     NumberBoostStates;
  UINT64    MsrData;
  PCI_ADDR  TempAddr;
  CPB_CTRL_REGISTER CpbCtrl;

  // Voltage = 0x80 + (voltage at boot time * 10)
  TempAddr.AddressValue = CPB_CTRL_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, TempAddr, &CpbCtrl, StdHeader);  // F4x15C
  NumberBoostStates = (UINT8) CpbCtrl.NumBoostStates;

  LibAmdMsrRead ((MSR_PSTATE_0 + NumberBoostStates), &MsrData, StdHeader);
  MaxVid = (UINT8) (((PSTATE_MSR *)&MsrData)->CpuVid);

  if ((MaxVid >= 0xF8) && (MaxVid <= 0xFF)) {
    Voltage = 0;
  } else {
    Voltage = (UINT8) ((155000L - (625 * MaxVid) + 5000) / 10000);
  }

  Voltage += 0x80;
  return (Voltage);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF15TnGetMemInfo
 *
 *    Get memory information.
 *
 *    @param[in,out]  CpuGetMemInfoPtr      Pointer to CPU_GET_MEM_INFO struct.
 *    @param[in]      StdHeader             Standard Head Pointer
 *
 */
VOID
DmiF15TnGetMemInfo (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  CpuGetMemInfoPtr->EccCapable = FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF15TnGetExtClock
 *
 *    Get the external clock Speed
 *
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval  ExtClock   - CPU external clock Speed.
 *
 */
UINT16
DmiF15TnGetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  return (EXTERNAL_CLOCK_100MHZ);
}

/* -----------------------------------------------------------------------------*/
CONST PROC_FAMILY_TABLE ROMDATA ProcFamily15TnDmiTable =
{
// This table is for Processor family 15h Trinity
  AMD_FAMILY_15_TN,               // ID for Family 15h Trinity
  DmiF15TnGetInfo,                // Transfer vectors for family
  DmiF15TnGetT4ProcFamily,        // Get type 4 processor family information
  DmiF15TnGetVoltage,             //   specific routines (above)
  DmiF15GetMaxSpeed,
  DmiF15TnGetExtClock,
  DmiF15TnGetMemInfo,             // Get memory information
  0,
  NULL
};


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
