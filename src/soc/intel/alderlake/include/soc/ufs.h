/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Alder Lake Processor PCH Datasheet
 * Document number: 645550
 */

#ifndef _SOC_ALDERLAKE_UFS_H_
#define _SOC_ALDERLAKE_UFS_H_

#include <soc/pci_devs.h>

/* Calculate _ADR for Intel UFS Controller */
#define UFS_ACPI_DEVICE (PCH_DEV_SLOT_ISH << 16 | 0x0007)

#define R_SCS_CFG_PCS		0x84
#define R_SCS_CFG_PG_CONFIG	0xA2

#define R_SCS_PCR_1C20		0x1C20
#define R_SCS_PCR_4820		0x4820
#define R_SCS_PCR_4020		0x4020
#define R_SCS_PCR_5820		0x5820
#define R_SCS_PCR_5C20		0x5C20
#define R_SCS_PCR_1078		0x1078

#define R_PMC_PWRM_LTR_IGN	0x1b0c
#endif
