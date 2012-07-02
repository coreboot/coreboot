/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init HwAcpi Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
****************************************************************************
*/
#include "FchPlatform.h"
#include "amdlib.h"
#include "cpuServices.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_HWACPI_HWACPILATE_FILECODE

#define AMD_CPUID_APICID_LPC_BID    0x00000001ul  // Local APIC ID, Logical Processor Count, Brand ID







///
/// PCI_IRQ_REG_BLOCK- FCH PCI IRQ registers block
///
typedef struct _PCI_IRQ_REG_BLOCK {
  UINT8                PciIrqIndex;       // PciIrqIndex - selects which PCI interrupt to map
  UINT8                PciIrqData;        // PciIrqData  - Interrupt #
} PCI_IRQ_REG_BLOCK;

STATIC PCI_IRQ_REG_BLOCK FchInternalDeviceIrqForApicMode[] = {
    { (FCH_IRQ_INTA | FCH_IRQ_IOAPIC), 0x10},
    { (FCH_IRQ_INTB | FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_INTC | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_INTD | FCH_IRQ_IOAPIC), 0x13},
    { (FCH_IRQ_INTE | FCH_IRQ_IOAPIC), 0x14},
    { (FCH_IRQ_INTF | FCH_IRQ_IOAPIC), 0x15},
    { (FCH_IRQ_INTG | FCH_IRQ_IOAPIC), 0x16},
    { (FCH_IRQ_INTH | FCH_IRQ_IOAPIC), 0x17},
    { (FCH_IRQ_HDAUDIO | FCH_IRQ_IOAPIC), 0x10},
    { (FCH_IRQ_GEC | FCH_IRQ_IOAPIC), 0x10},
    { (FCH_IRQ_SD | FCH_IRQ_IOAPIC), 0x10},
    { (FCH_IRQ_GPPINT0 | FCH_IRQ_IOAPIC), 0x10},
    { (FCH_IRQ_IDE | FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_USB18INTB | FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_USB19INTB | FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_USB22INTB | FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_GPPINT1 + FCH_IRQ_IOAPIC), 0x11},
    { (FCH_IRQ_USB18INTA | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_USB19INTA | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_USB22INTA | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_USB20INTC | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_GPPINT2 | FCH_IRQ_IOAPIC), 0x12},
    { (FCH_IRQ_SATA | FCH_IRQ_IOAPIC), 0x13},
    { (FCH_IRQ_GPPINT3 | FCH_IRQ_IOAPIC), 0x13},
  };

#define NUM_OF_DEVICE_FOR_APICIRQ  sizeof (FchInternalDeviceIrqForApicMode) / sizeof (PCI_IRQ_REG_BLOCK)

/**
 * FchInitLateHwAcpi - Prepare HwAcpi controller to boot to OS.
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateHwAcpi (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;
  UINT8                  i;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  if ( IsGCPU (LocalCfgPtr) ) {
    GcpuRelatedSetting (LocalCfgPtr);
  } else {
    //TNBU C3PopupSetting (LocalCfgPtr);
  }

  // Mt C1E Enable
  MtC1eEnable (LocalCfgPtr);

  if (LocalCfgPtr->Gpp.SerialDebugBusEnable == TRUE ) {
    RwMem (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE +  FCH_SDB_REG00, AccessWidth8, 0xFF, 0x05);
  }

  StressResetModeLate (LocalCfgPtr);
  SbSleepTrapControl (FALSE); /* TODO: Checkout if we need to disable sleep trap in Non-SMI mode. */
  for (i = 0; i < NUM_OF_DEVICE_FOR_APICIRQ; i++) {
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &FchInternalDeviceIrqForApicMode[i].PciIrqIndex, StdHeader);
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &FchInternalDeviceIrqForApicMode[i].PciIrqData, StdHeader);
  }
}

/**
 * IsGCPU - Is Gcpu Cpu?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsGCPU (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                   ExtendedFamily;
  UINT8                   ExtendedModel;
  UINT8                   BaseFamily;
  UINT8                   BaseModel;
  UINT8                   Stepping;
  UINT8                   Family;
  UINT8                   Model;
  CPUID_DATA              CpuId;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  LibAmdCpuidRead (AMD_CPUID_APICID_LPC_BID, &CpuId, StdHeader);

  ExtendedFamily = (UINT8) ((CpuId.EAX_Reg >> 20) & 0xff);
  ExtendedModel = (UINT8) ((CpuId.EAX_Reg >> 16) & 0xf);
  BaseFamily = (UINT8) ((CpuId.EAX_Reg >> 8) & 0xf);
  BaseModel = (UINT8) ((CpuId.EAX_Reg >> 4) & 0xf);
  Stepping = (UINT8) ((CpuId.EAX_Reg >> 0) & 0xf);
  Family = BaseFamily + ExtendedFamily;
  Model = (ExtendedModel << 4) + BaseModel;

  if ( (Family == 0x12) || \
       (Family == 0x14) || \
       (Family == 0x16)  ) {
    return TRUE;
  } else {
    return FALSE;
  }
}

