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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "update_ucode.h"
#include "nano_ucode_010f07d9.h"
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <stddef.h>
#include <cpu/cpu.h>
#include <arch/cpu.h>

typedef struct nano_ucode_header_st {
	u32 signature;
	u32 update_revision;

	u16 year;
	u8 day;
	u8 month;

	u32 applicable_fms;
	u32 checksum;
	u32 loader_revision;

	u32 rfu_1;
	u32 size;
	u32 payload_size;

	char name[8];

	u32 rfu_2;

}nano_ucode_header;

typedef u8 ucode_status;

static ucode_status nano_apply_ucode(const u32* location)
{
	printk(BIOS_INFO, "Attempting to apply microcode update\n");

	msr_t msr;

	msr.hi = msr.lo = 0;
	wrmsr(0x8b, msr);

	/* Address of ucode block goes in msr.lo for 32-bit mode */
	msr.lo = (unsigned int)(location);
	msr.hi = 0;
	wrmsr(MSR_IA32_BIOS_UPDT_TRIG, msr);

	/* Let's see if we updated succesfully */
	msr = rdmsr(MSR_UCODE_UPDATE_STATUS);

	return msr.lo & 0x07;
}

static void nano_print_ucode_info(const u32* location)
{
	nano_ucode_header * head = (void *)location;
	printk(BIOS_SPEW, "Microcode update information:\n");
	printk(BIOS_SPEW, "RAM address 0x%.8x:\n", (u32) location);
	printk(BIOS_SPEW, "Name: %8s\n",
		head->name
	);
	printk(BIOS_SPEW, "Date: %u/%u/%u\n", head->month,
	       head->day, head->year );

	/* How about a checksum ?
	 * Two's complement done over the entire file, including the header */
	int i; u32 check = 0;
	for(i = 0 ; i < (head->size >> 2); i++)
	{
		check += location[i];
	}
	if(check != 0) printk(BIOS_SPEW, "Checksum failed: 0x%x\n", check);
}

static void nano_print_ucode_status(ucode_status stat)
{
	switch(stat)
	{
	case UCODE_UPDATE_SUCCESS:
		printk(BIOS_INFO, "Microcode update succesful.\n");
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

void nano_update_ucode(void)
{
	msr_t msr;

	u32 fms = cpuid_eax(0x1);
	msr = rdmsr(MSR_IA32_BIOS_SIGN_ID);

	printk(BIOS_INFO, " Update ID before: 0x%.8x%.8x\n", msr.hi, msr.lo);

	/* Nano CPUs are able to combine multiple concurrent microcode updates,
	 * Thus, we may have more than one update to apply */
	size_t i;
	for(i = 0; microcode_updates[i] != 0; i++)
	{
		ucode_status stat;
		const u32* current = microcode_updates[i];
		/* Check if update matches our CPU */
		if(current[3] != fms) continue;
		/* HA!!!! Update found */
		nano_print_ucode_info(current);
		stat = nano_apply_ucode(current);
		/* The user might want to know how the update went */
		nano_print_ucode_status(stat);
	}

	fms = cpuid_eax(0x1);
	msr = rdmsr(MSR_IA32_BIOS_SIGN_ID);

	printk(BIOS_SPEW, " Update ID after: 0x%.8x%.8x\n", msr.hi, msr.lo);

}