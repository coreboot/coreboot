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
 * @e \$Revision: 49028 $   @e \$Date: 2011-03-16 09:20:07 +0800 (Wed, 16 Mar 2011) $
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuLateInit.h"
#include "cpuF12PowerMgmt.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X12_CPUF12DMI_FILECODE

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
DmiF12GetInfo (
  IN OUT   CPU_TYPE_INFO *CpuInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT8
DmiF12GetVoltage (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT16
DmiF12GetMaxSpeed (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT16
DmiF12GetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
DmiF12GetMemInfo (
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
 *  DmiF12GetInfo
 *
 *    Get CPU type information
 *
 *    @param[in,out]  CpuInfoPtr     Pointer to CPU_TYPE_INFO struct.
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
VOID
DmiF12GetInfo (
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
  case LN_SOCKET_FP1:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_NONE;
    break;
  case LN_SOCKET_FS1:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_FS1;
    break;
  case LN_SOCKET_FM1:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_FM1;
    break;
  default:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_UNKNOWN;
    break;
  }

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF12GetVoltage
 *
 *    Get the voltage value according to SMBIOS SPEC's requirement.
 *
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval  Voltage   - CPU Voltage.
 *
 */
UINT8
DmiF12GetVoltage (
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
 *  DmiF12GetMaxSpeed
 *
 *    Get the Max Speed
 *
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval  MaxSpeed   - CPU Max Speed.
 *
 */
UINT16
DmiF12GetMaxSpeed (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8 NumBoostStates;
  UINT32 P0Frequency;
  UINT32 PciData;
  PCI_ADDR PciAddress;
  PSTATE_CPU_FAMILY_SERVICES  *FamilyServices;

  FamilyServices = NULL;
  GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, (const VOID **) &FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_4, 0x15C);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
  NumBoostStates = (UINT8) ((PciData >> 2) & 7);

  FamilyServices->GetPstateFrequency (FamilyServices, NumBoostStates, &P0Frequency, StdHeader);
  return ((UINT16) P0Frequency);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF12GetExtClock
 *
 *    Get the external clock Speed
 *
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval  ExtClock   - CPU external clock Speed.
 *
 */
UINT16
DmiF12GetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  return (EXTERNAL_CLOCK_100MHZ);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF12GetMemInfo
 *
 *    Get memory information.
 *
 *    @param[in,out]  CpuGetMemInfoPtr      Pointer to CPU_GET_MEM_INFO struct.
 *    @param[in]      StdHeader             Standard Head Pointer
 *
 */
VOID
DmiF12GetMemInfo (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  // Llano does NOT support ECC DIMM
  CpuGetMemInfoPtr->EccCapable = FALSE;
  // Partition Row Position - 0 is for dual channel memory
  CpuGetMemInfoPtr->PartitionRowPosition = 0;
}

/*---------------------------------------------------------------------------------------
 * Processor Family Table
 *
 * Note: 'x' means we don't care this field
 *       047h = "E-Series"
 *       048h = "A-Series"
 *       002h = "Unknown"
 *-------------------------------------------------------------------------------------*/
CONST DMI_BRAND_ENTRY ROMDATA Family12BrandList[] =
{
  // Brand --> DMI ID translation table
  // PackageType, PgOfBrandId, NumberOfCores, String1ofBrandId, ValueSetToDmiTable
  {1, 0, 0, 1, 0x48},
  {1, 0, 1, 1, 0x48},
  {1, 0, 0, 2, 0x48},
  {1, 0, 1, 2, 0x48},
  {1, 0, 0, 3, 0x48},
  {1, 0, 1, 3, 0x48},
  {1, 0, 0, 4, 0x48},
  {1, 0, 1, 4, 0x48},
  {1, 0, 0, 5, 0x47},
  {1, 0, 1, 5, 0x47},
  {1, 0, 0, 6, 0x47},
  {1, 0, 1, 6, 0x47},
  {1, 0, 0, 7, 0x47},
  {1, 0, 1, 7, 0x47},
  {1, 0, 2, 1, 0x48},
  {1, 0, 3, 1, 0x48},
  {1, 0, 2, 2, 0x48},
  {1, 0, 3, 2, 0x48},
  {1, 0, 2, 3, 0x48},
  {1, 0, 3, 3, 0x48},
  {1, 0, 2, 4, 0x48},
  {1, 0, 3, 4, 0x48},
  {2, 0, 0, 1, 0x47},
  {2, 0, 0, 4, 0x47},
  {2, 0, 1, 1, 0x48},
  {2, 0, 1, 2, 0x47},
  {2, 0, 1, 5, 0x48},
  {2, 0, 1, 6, 0x48},
  {2, 0, 1, 7, 0x47},
  {2, 0, 2, 1, 0x48},
  {2, 0, 2, 4, 0x48},
  {2, 0, 2, 5, 0x48},
  {2, 0, 2, 6, 0x48},
  {2, 0, 3, 1, 0x48},
  {2, 0, 3, 2, 0x48},
  {2, 0, 3, 5, 0x48},
  {2, 0, 3, 6, 0x48},
  {2, 0, 3, 7, 0x48},
  {2, 0, 3, 8, 0x48},
  {'x', 'x', 'x', 'x', P_FAMILY_UNKNOWN}
};

CONST PROC_FAMILY_TABLE ROMDATA ProcFamily12DmiTable =
{
  AMD_FAMILY_12,                  // ID for Family 12h
  &DmiF12GetInfo,                 // Transfer vectors for family
  &DmiGetT4ProcFamilyFromBrandId, // Get type 4 processor family information from CPUID_8000_0001_EBX[BrandId]
  &DmiF12GetVoltage,              //   specific routines (above)
  &DmiF12GetMaxSpeed,
  &DmiF12GetExtClock,
  &DmiF12GetMemInfo,              // Get memory information
  (sizeof (Family12BrandList) / sizeof (Family12BrandList[0])), // Number of entries in following table
  &Family12BrandList[0]
};


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
