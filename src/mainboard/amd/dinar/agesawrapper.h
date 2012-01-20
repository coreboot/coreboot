/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
/* BITS Define */
#ifndef BIT0
#define BIT0        0x0000000000000001ull
#endif
#ifndef BIT1
#define BIT1        0x0000000000000002ull
#endif
#ifndef BIT2
#define BIT2        0x0000000000000004ull
#endif
#ifndef BIT3
#define BIT3        0x0000000000000008ull
#endif
#ifndef BIT4
#define BIT4        0x0000000000000010ull
#endif
#ifndef BIT5
#define BIT5        0x0000000000000020ull
#endif
#ifndef BIT6
#define BIT6        0x0000000000000040ull
#endif
#ifndef BIT7
#define BIT7        0x0000000000000080ull
#endif
#ifndef BIT8
#define BIT8        0x0000000000000100ull
#endif
#ifndef BIT9
#define BIT9        0x0000000000000200ull
#endif
#ifndef BIT10
#define BIT10       0x0000000000000400ull
#endif
#ifndef BIT11
#define BIT11       0x0000000000000800ull
#endif
#ifndef BIT12
#define BIT12       0x0000000000001000ull
#endif
#ifndef BIT13
#define BIT13       0x0000000000002000ull
#endif
#ifndef BIT14
#define BIT14       0x0000000000004000ull
#endif
#ifndef BIT15
#define BIT15       0x0000000000008000ull
#endif
#ifndef BIT16
#define BIT16       0x0000000000010000ull
#endif
#ifndef BIT17
#define BIT17       0x0000000000020000ull
#endif
#ifndef BIT18
#define BIT18       0x0000000000040000ull
#endif
#ifndef BIT19
#define BIT19       0x0000000000080000ull
#endif
#ifndef BIT20
#define BIT20       0x0000000000100000ull
#endif
#ifndef BIT21
#define BIT21       0x0000000000200000ull
#endif
#ifndef BIT22
#define BIT22       0x0000000000400000ull
#endif
#ifndef BIT23
#define BIT23       0x0000000000800000ull
#endif
#ifndef BIT24
#define BIT24       0x0000000001000000ull
#endif
#ifndef BIT25
#define BIT25       0x0000000002000000ull
#endif
#ifndef BIT26
#define BIT26       0x0000000004000000ull
#endif
#ifndef BIT27
#define BIT27       0x0000000008000000ull
#endif
#ifndef BIT28
#define BIT28       0x0000000010000000ull
#endif
#ifndef BIT29
#define BIT29       0x0000000020000000ull
#endif
#ifndef BIT30
#define BIT30       0x0000000040000000ull
#endif
#ifndef BIT31
#define BIT31       0x0000000080000000ull
#endif
#ifndef BIT32
#define BIT32       0x0000000100000000ull
#endif
#ifndef BIT33
#define BIT33       0x0000000200000000ull
#endif
#ifndef BIT34
#define BIT34       0x0000000400000000ull
#endif
#ifndef BIT35
#define BIT35       0x0000000800000000ull
#endif
#ifndef BIT36
#define BIT36       0x0000001000000000ull
#endif
#ifndef BIT37
#define BIT37       0x0000002000000000ull
#endif
#ifndef BIT38
#define BIT38       0x0000004000000000ull
#endif
#ifndef BIT39
#define BIT39       0x0000008000000000ull
#endif
#ifndef BIT40
#define BIT40       0x0000010000000000ull
#endif
#ifndef BIT41
#define BIT41       0x0000020000000000ull
#endif
#ifndef BIT42
#define BIT42       0x0000040000000000ull
#endif
#ifndef BIT43
#define BIT43       0x0000080000000000ull
#endif
#ifndef BIT44
#define BIT44       0x0000100000000000ull
#endif
#ifndef BIT45
#define BIT45       0x0000200000000000ull
#endif
#ifndef BIT46
#define BIT46       0x0000400000000000ull
#endif
#ifndef BIT47
#define BIT47       0x0000800000000000ull
#endif
#ifndef BIT48
#define BIT48       0x0001000000000000ull
#endif
#ifndef BIT49
#define BIT49       0x0002000000000000ull
#endif
#ifndef BIT50
#define BIT50       0x0004000000000000ull
#endif
#ifndef BIT51
#define BIT51       0x0008000000000000ull
#endif
#ifndef BIT52
#define BIT52       0x0010000000000000ull
#endif
#ifndef BIT53
#define BIT53       0x0020000000000000ull
#endif
#ifndef BIT54
#define BIT54       0x0040000000000000ull
#endif
#ifndef BIT55
#define BIT55       0x0080000000000000ull
#endif
#ifndef BIT56
#define BIT56       0x0100000000000000ull
#endif
#ifndef BIT57
#define BIT57       0x0200000000000000ull
#endif
#ifndef BIT58
#define BIT58       0x0400000000000000ull
#endif
#ifndef BIT59
#define BIT59       0x0800000000000000ull
#endif
#ifndef BIT60
#define BIT60       0x1000000000000000ull
#endif
#ifndef BIT61
#define BIT61       0x2000000000000000ull
#endif
#ifndef BIT62
#define BIT62       0x4000000000000000ull
#endif
#ifndef BIT63
#define BIT63       0x8000000000000000ull
#endif
/* Define AMD Ontario APPU SSID/SVID */
#define AMD_APU_SVID    0x1022
#define AMD_APU_SSID    0x1234
#define PCIE_BASE_ADDRESS   CONFIG_MMCONF_BASE_ADDRESS
#define MMIO_NP_BIT         BIT7

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

//void brazos_platform_stage(void);
UINT32 agesawrapper_amdinitreset (void);
UINT32 agesawrapper_amdinitearly (void);
UINT32 agesawrapper_amdinitenv (void);
UINT32 agesawrapper_amdinitlate (void);
UINT32 agesawrapper_amdinitpost (void);
UINT32 agesawrapper_amdinitmid (void);
void sb_After_Pci_Init (void);
void sb_Mid_Post_Init (void);
void sb_Late_Post (void);
UINT32 agesawrapper_amdreadeventlog (void);
UINT32 agesawrapper_amdinitmmio (void);
void *agesawrapper_getlateinitptr (int pick);

#endif
