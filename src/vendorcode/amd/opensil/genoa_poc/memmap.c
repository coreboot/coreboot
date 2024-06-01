/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <stdint.h>
#include <stdbool.h>
#include <SilCommon.h>
#include <Sil-api.h> // needed above ApobCmn.h
#include <ApobCmn.h>
#include <device/device.h>
#include <xPRF-api.h>
#include <cpu/amd/mtrr.h>
#include <cbmem.h>
#include <amdblocks/memmap.h>

#include "opensil.h"

static const char *hole_info_type(MEMORY_HOLE_TYPES type)
{
	const struct hole_type {
		MEMORY_HOLE_TYPES type;
		const char *string;
	} types[] = {
		{UMA, "UMA"},
		{MMIO, "MMIO"},
		{PrivilegedDRAM, "PrivilegedDRAM"},
		{Reserved1TbRemap, "Reserved1TbRemap"},
		{ReservedSLink, "ReservedSLink"},
		{ReservedSLinkAlignment, "ReservedSLinkAlignment"},
		{ReservedDrtm, "ReservedDrtm"},
		{ReservedCvip, "ReservedCvip"},
		{ReservedSmuFeatures, "ReservedSmuFeatures"},
		{ReservedFwtpm, "ReservedFwtpm"},
		{ReservedMpioC20, "ReservedMpioC20"},
		{ReservedNbif, "ReservedNbif"},
		{ReservedCxl, "ReservedCxl"},
		{ReservedCxlAlignment, "ReservedCxlAlignment"},
		{ReservedCpuTmr, "ReservedCpuTmr"},
		{ReservedRasEinj, "ReservedRasEinj"},
		{MaxMemoryHoleTypes, "MaxMemoryHoleTypes"},
	};

	int i;
	for (i = 0; i < ARRAY_SIZE(types); i++)
		if (type == types[i].type)
			break;
	if (i == ARRAY_SIZE(types))
		return "Unknown type";
	return types[i].string;
}

static uint64_t top_of_mem;
static uint32_t n_holes;
static MEMORY_HOLE_DESCRIPTOR *hole_info;

static void get_hole_info(void)
{
	static bool done;
	if (done)
		return;
        SIL_STATUS status = xPrfGetSystemMemoryMap(&n_holes, &top_of_mem, (void **)&hole_info);
	SIL_STATUS_report("xPrfGetSystemMemoryMap", status);
	// Make sure hole_info does not get initialized to something odd by xPRF on failure
	if (status != SilPass)
		hole_info = NULL;
	done = true;
}


static void print_memory_holes(void *unused)
{
	get_hole_info();
	if (hole_info == NULL)
		return;

	printk(BIOS_DEBUG, "APOB: top of memory 0x%016llx\n", top_of_mem);
	printk(BIOS_DEBUG, "The following holes are reported in APOB\n");
	for (int hole = 0; hole < n_holes; hole++) {
		printk(BIOS_DEBUG, "  Base: 0x%016llx, Size: 0x%016llx, Type: %02d:%s\n",
		       hole_info[hole].Base, hole_info[hole].Size, hole_info[hole].Type,
		       hole_info_type(hole_info[hole].Type));
	}
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_ENTRY, print_memory_holes, NULL);

// This assumes holes are allocated
void add_opensil_memmap(struct device *dev, unsigned long *idx)
{
	// Account for UMA and TSEG
	const uint32_t mem_usable = cbmem_top();
	const uint32_t top_mem = ALIGN_DOWN(get_top_of_mem_below_4gb(), 1 * MiB);
	if (mem_usable != top_mem)
		reserved_ram_from_to(dev, (*idx)++, mem_usable, top_mem);

	// Check if we're done
	if (top_of_mem <= 4ULL * GiB)
		return;

	// Holes in upper DRAM
	// This assumes all the holes in upper DRAM are continuous
	get_hole_info();
	if (hole_info == NULL)
		return;
	uint64_t lowest_upper_hole_base = top_of_mem;
	uint64_t highest_upper_hole_end = 4ULL * GiB;
	for (int hole = 0; hole < n_holes; hole++) {
		if (hole_info[hole].Type == MMIO)
			continue;
		if (hole_info[hole].Base < 4ULL * GiB)
			continue;
		lowest_upper_hole_base = MIN(lowest_upper_hole_base, hole_info[hole].Base);
		highest_upper_hole_end = MAX(highest_upper_hole_end, hole_info[hole].Base + hole_info[hole].Size);
		if (hole_info[hole].Type == UMA)
			mmio_range(dev, (*idx)++, hole_info[hole].Base, hole_info[hole].Size);
		else
			reserved_ram_range(dev, (*idx)++, hole_info[hole].Base, hole_info[hole].Size);
	}

	ram_from_to(dev, (*idx)++, 4ULL * GiB, lowest_upper_hole_base);

	// Do we need this?
	if (top_of_mem > highest_upper_hole_end)
		ram_from_to(dev, (*idx)++, highest_upper_hole_end, top_of_mem);
}
