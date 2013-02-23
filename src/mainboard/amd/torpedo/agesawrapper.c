/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include <stdint.h>
#include <string.h>
#include "agesawrapper.h"
#include "BiosCallOuts.h"
#include "cpuRegisters.h"
#include "cpuCacheInit.h"
#include "cpuApicUtilities.h"
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"
#include "Dispatcher.h"
#include "cpuCacheInit.h"
#include "amdlib.h"
#include "PlatformGnbPcieComplex.h"
#include "Filecode.h"
#include <arch/io.h>

#define FILECODE UNASSIGNED_FILE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/* ACPI table pointers returned by AmdInitLate */
VOID *DmiTable    = NULL;
VOID *AcpiPstate  = NULL;
VOID *AcpiSrat    = NULL;
VOID *AcpiSlit    = NULL;

VOID *AcpiWheaMce = NULL;
VOID *AcpiWheaCmc = NULL;
VOID *AcpiAlib    = NULL;


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
UINT32
ReadAmdSbPmr (
  IN     UINT8                      IndexValue,
  OUT    UINT8                      *DataValue
  );

UINT32
WriteAmdSbPmr (
  IN     UINT8                      IndexValue,
  IN     UINT8                      DataValue
  );

VOID
ClearSBSmiAndWake (
  IN UINT16             PmBase
  );

VOID
ClearAllSmiEnableInPmio (
  VOID
  );



/* Read SB Power Management Area */
UINT32
ReadAmdSbPmr (
  IN     UINT8                      IndexValue,
  OUT    UINT8                      *DataValue
  )
{
  WriteIo8 (SB_PM_INDEX_PORT, IndexValue);
  *DataValue = ReadIo8 (SB_PM_DATA_PORT);
  return 0;
}

/* Write ATI SB Power Management Area */
UINT32
WriteAmdSbPmr (
  IN     UINT8                      IndexValue,
  IN     UINT8                      DataValue
  )
{
  WriteIo8 (SB_PM_INDEX_PORT, IndexValue);
  WriteIo8 (SB_PM_DATA_PORT, DataValue);
  return 0;
}

/* Clear any SMI status or wake status left over from boot. */
VOID
ClearSBSmiAndWake (
  IN UINT16             PmBase
  )
{
  UINT16  Pm1Sts;
  UINT32  Pm1Cnt;
  UINT32  Gpe0Sts;

  /*  Read the ACPI registers */
  Pm1Sts  = ReadIo16 (PmBase + R_SB_ACPI_PM1_STATUS);
  Pm1Cnt  = ReadIo32 (PmBase + R_SB_ACPI_PM1_STATUS);
  Gpe0Sts = ReadIo32 (PmBase + R_SB_ACPI_EVENT_STATUS);

  /* Clear any SMI or wake state from the boot */
  Pm1Sts &= B_PWR_BTN_STATUS + B_WAKEUP_STATUS;
  Pm1Cnt &= ~(B_SCI_EN);

  /* Write back */
  WriteIo16 (PmBase + R_SB_ACPI_PM1_STATUS, Pm1Sts);
  WriteIo32 (PmBase + R_SB_ACPI_PM_CONTROL, Pm1Cnt);
  WriteIo32 (PmBase + R_SB_ACPI_EVENT_STATUS, Gpe0Sts);
}

