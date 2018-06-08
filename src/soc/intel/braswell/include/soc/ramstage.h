/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#ifndef _SOC_RAMSTAGE_H_
#define _SOC_RAMSTAGE_H_

#include <chip.h>
#include <device/device.h>
#include <fsp/ramstage.h>

#define V_PCH_LPC_RID_A0		0x00  // A0 Stepping
#define V_PCH_LPC_RID_A1		0x04  // A1 Stepping
#define V_PCH_LPC_RID_A2		0x08  // A2 Stepping
#define V_PCH_LPC_RID_A3		0x0C  // A3 Stepping
#define V_PCH_LPC_RID_A4		0x80  // A4 Stepping
#define V_PCH_LPC_RID_A5		0x84  // A5 Stepping
#define V_PCH_LPC_RID_A6		0x88  // A6 Stepping
#define V_PCH_LPC_RID_A7		0x8C  // A7 Stepping
#define V_PCH_LPC_RID_B0		0x10  // B0 Stepping
#define V_PCH_LPC_RID_B1		0x14  // B1 Stepping
#define V_PCH_LPC_RID_B2		0x18  // B2 Stepping
#define V_PCH_LPC_RID_B3		0x1C  // B3 Stepping
#define V_PCH_LPC_RID_B4		0x90  // B4 Stepping
#define V_PCH_LPC_RID_B5		0x94  // B5 Stepping
#define V_PCH_LPC_RID_B6		0x98  // B6 Stepping
#define V_PCH_LPC_RID_B7		0x9C  // B7 Stepping
#define V_PCH_LPC_RID_C0		0x20  // C0 Stepping
#define V_PCH_LPC_RID_C1		0x24  // C1 Stepping
#define V_PCH_LPC_RID_C2		0x28  // C2 Stepping
#define V_PCH_LPC_RID_C3		0x2C  // C3 Stepping
#define V_PCH_LPC_RID_C4		0xA0  // C4 Stepping
#define V_PCH_LPC_RID_C5		0xA4  // C5 Stepping
#define V_PCH_LPC_RID_C6		0xA8  // C6 Stepping
#define V_PCH_LPC_RID_C7		0xAC  // C7 Stepping
#define V_PCH_LPC_RID_D0		0x30  // D0 Stepping
#define V_PCH_LPC_RID_D1		0x34  // D1 Stepping
#define V_PCH_LPC_RID_D2		0x38  // D2 Stepping
#define V_PCH_LPC_RID_D3		0x3C  // D3 Stepping
#define V_PCH_LPC_RID_D4		0xB0  // D4 Stepping
#define V_PCH_LPC_RID_D5		0xB4  // D5 Stepping
#define V_PCH_LPC_RID_D6		0xB8  // D6 Stepping
#define V_PCH_LPC_RID_D7		0xBC  // D7 Stepping
#define B_PCH_LPC_RID_STEPPING_MASK	0xFC  // SoC Stepping Mask (Ignoring Package Type)

enum {
	SocA0		= 0,
	SocA1		= 1,
	SocA2		= 2,
	SocA3		= 3,
	SocA4		= 4,
	SocA5		= 5,
	SocA6		= 6,
	SocA7		= 7,
	SocB0		= 8,
	SocB1		= 9,
	SocB2		= 10,
	SocB3		= 11,
	SocB4		= 12,
	SocB5		= 13,
	SocB6		= 14,
	SocB7		= 15,
	SocC0		= 16,
	SocC1		= 17,
	SocC2		= 18,
	SocC3		= 19,
	SocC4		= 20,
	SocC5		= 21,
	SocC6		= 22,
	SocC7		= 23,
	SocD0		= 24,
	SocD1		= 25,
	SocD2		= 26,
	SocD3		= 27,
	SocD4		= 28,
	SocD5		= 29,
	SocD6		= 30,
	SocD7		= 31,
	SocSteppingMax
};

/*
 * The soc_init_pre_device() function is called prior to device
 * initialization, but it's after console and cbmem has been reinitialized.
 */
void soc_init_pre_device(struct soc_intel_braswell_config *config);
void soc_init_cpus(struct device *dev);
void set_max_freq(void);
void southcluster_enable_dev(struct device *dev);
void scc_enable_acpi_mode(struct device *dev, int iosf_reg, int nvs_index);
int SocStepping(void);
void board_silicon_USB2_override(SILICON_INIT_UPD *params);

extern struct pci_operations soc_pci_ops;

#endif /* _SOC_RAMSTAGE_H_ */
