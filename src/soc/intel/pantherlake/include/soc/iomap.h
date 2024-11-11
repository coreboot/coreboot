/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PANTHERLAKE_IOMAP_H_
#define _SOC_PANTHERLAKE_IOMAP_H_

#include <soc/pcr_ids.h>

/*
 * Memory-mapped I/O registers.
 */

#define PCH_PRESERVED_BASE_ADDRESS	0xfd800000
#define PCH_PRESERVED_BASE_SIZE	0x1000000

#define MCH_BASE_ADDRESS	0xfedc0000
#define MCH_BASE_SIZE		0x20000

/* System Agent Fabric (SAF) */
#define SAF_BASE_ADDRESS	0xfa000000
#define SAF_BASE_SIZE		0x2000000

/* Add dummy entry to cater common/block/acpi/acpi/northbridge.asl */
#define DMI_BASE_SIZE		0

#define EP_BASE_ADDRESS		0xfeda1000
#define EP_BASE_SIZE		0x1000

#define HPET_BASE_ADDRESS	0xfed00000

#define PCH_PWRM_BASE_ADDRESS	0xfe000000
#define PCH_PWRM_BASE_SIZE	0x10000

#define GPIO_BASE_SIZE		0x10000

#define HECI1_BASE_ADDRESS	0xfeda2000

/* VT-d  512KB */
#define VTD_BASE_ADDRESS	0xfc800000
#define VTD_BASE_SIZE		0x80000

/* GFX VT-d  64KB */
#define GFXVT_BASE_ADDRESS	0xfc800000
#define GFXVT_BASE_SIZE		0x10000

/* Non-GFX VT-d  64KB */
#define VTVC0_BASE_ADDRESS	0xfc810000
#define VTVC0_BASE_SIZE		0x10000

/* IOC VT-d 64KB */
#define IOCVTD_BASE_ADDRESS	0xfc820000
#define IOCVTD_BASE_SIZE	0x10000

#define UART_BASE_SIZE		0x1000
#define UART_BASE_0_ADDRESS	CONFIG_CONSOLE_UART_BASE_ADDRESS
/* Both UART BAR 0 and 1 are 4KB in size */
#define UART_BASE_0_ADDR(x)	(UART_BASE_0_ADDRESS + (2 * \
					UART_BASE_SIZE * (x)))
#define UART_BASE(x)		UART_BASE_0_ADDR(x)

#define EARLY_GSPI_BASE_ADDRESS	0xfe030000

#define EARLY_I2C_BASE_ADDRESS	0xfe020000
#define EARLY_I2C_BASE(x)	(EARLY_I2C_BASE_ADDRESS + (0x2000 * (x)))

#define SPI_BASE_ADDRESS	0xfe010000

/* REGBAR 128MB */
#define REG_BASE_ADDRESS	0xf0000000
#define REG_BASE_SIZE		(128 * MiB)

#define P2SB_BAR		CONFIG_PCR_BASE_ADDRESS
#define P2SB_SIZE		(256 * MiB)

/* PCH P2SB2 256MB */
#define P2SB2_BAR		CONFIG_P2SB_2_PCR_BASE_ADDRESS
#define P2SB2_SIZE		(256 * MiB)
#define IOE_P2SB_BAR		P2SB2_BAR
#define IOE_P2SB_SIZE		P2SB2_SIZE

/* IOM_BASE_ADDR = ((long int) Ps2bMmioBase | (int) (((Offset) & 0x0F0000) << 8) \
 * | ((unsigned char)(Pid) << 16) | (short int) ((Offset) & 0xFFFF))
 *
 * Where, Ps2bMmioBase = 0x4010000000, Offset = 0x0, Pid = 0x80
 */
#define IOM_BASE_ADDR		0x4010800000
#define IOM_BASE_SIZE		0x10000
#define IOM_BASE_ADDR_MAX	0x401080ffff /* ((IOM_BASE_ADDR + IOM_BASE_SIZE) - 1) */

/* I/O port address space */
#define ACPI_BASE_ADDRESS	0x1800
#define ACPI_BASE_SIZE		0x100

#define TCO_BASE_ADDRESS	0x400
#define TCO_BASE_SIZE		0x20

#endif /* _SOC_PANTHERLAKE_IOMAP_H_ */
