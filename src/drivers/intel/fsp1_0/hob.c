/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <lib.h> // hexdump
#include "fsp_util.h"


/** Displays a GUID's address and value
 *
 * @param guid pointer to the GUID to display
 */
void printguid(EFI_GUID *guid)
{
	printk(BIOS_SPEW,"Address: %p Guid: %08lx-%04x-%04x-",
			guid, (unsigned long)guid->Data1,
			guid->Data2, guid->Data3);
	printk(BIOS_SPEW,"%02x%02x%02x%02x%02x%02x%02x%02x\n",
			guid->Data4[0], guid->Data4[1],
			guid->Data4[2], guid->Data4[3],
			guid->Data4[4], guid->Data4[5],
			guid->Data4[6], guid->Data4[7] );
}

void print_hob_mem_attributes(void *Hobptr)
{
	EFI_HOB_MEMORY_ALLOCATION *HobMemoryPtr = (EFI_HOB_MEMORY_ALLOCATION *)Hobptr;
	EFI_MEMORY_TYPE Hobmemtype = HobMemoryPtr->AllocDescriptor.MemoryType;
	u64 Hobmemaddr = HobMemoryPtr->AllocDescriptor.MemoryBaseAddress;
	u64 Hobmemlength = HobMemoryPtr->AllocDescriptor.MemoryLength;
	const char * Hobmemtypenames[15];

	Hobmemtypenames[0] = "EfiReservedMemoryType";
	Hobmemtypenames[1] = "EfiLoaderCode";
	Hobmemtypenames[2] = "EfiLoaderData";
	Hobmemtypenames[3] = "EfiBootServicesCode";
	Hobmemtypenames[4] = "EfiBootServicesData";
	Hobmemtypenames[5] = "EfiRuntimeServicesCode";
	Hobmemtypenames[6] = "EfiRuntimeServicesData";
	Hobmemtypenames[7] = "EfiConventionalMemory";
	Hobmemtypenames[8] = "EfiUnusableMemory";
	Hobmemtypenames[9] = "EfiACPIReclaimMemory";
	Hobmemtypenames[10] = "EfiACPIMemoryNVS";
	Hobmemtypenames[11] = "EfiMemoryMappedIO";
	Hobmemtypenames[12] = "EfiMemoryMappedIOPortSpace";
	Hobmemtypenames[13] = "EfiPalCode";
	Hobmemtypenames[14] = "EfiMaxMemoryType";

	printk(BIOS_SPEW, "  Memory type %s (0x%x)\n",
			Hobmemtypenames[(u32)Hobmemtype], (u32) Hobmemtype);
	printk(BIOS_SPEW, "  at location 0x%0lx with length 0x%0lx\n",
			(unsigned long)Hobmemaddr, (unsigned long)Hobmemlength);
}

void print_hob_resource_attributes(void *Hobptr)
{
	EFI_HOB_RESOURCE_DESCRIPTOR *HobResourcePtr =
		(EFI_HOB_RESOURCE_DESCRIPTOR *)Hobptr;
	u32 Hobrestype   = HobResourcePtr->ResourceType;
	u32 Hobresattr   = HobResourcePtr->ResourceAttribute;
	u64 Hobresaddr   = HobResourcePtr->PhysicalStart;
	u64 Hobreslength = HobResourcePtr->ResourceLength;
	const char *Hobrestypestr = NULL;

	// HOB Resource Types
	switch (Hobrestype) {
	case EFI_RESOURCE_SYSTEM_MEMORY:
		Hobrestypestr = "EFI_RESOURCE_SYSTEM_MEMORY"; break;
	case EFI_RESOURCE_MEMORY_MAPPED_IO:
		Hobrestypestr = "EFI_RESOURCE_MEMORY_MAPPED_IO"; break;
	case EFI_RESOURCE_IO:
		Hobrestypestr = "EFI_RESOURCE_IO"; break;
	case EFI_RESOURCE_FIRMWARE_DEVICE:
		Hobrestypestr = "EFI_RESOURCE_FIRMWARE_DEVICE"; break;
	case EFI_RESOURCE_MEMORY_MAPPED_IO_PORT:
		Hobrestypestr = "EFI_RESOURCE_MEMORY_MAPPED_IO_PORT"; break;
	case EFI_RESOURCE_MEMORY_RESERVED:
		Hobrestypestr = "EFI_RESOURCE_MEMORY_RESERVED"; break;
	case EFI_RESOURCE_IO_RESERVED:
		Hobrestypestr = "EFI_RESOURCE_IO_RESERVED"; break;
	case EFI_RESOURCE_MAX_MEMORY_TYPE:
		Hobrestypestr = "EFI_RESOURCE_MAX_MEMORY_TYPE"; break;
	default:
		Hobrestypestr = "EFI_RESOURCE_UNKNOWN"; break;
	}

	printk(BIOS_SPEW, "  Resource %s (0x%0x) has attributes 0x%0x\n",
			Hobrestypestr, Hobrestype, Hobresattr);
	printk(BIOS_SPEW, "  at location 0x%0lx with length 0x%0lx\n",
			(unsigned long)Hobresaddr, (unsigned long)Hobreslength);
}

