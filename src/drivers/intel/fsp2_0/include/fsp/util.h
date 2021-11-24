/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _FSP2_0_UTIL_H_
#define _FSP2_0_UTIL_H_

#include <boot/coreboot_tables.h>
#include <cbfs.h>
#include <commonlib/region.h>
#include <arch/cpu.h>
#include <fsp/api.h>
#include <efi/efi_datatype.h>
#include <fsp/info_header.h>
#include <memrange.h>
#include <program_loading.h>
#include <types.h>

#define FSP_VER_LEN	30

/* Macro for checking and loading array type configs into array type UPDs */
#define FSP_ARRAY_LOAD(dst, src) \
do { \
	_Static_assert(ARRAY_SIZE(dst) >= ARRAY_SIZE(src), "copy buffer overflow!"); \
	memcpy(dst, src, sizeof(src)); \
} while (0)

struct hob_header {
	uint16_t type;
	uint16_t length;
} __packed;

struct fsp_nvs_hob2_data_region_header {
	efi_physical_address nvs_data_ptr;
	uint64_t nvs_data_length;
};

struct fsp_notify_params {
	enum fsp_notify_phase phase;
};

enum fsp_multi_phase_action {
	GET_NUMBER_OF_PHASES = 0,
	EXECUTE_PHASE = 1
};

struct fsp_multi_phase_params {
	enum fsp_multi_phase_action multi_phase_action;
	uint32_t phase_index;
	void *multi_phase_param_ptr;
};

struct hob_resource {
	uint8_t owner_guid[16];
	uint32_t type;
	uint32_t attribute_type;
	uint64_t addr;
	uint64_t length;
} __packed;

union fsp_revision {
	uint32_t val;
	struct {
		uint8_t bld_num;
		uint8_t revision;
		uint8_t minor;
		uint8_t major;
	} rev;
};

union extended_fsp_revision {
	uint16_t val;
	struct {
		uint8_t bld_num;
		uint8_t revision;
	} rev;
};

#if CONFIG_UDK_VERSION < CONFIG_UDK_2017_VERSION
enum resource_type {
	EFI_RESOURCE_SYSTEM_MEMORY		= 0,
	EFI_RESOURCE_MEMORY_MAPPED_IO		= 1,
	EFI_RESOURCE_IO				= 2,
	EFI_RESOURCE_FIRMWARE_DEVICE		= 3,
	EFI_RESOURCE_MEMORY_MAPPED_IO_PORT	= 4,
	EFI_RESOURCE_MEMORY_RESERVED		= 5,
	EFI_RESOURCE_IO_RESERVED		= 6,
	EFI_RESOURCE_MAX_MEMORY_TYPE		= 7,
};
#endif

enum hob_type {
	HOB_TYPE_HANDOFF			= 0x0001,
	HOB_TYPE_MEMORY_ALLOCATION		= 0x0002,
	HOB_TYPE_RESOURCE_DESCRIPTOR		= 0x0003,
	HOB_TYPE_GUID_EXTENSION			= 0x0004,
	HOB_TYPE_FV				= 0x0005,
	HOB_TYPE_CPU				= 0x0006,
	HOB_TYPE_MEMORY_POOL			= 0x0007,
	HOB_TYPE_FV2				= 0x0009,
	HOB_TYPE_LOAD_PEIM_UNUSED		= 0x000A,
	HOB_TYPE_UCAPSULE			= 0x000B,
	HOB_TYPE_UNUSED				= 0xFFFE,
	HOB_TYPE_END_OF_HOB_LIST		= 0xFFFF,
};

extern const uint8_t fsp_bootloader_tolum_guid[16];
extern const uint8_t fsp_nv_storage_guid[16];
extern const uint8_t fsp_reserved_memory_guid[16];

const void *fsp_get_hob_list(void);
void *fsp_get_hob_list_ptr(void);
const void *fsp_find_extension_hob_by_guid(const uint8_t *guid, size_t *size);
const void *fsp_find_nv_storage_data(size_t *size);
int fsp_find_range_hob(struct range_entry *re, const uint8_t guid[16]);
void fsp_display_fvi_version_hob(void);
void fsp_find_reserved_memory(struct range_entry *re);
const struct hob_resource *fsp_hob_header_to_resource(
	const struct hob_header *hob);
const struct hob_header *fsp_next_hob(const struct hob_header *parent);
bool fsp_guid_compare(const uint8_t guid1[16], const uint8_t guid2[16]);
void fsp_find_bootloader_tolum(struct range_entry *re);
void fsp_get_version(char *buf);
/* fsp_verify_upd_header_signature calls die() on signature mismatch */
void fsp_verify_upd_header_signature(uint64_t upd_signature, uint64_t expected_signature);
void lb_string_platform_blob_version(struct lb_header *header);
void report_fspt_output(void);
void soc_validate_fspm_header(const struct fsp_header *hdr);

/* Fill in header and validate a loaded FSP component. */
enum cb_err fsp_validate_component(struct fsp_header *hdr, void *fsp_blob, size_t size);

struct fsp_load_descriptor {
	/* fsp_prog object will have region_device initialized to final
	 * load location in memory. */
	struct prog fsp_prog;
	/* CBFS allocator to place loaded FSP. NULL to map flash directly. */
	cbfs_allocator_t alloc;
	/* Optional argument to be utilized by get_destination() callback. */
	void *arg;
};

/* Load the FSP component described by fsp_load_descriptor from cbfs. The FSP
 * header object will be validated and filled in on successful load. */
enum cb_err fsp_load_component(struct fsp_load_descriptor *fspld, struct fsp_header *hdr);

/*
 * Handle FSP reboot request status. Chipset/soc is expected to provide
 * chipset_handle_reset() that deals with reset type codes specific to given
 * SoC. If the requested status is not a reboot status or unhandled, this
 * function does nothing.
 */
void fsp_handle_reset(uint32_t status);

/* SoC/chipset must provide this to handle platform-specific reset codes */
void chipset_handle_reset(uint32_t status);

typedef asmlinkage uint32_t (*temp_ram_exit_fn)(void *param);
typedef asmlinkage uint32_t (*fsp_memory_init_fn)
				   (void *raminit_upd, void **hob_list);
typedef asmlinkage uint32_t (*fsp_silicon_init_fn)(void *silicon_upd);
typedef asmlinkage uint32_t (*fsp_multi_phase_si_init_fn)(struct fsp_multi_phase_params *);
typedef asmlinkage uint32_t (*fsp_notify_fn)(struct fsp_notify_params *);
#include <fsp/debug.h>

#endif /* _FSP2_0_UTIL_H_ */
