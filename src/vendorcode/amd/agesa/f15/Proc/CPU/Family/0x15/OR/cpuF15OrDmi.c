/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD DMI Record Creation API, and related functions for Fmaily15h Orichi.
 *
 * Contains code that produce the DMI related information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 58290 $   @e \$Date: 2011-08-25 00:02:47 -0600 (Thu, 25 Aug 2011) $
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
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "cpuLateInit.h"
#include "cpuF15Dmi.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "cpuServices.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_CPUF15ORDMI_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
CONST CHAR8 ROMDATA str_Opteron_62[] = "AMD Opteron(tm) Processor 62";
CONST CHAR8 ROMDATA str_Opteron_42[] = "AMD Opteron(tm) Processor 42";
CONST CHAR8 ROMDATA str_Opteron_3[] = "AMD Opteron(tm) Processor 3";
CONST CHAR8 ROMDATA str_FX_AM3[] = "AMD FX(tm)-";
/*---------------------------------------------------------------------------------------
 * Processor Family Table
 *       03Dh = "AMD Opteron(TM) 6200 Processor Family"
 *       03Eh = "AMD Opteron(TM) 4200 Processor Family"
 *       03Fh = "AMD FX(TM) Series Processor"
 *-------------------------------------------------------------------------------------*/
CONST CPU_T4_PROC_FAMILY ROMDATA F15OrG34T4ProcFamily[] =
{
  {str_Opteron_62, 0x3D}
};

CONST CPU_T4_PROC_FAMILY ROMDATA F15OrC32T4ProcFamily[] =
{
  {str_Opteron_42, 0x3E}
};