/* Clear all SMI enable bit in PMIO register */
VOID
ClearAllSmiEnableInPmio (
  VOID
  )
{
  UINT32  AcpiMmioAddr;
  UINT32  SmiMmioAddr;
  UINT8   Data8 = 0 ;
  UINT16  Data16 = 0;

  /* Get SB900 MMIO Base (AcpiMmioAddr) */
  ReadAmdSbPmr (SB_PMIOA_REG24 + 3, &Data8);
  Data16=Data8<<8;
  ReadAmdSbPmr (SB_PMIOA_REG24 + 2, &Data8);
  Data16|=Data8;
  AcpiMmioAddr = (UINT32)Data16 << 16;
  SmiMmioAddr = AcpiMmioAddr + SMI_BASE;

  Mmio32 (SmiMmioAddr, 0xA0) = 0x0;
  Mmio32 (SmiMmioAddr, 0xA4) = 0x0;
  Mmio32 (SmiMmioAddr, 0xA8) = 0x0;
  Mmio32 (SmiMmioAddr, 0xAC) = 0x0;
  Mmio32 (SmiMmioAddr, 0xB0) = 0x0;
  Mmio32 (SmiMmioAddr, 0xB4) = 0x0;
  Mmio32 (SmiMmioAddr, 0xB8) = 0x0;
  Mmio32 (SmiMmioAddr, 0xBC) = 0x0;
  Mmio32 (SmiMmioAddr, 0xC0) = 0x0;
  Mmio32 (SmiMmioAddr, 0xC4) = 0x0;
}

UINT32
agesawrapper_amdinitcpuio (
  VOID
  )
{
  AGESA_STATUS                  Status;
  UINT64                        MsrReg;
  UINT32                        PciData;
  PCI_ADDR                      PciAddress;
  AMD_CONFIG_PARAMS             StdHeader;

  /* Enable MMIO on AMD CPU Address Map Controller */

  /* Start to set MMIO 0000A0000-0000BFFFF to Node0 Link0 */
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x84);
  PciData = 0x00000B00;
  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x80);
  PciData = 0x00000A03;
  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

  /* Set TOM-DFFFFFFF to Node0 Link0. */
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x8C);
  PciData = 0x00DFFF00;
  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
  LibAmdMsrRead (0xC001001A, &MsrReg, &StdHeader);
  MsrReg = (MsrReg >> 8) | 3;
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x88);
  PciData = (UINT32)MsrReg;
  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
  /* Set E0000000-FFFFFFFF to Node0 Link0 with NP set. */
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0xBC);
  PciData = 0x00FFFF00 | 0x80;
  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0xB8);
  PciData = (PCIE_BASE_ADDRESS >> 8) | 03;
  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
  /* Start to set PCIIO 0000-FFFF to Node0 Link0 with ISA&VGA set. */
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0xC4);
//-  PciData = 0x0000F000;
  PciData = 0x00FFF000;
  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0xC0);
  PciData = 0x00000013;
  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
  Status = AGESA_SUCCESS;
  return (UINT32)Status;
}

UINT32
agesawrapper_amdinitmmio (
  VOID
  )
{
  AGESA_STATUS                  Status;
  UINT64                        MsrReg;
  UINT32                        PciData;
  PCI_ADDR                      PciAddress;
  AMD_CONFIG_PARAMS             StdHeader;

  /*
   Set the MMIO Configuration Base Address and Bus Range onto MMIO configuration base
   Address MSR register.
  */
  MsrReg = CONFIG_MMCONF_BASE_ADDRESS | (8 << 2) | 1;
  LibAmdMsrWrite (0xC0010058, &MsrReg, &StdHeader);

  /*
   Set the NB_CFG MSR register. Enable CF8 extended configuration cycles.
  */
  LibAmdMsrRead (0xC001001F, &MsrReg, &StdHeader);
  MsrReg = MsrReg | 0x0000400000000000ull;
  LibAmdMsrWrite (0xC001001F, &MsrReg, &StdHeader);

  /* Set Ontario Link Data */
//-  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0xE0);
//-  PciData = 0x01308002;
//-  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
//-  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0xE4);
//-  PciData = (AMD_APU_SSID<<0x10)|AMD_APU_SVID;
//-  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

  /* Enable Non-Post Memory in CPU */
  PciData = ((CONFIG_MMCONF_BASE_ADDRESS >> 8) | 0x3FF80);
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x018, 0x01, 0xA4);
  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

  PciData = ((CONFIG_MMCONF_BASE_ADDRESS >> 8) | 0x03);
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x018, 0x01, 0xA0);
  LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

  /* Enable memory access */
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0x04);
  LibAmdPciRead(AccessWidth8, PciAddress, &PciData, &StdHeader);

  PciData |= BIT1;
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0x04);
  LibAmdPciWrite(AccessWidth8, PciAddress, &PciData, &StdHeader);

  /* Set ROM cache onto WP to decrease post time */
  MsrReg = (0x0100000000ull - CONFIG_ROM_SIZE) | 5ull;
  LibAmdMsrWrite (0x20C, &MsrReg, &StdHeader);
  MsrReg = ((1ULL << CONFIG_CPU_ADDR_BITS) - CONFIG_ROM_SIZE) | 0x800ull;
  LibAmdMsrWrite (0x20D, &MsrReg, &StdHeader);

  /* Clear all pending SMI. On S3 clear power button enable so it wll not generate an SMI */
