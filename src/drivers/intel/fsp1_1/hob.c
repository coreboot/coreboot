/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hlt.h>
#include <console/console.h>
#include <fsp/util.h>
#include <ip_checksum.h>
#include <string.h>

/* Compares two EFI GUIDs. Returns true of the GUIDs match, false otherwise. */
static bool compare_guid(const EFI_GUID *guid1, const EFI_GUID *guid2)
{
	return !memcmp(guid1, guid2, sizeof(EFI_GUID));
}

/* Returns the pointer to the HOB list. */
void *get_hob_list(void)
{
	void *hob_list;

	hob_list = fsp_get_hob_list();
	if (hob_list == NULL)
		die("Call fsp_set_runtime() before this call!\n");
	return hob_list;
}

/* Returns the next instance of a HOB type from the starting HOB. */
void *get_next_hob(uint16_t type, const void *hob_start)
{
	EFI_PEI_HOB_POINTERS hob;

	if (!hob_start)
		return NULL;

	hob.Raw = (UINT8 *)hob_start;

	/* Parse the HOB list until end of list or matching type is found. */
	while (!END_OF_HOB_LIST(hob.Raw)) {
		if (hob.Header->HobType == type)
			return hob.Raw;
		if (GET_HOB_LENGTH(hob.Raw) < sizeof(*hob.Header))
			break;
		hob.Raw = GET_NEXT_HOB(hob.Raw);
	}
	return NULL;
}

/* Returns the first instance of a HOB type among the whole HOB list. */
void *get_first_hob(uint16_t type)
{
	return get_next_hob(type, get_hob_list());
}

/* Returns the next instance of the matched GUID HOB from the starting HOB. */
void *get_next_guid_hob(const EFI_GUID *guid, const void *hob_start)
{
	EFI_PEI_HOB_POINTERS hob;

	hob.Raw = (uint8_t *)hob_start;
	while ((hob.Raw = get_next_hob(EFI_HOB_TYPE_GUID_EXTENSION, hob.Raw))
					!= NULL) {
		if (compare_guid(guid, &hob.Guid->Name))
			break;
		hob.Raw = GET_NEXT_HOB(hob.Raw);
	}
	return hob.Raw;
}

/*
 * Returns the first instance of the matched GUID HOB among the whole HOB list.
 */
void *get_first_guid_hob(const EFI_GUID *guid)
{
	return get_next_guid_hob(guid, get_hob_list());
}

/*
 * Returns the next instance of the matching resource HOB from the starting HOB.
 */
void *get_next_resource_hob(const EFI_GUID *guid, const void *hob_start)
{
	EFI_PEI_HOB_POINTERS hob;

	hob.Raw = (UINT8 *)hob_start;
	while ((hob.Raw = get_next_hob(EFI_HOB_TYPE_RESOURCE_DESCRIPTOR,
					    hob.Raw)) != NULL) {
		if (compare_guid(guid, &hob.ResourceDescriptor->Owner))
			break;
		hob.Raw = GET_NEXT_HOB(hob.Raw);
	}
	return hob.Raw;
}

/*
 * Returns the first instance of the matching resource HOB among the whole HOB
 * list.
 */
void *get_first_resource_hob(const EFI_GUID *guid)
{
	return get_next_resource_hob(guid, get_hob_list());
}

static void print_hob_mem_attributes(void *hob_ptr)
{
	EFI_MEMORY_TYPE hob_mem_type;
	EFI_HOB_MEMORY_ALLOCATION *hob_memory_ptr = hob_ptr;
	u64 hob_mem_addr = hob_memory_ptr->AllocDescriptor.MemoryBaseAddress;
	u64 hob_mem_length = hob_memory_ptr->AllocDescriptor.MemoryLength;

	hob_mem_type = hob_memory_ptr->AllocDescriptor.MemoryType;

	static const char *hob_mem_type_names[15] = {
		[EfiReservedMemoryType] = "EfiReservedMemoryType",
		[EfiLoaderCode] = "EfiLoaderCode",
		[EfiLoaderData] = "EfiLoaderData",
		[EfiBootServicesCode] = "EfiBootServicesCode",
		[EfiBootServicesData] = "EfiBootServicesData",
		[EfiRuntimeServicesCode] = "EfiRuntimeServicesCode",
		[EfiRuntimeServicesData] = "EfiRuntimeServicesData",
		[EfiConventionalMemory] = "EfiConventionalMemory",
		[EfiUnusableMemory] = "EfiUnusableMemory",
		[EfiACPIReclaimMemory] = "EfiACPIReclaimMemory",
		[EfiACPIMemoryNVS] = "EfiACPIMemoryNVS",
		[EfiMemoryMappedIO] = "EfiMemoryMappedIO",
		[EfiMemoryMappedIOPortSpace] = "EfiMemoryMappedIOPortSpace",
		[EfiPalCode] = "EfiPalCode",
		[EfiMaxMemoryType] = "EfiMaxMemoryType",
	};

	if (hob_mem_type >= ARRAY_SIZE(hob_mem_type_names))
		hob_mem_type = EfiReservedMemoryType;

	printk(BIOS_SPEW, "  Memory type %s (0x%x)\n",
			hob_mem_type_names[hob_mem_type],
			(u32)hob_mem_type);
	printk(BIOS_SPEW, "  at location 0x%0lx with length 0x%0lx\n",
			(unsigned long)hob_mem_addr,
			(unsigned long)hob_mem_length);
}

