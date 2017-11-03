/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Raptor Engineering, LLC
 * Copyright 2017 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_STONEYRIDGE_IOMAP_H__
#define __SOC_STONEYRIDGE_IOMAP_H__

/* MMIO Ranges */
#define SPI_BASE_ADDRESS		0xfec10000
#define IO_APIC2_ADDR			0xfec20000

#if IS_ENABLED(CONFIG_HPET_ADDRESS_OVERRIDE)
#error HPET address override is not allowed and must be fixed at 0xfed00000
#endif
#define HPET_BASE_ADDRESS		0xfed00000

/* Register blocks at fixed offsets from FED8_0000h and enabled in PMx04[1] */
#define AMD_SB_ACPI_MMIO_ADDR		0xfed80000
#define APU_SMI_BASE			0xfed80200
#define PM_MMIO_BASE			0xfed80300
#define APU_UART0_BASE			0xfedc6000
#define APU_UART1_BASE			0xfedc8000

#define FLASH_BASE_ADDR			((0xffffffff - CONFIG_ROM_SIZE) + 1)

/* I/O Ranges */
#define ACPI_SMI_CTL_PORT		0xb2
#define STONEYRIDGE_ACPI_IO_BASE	CONFIG_STONEYRIDGE_ACPI_IO_BASE
#define  ACPI_PM_EVT_BLK	(STONEYRIDGE_ACPI_IO_BASE + 0x00) /* 4 bytes */
#define  ACPI_PM1_CNT_BLK	(STONEYRIDGE_ACPI_IO_BASE + 0x04) /* 2 bytes */
#define  ACPI_CPU_CONTROL	(STONEYRIDGE_ACPI_IO_BASE + 0x08) /* 6 bytes */
#define  ACPI_GPE0_BLK		(STONEYRIDGE_ACPI_IO_BASE + 0x10) /* 8 bytes */
#define  ACPI_PM_TMR_BLK	(STONEYRIDGE_ACPI_IO_BASE + 0x18) /* 4 bytes */
#define SMB_BASE_ADDR			0xb00
#define AB_INDX				0xcd8
#define AB_DATA				(AB_INDX+4)
#define SYS_RESET			0xcf9

#endif /* __SOC_STONEYRIDGE_IOMAP_H__ */