//-  ClearSBSmiAndWake (SB_ACPI_BASE_ADDRESS);
//-  ClearAllSmiEnableInPmio ();

  Status = AGESA_SUCCESS;
  return (UINT32)Status;
}

UINT32
agesawrapper_amdinitreset (
  VOID
  )
{
  AGESA_STATUS status;
  AMD_INTERFACE_PARAMS AmdParamStruct;
  AMD_RESET_PARAMS AmdResetParams;

  LibAmdMemFill (&AmdParamStruct,
                 0,
                 sizeof (AMD_INTERFACE_PARAMS),
                 &(AmdParamStruct.StdHeader));


  LibAmdMemFill (&AmdResetParams,
                 0,
                 sizeof (AMD_RESET_PARAMS),
                 &(AmdResetParams.StdHeader));

  AmdParamStruct.AgesaFunctionName = AMD_INIT_RESET;
  AmdParamStruct.AllocationMethod = ByHost;
  AmdParamStruct.NewStructSize = sizeof(AMD_RESET_PARAMS);
  AmdParamStruct.NewStructPtr = &AmdResetParams;
  AmdParamStruct.StdHeader.AltImageBasePtr = 0;
  AmdParamStruct.StdHeader.CalloutPtr = NULL;
  AmdParamStruct.StdHeader.Func = 0;
  AmdParamStruct.StdHeader.ImageBasePtr = 0;
  AmdCreateStruct (&AmdParamStruct);
  AmdResetParams.HtConfig.Depth = 0;

  status = AmdInitReset ((AMD_RESET_PARAMS *)AmdParamStruct.NewStructPtr);
  if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
  AmdReleaseStruct (&AmdParamStruct);
  return (UINT32)status;
 }

UINT32
agesawrapper_amdinitearly (
  VOID
  )
{
  AGESA_STATUS status;
  AMD_INTERFACE_PARAMS AmdParamStruct;
  AMD_EARLY_PARAMS     *AmdEarlyParamsPtr;

  LibAmdMemFill (&AmdParamStruct,
                 0,
                 sizeof (AMD_INTERFACE_PARAMS),
                 &(AmdParamStruct.StdHeader));

  AmdParamStruct.AgesaFunctionName = AMD_INIT_EARLY;
  AmdParamStruct.AllocationMethod = PreMemHeap;
  AmdParamStruct.StdHeader.AltImageBasePtr = 0;
  AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
  AmdParamStruct.StdHeader.Func = 0;
  AmdParamStruct.StdHeader.ImageBasePtr = 0;
  AmdCreateStruct (&AmdParamStruct);

  AmdEarlyParamsPtr = (AMD_EARLY_PARAMS *)AmdParamStruct.NewStructPtr;
  OemCustomizeInitEarly (AmdEarlyParamsPtr);

  status = AmdInitEarly ((AMD_EARLY_PARAMS *)AmdParamStruct.NewStructPtr);
  if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
  AmdReleaseStruct (&AmdParamStruct);

  return (UINT32)status;
}

