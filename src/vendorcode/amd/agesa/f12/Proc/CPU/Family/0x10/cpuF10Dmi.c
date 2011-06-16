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
 * @e \$Revision: 46895 $   @e \$Date: 2011-02-11 13:13:34 +0800 (Fri, 11 Feb 2011) $
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
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "cpuLateInit.h"
#include "cpuF10PowerMgmt.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuF10Utilities.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10DMI_FILECODE

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
STATIC
F10Translate7BitVidTo6Bit (
  IN OUT   UINT8 * MaxVidPtr
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF10GetInfo
 *
 *    Get CPU type information
 *
 *    @param[in,out]  CpuInfoPtr     Pointer to CPU_TYPE_INFO struct.
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
VOID
DmiF10GetInfo (
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
  CpuInfoPtr->BrandId.Pg = (UINT8) (CpuId.EBX_Reg >> 15) & 0x1; // bit 15
  CpuInfoPtr->BrandId.String1 = (UINT8) (CpuId.EBX_Reg >> 11) & 0xF; // bit 14:11
  CpuInfoPtr->BrandId.Model = (UINT8) (CpuId.EBX_Reg >> 4) & 0x7F; // bit 10:4
  CpuInfoPtr->BrandId.String2 = (UINT8) (CpuId.EBX_Reg & 0xF); // bit 3:0

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  CpuInfoPtr->TotalCoreNumber = FamilySpecificServices->GetNumberOfPhysicalCores (FamilySpecificServices, StdHeader);
  CpuInfoPtr->TotalCoreNumber--;

  LibAmdCpuidRead (AMD_CPUID_ASIZE_PCCOUNT, &CpuId, StdHeader);
  CpuInfoPtr->EnabledCoreNumber = (UINT8) (CpuId.ECX_Reg & 0xFF); // bit 7:0

  switch (CpuInfoPtr->PackageType) {
  case DR_SOCKET_1207:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_F1207;
    break;
  case DR_SOCKET_AM3:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_AM3;
    break;
  case DR_SOCKET_S1G3:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_S1GX;
    break;
  case DR_SOCKET_G34:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_G34;
    break;
  case DR_SOCKET_ASB2:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_NONE;
    break;
  case DR_SOCKET_C32:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_C32;
    break;
  default:
    CpuInfoPtr->ProcUpgrade = P_UPGRADE_UNKNOWN;
    break;
  }

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF10GetVoltage
 *
 *    Get the voltage value according to SMBIOS SPEC's requirement.
 *
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval  Voltage   - CPU Voltage.
 *
 */
UINT8
DmiF10GetVoltage (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8     MaxVid;
  UINT8     Voltage;
  UINT8     NumberBoostStates;
  UINT32    Pvimode;
  UINT32    CurrentNodeNum;
  UINT64    MsrData;
  PCI_ADDR  TempAddr;
  CPU_LOGICAL_ID    CpuFamilyRevision;
  CPB_CTRL_REGISTER CpbCtrl;

  // Voltage = 0x80 + (voltage at boot time * 10)
  GetCurrentNodeNum (&CurrentNodeNum, StdHeader);
  TempAddr.AddressValue = MAKE_SBDFO (0, 0, (24 + CurrentNodeNum), FUNC_3, PW_CTL_MISC_REG);
  LibAmdPciReadBits (TempAddr, 8, 8, &Pvimode, (VOID *)StdHeader);
  //Pvimode is a 1-bit register field:  1-PVI 0-SVI

  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  if ((CpuFamilyRevision.Revision & AMD_F10_PH_ALL) != 0) {
    TempAddr.AddressValue = MAKE_SBDFO (0, 0, (24 + CurrentNodeNum), FUNC_4, CPB_CTRL_REG);
    LibAmdPciRead (AccessWidth32, TempAddr, &CpbCtrl, StdHeader);  // F4x15C
    NumberBoostStates = (UINT8) CpbCtrl.NumBoostStates;
  } else {
    NumberBoostStates = 0;
  }

  LibAmdMsrRead ((MSR_PSTATE_0 + NumberBoostStates), &MsrData, StdHeader);
  MaxVid = (UINT8) (((PSTATE_MSR *)&MsrData)->CpuVid);

  if (Pvimode) {
    // PVI mode
    F10Translate7BitVidTo6Bit (&MaxVid);
    if (MaxVid >= 0x20) {
      Voltage = (UINT8) ((7625 - (125 * (MaxVid - 0x20)) + 500) / 1000);
    } else {
      Voltage = (UINT8) ((1550 - (25 * MaxVid) + 50) / 100);
    }
  } else {
    // is SVI mode
    if ((MaxVid >= 0x7C) && (MaxVid <= 0x7F)) {
      Voltage = 0;
    } else {
      Voltage = (UINT8) ((15500 - (125 * MaxVid) + 500) / 1000);
    }
  }

  Voltage += 0x80;
  return (Voltage);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF10GetMaxSpeed
 *
 *    Get the Max Speed
 *
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval  MaxSpeed   - CPU Max Speed.
 *
 */
UINT16
DmiF10GetMaxSpeed (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8 NumBoostStates;
  UINT32 P0Frequency;
  PSTATE_CPU_FAMILY_SERVICES  *FamilyServices;

  FamilyServices = NULL;
  GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, &FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);
  NumBoostStates = F10GetNumberOfBoostedPstatesOnCore (StdHeader);

  FamilyServices->GetPstateFrequency (FamilyServices, NumBoostStates, &P0Frequency, StdHeader);
  return ((UINT16) P0Frequency);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF10GetExtClock
 *
 *    Get the external clock Speed
 *
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval  ExtClock   - CPU external clock Speed.
 *
 */
UINT16
DmiF10GetExtClock (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  return (EXTERNAL_CLOCK_DFLT);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF10GetMemInfo
 *
 *    Get memory information.
 *
 *    @param[in,out]  CpuGetMemInfoPtr      Pointer to CPU_GET_MEM_INFO struct.
 *    @param[in]      StdHeader             Standard Head Pointer
 *
 */
VOID
DmiF10GetMemInfo (
  IN OUT   CPU_GET_MEM_INFO  *CpuGetMemInfoPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32 PciData;
  PCI_ADDR PciAddress;

  // DCT 0
  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_2, 0x90);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
  // Check if F2x90[DimmEccEn] is set
  if ((PciData & 0x00080000) != 0) {
    CpuGetMemInfoPtr->EccCapable = TRUE;
  } else {
    // DCT 1
    PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_2, 0x190);
    LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
    // Check if F2x190[DimmEccEn] is set
    if ((PciData & 0x00080000) != 0) {
      CpuGetMemInfoPtr->EccCapable = TRUE;
    }
  }

  // Partition Row Position - 0 is for dual channel memory
  CpuGetMemInfoPtr->PartitionRowPosition = 0;
}

/*---------------------------------------------------------------------------------------
 * Processor Family Table
 *
 * Note: 'x' means we don't care this field
 *       002h = "Unknown"
 *       038h = "AMD Turion(TM) II Ultra Dual-Core Mobile M Processor Family"
 *       039h = "AMD Turion(TM) II Dual-Core Mobile M Processor Family"
 *       03Ah = "AMD Athlon(TM) II Dual-Core M Processor Family"
 *       083h = "AMD Athlon(tm) 64 Processor Family"
 *       084h = "AMD Opteron(TM) Processor Family"
 *       085h = "AMD Sempron(tm) Processor Family"
 *       087h = "Dual-Core AMD Opteron Processor Family"
 *       08Ah = "Quad-Core AMD Opteron Processor Family"
 *       08Ch = "AMD Phenom FX Quad-Core Processor Family"
 *       08Dh = "AMD Phenom X4 Quad-Core Processor Family"
 *       08Eh = "AMD Phenom X2 Dual-Core Processor Family"
 *       08Fh = "AMD Athlon X2 Dual-Core Processor Family"
 *       0E6h = "Embedded AMD Opteron Processor Family"
 *       0E7h = "AMD Phenom Triple-Core Processor Family"
 *       0ECh = "AMD Phenom(TM) II Processor Family"
 *       0EDh = "AMD Athlon(TM) II Processor Family"
 *       0EEh = "Six-Core AMD Opteron(TM) Processor Family"
 *       0EFh = "AMD Sempron(TM) M Processor Family"
 *-------------------------------------------------------------------------------------*/
CONST DMI_BRAND_ENTRY ROMDATA Family10BrandList[] =
{
  // Brand --> DMI ID translation table
  // PackageType, PgOfBrandId, NumberOfCores, String1ofBrandId, ValueSetToDmiTable
  // {'x', 'x', 'x', 'x', 0x02} MUST be the last one.
    {0, 0, 1, 0, 0x87},
    {0, 0, 1, 1, 0x87},
    {0, 0, 2, 0, 0xE6},
    {0, 0, 3, 0, 0x8A},
    {0, 0, 3, 1, 0x8A},
    {0, 0, 3, 2, 0xE6},
    {0, 0, 3, 3, 0xE6},
    {0, 0, 3, 4, 0xE6},
    {0, 0, 3, 5, 0x8C},
    {0, 0, 5, 0, 0xEE},
    {0, 0, 5, 1, 0xEE},
    {0, 1, 3, 1, 0xE6},
    {0, 1, 5, 1, 0xE6},
    {1, 0, 0, 0, 0x83},
    {1, 0, 0, 1, 0x85},
    {1, 0, 0, 2, 0x85},
    {1, 0, 0, 3, 0xED},
    {1, 0, 1, 0, 0x87},
    {1, 0, 1, 1, 0x8F},
    {1, 0, 1, 2, 0xED},
    {1, 0, 1, 3, 0xED},
    {1, 0, 1, 4, 0xED},
    {1, 0, 1, 5, 0xED},
    {1, 0, 1, 6, 0xED},
    {1, 0, 1, 7, 0xEC},
    {1, 0, 1, 8, 0xED},
    {1, 0, 1, 9, 0xED},
    {1, 0, 1, 0xA, 0xEC},
    {1, 0, 1, 0xB, 0xEC},
    {1, 0, 1, 0xC, 0x85},
    {1, 0, 2, 0, 0xE7},
    {1, 0, 2, 1, 0xEC},
    {1, 0, 2, 2, 0xEC},
    {1, 0, 2, 3, 0xEC},
    {1, 0, 2, 4, 0xEC},
    {1, 0, 2, 5, 0xED},
    {1, 0, 2, 6, 0xED},
    {1, 0, 2, 7, 0xED},
    {1, 0, 2, 8, 0xEC},
    {1, 0, 2, 9, 0xED},
    {1, 0, 2, 0xA, 0xED},
    {1, 0, 3, 0, 0x8A},
    {1, 0, 3, 1, 0x8C},
    {1, 0, 3, 2, 0x8D},
    {1, 0, 3, 3, 0xEC},
    {1, 0, 3, 4, 0xEC},
    {1, 0, 3, 5, 0xEC},
    {1, 0, 3, 6, 0xEC},
    {1, 0, 3, 7, 0xEC},
    {1, 0, 3, 8, 0xEC},
    {1, 0, 3, 9, 0xEC},
    {1, 0, 3, 0xA, 0xED},
    {1, 0, 3, 0xB, 0xED},
    {1, 0, 3, 0xC, 0xED},
    {1, 0, 3, 0xD, 0xED},
    {1, 0, 3, 0xE, 0xEC},
    {1, 0, 3, 0xF, 0xED},
    {1, 0, 5, 0, 0xEC},
    {1, 1, 1, 1, 0xED},
    {1, 1, 1, 2, 0xED},
    {1, 1, 3, 0, 0xEC},
    {1, 1, 3, 1, 0xEC},
    {1, 1, 3, 2, 0xEC},
    {1, 1, 3, 3, 0xEC},
    {1, 1, 3, 4, 0xEC},
    {2, 0, 0, 0, 0xEF},
    {2, 0, 0, 1, 0xEF},
    {2, 0, 1, 0, 0x38},
    {2, 0, 1, 1, 0x39},
    {2, 0, 1, 2, 0x3A},
    {2, 0, 1, 3, 0x39},
    {2, 0, 1, 4, 0xED},
    {2, 0, 1, 5, 0xEC},
    {2, 0, 1, 6, 0xEC},
    {2, 0, 1, 7, 0x39},
    {2, 0, 1, 8, 0xED},
    {2, 0, 1, 9, 0xEC},
    {2, 0, 2, 2, 0xEC},
    {2, 0, 2, 3, 0xEC},
    {2, 0, 2, 4, 0xEC},
    {2, 0, 3, 1, 0xEC},
    {2, 0, 3, 2, 0xEC},
    {2, 0, 3, 3, 0xEC},
    {3, 0, 7, 0, 0x84},
    {3, 0, 0xB, 0, 0x84},
    {3, 1, 7, 1, 0xE6},
    {4, 0, 0, 1, 0xED},
    {4, 0, 0, 2, 0xEF},
    {4, 0, 0, 3, 0xED},
    {4, 0, 1, 1, 0x39},
    {4, 0, 1, 2, 0x3A},
    {4, 0, 1, 3, 0xEF},
    {4, 0, 1, 4, 0x39},
    {4, 0, 1, 5, 0x3A},
    {5, 0, 3, 0, 0x84},
    {5, 0, 5, 0, 0x84},
    {5, 1, 3, 1, 0xE6},
    {5, 1, 5, 1, 0xE6},
    {'x', 'x', 'x', 'x', P_FAMILY_UNKNOWN}
  };

CONST PROC_FAMILY_TABLE ROMDATA ProcFamily10DmiTable =
{
// This table is for Processor family 10h
  AMD_FAMILY_10,                  // ID for Family 10h
  DmiF10GetInfo,                  // Transfer vectors for family
  DmiGetT4ProcFamilyFromBrandId,  // Get type 4 processor family information from CPUID_8000_0001_EBX[BrandId]
  DmiF10GetVoltage,               //   specific routines (above)
  DmiF10GetMaxSpeed,
  DmiF10GetExtClock,
  DmiF10GetMemInfo,               // Get memory information
  (sizeof (Family10BrandList) / sizeof (Family10BrandList[0])), // Number of entries in following table
  &Family10BrandList[0]
};


/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  F10Translate7BitVidTo6Bit
 *
 *    translate 7 bit VID to 6 bit VID
 *
 *    @param[in, out]  MaxVidPtr   - Pointer to MaxVid.
 */
VOID
STATIC
F10Translate7BitVidTo6Bit (
  IN OUT   UINT8 * MaxVidPtr
  )
{
  if ((*MaxVidPtr >= 0x5E) && (*MaxVidPtr <= 0x7F)) {
    *MaxVidPtr = 0x3F;
  } else if ((*MaxVidPtr >= 0x3F) && (*MaxVidPtr <= 0x5D)) {
    *MaxVidPtr = *MaxVidPtr - 0x1F;
  } else if (*MaxVidPtr <= 0x3E) {
    *MaxVidPtr = (*MaxVidPtr & 0x7E) >> 1;
  }
}

