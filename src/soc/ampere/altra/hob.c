/* SPDX-License-Identifier: GPL-2.0-only */

#include "hob.h"
#include <commonlib/helpers.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <stddef.h>
#include <string.h>
#include <uuid.h>

/*
 * TF-A leaves an EDK2 style HOB list at AC01_TFA_HOB_BASE (see "Altra ATF
 * Interface Specification", "ATF Hand-Off Block"). EDK2 copies the platform
 * information GUID HOB out of it (AmpereAltraPkg Drivers/ATFHobPei).
 */

#define EFI_HOB_TYPE_GUID_EXTENSION	0x0004
#define EFI_HOB_TYPE_END_OF_HOB_LIST	0xffff

struct efi_hob_header {
	uint16_t type;
	uint16_t length;
	uint32_t reserved;
};

struct efi_hob_guid {
	struct efi_hob_header header;
	guid_t name;
};

#define PLATFORM_INFO_HOB_GUID \
	GUID_INIT(0x7f73e372, 0x7183, 0x4022, \
	0xb3, 0x76, 0x78, 0x30, 0x32, 0x6d, 0x79, 0xb4)

static const void *find_guid_hob(const guid_t *guid, size_t *size)
{
	uintptr_t p = AC01_TFA_HOB_BASE;
	const uintptr_t end = AC01_TFA_HOB_BASE + AC01_TFA_HOB_SIZE;

	while (p + sizeof(struct efi_hob_header) <= end) {
		const struct efi_hob_header *hdr = (const void *)p;

		if (hdr->type == EFI_HOB_TYPE_END_OF_HOB_LIST)
			return NULL;
		if (hdr->length < sizeof(*hdr) || p + hdr->length > end)
			return NULL;

		if (hdr->type == EFI_HOB_TYPE_GUID_EXTENSION &&
		    hdr->length >= sizeof(struct efi_hob_guid)) {
			const struct efi_hob_guid *guid_hob = (const void *)p;

			if (!guidcmp(&guid_hob->name, guid)) {
				*size = hdr->length - sizeof(*guid_hob);
				return guid_hob + 1;
			}
		}

		p += ALIGN_UP(hdr->length, 8);
	}

	return NULL;
}

static bool dram_info_valid(const struct ac01_dram_info *dram)
{
	if (dram->num_region == 0 || dram->num_region > AC01_DRAM_MAX_REGIONS)
		return false;

	for (unsigned int i = 0; i < dram->num_region; i++) {
		if (dram->size[i] == 0 || !IS_ALIGNED(dram->base[i], 64 * KiB))
			return false;
	}

	return true;
}

static void print_platform_info(const struct ac01_platform_info *info)
{
	char buf[sizeof(info->cpu_info) + 1];

	memcpy(buf, info->cpu_info, sizeof(info->cpu_info));
	buf[sizeof(info->cpu_info)] = '\0';
	printk(BIOS_INFO, "TF-A HOB: CPU: %s, SCU product ID 0x%x, AHBC ID 0x%x\n", buf,
	       info->scu_product_id[0], info->ahbc_id[0]);

	memcpy(buf, info->smpmpro_ver, sizeof(info->smpmpro_ver));
	buf[sizeof(info->smpmpro_ver)] = '\0';
	printk(BIOS_INFO, "TF-A HOB: SMpro/PMpro version: %s\n", buf);

	const struct ac01_dram_info *dram = &info->dram_info;
	for (unsigned int i = 0; i < dram->num_region; i++)
		printk(BIOS_INFO, "TF-A HOB: DRAM region %u: 0x%llx - 0x%llx (node %llu)\n", i,
		       dram->base[i], dram->base[i] + dram->size[i] - 1, dram->node[i]);
}

/* Offsets of the fields in PLATFORM_INFO_HOB */
_Static_assert(offsetof(struct ac01_platform_info, tpm2_info) == 19184, "HOB layout");
_Static_assert(offsetof(struct ac01_platform_info, ecid) == 19340, "HOB layout");

static size_t info_size;

const struct ac01_platform_info *ac01_platform_info(void)
{
	static const struct ac01_platform_info *info;
	static bool searched;

	if (searched)
		return info;
	searched = true;

	const guid_t guid = PLATFORM_INFO_HOB_GUID;
	size_t size = 0;
	const struct ac01_platform_info *hob = find_guid_hob(&guid, &size);

	if (!hob) {
		printk(BIOS_ERR, "TF-A HOB: no platform information HOB at 0x%x\n",
		       AC01_TFA_HOB_BASE);
		return NULL;
	}

	if (size < offsetof(struct ac01_platform_info, tpm2_info) ||
	    !dram_info_valid(&hob->dram_info)) {
		printk(BIOS_ERR, "TF-A HOB: platform information HOB (%zu bytes) is not valid\n",
		       size);
		return NULL;
	}

	info = hob;
	info_size = size;
	print_platform_info(info);
	return info;
}

const uint32_t *ac01_ecid(void)
{
	const struct ac01_platform_info *info = ac01_platform_info();

	if (!info || info_size < offsetof(struct ac01_platform_info, ecid) + sizeof(info->ecid))
		return NULL;

	return info->ecid[0];
}

bool ac01_is_altra(void)
{
	const struct ac01_platform_info *info = ac01_platform_info();

	return !info || (info->scu_product_id[0] & 0xff) == 0x01;
}

unsigned int ac01_active_sockets(void)
{
	const struct ac01_platform_info *info = ac01_platform_info();
	unsigned int sockets = 0;

	if (!info)
		return 1;

	for (unsigned int s = 0; s < AC01_MAX_SOCKETS; s++) {
		for (unsigned int i = 0; i < ARRAY_SIZE(info->cluster_en[s].enable_mask); i++) {
			if (info->cluster_en[s].enable_mask[i]) {
				sockets++;
				break;
			}
		}
	}

	return sockets ? sockets : 1;
}
