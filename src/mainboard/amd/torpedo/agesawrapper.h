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


#ifndef _AGESAWRAPPER_H_
#define _AGESAWRAPPER_H_

#include <stdint.h>
#include "Porting.h"
#include "AGESA.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
/* Define AMD Ontario APPU SSID/SVID */
#define AMD_APU_SVID    0x1022
#define AMD_APU_SSID    0x1234
#define PCIE_BASE_ADDRESS   CONFIG_MMCONF_BASE_ADDRESS

/* Hudson-2 ACPI PmIO Space Define */
#define SB_ACPI_BASE_ADDRESS              0x0400
#define ACPI_MMIO_BASE  0xFED80000
#define SB_CFG_BASE     0x000   // DWORD
#define GPIO_BASE       0x100   // BYTE
#define SMI_BASE        0x200   // DWORD
#define PMIO_BASE       0x300   // DWORD
#define PMIO2_BASE      0x400   // BYTE
#define BIOS_RAM_BASE   0x500   // BYTE
#define CMOS_RAM_BASE   0x600   // BYTE
#define CMOS_BASE       0x700   // BYTE
#define ASF_BASE        0x900   // DWORD
#define SMBUS_BASE      0xA00   // DWORD
#define WATCHDOG_BASE   0xB00   // ??
#define HPET_BASE       0xC00   // DWORD
#define IOMUX_BASE      0xD00   // BYTE
#define MISC_BASE       0xE00
#define SERIAL_DEBUG_BASE  0x1000
#define GFX_DAC_BASE       0x1400
#define CEC_BASE           0x1800
#define XHCI_BASE          0x1C00
#define ACPI_SMI_DATA_PORT                0xB1
#define R_SB_ACPI_PM1_STATUS              0x00
#define R_SB_ACPI_PM1_ENABLE              0x02
#define R_SB_ACPI_PM_CONTROL              0x04
#define R_SB_ACPI_EVENT_STATUS            0x20
#define R_SB_ACPI_EVENT_ENABLE            0x24
#define   B_PWR_BTN_STATUS                BIT8
#define   B_WAKEUP_STATUS                 BIT15
#define   B_SCI_EN                        BIT0
#define SB_PM_INDEX_PORT                  0xCD6
#define SB_PM_DATA_PORT                   0xCD7
#define SB_PMIOA_REG24          0x24        //  AcpiMmioEn
#define MmioAddress( BaseAddr, Register ) \
  ( (UINTN)BaseAddr + \
    (UINTN)(Register) \
  )
#define Mmio32Ptr( BaseAddr, Register ) \
  ( (volatile UINT32 *)MmioAddress( BaseAddr, Register ) )
#define Mmio32( BaseAddr, Register ) \
  *Mmio32Ptr( BaseAddr, Register )

enum {
  PICK_DMI,       /* DMI Interface */
  PICK_PSTATE,    /* Acpi Pstate SSDT Table */
  PICK_SRAT,      /* SRAT Table */
  PICK_SLIT,      /* SLIT Table */
  PICK_WHEA_MCE,  /* WHEA MCE table */
  PICK_WHEA_CMC,  /* WHEA CMV table */
  PICK_ALIB,      /* SACPI SSDT table with ALIB implementation */
};



/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  UINT32 CalloutName;
  AGESA_STATUS (*CalloutPtr) (UINT32 Func, UINT32 Data, VOID* ConfigPtr);
} BIOS_CALLOUT_STRUCT;

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

UINT32 agesawrapper_amdinitreset (void);
UINT32 agesawrapper_amdinitearly (void);
UINT32 agesawrapper_amdinitenv (void);
UINT32 agesawrapper_amdinitlate (void);
UINT32 agesawrapper_amdinitpost (void);
UINT32 agesawrapper_amdinitmid (void);

UINT32 agesawrapper_amdreadeventlog (void);
UINT32 agesawrapper_amdinitcpuio (void);
UINT32 agesawrapper_amdinitmmio (void);
UINT32 agesawrapper_amdlaterunaptask (UINT32 Data, VOID *ConfigPtr);
void *agesawrapper_getlateinitptr (int pick);

#endif
