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

static ucode_update_status nano_apply_ucode(const u32* location)
{
	printk(BIOS_INFO, "Attempting to apply microcode update\n");

	msr_t msr;
	const nano_ucode_header * head = (void *)location;

	/* FIXME: We are not sure this is needed, but we are doing it anyway */
	msr.hi = msr.lo = 0;
	//wrmsr(MSR_IA32_BIOS_SIGN_ID, msr);

	/* Address of ucode block goes in msr.lo for 32-bit mode
	 * Now remember, we need to pass the address of the actual microcode,
	 * not the header. The header is just there to help us.
	 */
	msr.lo = (unsigned int)(&(head->ucode_start));
	msr.hi = 0;
	wrmsr(MSR_IA32_BIOS_UPDT_TRIG, msr);

	/* Let's see if we updated succesfully */
	msr = rdmsr(MSR_UCODE_UPDATE_STATUS);

	return msr.lo & 0x07;
}

static void nano_print_ucode_info(const u32* location)
{
	const nano_ucode_header * head = (void *)location;
	printk(BIOS_SPEW, "Microcode update information:\n");
	printk(BIOS_SPEW, "Name: %8s\n",
		head->name
	);
	printk(BIOS_SPEW, "Date: %u/%u/%u\n", head->month,
	       head->day, head->year );
}

static ucode_validity nano_ucode_is_valid(const u32* location)
{
	const nano_ucode_header * head = (void *)location;
	/* We must have a valid signature */
	if(head->signature != NANO_UCODE_SIGNATURE)
		return NANO_UCODE_SIGNATURE_ERROR;
	/* The size of the head must be exactly 12 double words */
	if(( head->total_size - head->payload_size) != NANO_UCODE_HEADER_SIZE)
		return NANO_UCODE_WRONG_SIZE;

	/* How about a checksum ? Checksum must be 0
	 * Two's complement done over the entire file, including the header */
	int i; u32 check = 0;
	for(i = 0 ; i < ((head->total_size) >> 2); i++) {
		check += location[i];
	}
	if(check != 0)
		return NANO_UCODE_CHECKSUM_FAIL;
	/* Made it here huh?
	 * If there's anything else wrong, the CPU will reject the update */
	return NANO_UCODE_VALID;
}

static void nano_print_ucode_invalid_reason(ucode_validity why)
{
	switch(why) {
	case NANO_UCODE_VALID:
		break; /* No error */
	case NANO_UCODE_SIGNATURE_ERROR:
		printk(BIOS_SPEW, "Microcode signature is invalid.\n");
		break;
	case NANO_UCODE_WRONG_SIZE:
		printk(BIOS_SPEW, "Microcode header has wrong size.\n");
		break;
	case NANO_UCODE_CHECKSUM_FAIL:
		printk(BIOS_SPEW, "Microcode checksum verification failed.\n");
		break;
	default:
		/* We should _never get this */
		printk(BIOS_SPEW, "Microcode check failed with unknown code."
				  "Please report this issue.\n" );
	}
}

static void nano_print_ucode_status(ucode_update_status stat)
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

unsigned int nano_update_ucode(void)
{
	unsigned int n_updates = 0;
	u32 fms = cpuid_eax(0x1);
	const nano_ucode_header * head;
	/* Nano CPUs are able to combine multiple concurrent microcode updates,
	 * Thus, we may have more than one update to apply */
	size_t i;
	for(i = 0; microcode_updates[i] != 0; i++)
	{
		ucode_update_status stat;
		const u32* current = microcode_updates[i];
		head = (void *)current;
		/* Check if update matches our CPU */
		if(head->applicable_fms != fms) continue;
		/* HA!!!! Update found, but is it valid */
		ucode_validity is_valid = nano_ucode_is_valid(current);
		if(is_valid != NANO_UCODE_VALID) {
			nano_print_ucode_invalid_reason(is_valid);
			continue;
		}
		nano_print_ucode_info(current);
		stat = nano_apply_ucode(current);
		/* The user might want to know how the update went */
		nano_print_ucode_status(stat);
		if(stat == UCODE_UPDATE_SUCCESS) n_updates++;
	}

	return n_updates;

}