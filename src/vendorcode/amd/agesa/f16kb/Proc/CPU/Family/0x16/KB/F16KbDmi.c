/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD DMI Record Creation API, and related functions for Family16h Kabini.
 *
 * Contains code that produce the DMI related information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16/KB
 * @e \$Revision: 87400 $   @e \$Date: 2013-02-01 12:14:44 -0600 (Fri, 01 Feb 2013) $
 *
 */
/*****************************************************************************
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
#include "cpuPstateTables.h"
#include "cpuLateInit.h"
#include "cpuF16Dmi.h"
#include "cpuF16PowerMgmt.h"
#include "F16KbPowerMgmt.h"
#include "cpuServices.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_KB_F16KBDMI_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 * Processor Family Table
 *
 *-------------------------------------------------------------------------------------*/
CONST CHAR8 ROMDATA str_A6[] = "AMD A6-";
CONST CHAR8 ROMDATA str_A4[] = "AMD A4-";
CONST CHAR8 ROMDATA str_E2[] = "AMD E2-";
CONST CHAR8 ROMDATA str_E1[] = "AMD E1-";
CONST CHAR8 ROMDATA str_GX[] = "AMD GX-";
/*---------------------------------------------------------------------------------------
 * Processor Family Table
 *
 *       047h = "E-Series"
 *       048h = "A-Series"
 *       049h = "GX-Series"
 *-------------------------------------------------------------------------------------*/
CONST CPU_T4_PROC_FAMILY ROMDATA F16KbFT3T4ProcFamily[] =
{
  {str_A6, 0x48},
  {str_A4, 0x48},
  {str_E2, 0x47},
  {str_E1, 0x47},
  {str_GX, 0x49},
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
DmiF16KbGetInfo (
  IN OUT   CPU_TYPE_INFO *CpuInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
DmiF16KbGetT4ProcFamily (
  IN OUT   UINT8 *T4ProcFamily,
  IN       PROC_FAMILY_TABLE *CpuDmiProcFamilyTable,
  IN       CPU_TYPE_INFO *CpuInfo,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT8
DmiF16KbGetVoltage (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
DmiF16KbGetMemInfo (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT16
DmiF16KbGetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF16KbGetInfo
 *
 *    Get CPU type information
 *
 *    @param[in,out]  CpuInfoPtr     Pointer to CPU_TYPE_INFO struct.
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
VOID
DmiF16KbGetInfo (
  IN OUT   CPU_TYPE_INFO *CpuInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
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
  case KB_SOCKET_FT3:
    /// Use 'NONE' for BGA package
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_NONE;
    break;
  default:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_UNKNOWN;
    break;
  }

  // L1 Size & Associativity
  LibAmdCpuidRead (AMD_CPUID_TLB_L1Cache, &CpuId, StdHeader);
  CpuInfoPtr->CacheInfo.L1CacheSize = (UINT32) (((CpuId.ECX_Reg >> 24) + (CpuId.EDX_Reg >> 24)) * (CpuInfoPtr->TotalCoreNumber + 1));

  CpuInfoPtr->CacheInfo.L1CacheAssoc = DMI_ASSOCIATIVE_2_WAY; // Per the BKDG, this is hard-coded to 2-Way.

  // L2 Size & Associativity
  LibAmdCpuidRead (AMD_CPUID_L2L3Cache_L2TLB, &CpuId, StdHeader);
  CpuInfoPtr->CacheInfo.L2CacheSize = (UINT32) (CpuId.ECX_Reg >> 16);

  CpuInfoPtr->CacheInfo.L2CacheAssoc = DMI_ASSOCIATIVE_16_WAY; // Per the BKDG, this is hard-coded to 16-Way.

  // L3 Size & Associativity
  CpuInfoPtr->CacheInfo.L3CacheSize = 0;
  CpuInfoPtr->CacheInfo.L3CacheAssoc = DMI_ASSOCIATIVE_UNKNOWN;
  }

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF16KbGetT4ProcFamily
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
DmiF16KbGetT4ProcFamily (
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
  case KB_SOCKET_FT3:
    DmiStringTable = (CONST VOID *) &F16KbFT3T4ProcFamily[0];
    NumberOfDmiString = sizeof (F16KbFT3T4ProcFamily) / sizeof (CPU_T4_PROC_FAMILY);
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
 *  DmiF16KbGetVoltage
 *
 *    Get the voltage value according to SMBIOS SPEC's requirement.
 *
 *    @param[in]       StdHeader      Standard Head Pointer
 *
 *    @retval  Voltage   - CPU Voltage.
 *
 */
UINT8
DmiF16KbGetVoltage (
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
 *  DmiF16KbGetMemInfo
 *
 *    Get memory information.
 *
 *    @param[in,out]  CpuGetMemInfoPtr      Pointer to CPU_GET_MEM_INFO struct.
 *    @param[in]      StdHeader             Standard Head Pointer
 *
 */
VOID
DmiF16KbGetMemInfo (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32 PciData;
  PCI_ADDR PciAddress;

  CpuGetMemInfoPtr->EccCapable = FALSE;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_2, 0x90);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
  // Check if F2x90[DimmEccEn] is set
  if ((PciData & 0x00080000) != 0) {
    CpuGetMemInfoPtr->EccCapable = TRUE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF16KbGetExtClock
 *
 *    Get the external clock Speed
 *
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval  ExtClock   - CPU external clock Speed.
 *
 */
UINT16
DmiF16KbGetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  return (EXTERNAL_CLOCK_100MHZ);
}

/* -----------------------------------------------------------------------------*/
CONST PROC_FAMILY_TABLE ROMDATA ProcFamily16KbDmiTable =
{
// This table is for Processor family 16h Kabini
  AMD_FAMILY_16_KB,               // ID for Family 16h Kabini
  DmiF16KbGetInfo,                // Transfer vectors for family
  DmiF16KbGetT4ProcFamily,        // Get type 4 processor family information
  DmiF16KbGetVoltage,             //   specific routines (above)
  DmiF16GetMaxSpeed,
  DmiF16KbGetExtClock,
  DmiF16KbGetMemInfo,             // Get memory information
  0,
  NULL
};


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
