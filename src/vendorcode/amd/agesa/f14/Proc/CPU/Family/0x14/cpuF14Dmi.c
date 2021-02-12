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
 * @e \$Revision: 38893 $   @e \$Date: 2010-10-01 23:54:37 +0800 (Fri, 01 Oct 2010) $
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
#include "cpuRegisters.h"
#include "OptionDmi.h"
#include "cpuLateInit.h"
#include "cpuF14PowerMgmt.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "cpuF14Utilities.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X14_CPUF14DMI_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
DmiF14GetInfo (
  IN OUT   CPU_TYPE_INFO *CpuInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT8
DmiF14GetVoltage (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT16
DmiF14GetMaxSpeed (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT16
DmiF14GetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
DmiF14GetMemInfo (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF14GetInfo
 *
 *    Get CPU type information
 *
 *    @param[in,out]  CpuInfoPtr     Pointer to CPU_TYPE_INFO struct.
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
VOID
DmiF14GetInfo (
  IN OUT   CPU_TYPE_INFO *CpuInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  CPUID_DATA CpuId;

  LibAmdCpuidRead (AMD_CPUID_FMF, &CpuId, StdHeader);
  CpuInfoPtr->ExtendedFamily = (UINT8) (CpuId.EAX_Reg >> 20) & 0xFF; // bit 27:20
  CpuInfoPtr->ExtendedModel = (UINT8) (CpuId.EAX_Reg >> 16) & 0xF; // bit 19:16
  CpuInfoPtr->BaseFamily = (UINT8) (CpuId.EAX_Reg >> 8) & 0xF; // bit 11:8
  CpuInfoPtr->BaseModel = (UINT8) (CpuId.EAX_Reg >> 4)  & 0xF; // bit 7:4
  CpuInfoPtr->Stepping = (UINT8) (CpuId.EAX_Reg & 0xF); // bit 3:0

  CpuInfoPtr->PackageType = (UINT8) (CpuId.EBX_Reg >> 28) & 0xF; // bit 31:28
  CpuInfoPtr->BrandId.Pg = (UINT8) (CpuId.EBX_Reg >> 15) & 0x1; // bit 15
  CpuInfoPtr->BrandId.String1 = (UINT8) (CpuId.EBX_Reg >> 11) & 0xF; // bit 14:11
  CpuInfoPtr->BrandId.Model = (UINT8) (CpuId.EBX_Reg >> 4) & 0x7F; // bit 10:4
  CpuInfoPtr->BrandId.String2 = (UINT8) (CpuId.EBX_Reg & 0xF); // bit 3:0

  LibAmdCpuidRead (AMD_CPUID_ASIZE_PCCOUNT, &CpuId, StdHeader);
  CpuInfoPtr->TotalCoreNumber = (UINT8) (CpuId.ECX_Reg & 0xFF); // bit 7:0
  CpuInfoPtr->EnabledCoreNumber = (UINT8) (CpuId.ECX_Reg & 0xFF); // bit 7:0

  switch (CpuInfoPtr->PackageType) {
  case ON_SOCKET_FT1:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_NONE;
    break;
  default:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_UNKNOWN;
    break;
  }

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF14GetVoltage
 *
 *    Get the voltage value according to SMBIOS SPEC's requirement.
 *
 *    @param[in]     StdHeader      Standard Head Pointer
 *
 *    @retval  Voltage   - CPU Voltage.
 *
 */
UINT8
DmiF14GetVoltage (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8     MaxVid;
  UINT8     Voltage;
  UINT8     NumberBoostStates;
  UINT64    MsrData;
  PCI_ADDR  TempAddr;
  CPU_LOGICAL_ID    CpuFamilyRevision;
  CPB_CTRL_REGISTER CpbCtrl;

  // Voltage = 0x80 + (voltage at boot time * 10)
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  if ((CpuFamilyRevision.Revision & (AMD_F14_ON_Ax | AMD_F14_ON_Bx)) == 0) {
    TempAddr.AddressValue = CPB_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, TempAddr, &CpbCtrl, StdHeader);  // F4x15C
    NumberBoostStates = (UINT8) CpbCtrl.NumBoostStates;
  } else {
    NumberBoostStates = 0;
  }

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
 *  DmiF14GetMaxSpeed
 *
 *    Get the Max Speed
 *
 *    @param[in]     StdHeader      Standard Head Pointer
 *
 *    @retval  MaxSpeed   - CPU Max Speed.
 *
 */
UINT16
DmiF14GetMaxSpeed (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8  NumBoostStates;
  UINT32 P0Frequency;
  UINT32 PciData;
  PCI_ADDR PciAddress;
  PSTATE_CPU_FAMILY_SERVICES  *FamilyServices;
  CPU_LOGICAL_ID  CpuFamilyRevision;

  FamilyServices = NULL;
  GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, (const VOID **)&FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  if ((CpuFamilyRevision.Revision & (AMD_F14_ON_Ax | AMD_F14_ON_Bx)) == 0) {
    PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_4, 0x15C);
    LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
    NumBoostStates = (UINT8) ((PciData >> 2) & 7);
  } else {
    NumBoostStates = 0;
  }

  FamilyServices->GetPstateFrequency (FamilyServices, NumBoostStates, &P0Frequency, StdHeader);
  return ((UINT16) P0Frequency);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF14GetExtClock
 *
 *    Get the external clock Speed
 *
 *    @param[in, out] StdHeader      Standard Head Pointer
 *
 *    @retval  ExtClock   - CPU external clock Speed.
 *
 */
UINT16
DmiF14GetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  return (EXTERNAL_CLOCK_100MHZ);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF14GetMemInfo
 *
 *    Get memory information.
 *
 *    @param[in,out]  CpuGetMemInfoPtr      Pointer to CPU_GET_MEM_INFO struct.
 *    @param[in]      StdHeader             Standard Head Pointer
 *
 */
VOID
DmiF14GetMemInfo (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  // Ontario only has one DCT and does NOT support ECC DIMM
  CpuGetMemInfoPtr->EccCapable = FALSE;
  // Partition Row Position - 2 is for single channel memory
  CpuGetMemInfoPtr->PartitionRowPosition = 2;
}

/*---------------------------------------------------------------------------------------
 * Processor Family Table
 *
 * Note: 'x' means we don't care this field
 *       046h = "AMD C-Series Processor"
 *       047h = "AMD E-Series Processor"
 *       048h = "AMD S-Series Processor"
 *       049h = "AMD G-Series Processor"
 *       002h = "Unknown"
 *-------------------------------------------------------------------------------------*/
CONST DMI_BRAND_ENTRY ROMDATA Family14BrandList[] =
{
  // Brand --> DMI ID translation table
  // PackageType, PgOfBrandId, NumberOfCores, String1ofBrandId, ValueSetToDmiTable
  {0, 0, 'x', 1, 0x46},
  {0, 0, 'x', 2, 0x47},
  {0, 0, 'x', 4, 0x49},
  {'x', 'x', 'x', 'x', 0x02}
};

CONST PROC_FAMILY_TABLE ROMDATA ProcFamily14DmiTable =
{
  AMD_FAMILY_14,                  // ID for Family 14h
  &DmiF14GetInfo,                 // Transfer vectors for family
  &DmiF14GetVoltage,              //   specific routines (above)
  &DmiF14GetMaxSpeed,
  &DmiF14GetExtClock,
  &DmiF14GetMemInfo,              // Get memory information
  ARRAY_SIZE(Family14BrandList), // Number of entries in following table
  &Family14BrandList[0]
};


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