CONST CPU_T4_PROC_FAMILY ROMDATA F15OrAM3T4ProcFamily[] =
{
  {str_FX_AM3, 0x3F},
  {str_Opteron_3, 0xE4}
};
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
DmiF15OrGetInfo (
  IN OUT   CPU_TYPE_INFO *CpuInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
DmiF15OrGetT4ProcFamily (
  IN OUT   UINT8 *T4ProcFamily,
  IN       PROC_FAMILY_TABLE *CpuDmiProcFamilyTable,
  IN       CPU_TYPE_INFO *CpuInfo,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT8
DmiF15OrGetVoltage (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
DmiF15OrGetMemInfo (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT16
DmiF15OrGetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF15OrGetInfo
 *
 *    Get CPU type information
 *
 *    @param[in,out]  CpuInfoPtr     Pointer to CPU_TYPE_INFO struct.
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
VOID
DmiF15OrGetInfo (
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
  // Family 15h Orochi doesn't have CPUID_8000_0001_EBX[BrandId]
  CpuInfoPtr->BrandId.Pg = 0;
  CpuInfoPtr->BrandId.String1 = 0;
  CpuInfoPtr->BrandId.Model = 0;
  CpuInfoPtr->BrandId.String2 = 0;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  CpuInfoPtr->TotalCoreNumber = FamilySpecificServices->GetNumberOfPhysicalCores (FamilySpecificServices, StdHeader);
  CpuInfoPtr->TotalCoreNumber--;

  LibAmdCpuidRead (AMD_CPUID_ASIZE_PCCOUNT, &CpuId, StdHeader);
  CpuInfoPtr->EnabledCoreNumber = (UINT8) (CpuId.ECX_Reg & 0xFF); // bit 7:0

  switch (CpuInfoPtr->PackageType) {
  case OR_SOCKET_AM3:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_AM3;
    break;
  case OR_SOCKET_G34:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_G34;
    break;
  case OR_SOCKET_C32:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_C32;
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
  LibAmdCpuidRead (AMD_CPUID_TLB_L1Cache, &CpuId, StdHeader);
  CpuInfoPtr->L1CacheSize = (UINT32) (((UINT8) ((CpuId.ECX_Reg >> 24) * NumOfCoresPerCU) + (UINT8) (CpuId.EDX_Reg >> 24)) * (CpuInfoPtr->EnabledCoreNumber + 1) / NumOfCoresPerCU);

  LibAmdCpuidRead (AMD_CPUID_L2L3Cache_L2TLB, &CpuId, StdHeader);
  CpuInfoPtr->L2CacheSize = (UINT32) ((UINT16) (CpuId.ECX_Reg >> 16) * (CpuInfoPtr->EnabledCoreNumber + 1) / NumOfCoresPerCU);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF15OrGetT4ProcFamily
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
DmiF15OrGetT4ProcFamily (
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
  case OR_SOCKET_G34:
    DmiStringTable = (CONST VOID *) &F15OrG34T4ProcFamily[0];
    NumberOfDmiString = sizeof (F15OrG34T4ProcFamily) / sizeof (CPU_T4_PROC_FAMILY);
    break;
  case OR_SOCKET_C32:
    DmiStringTable = (CONST VOID *) &F15OrC32T4ProcFamily[0];
    NumberOfDmiString = sizeof (F15OrC32T4ProcFamily) / sizeof (CPU_T4_PROC_FAMILY);
    break;
  case OR_SOCKET_AM3:
    DmiStringTable = (CONST VOID *) &F15OrAM3T4ProcFamily[0];
    NumberOfDmiString = sizeof (F15OrAM3T4ProcFamily) / sizeof (CPU_T4_PROC_FAMILY);
    break;
  default:
    DmiStringTable = NULL;
    NumberOfDmiString = 0;
    break;
  }

  // Find out which DMI string matches currect processor's name string
  *T4ProcFamily = P_FAMILY_UNKNOWN;
  if ((DmiStringTable != NULL) && (NumberOfDmiString != 0)) {
    for (i = 0; i < NumberOfDmiString; i++) {
      DmiString = (((CPU_T4_PROC_FAMILY *) DmiStringTable)[i]).Stringstart;
      if (IsSourceStrContainTargetStr (NameString, DmiString, StdHeader)) {
        *T4ProcFamily = (((CPU_T4_PROC_FAMILY *) DmiStringTable)[i]).T4ProcFamilySetting;
      }
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF15OrGetVoltage
 *
 *    Get the voltage value according to SMBIOS SPEC's requirement.
 *
 *    @param[in]       StdHeader      Standard Head Pointer
 *
 *    @retval  Voltage   - CPU Voltage.
 *
 */
UINT8
DmiF15OrGetVoltage (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8     MaxVid;
  UINT8     Voltage;
  UINT8     NumberBoostStates;
  UINT32    CurrentNodeNum;
  UINT64    MsrData;
  PCI_ADDR  TempAddr;
  CPB_CTRL_REGISTER CpbCtrl;

  // Voltage = 0x80 + (voltage at boot time * 10)
  GetCurrentNodeNum (&CurrentNodeNum, StdHeader);
  TempAddr.AddressValue = MAKE_SBDFO (0, 0, (24 + CurrentNodeNum), FUNC_4, CPB_CTRL_REG);
  LibAmdPciRead (AccessWidth32, TempAddr, &CpbCtrl, StdHeader);  // F4x15C
  NumberBoostStates = (UINT8) CpbCtrl.NumBoostStates;

  LibAmdMsrRead ((MSR_PSTATE_0 + NumberBoostStates), &MsrData, StdHeader);
  MaxVid = (UINT8) (((PSTATE_MSR *)&MsrData)->CpuVid);


  if ((MaxVid >= 0x7C) && (MaxVid <= 0x7F)) {
    Voltage = 0;
  } else {
    Voltage = (UINT8) ((15500 - (125 * MaxVid) + 500) / 1000);
  }

  Voltage += 0x80;
  return (Voltage);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF15OrGetMemInfo
 *
 *    Get memory information.
 *
 *    @param[in,out]  CpuGetMemInfoPtr      Pointer to CPU_GET_MEM_INFO struct.
 *    @param[in]      StdHeader             Standard Head Pointer
 *
 */
VOID
DmiF15OrGetMemInfo (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32 PciData;
  PCI_ADDR PciAddress;

  CpuGetMemInfoPtr->EccCapable = FALSE;
  // Orochi uses the different way of access to each DCT
  //
  // Switch to DCT 0
  //
  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_1, 0x10C);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
  PciData &= 0xFFFFFFFE;
  LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);

  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_2, 0x90);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
  // Check if F2x90[DimmEccEn] is set
  if ((PciData & 0x00080000) != 0) {
    CpuGetMemInfoPtr->EccCapable = TRUE;
  } else {
    //
    // Switch to DCT 1
    //
    PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_1, 0x10C);
    LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
    PciData |= 0x00000001;
    LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);

    PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_2, 0x90);
    LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
    // Check if F2x90[DimmEccEn] is set
    if ((PciData & 0x00080000) != 0) {
      CpuGetMemInfoPtr->EccCapable = TRUE;
    }
  }
  // Errata #505
  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_1, 0x10C);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
  PciData &= 0xFFFFFFFE;
  LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
  // Partition Row Position - 0 is for dual channel memory
  CpuGetMemInfoPtr->PartitionRowPosition = 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF15OrGetExtClock
 *
 *    Get the external clock Speed
 *
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval  ExtClock   - CPU external clock Speed.
 *
 */
UINT16
DmiF15OrGetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  return (EXTERNAL_CLOCK_DFLT);
}

CONST PROC_FAMILY_TABLE ROMDATA ProcFamily15OrDmiTable =
{
// This table is for Processor family 15h Orochi
  AMD_FAMILY_15_OR,               // ID for Family 15h Orochi
  DmiF15OrGetInfo,                // Transfer vectors for family
  DmiF15OrGetT4ProcFamily,        // Get type 4 processor family information
  DmiF15OrGetVoltage,             //   specific routines (above)
  DmiF15GetMaxSpeed,
  DmiF15OrGetExtClock,
  DmiF15OrGetMemInfo,             // Get memory information
  0,
  NULL
};


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
