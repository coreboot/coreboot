/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <console/console.h>

#include <arch/cpu.h>
#include <arch/io.h>
#include "raminit.h"
#include <northbridge/amd/amdmct/amddefs.h>

#ifndef __PRE_RAM__
#include <include/device/pci_ops.h>
#include <include/device/pci_def.h>
u32 Get_NB32(u32 dev, u32 reg)
{
        return pci_read_config32(dev_find_slot(0, PCI_DEV2DEVFN(dev)), reg);
}
#endif

uint64_t mctGetLogicalCPUID(u32 Node)
{
	/* Converts the CPUID to a logical ID MASK that is used to check
	 CPU version support versions */
	u32 dev;
	u32 val, valx;
	u32 family, model, stepping;
	uint64_t ret;

	if (Node == 0xFF) { /* current node */
		val = cpuid_eax(0x80000001);
	} else {
		dev = PA_NBMISC(Node);
		val = Get_NB32(dev, 0xfc);
	}

	family = ((val >> 8) & 0x0f) + ((val >> 20) & 0xff);
	model = ((val >> 4) & 0x0f) | ((val >> (16-4)) & 0xf0);
	stepping = val & 0x0f;

	valx = (family << 12) | (model << 4) | (stepping);

	switch (valx) {
	case 0x10000:
		ret = AMD_DR_A0A;
		break;
	case 0x10001:
		ret = AMD_DR_A1B;
		break;
	case 0x10002:
		ret = AMD_DR_A2;
		break;
	case 0x10020:
		ret = AMD_DR_B0;
		break;
	case 0x10021:
		ret = AMD_DR_B1;
		break;
	case 0x10022:
		ret = AMD_DR_B2;
		break;
	case 0x10023:
		ret = AMD_DR_B3;
		break;
	case 0x10042:
		ret = AMD_RB_C2;
		break;
	case 0x10043:
		ret = AMD_RB_C3;
		break;
	case 0x10062:
		ret = AMD_DA_C2;
		break;
	case 0x10063:
		ret = AMD_DA_C3;
		break;
	case 0x10080:
		ret = AMD_HY_D0;
		break;
	case 0x10081:
	case 0x10091:
		ret = AMD_HY_D1;
		break;
	case 0x100a0:
		ret = AMD_PH_E0;
		break;
	case 0x15012:
	case 0x1501f:
		ret = AMD_OR_B2;
		break;
	case 0x15020:
	case 0x15101:
		ret = AMD_OR_C0;
		break;
	default:
		/* FIXME: mabe we should die() here. */
		printk(BIOS_ERR, "FIXME! CPU Version unknown or not supported! %08x\n", valx);
		ret = 0;
	}

	return ret;
}