static void print_hob_resource_attributes(void *hob_ptr)
{
	EFI_HOB_RESOURCE_DESCRIPTOR *hob_resource_ptr =
		(EFI_HOB_RESOURCE_DESCRIPTOR *)hob_ptr;
	u32 hob_res_type   = hob_resource_ptr->ResourceType;
	u32 hob_res_attr   = hob_resource_ptr->ResourceAttribute;
	u64 hob_res_addr   = hob_resource_ptr->PhysicalStart;
	u64 hob_res_length = hob_resource_ptr->ResourceLength;
	const char *hob_res_type_str = NULL;

	/* HOB Resource Types */
	switch (hob_res_type) {
	case EFI_RESOURCE_SYSTEM_MEMORY:
		hob_res_type_str = "EFI_RESOURCE_SYSTEM_MEMORY";
		break;
	case EFI_RESOURCE_MEMORY_MAPPED_IO:
		hob_res_type_str = "EFI_RESOURCE_MEMORY_MAPPED_IO";
		break;
	case EFI_RESOURCE_IO:
		hob_res_type_str = "EFI_RESOURCE_IO";
		break;
	case EFI_RESOURCE_FIRMWARE_DEVICE:
		hob_res_type_str = "EFI_RESOURCE_FIRMWARE_DEVICE";
		break;
	case EFI_RESOURCE_MEMORY_MAPPED_IO_PORT:
		hob_res_type_str = "EFI_RESOURCE_MEMORY_MAPPED_IO_PORT";
		break;
	case EFI_RESOURCE_MEMORY_RESERVED:
		hob_res_type_str = "EFI_RESOURCE_MEMORY_RESERVED";
		break;
	case EFI_RESOURCE_IO_RESERVED:
		hob_res_type_str = "EFI_RESOURCE_IO_RESERVED";
		break;
	case EFI_RESOURCE_MAX_MEMORY_TYPE:
		hob_res_type_str = "EFI_RESOURCE_MAX_MEMORY_TYPE";
		break;
	default:
		hob_res_type_str = "EFI_RESOURCE_UNKNOWN";
		break;
	}

	printk(BIOS_SPEW, "  Resource %s (0x%0x) has attributes 0x%0x\n",
			hob_res_type_str, hob_res_type, hob_res_attr);
	printk(BIOS_SPEW, "  at location 0x%0lx with length 0x%0lx\n",
			(unsigned long)hob_res_addr,
			(unsigned long)hob_res_length);
}

