/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <fsp/util.h>

struct hob_type_name {
	uint16_t type;
	const char *name;
} __packed;

static const struct hob_type_name hob_type_names[] = {
	{ HOB_TYPE_HANDOFF, "HOB_TYPE_HANDOFF" },
	{ HOB_TYPE_MEMORY_ALLOCATION, "HOB_TYPE_MEMORY_ALLOCATION" },
	{ HOB_TYPE_RESOURCE_DESCRIPTOR, "HOB_TYPE_RESOURCE_DESCRIPTOR" },
	{ HOB_TYPE_GUID_EXTENSION, "HOB_TYPE_GUID_EXTENSION" },
	{ HOB_TYPE_FV, "HOB_TYPE_FV" },
	{ HOB_TYPE_CPU, "HOB_TYPE_CPU" },
	{ HOB_TYPE_MEMORY_POOL, "HOB_TYPE_MEMORY_POOL" },
	{ HOB_TYPE_FV2, "HOB_TYPE_FV2" },
	{ HOB_TYPE_LOAD_PEIM_UNUSED, "HOB_TYPE_LOAD_PEIM_UNUSED" },
	{ HOB_TYPE_UCAPSULE, "HOB_TYPE_UCAPSULE" },
	{ HOB_TYPE_UNUSED, "HOB_TYPE_UNUSED" },
	{ HOB_TYPE_END_OF_HOB_LIST, "HOB_TYPE_END_OF_HOB_LIST" }
};

static const char *resource_names[] = {
	[EFI_RESOURCE_SYSTEM_MEMORY]		= "SYSTEM_MEMORY",
	[EFI_RESOURCE_MEMORY_MAPPED_IO]		= "MMIO",
	[EFI_RESOURCE_IO]			= "IO",
	[EFI_RESOURCE_FIRMWARE_DEVICE]		= "FIRMWARE_DEVICE",
	[EFI_RESOURCE_MEMORY_MAPPED_IO_PORT]	= "MMIO_PORT",
	[EFI_RESOURCE_MEMORY_RESERVED]		= "MEMORY_RESERVED",
	[EFI_RESOURCE_IO_RESERVED]		= "IO_RESERVED",
};

static const uint8_t bootloader_temp_memory_guid[16] = {
	0x6c, 0xf4, 0xcf, 0xbb, 0xd3, 0xc8, 0x13, 0x41,
	0x89, 0x85, 0xb9, 0xd4, 0xf3, 0xb3, 0xf6, 0x4e
};

static const uint8_t empty_guid[16] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const uint8_t fsp_graphics_info_guid[16] = {
	0xce, 0x2c, 0xf6, 0x39, 0x25, 0x68, 0x69, 0x46,
	0xbb, 0x56, 0x54, 0x1a, 0xba, 0x75, 0x3a, 0x07
};

static const uint8_t fsp_info_header_guid[16] = {
	0xbe, 0x40, 0x27, 0x91, 0x84, 0x22, 0x34, 0x47,
	0xb9, 0x71, 0x84, 0xb0, 0x27, 0x35, 0x3f, 0x0c
};

static const uint8_t smbios_memory_info_guid[16] = {
	0x8c, 0x10, 0xa1, 0x01, 0xee, 0x9d, 0x84, 0x49,
	0x88, 0xc3, 0xee, 0xe8, 0xc4, 0x9e, 0xfb, 0x89
};

static const uint8_t tseg_guid[16] = {
	0x7c, 0x74, 0x38, 0xd0, 0x0c, 0xd0, 0x80, 0x49,
	0xb3, 0x19, 0x49, 0x01, 0x99, 0xa4, 0x7d, 0x55
};

struct guid_name_map {
	const void *guid;
	const char *name;
};

static const struct guid_name_map  guid_names[] = {
	{ bootloader_temp_memory_guid,	"FSP_BOOTLOADER_TEMP_MEMORY_HOB_GUID" },
	{ fsp_bootloader_tolum_guid,	"BOOTLOADER_TOLUM" },
	{ empty_guid,			"No GUID specified" },
	{ fsp_info_header_guid,		"FSP_INFO_HEADER_GUID" },
	{ fsp_reserved_memory_guid,	"FSP_RESERVED_MEMORY" },
	{ fsp_nv_storage_guid,		"FSP_NV_STORAGE" },
	{ fsp_graphics_info_guid,	"GRAPHICS INFO" },
	{ smbios_memory_info_guid,	"FSP_SMBIOS_MEMORY_INFO_GUID" },
	{ tseg_guid,			"TSEG" },
};