UINT32
agesawrapper_amdinitpost (
  VOID
  )
{
  AGESA_STATUS status;
  UINT16                  i;
  UINT32          *HeadPtr;
  AMD_INTERFACE_PARAMS  AmdParamStruct;
  BIOS_HEAP_MANAGER    *BiosManagerPtr;

  LibAmdMemFill (&AmdParamStruct,
                 0,
                 sizeof (AMD_INTERFACE_PARAMS),
                 &(AmdParamStruct.StdHeader));

  AmdParamStruct.AgesaFunctionName = AMD_INIT_POST;
  AmdParamStruct.AllocationMethod = PreMemHeap;
  AmdParamStruct.StdHeader.AltImageBasePtr = 0;
  AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
  AmdParamStruct.StdHeader.Func = 0;
  AmdParamStruct.StdHeader.ImageBasePtr = 0;

  AmdCreateStruct (&AmdParamStruct);
  status = AmdInitPost ((AMD_POST_PARAMS *)AmdParamStruct.NewStructPtr);
  if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
  AmdReleaseStruct (&AmdParamStruct);

  /* Initialize heap space */
  BiosManagerPtr = (BIOS_HEAP_MANAGER *)BIOS_HEAP_START_ADDRESS;

  HeadPtr = (UINT32 *) ((UINT8 *) BiosManagerPtr + sizeof (BIOS_HEAP_MANAGER));
  for (i = 0; i < ((BIOS_HEAP_SIZE/4) - (sizeof (BIOS_HEAP_MANAGER)/4)); i++)
  {
    *HeadPtr = 0x00000000;
    HeadPtr++;
  }
  BiosManagerPtr->StartOfAllocatedNodes = 0;
  BiosManagerPtr->StartOfFreedNodes = 0;

  return (UINT32)status;
}

UINT32
agesawrapper_amdinitenv (
  VOID
  )
{
  AGESA_STATUS status;
  AMD_INTERFACE_PARAMS AmdParamStruct;

  LibAmdMemFill (&AmdParamStruct,
                 0,
                 sizeof (AMD_INTERFACE_PARAMS),
                 &(AmdParamStruct.StdHeader));

  AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
  AmdParamStruct.AllocationMethod = PostMemDram;
  AmdParamStruct.StdHeader.AltImageBasePtr = 0;
  AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
  AmdParamStruct.StdHeader.Func = 0;
  AmdParamStruct.StdHeader.ImageBasePtr = 0;
  AmdCreateStruct (&AmdParamStruct);
  status = AmdInitEnv ((AMD_ENV_PARAMS *)AmdParamStruct.NewStructPtr);
  if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
  AmdReleaseStruct (&AmdParamStruct);

  return (UINT32)status;
}

VOID *
agesawrapper_getlateinitptr (
  int pick
  )
{
  switch (pick) {
    case PICK_DMI:
      return DmiTable;

    case PICK_PSTATE:
      return AcpiPstate;

    case PICK_SRAT:
      return AcpiSrat;

    case PICK_SLIT:
      return AcpiSlit;
    case PICK_WHEA_MCE:
      return AcpiWheaMce;
    case PICK_WHEA_CMC:
      return AcpiWheaCmc;
    case PICK_ALIB:
      return AcpiAlib;
    default:
      return NULL;
  }
}

UINT32
agesawrapper_amdinitmid (
  VOID
  )
{
  AGESA_STATUS status;
  AMD_INTERFACE_PARAMS AmdParamStruct;

  /* Enable MMIO on AMD CPU Address Map Controller */
  agesawrapper_amdinitcpuio ();

  LibAmdMemFill (&AmdParamStruct,
                 0,
                 sizeof (AMD_INTERFACE_PARAMS),
                 &(AmdParamStruct.StdHeader));

  AmdParamStruct.AgesaFunctionName = AMD_INIT_MID;
  AmdParamStruct.AllocationMethod = PostMemDram;
  AmdParamStruct.StdHeader.AltImageBasePtr = 0;
  AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
  AmdParamStruct.StdHeader.Func = 0;
  AmdParamStruct.StdHeader.ImageBasePtr = 0;

  AmdCreateStruct (&AmdParamStruct);

  status = AmdInitMid ((AMD_MID_PARAMS *)AmdParamStruct.NewStructPtr);
  if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
  AmdReleaseStruct (&AmdParamStruct);

  return (UINT32)status;
}