static const char *get_hob_type_string(void *hob_ptr)
{
	EFI_PEI_HOB_POINTERS hob;
	const char *hob_type_string;
	const EFI_GUID fsp_reserved_guid =
		FSP_RESERVED_MEMORY_RESOURCE_HOB_GUID;
	const EFI_GUID mrc_guid = FSP_NON_VOLATILE_STORAGE_HOB_GUID;
	const EFI_GUID bootldr_tmp_mem_guid =
		FSP_BOOTLOADER_TEMP_MEMORY_HOB_GUID;
	const EFI_GUID bootldr_tolum_guid = FSP_BOOTLOADER_TOLUM_HOB_GUID;
	const EFI_GUID graphics_info_guid = EFI_PEI_GRAPHICS_INFO_HOB_GUID;
	const EFI_GUID memory_info_hob_guid = FSP_SMBIOS_MEMORY_INFO_GUID;

	hob.Header = (EFI_HOB_GENERIC_HEADER *)hob_ptr;
	switch (hob.Header->HobType) {
	case EFI_HOB_TYPE_HANDOFF:
		hob_type_string = "EFI_HOB_TYPE_HANDOFF";
		break;
	case EFI_HOB_TYPE_MEMORY_ALLOCATION:
		hob_type_string = "EFI_HOB_TYPE_MEMORY_ALLOCATION";
		break;
	case EFI_HOB_TYPE_RESOURCE_DESCRIPTOR:
		if (compare_guid(&fsp_reserved_guid, &hob.Guid->Name))
			hob_type_string = "FSP_RESERVED_MEMORY_RESOURCE_HOB";
		else if (compare_guid(&bootldr_tolum_guid, &hob.Guid->Name))
			hob_type_string = "FSP_BOOTLOADER_TOLUM_HOB_GUID";
		else
			hob_type_string = "EFI_HOB_TYPE_RESOURCE_DESCRIPTOR";
		break;
	case EFI_HOB_TYPE_GUID_EXTENSION:
		if (compare_guid(&bootldr_tmp_mem_guid, &hob.Guid->Name))
			hob_type_string = "FSP_BOOTLOADER_TEMP_MEMORY_HOB";
		else if (compare_guid(&mrc_guid, &hob.Guid->Name))
			hob_type_string = "FSP_NON_VOLATILE_STORAGE_HOB";
		else if (compare_guid(&graphics_info_guid, &hob.Guid->Name))
			hob_type_string = "EFI_PEI_GRAPHICS_INFO_HOB_GUID";
		else if (compare_guid(&memory_info_hob_guid, &hob.Guid->Name))
			hob_type_string = "FSP_SMBIOS_MEMORY_INFO_GUID";
		else
			hob_type_string = "EFI_HOB_TYPE_GUID_EXTENSION";
		break;
	case EFI_HOB_TYPE_MEMORY_POOL:
		hob_type_string = "EFI_HOB_TYPE_MEMORY_POOL";
		break;
	case EFI_HOB_TYPE_UNUSED:
		hob_type_string = "EFI_HOB_TYPE_UNUSED";
		break;
	case EFI_HOB_TYPE_END_OF_HOB_LIST:
		hob_type_string = "EFI_HOB_TYPE_END_OF_HOB_LIST";
		break;
	default:
		hob_type_string = "EFI_HOB_TYPE_UNRECOGNIZED";
		break;
	}

	return hob_type_string;
}

/*
 * Print out a structure of all the HOBs
 * that match a certain type:
 * Print all types			(0x0000)
 * EFI_HOB_TYPE_HANDOFF		(0x0001)
 * EFI_HOB_TYPE_MEMORY_ALLOCATION	(0x0002)
 * EFI_HOB_TYPE_RESOURCE_DESCRIPTOR	(0x0003)
 * EFI_HOB_TYPE_GUID_EXTENSION		(0x0004)
 * EFI_HOB_TYPE_MEMORY_POOL		(0x0007)
 * EFI_HOB_TYPE_UNUSED			(0xFFFE)
 * EFI_HOB_TYPE_END_OF_HOB_LIST	(0xFFFF)
 */
void print_hob_type_structure(u16 hob_type, void *hob_list_ptr)
{
	void *current_hob;
	u32 current_type;
	const char *current_type_str;

	/*
	 * Print out HOBs of our desired type until
	 * the end of the HOB list
	 */
	printk(BIOS_DEBUG, "\n=== FSP HOB Data Structure ===\n");
	printk(BIOS_DEBUG, "%p: hob_list_ptr\n", hob_list_ptr);
	for (current_hob = hob_list_ptr; !END_OF_HOB_LIST(current_hob);
	    current_hob = GET_NEXT_HOB(current_hob)) {

		EFI_HOB_GENERIC_HEADER *current_header_ptr =
			(EFI_HOB_GENERIC_HEADER *)current_hob;

		/* Get the type of this HOB */
		current_type = current_header_ptr->HobType;
		current_type_str = get_hob_type_string(current_hob);

		if (current_type == hob_type || hob_type == 0x0000) {
			printk(BIOS_DEBUG, "HOB %p is an %s (type 0x%0x)\n",
					current_hob, current_type_str,
					current_type);
			switch (current_type) {
			case EFI_HOB_TYPE_MEMORY_ALLOCATION:
				print_hob_mem_attributes(current_hob);
				break;
			case EFI_HOB_TYPE_RESOURCE_DESCRIPTOR:
				print_hob_resource_attributes(current_hob);
				break;
			}
		}
	}
	printk(BIOS_DEBUG, "=== End of FSP HOB Data Structure ===\n\n");
}
