/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
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
#include <fsp/util.h>

void soc_display_fspm_upd_params(const FSPM_UPD *fspm_old_upd,
	const FSPM_UPD *fspm_new_upd)
{
	const FSP_M_CONFIG *new;
	const FSP_M_CONFIG *old;

	old = &fspm_old_upd->FspmConfig;
	new = &fspm_new_upd->FspmConfig;

	/* Display the parameters for MemoryInit */
	printk(BIOS_SPEW, "UPD values for MemoryInit at: %p\n", new);
	fsp_display_upd_value("AddrMode", sizeof(old->AddrMode),
		old->AddrMode, new->AddrMode);
	fsp_display_upd_value("ChanMask", sizeof(old->ChanMask),
		old->ChanMask, new->ChanMask);
	fsp_display_upd_value("ChanWidth", sizeof(old->ChanWidth),
		old->ChanWidth, new->ChanWidth);
	fsp_display_upd_value("DramDensity", sizeof(old->DramDensity),
		old->DramDensity, new->DramDensity);
	fsp_display_upd_value("DramRonVal", sizeof(old->DramRonVal),
		old->DramRonVal, new->DramRonVal);
	fsp_display_upd_value("DramRttNomVal", sizeof(old->DramRttNomVal),
		old->DramRttNomVal, new->DramRttNomVal);
	fsp_display_upd_value("DramRttWrVal", sizeof(old->DramRttWrVal),
		old->DramRttWrVal, new->DramRttWrVal);
	fsp_display_upd_value("DramSpeed", sizeof(old->DramSpeed),
		old->DramSpeed, new->DramSpeed);
	fsp_display_upd_value("DramType", sizeof(old->DramType),
		old->DramType, new->DramType);
	fsp_display_upd_value("DramWidth", sizeof(old->DramWidth),
		old->DramWidth, new->DramWidth);
	fsp_display_upd_value("EccScrubBlkSize", sizeof(old->EccScrubBlkSize),
		old->EccScrubBlkSize, new->EccScrubBlkSize);
	fsp_display_upd_value("EccScrubInterval", sizeof(old->EccScrubInterval),
		old->EccScrubInterval, new->EccScrubInterval);
	fsp_display_upd_value("Flags", sizeof(old->Flags), old->Flags,
		new->Flags);
	fsp_display_upd_value("FspReservedMemoryLength",
		sizeof(old->FspReservedMemoryLength),
		old->FspReservedMemoryLength, new->FspReservedMemoryLength);
	fsp_display_upd_value("RankMask", sizeof(old->RankMask), old->RankMask,
		new->RankMask);
	fsp_display_upd_value("RmuBaseAddress", sizeof(old->RmuBaseAddress),
		old->RmuBaseAddress, new->RmuBaseAddress);
	fsp_display_upd_value("RmuLength", sizeof(old->RmuLength),
		old->RmuLength, new->RmuLength);
	fsp_display_upd_value("SerialPortPollForChar",
		sizeof(old->SerialPortPollForChar),
		old->SerialPortPollForChar, new->SerialPortPollForChar);
	fsp_display_upd_value("SerialPortReadChar",
		sizeof(old->SerialPortReadChar),
		old->SerialPortReadChar, new->SerialPortReadChar);
	fsp_display_upd_value("SerialPortWriteChar",
		sizeof(old->SerialPortWriteChar),
		old->SerialPortWriteChar, new->SerialPortWriteChar);
	fsp_display_upd_value("SmmTsegSize", sizeof(old->SmmTsegSize),
		old->SmmTsegSize, new->SmmTsegSize);
	fsp_display_upd_value("SocRdOdtVal", sizeof(old->SocRdOdtVal),
		old->SocRdOdtVal, new->SocRdOdtVal);
	fsp_display_upd_value("SocWrRonVal", sizeof(old->SocWrRonVal),
		old->SocWrRonVal, new->SocWrRonVal);
	fsp_display_upd_value("SocWrSlewRate", sizeof(old->SocWrSlewRate),
		old->SocWrSlewRate, new->SocWrSlewRate);
	fsp_display_upd_value("SrInt", sizeof(old->SrInt), old->SrInt,
		new->SrInt);
	fsp_display_upd_value("SrTemp", sizeof(old->SrTemp), old->SrTemp,
		new->SrTemp);
	fsp_display_upd_value("tCL", sizeof(old->tCL), old->tCL, new->tCL);
	fsp_display_upd_value("tFAW", sizeof(old->tFAW), old->tFAW, new->tFAW);
	fsp_display_upd_value("tRAS", sizeof(old->tRAS), old->tRAS, new->tRAS);
	fsp_display_upd_value("tRRD", sizeof(old->tRRD), old->tRRD, new->tRRD);
	fsp_display_upd_value("tWTR", sizeof(old->tWTR), old->tWTR, new->tWTR);
}