static const char *resource_name(uint32_t type)
{
	if (type >= ARRAY_SIZE(resource_names))
		return "UNKNOWN";
	return resource_names[type];
}

void fsp_print_resource_descriptor(const void *base)
{
	const struct hob_resource *res;

	res = fsp_hob_header_to_resource(base);

	printk(BIOS_SPEW, "Resource %s, attribute %x\n",
			   resource_name(res->type), res->attribute_type);
	printk(BIOS_SPEW, "\t0x%08llx + 0x%08llx\n", res->addr, res->length);
	if (!fsp_guid_compare(res->owner_guid, empty_guid)) {
		printk(BIOS_SPEW, "\tOwner GUID: ");
		fsp_print_guid(BIOS_SPEW, res->owner_guid);
		printk(BIOS_SPEW, " (%s)\n",
			fsp_get_guid_name(res->owner_guid));
	}
}

void fsp_print_memory_resource_hobs(void)
{
	const struct hob_header *hob = fsp_get_hob_list();

	for (; hob->type != HOB_TYPE_END_OF_HOB_LIST;
		hob = fsp_next_hob(hob)) {
		if (hob->type == HOB_TYPE_RESOURCE_DESCRIPTOR)
			fsp_print_resource_descriptor(hob);
	}
}

const char *fsp_get_hob_type_name(const struct hob_header *hob)
{
	size_t index;
	const char *name;

	for (index = 0; index < ARRAY_SIZE(hob_type_names); index++)
		if (hob->type == hob_type_names[index].type)
			return hob_type_names[index].name;

	/* Get name for SOC specific hob */
	name = soc_get_hob_type_name(hob);
	if (name != NULL)
		return name;
	return "Unknown HOB type";
}

const char *fsp_get_guid_name(const uint8_t *guid)
{
	size_t index;
	const char *name;

	/* Compare the GUID values in this module */
	for (index = 0; index < ARRAY_SIZE(guid_names); index++)
		if (fsp_guid_compare(guid, guid_names[index].guid))
			return guid_names[index].name;

	/* Get GUID name from SOC */
	name = soc_get_guid_name(guid);
	if (name != NULL)
		return name;
	return "Unknown GUID";
}

__weak const char *soc_get_hob_type_name(
	const struct hob_header *hob)
{
	return NULL;
}

void fsp_print_guid_extension_hob(const struct hob_header *hob)
{
	const struct hob_resource *res;

	res = fsp_hob_header_to_resource(hob);
	printk(BIOS_SPEW, "\t");
	fsp_print_guid(BIOS_SPEW, res->owner_guid);
	printk(BIOS_SPEW, ": %s\n", fsp_get_guid_name(res->owner_guid));

	/* Some of the SoC FSP specific hobs are of type HOB_TYPE_GUID_EXTENSION */
	soc_display_hob(hob);
}

__weak const char *soc_get_guid_name(const uint8_t *guid)
{
	return NULL;
}

void fsp_display_hobs(void)
{
	const struct hob_header *hob = fsp_get_hob_list();

	/* Display the HOB list pointer */
	printk(BIOS_SPEW, "\n=== FSP HOBs ===\n");
	printk(BIOS_SPEW, "%p: hob_list_ptr\n", hob);

	/* Walk the list of HOBs */
	while (1) {
		/* Display the HOB header */
		printk(BIOS_SPEW, "%p, 0x%08x bytes: %s\n", hob, hob->length,
			fsp_get_hob_type_name(hob));
		switch (hob->type) {
		default:
			soc_display_hob(hob);
			break;

		case HOB_TYPE_END_OF_HOB_LIST:
			printk(BIOS_SPEW, "=== End of FSP HOBs ===\n\n");
			return;

		case HOB_TYPE_RESOURCE_DESCRIPTOR:
			fsp_print_resource_descriptor(hob);
			break;

		case HOB_TYPE_GUID_EXTENSION:
			fsp_print_guid_extension_hob(hob);
			break;
		}
		hob = fsp_next_hob(hob);
	}
}

__weak void soc_display_hob(const struct hob_header *hob)
{
}