const char * get_hob_type_string(void *Hobptr)
{
	EFI_HOB_GENERIC_HEADER *HobHeaderPtr = (EFI_HOB_GENERIC_HEADER *)Hobptr;
	u16 Hobtype = HobHeaderPtr->HobType;
	const char *Hobtypestring = NULL;

	switch (Hobtype) {
	case EFI_HOB_TYPE_HANDOFF:
		Hobtypestring = "EFI_HOB_TYPE_HANDOFF"; break;
	case EFI_HOB_TYPE_MEMORY_ALLOCATION:
		Hobtypestring = "EFI_HOB_TYPE_MEMORY_ALLOCATION"; break;
	case EFI_HOB_TYPE_RESOURCE_DESCRIPTOR:
		Hobtypestring = "EFI_HOB_TYPE_RESOURCE_DESCRIPTOR"; break;
	case EFI_HOB_TYPE_GUID_EXTENSION:
		Hobtypestring = "EFI_HOB_TYPE_GUID_EXTENSION"; break;
	case EFI_HOB_TYPE_MEMORY_POOL:
		Hobtypestring = "EFI_HOB_TYPE_MEMORY_POOL"; break;
	case EFI_HOB_TYPE_UNUSED:
		Hobtypestring = "EFI_HOB_TYPE_UNUSED"; break;
	case EFI_HOB_TYPE_END_OF_HOB_LIST:
		Hobtypestring = "EFI_HOB_TYPE_END_OF_HOB_LIST"; break;
	default:
		Hobtypestring = "EFI_HOB_TYPE_UNRECOGNIZED"; break;
	}

	return Hobtypestring;
}

/** Displays the length, location, and GUID value of a GUID extension
 *
 * The EFI_HOB_GUID_TYPE is very basic - it just contains the standard
 * HOB header containing the HOB type and length, and a GUID for
 * identification.  The rest of the data is undefined and must be known
 * based on the GUID.
 *
 * This displays the entire HOB length, and the location of the start
 * of the HOB, *NOT* the length of or the start of the data inside the HOB.
 *
 * @param Hobptr
 */
void print_guid_type_attributes(void *Hobptr)
{
	printk(BIOS_SPEW, "  at location %p with length0x%0lx\n  ",
		Hobptr, (unsigned long)(((EFI_PEI_HOB_POINTERS *) \
		Hobptr)->Guid->Header.HobLength));
	printguid(&(((EFI_HOB_GUID_TYPE *)Hobptr)->Name));

}

/* Print out a structure of all the HOBs
 * that match a certain type:
 * Print all types			(0x0000)
 * EFI_HOB_TYPE_HANDOFF			(0x0001)
 * EFI_HOB_TYPE_MEMORY_ALLOCATION	(0x0002)
 * EFI_HOB_TYPE_RESOURCE_DESCRIPTOR	(0x0003)
 * EFI_HOB_TYPE_GUID_EXTENSION		(0x0004)
 * EFI_HOB_TYPE_MEMORY_POOL		(0x0007)
 * EFI_HOB_TYPE_UNUSED			(0xFFFE)
 * EFI_HOB_TYPE_END_OF_HOB_LIST	(0xFFFF)
 */
