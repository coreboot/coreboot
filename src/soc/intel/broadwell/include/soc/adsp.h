/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
 * Foundation, Inc.
 */

#ifndef _BROADWELL_ADSP_H_
#define _BROADWELL_ADSP_H_

#define ADSP_PCI_IRQ			23
#define ADSP_ACPI_IRQ			3
#define  ADSP_ACPI_IRQEN		(1 << 3)

#define ADSP_SHIM_BASE_LPT		0xe7000
#define ADSP_SHIM_BASE_WPT		0xfb000
#define  ADSP_SHIM_LTRC			0xe0
#define   ADSP_SHIM_LTRC_VALUE		0x3003
#define  ADSP_SHIM_IMC			0x28
#define  ADSP_SHIM_IPCD			0x40

#define ADSP_PCI_VDRTCTL0		0xa0
#define  ADSP_VDRTCTL0_D3PGD_LPT	(1 << 1)
#define  ADSP_VDRTCTL0_D3PGD_WPT	(1 << 0)
#define  ADSP_VDRTCTL0_D3SRAMPGD_LPT	(1 << 2)
#define  ADSP_VDRTCTL0_D3SRAMPGD_WPT	(1 << 1)
#define ADSP_PCI_VDRTCTL1		0xa4
#define ADSP_PCI_VDRTCTL2		0xa8
#define  ADSP_VDRTCTL2_VALUE		0x00000fff

#define ADSP_IOBP_VDLDAT1		0xd7000624
#define  ADSP_VDLDAT1_VALUE		0x00040100
#define ADSP_IOBP_VDLDAT2		0xd7000628
#define  ADSP_IOBP_ACPI_IRQ3		0xd9d8
#define  ADSP_IOBP_ACPI_IRQ3I		0xd8d9
#define  ADSP_IOBP_ACPI_IRQ4		0xdbda
#define ADSP_IOBP_PMCTL			0xd70001e0
#define  ADSP_PMCTL_VALUE		0x3f
#define ADSP_IOBP_PCICFGCTL		0xd7000500
#define  ADSP_PCICFGCTL_PCICD		(1 << 0)
#define  ADSP_PCICFGCTL_ACPIIE		(1 << 1)
#define  ADSP_PCICFGCTL_SPCBAD		(1 << 7)

#endif