UINT32
agesawrapper_amdinitlate (
  VOID
  )
{
  AGESA_STATUS Status;
  AMD_LATE_PARAMS AmdLateParams;

  LibAmdMemFill (&AmdLateParams,
                 0,
                 sizeof (AMD_LATE_PARAMS),
                 &(AmdLateParams.StdHeader));

  AmdLateParams.StdHeader.AltImageBasePtr = 0;
  AmdLateParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
  AmdLateParams.StdHeader.Func = 0;
  AmdLateParams.StdHeader.ImageBasePtr = 0;

  Status = AmdInitLate (&AmdLateParams);
  if (Status != AGESA_SUCCESS) {
    agesawrapper_amdreadeventlog();
    ASSERT(Status == AGESA_SUCCESS);
  }

  DmiTable       = AmdLateParams.DmiTable;
  AcpiPstate     = AmdLateParams.AcpiPState;
  AcpiSrat       = AmdLateParams.AcpiSrat;
  AcpiSlit       = AmdLateParams.AcpiSlit;

  AcpiWheaMce    = AmdLateParams.AcpiWheaMce;
  AcpiWheaCmc    = AmdLateParams.AcpiWheaCmc;
  AcpiAlib       = AmdLateParams.AcpiAlib;

  return (UINT32)Status;
}

UINT32
agesawrapper_amdlaterunaptask (
  UINT32 Data,
  VOID *ConfigPtr
  )
{
  AGESA_STATUS Status;
  AP_EXE_PARAMS ApExeParams;

  LibAmdMemFill (&ApExeParams,
                 0,
                 sizeof (AP_EXE_PARAMS),
                 &(ApExeParams.StdHeader));

  ApExeParams.StdHeader.AltImageBasePtr = 0;
  ApExeParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
  ApExeParams.StdHeader.Func = 0;
  ApExeParams.StdHeader.ImageBasePtr = 0;

  Status = AmdLateRunApTask (&ApExeParams);
  if (Status != AGESA_SUCCESS) {
    agesawrapper_amdreadeventlog();
    ASSERT(Status == AGESA_SUCCESS);
  }

  return (UINT32)Status;
}

UINT32
agesawrapper_amdreadeventlog (
  VOID
  )
{
  AGESA_STATUS Status;
  EVENT_PARAMS AmdEventParams;

  LibAmdMemFill (&AmdEventParams,
                 0,
                 sizeof (EVENT_PARAMS),
                 &(AmdEventParams.StdHeader));

  AmdEventParams.StdHeader.AltImageBasePtr = 0;
  AmdEventParams.StdHeader.CalloutPtr = NULL;
  AmdEventParams.StdHeader.Func = 0;
  AmdEventParams.StdHeader.ImageBasePtr = 0;
  Status = AmdReadEventLog (&AmdEventParams);
  while (AmdEventParams.EventClass != 0) {
    printk(BIOS_DEBUG,"\nEventLog:  EventClass = %lx, EventInfo = %lx.\n",AmdEventParams.EventClass,AmdEventParams.EventInfo);
    printk(BIOS_DEBUG,"  Param1 = %lx, Param2 = %lx.\n",AmdEventParams.DataParam1,AmdEventParams.DataParam2);
    printk(BIOS_DEBUG,"  Param3 = %lx, Param4 = %lx.\n",AmdEventParams.DataParam3,AmdEventParams.DataParam4);
    Status = AmdReadEventLog (&AmdEventParams);
  }

  return (UINT32)Status;
}