void print_hob_type_structure(u16 Hobtype, void *Hoblistptr)
{
	u32 *Currenthob;
	u32 *Nexthob = NULL;
	u8  Lasthob = 0;
	u32 Currenttype;
	const char *Currenttypestr;

	Currenthob = Hoblistptr;

	/* Print out HOBs of our desired type until
	 * the end of the HOB list
	 */
	printk(BIOS_DEBUG, "\n=== FSP HOB Data Structure ===\n");
	printk(BIOS_DEBUG, "FSP Hoblistptr: 0x%0x\n",
			(u32) Hoblistptr);
	do {
		EFI_HOB_GENERIC_HEADER *CurrentHeaderPtr =
			(EFI_HOB_GENERIC_HEADER *)Currenthob;
		Currenttype = CurrentHeaderPtr->HobType;  /* Get the type of this HOB */
		Currenttypestr = get_hob_type_string(Currenthob);

		if (Currenttype == Hobtype || Hobtype == 0x0000) {
			printk(BIOS_DEBUG, "HOB 0x%0x is an %s (type 0x%0x)\n",
					(u32) Currenthob, Currenttypestr, Currenttype);
			switch (Currenttype) {
			case EFI_HOB_TYPE_MEMORY_ALLOCATION:
				print_hob_mem_attributes(Currenthob); break;
			case EFI_HOB_TYPE_RESOURCE_DESCRIPTOR:
				print_hob_resource_attributes(Currenthob); break;
			case EFI_HOB_TYPE_GUID_EXTENSION:
				print_guid_type_attributes(Currenthob);	break;
			}
		}

		Lasthob = END_OF_HOB_LIST(Currenthob);	/* Check for end of HOB list */
		if (!Lasthob) {
			Nexthob = GET_NEXT_HOB(Currenthob);	/* Get next HOB pointer */
			Currenthob = Nexthob;	// Start on next HOB
		}
	} while (!Lasthob);
	printk(BIOS_DEBUG, "=== End of FSP HOB Data Structure ===\n\n");
}


/** Finds a HOB entry based on type and guid
 *
 * @param current_hob pointer to the start of the HOB list
 * @param guid the GUID of the HOB entry to find
 * @return pointer to the start of the requested HOB or NULL if not found.
 */
void * find_hob_by_guid(void *current_hob, EFI_GUID *guid)
{
	do {
		switch (((EFI_HOB_GENERIC_HEADER *)current_hob)->HobType) {

		case EFI_HOB_TYPE_MEMORY_ALLOCATION:
			if (guids_are_equal(guid, &(((EFI_HOB_MEMORY_ALLOCATION *) \
				current_hob)->AllocDescriptor.Name)))
				return current_hob;
			break;
		case EFI_HOB_TYPE_RESOURCE_DESCRIPTOR:
			if (guids_are_equal(guid,
				&(((EFI_HOB_RESOURCE_DESCRIPTOR *) \
				current_hob)->Owner)))
				return current_hob;
			break;
		case EFI_HOB_TYPE_GUID_EXTENSION:
			if (guids_are_equal(guid, &(((EFI_HOB_GUID_TYPE *) \
					current_hob)->Name)))
				return current_hob;
			break;
		}

		if (!END_OF_HOB_LIST(current_hob))
			current_hob = GET_NEXT_HOB(current_hob); /* Get next HOB pointer */
	} while (!END_OF_HOB_LIST(current_hob));

	return NULL;
}

/** Compares a pair of GUIDs to see if they are equal
 *
 * GUIDs are 128 bits long, so compare them as pairs of quadwords.
 *
 * @param guid1 pointer to the first of the GUIDs to compare
 * @param guid2 pointer to the second of the GUIDs to compare
 * @return 1 if the GUIDs were equal, 0 if GUIDs were not equal
 */
uint8_t guids_are_equal(EFI_GUID *guid1, EFI_GUID *guid2)
{
	uint64_t* guid_1 = (void *) guid1;
	uint64_t* guid_2 = (void *) guid2;

	if ((*(guid_1) != *(guid_2)) || (*(guid_1 + 1) != *(guid_2 + 1)))
		return 0;

	return 1;
}
