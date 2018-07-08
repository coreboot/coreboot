/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "update_ucode.h"
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <stddef.h>
#include <cpu/cpu.h>
#include <arch/cpu.h>
#include <cbfs.h>

static ucode_update_status nano_apply_ucode(const nano_ucode_header *ucode)
{
	printk(BIOS_SPEW, "Attempting to apply microcode update\n");

	msr_t msr;
	/* Address of ucode block goes in msr.lo for 32-bit mode
	 * Now remember, we need to pass the address of the actual microcode,
	 * not the header. The header is just there to help us. */
	msr.lo = (unsigned int)(&(ucode->ucode_start));
	msr.hi = 0;
	wrmsr(MSR_IA32_BIOS_UPDT_TRIG, msr);

	/* Let's see if we updated successfully */
	msr = rdmsr(MSR_UCODE_UPDATE_STATUS);

	return msr.lo & 0x07;
}

static void nano_print_ucode_info(const nano_ucode_header *ucode)
{
	printk(BIOS_SPEW, "Microcode update information:\n");
	printk(BIOS_SPEW, "Name: %8s\n", ucode->name );
	printk(BIOS_SPEW, "Date: %u/%u/%u\n", ucode->month,
	       ucode->day, ucode->year );
}

static ucode_validity nano_ucode_is_valid(const nano_ucode_header *ucode)
{
	/* We must have a valid signature */
	if (ucode->signature != NANO_UCODE_SIGNATURE)
		return NANO_UCODE_SIGNATURE_ERROR;
	/* The size of the head must be exactly 12 double words */
	if ( (ucode->total_size - ucode->payload_size) != NANO_UCODE_HEADER_SIZE)
		return NANO_UCODE_WRONG_SIZE;

	/* How about a checksum ? Checksum must be 0
	 * Two's complement done over the entire file, including the header */
	int i;
	u32 check = 0;
	u32 *raw = (void *) ucode;
	for (i = 0; i < ((ucode->total_size) >> 2); i++) {
		check += raw[i];
	}
	if (check != 0)
		return NANO_UCODE_CHECKSUM_FAIL;
	/* Made it here huh? Then it looks valid to us.
	 * If there's anything else wrong, the CPU will reject the update */
	return NANO_UCODE_VALID;
}

static void nano_print_ucode_status(ucode_update_status stat)
{
	switch(stat)
	{
	case UCODE_UPDATE_SUCCESS:
		printk(BIOS_INFO, "Microcode update successful.\n");
		break;
	case UCODE_UPDATE_FAIL:
		printk(BIOS_ALERT, "Microcode update failed, bad environment."
				   "Update was not applied.\n");
		break;
	case UCODE_UPDATE_WRONG_CPU:
		printk(BIOS_ALERT, "Update not applicable to this CPU.\n");
		break;
	case UCODE_INVALID_UPDATE_BLOCK:
		printk(BIOS_ALERT, "Microcode block invalid."
				   "Update was not applied.\n");
		break;
	default:
		printk(BIOS_ALERT, "Unknown status. No update applied.\n");
	}
}

unsigned int nano_update_ucode(void)
{
	size_t i;
	unsigned int n_updates = 0;
	u32 fms = cpuid_eax(0x1);
	/* Considering we are running with eXecute-In-Place (XIP), there's no
	 * need to worry that accessing data from ROM will slow us down.
	 * Microcode data should be aligned to a 4-byte boundary, but CBFS
	 * already does that for us (Do you, CBFS?) */
	u32 *ucode_data;
	size_t ucode_len;

	ucode_data = cbfs_boot_map_with_leak("cpu_microcode_blob.bin",
					     CBFS_TYPE_MICROCODE, &ucode_len);
	/* Oops, did you forget to include the microcode ? */
	if (ucode_data == NULL) {
		printk(BIOS_ALERT, "WARNING: No microcode file found in CBFS. "
				   "Aborting microcode updates\n");
		return 0;
	}

	/* We might do a lot of loops searching for the microcode updates, but
	 * keep in mind, nano_ucode_is_valid searches for the signature before
	 * doing anything else. */
	for ( i = 0; i < (ucode_len >> 2); /* don't increment i here */ )
	{
		ucode_update_status stat;
		const nano_ucode_header * ucode = (void *)(&ucode_data[i]);
		if (nano_ucode_is_valid(ucode) != NANO_UCODE_VALID) {
			i++;
			continue;
		}
		/* Since we have a valid microcode, there's no need to search
		 * in this region, so we restart our search at the end of this
		 * microcode */
		i += (ucode->total_size >> 2);
		/* Is the microcode compatible with our CPU? */
		if (ucode->applicable_fms != fms) continue;
		/* For our most curious users */
		nano_print_ucode_info(ucode);
		/* The meat of the pie */
		stat = nano_apply_ucode(ucode);
		/* The user might want to know how the update went */
		nano_print_ucode_status(stat);
		if (stat == UCODE_UPDATE_SUCCESS) n_updates++;
	}

	return n_updates;
}
