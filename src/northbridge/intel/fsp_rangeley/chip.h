/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2015 Intel Corporation
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

#ifndef _FSP_RANGELEY_CHIP_H_
#define _FSP_RANGELEY_CHIP_H_

#include <arch/acpi.h>
#include <drivers/intel/fsp1_0/fsp_values.h>

struct northbridge_intel_fsp_rangeley_config {

	/* Set the CPGC exp_loop_cnt field for RMT execution 2^(exp_loop_cnt -1) */
	/* Valid values: 0 - 15 */
	uint8_t MrcRmtCpgcExpLoopCntValue;
	/* Set the CPGC num_bursts field for RMT execution 2^(num_bursts -1) */
	/* Valid values: 0 - 15 */
	uint8_t MrcRmtCpgcNumBursts;
	/* DIMM SPD SMBus Addresses */
	uint8_t SpdBaseAddress_0_0;
	uint8_t SpdBaseAddress_0_1;
	uint8_t SpdBaseAddress_1_0;
	uint8_t SpdBaseAddress_1_1;

	uint8_t EnableLan;
	uint8_t EnableSata2;
	uint8_t EnableSata3;
	uint8_t EnableIQAT;
	uint8_t EnableUsb20;
	uint8_t PrintDebugMessages;
	uint8_t Fastboot;
	uint8_t EccSupport;
	uint8_t SpdWriteProtect;
	/* Enable = Memory Down, Disable = DIMM */
	uint8_t MemoryDown;
	/* Enable the Rank Margin Tool, needs PrintDebugMessages */
	uint8_t MrcRmtSupport;

	/* PCIe port bifurcation control */
	uint8_t Bifurcation;
	#define BIFURCATION_4_4_4_4             0
	#define BIFURCATION_4_4_8               1
	#define BIFURCATION_8_4_4               2
	#define BIFURCATION_8_8                 3
	#define BIFURCATION_16                  4

	/* PCIe port de-emphasis control */
	uint8_t PcdPcieRootPort1DeEmphasis;
	uint8_t PcdPcieRootPort2DeEmphasis;
	uint8_t PcdPcieRootPort3DeEmphasis;
	uint8_t PcdPcieRootPort4DeEmphasis;
	#define DE_EMPHASIS_DEFAULT             0
	#define DE_EMPHASIS_MINUS_6_0_DB        1
	#define DE_EMPHASIS_MINUS_3_5_DB        2
};

#endif
