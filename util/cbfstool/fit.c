/* Firmware Interface Table support */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fit.h"

/* FIXME: This code assumes it is being executed on a little endian machine. */

#define FIT_POINTER_LOCATION 0xffffffc0
#define FIT_TABLE_LOWEST_ADDRESS ((uint32_t)(-(16 << 20)))
#define FIT_ENTRY_CHECKSUM_VALID 0x80
#define FIT_HEADER_VERSION 0x0100
#define FIT_HEADER_ADDRESS "_FIT_   "
#define FIT_MICROCODE_VERSION 0x0100
#define FIT_TXT_VERSION 0x0100

#define FIT_SIZE_ALIGNMENT 16

struct fit_entry {
	/**
	 * Address is the base address of the firmware component
	 * must be aligned on 16 byte boundary
	 */
	uint64_t address;
	/**
	 * Size is the span of the component in multiple of 16 bytes
	 * Bits [24:31] are reserved and  must be set to 0
	 */
	uint32_t size_reserved;
	/**
	 * Component's version number in binary coded decimal (BCD) format.
	 * For the FIT header entry, the value in this field will indicate the
	 * revision number of the FIT data structure. The upper byte of the
	 * revision field indicates the major revision and the lower byte
	 * indicates the minor revision.
	 */
	uint16_t version;
	/**
	 * FIT types 0x00 to 0x7F
	 * Bit 7 (C_V) indicates whether component has valid checksum.
	 */
	uint8_t  type_checksum_valid;
	/**
	 * Component's checksum. The modulo sum of all the bytes in the
	 * component and the value in this field (Chksum) must add up to zero.
	 * This field is only valid if the C_V flag is non-zero.
	 */
	uint8_t  checksum;
} __packed;

struct fit_table {
	struct fit_entry header;
	struct fit_entry entries[];
} __packed;

struct microcode_header {
	uint32_t version;
	uint32_t revision;
	uint32_t date;
	uint32_t processor_signature;
	uint32_t checksum;
	uint32_t loader_revision;
	uint32_t processor_flags;
	uint32_t data_size;
	uint32_t total_size;
	uint8_t  reserved[12];
} __packed;

struct microcode_entry {
	int offset;
	int size;
};

static inline void *rom_buffer_pointer(struct buffer *buffer, int offset)
{
	return &buffer->data[offset];
}

static inline size_t fit_entry_size_bytes(const struct fit_entry *entry)
{
	return (entry->size_reserved & 0xffffff) << 4;
}

static inline void fit_entry_update_size(struct fit_entry *entry,
					 const int size_bytes)
{
	/* Size is multiples of 16 bytes. */
	entry->size_reserved = (size_bytes >> 4) & 0xffffff;
}

static inline void fit_entry_add_size(struct fit_entry *entry,
				      const int size_bytes)
{
	int size = fit_entry_size_bytes(entry);
	size += size_bytes;
	fit_entry_update_size(entry, size);
}

static inline int fit_entry_type(struct fit_entry *entry)
{
	return entry->type_checksum_valid & ~FIT_ENTRY_CHECKSUM_VALID;
}

/*
 * Get an offset from a host pointer. This function assumes the ROM is located
 * in the host address space at [4G - romsize -> 4G). It also assume all
 * pointers have values within this address range.
 */
static inline int ptr_to_offset(fit_offset_converter_t helper,
				const struct buffer *region, uint32_t host_ptr)
{
	return helper(region, -host_ptr);
}

/*
 * Get a pointer from an offset. This function assumes the ROM is located
 * in the host address space at [4G - romsize -> 4G). It also assume all
 * pointers have values within this address range.
 */
static inline uint32_t offset_to_ptr(fit_offset_converter_t helper,
				     const struct buffer *region, int offset)
{
	return -helper(region, offset);
}

/*
 * Return the number of FIT entries.
 */
static inline size_t fit_table_entries(const struct fit_table *fit)
{
	if (!fit)
		return 0;

	return (fit_entry_size_bytes(&fit->header) / FIT_SIZE_ALIGNMENT) - 1;
}

/*
 * Return the number of unused entries.
 */
static inline size_t fit_free_space(struct fit_table *fit,
				 const size_t max_entries)
{
	if (!fit)
		return 0;

	return max_entries - fit_table_entries(fit);
}

/*
 * Sort entries by type and fill gaps (entries with type unused).
 * To be called after adding or deleting entries.
 *
 * This one is critical, as mentioned in Chapter 1.2.1 "FIT Ordering Rules"
 * "Firmware Interface Table BIOS Specification".
 *
 * We need to use a stable sorting algorithm, as the order of
 * FIT_TYPE_BIOS_STARTUP matter for measurements.
 */
static void sort_fit_table(struct fit_table *fit)
{
	struct fit_entry tmp;
	size_t i, j;
	int swapped;

	/* Bubble sort entries */
	for (j = 0; j < fit_table_entries(fit) - 1; j++) {
		swapped = 0;
		for (i = 0; i < fit_table_entries(fit) - j - 1; i++) {
			if (fit->entries[i].type_checksum_valid <=
			    fit->entries[i + 1].type_checksum_valid)
				continue;
			/* SWAP entries */
			memcpy(&tmp, &fit->entries[i], sizeof(tmp));
			memcpy(&fit->entries[i], &fit->entries[i + 1],
			       sizeof(fit->entries[i]));
			memcpy(&fit->entries[i + 1], &tmp,
			       sizeof(fit->entries[i + 1]));
			swapped = 1;
		}
		if (!swapped)
			break;
	}
}

static int fit_table_verified(struct fit_table *table)
{
	if (!table)
		return 0;

	/* Check that the address field has the proper signature. */
	if (strncmp((const char *)&table->header.address, FIT_HEADER_ADDRESS,
			sizeof(table->header.address)))
		return 0;

	if (table->header.version != FIT_HEADER_VERSION)
		return 0;

	if (fit_entry_type(&table->header) != FIT_TYPE_HEADER)
		return 0;

	/* Assume that the FIT table contains at least the header */
	if (fit_entry_size_bytes(&table->header) < sizeof(struct fit_entry))
		return 0;

	return 1;
}

/*
 * Update the FIT checksum.
 * To be called after modifiying the table.
 */
static void update_fit_checksum(struct fit_table *fit)
{
	int size_bytes;
	uint8_t *buffer;
	uint8_t result;
	int i;

	if (!fit)
		return;

	fit->header.checksum = 0;
	size_bytes = fit_entry_size_bytes(&fit->header);
	result = 0;
	buffer = (void *)fit;
	for (i = 0; i < size_bytes; i++)
		result += buffer[i];
	fit->header.checksum = -result;
}

/*
 * Return a pointer to the next free entry.
 * Caller must take care if enough space is available.
 */
static struct fit_entry *get_next_free_entry(struct fit_table *fit)
{
	return &fit->entries[fit_table_entries(fit)];
}

static void fit_location_from_cbfs_header(uint32_t *current_offset,
					  uint32_t *file_length, void *ptr)
{
	struct buffer buf;
	struct cbfs_file header;
	memset(&buf, 0, sizeof(buf));

	buf.data = ptr;
	buf.size = sizeof(header);

	bgets(&buf, header.magic, sizeof(header.magic));
	header.len = xdr_be.get32(&buf);
	header.type = xdr_be.get32(&buf);
	header.attributes_offset = xdr_be.get32(&buf);
	header.offset = xdr_be.get32(&buf);

	*current_offset = header.offset;
	*file_length = header.len;
}

static int
parse_microcode_blob(struct cbfs_image *image,
		     const char *blob_name,
		     size_t *mcus_found,
		     struct microcode_entry *mcus,
		     const size_t max_fit_entries)
{
	size_t num_mcus;
	uint32_t current_offset;
	uint32_t file_length;
	struct cbfs_file *mcode_file;

	mcode_file = cbfs_get_entry(image, blob_name);
	if (!mcode_file)
		return 1;

	fit_location_from_cbfs_header(&current_offset, &file_length,
				      mcode_file);
	current_offset += cbfs_get_entry_addr(image, mcode_file);

	num_mcus = 0;
	while (file_length > sizeof(struct microcode_header)) {
		const struct microcode_header *mcu_header;

		mcu_header = rom_buffer_pointer(&image->buffer, current_offset);
		if (!mcu_header) {
			ERROR("Couldn't parse microcode header.\n");
			return 1;
		}

		/* Newer microcode updates include a size field, whereas older
		 * containers set it at 0 and are exactly 2048 bytes long */
		uint32_t total_size = mcu_header->total_size ?: 2048;

		/* Quickly sanity check a prospective microcode update. */
		if (total_size < sizeof(*mcu_header) ||
		    total_size > file_length)
			break;

		/* FIXME: Should the checksum be validated? */
		mcus[num_mcus].offset = current_offset;
		mcus[num_mcus].size = total_size;

		/* Proceed to next payload. */
		current_offset += mcus[num_mcus].size;
		file_length -= mcus[num_mcus].size;
		num_mcus++;
		/* Reached limit of FIT entries. */
		if (num_mcus == max_fit_entries)
			break;
		if (file_length < sizeof(struct microcode_header))
			break;
	}

	/* Update how many microcode updates we found. */
	*mcus_found = num_mcus;

	return 0;
}

/* There can be zero or more FIT_TYPE_MICROCODE entries */
static void update_fit_ucode_entry(struct fit_table *fit,
				   struct fit_entry *entry,
				   const uint64_t mcu_addr)
{
	entry->address = mcu_addr;
	/*
	 * While loading MCU, its size is not referred from FIT and
	 * rather from the MCU header, hence we can assign zero here.
	 */
	entry->size_reserved = 0;
	entry->type_checksum_valid = FIT_TYPE_MICROCODE;
	entry->version = FIT_MICROCODE_VERSION;
	entry->checksum = 0;
	fit_entry_add_size(&fit->header, sizeof(struct fit_entry));
}

/*
 * There can be zero or one FIT_TYPE_BIOS_ACM entry per table.
 * In case there's a FIT_TYPE_BIOS_ACM entry, at least one
 * FIT_TYPE_BIOS_STARTUP entry must exist.
 *
 * The caller has to provide valid arguments as those aren't verfied.
 */
static void update_fit_bios_acm_entry(struct fit_table *fit,
				      struct fit_entry *entry,
				      const uint64_t acm_addr)
{
	entry->address = acm_addr;
	/*
	 * The Address field points to a BIOS ACM. The Address field points to
	 * the first byte of the AC module header. When BIOS ACM is loaded in
	 * Authenticated Code RAM, one MTRR base/limit pair is used to map it.
	 */
	entry->size_reserved = 0;
	entry->type_checksum_valid = FIT_TYPE_BIOS_ACM;
	entry->version = FIT_TXT_VERSION;
	entry->checksum = 0;
	fit_entry_add_size(&fit->header, sizeof(struct fit_entry));
}

/*
 * In case there's a FIT_TYPE_BIOS_ACM entry, at least one
 * FIT_TYPE_BIOS_STARTUP entry must exist.
 *
 * The caller has to provide valid arguments as those aren't verfied.
 */
static void update_fit_bios_startup_entry(struct fit_table *fit,
					  struct fit_entry *entry,
					  const uint64_t sm_addr,
					  const uint32_t sm_size)
{
	entry->address = sm_addr;
	assert(sm_size % 16 == 0);
	/*
	 * BIOS Startup code is defined as the code that gets control at the
	 * reset vector and continues the chain of trust in TCG-compliant
	 * fashion. In addition, this code may also configure memory and SMRAM.
	 */
	fit_entry_update_size(entry, sm_size);
	entry->type_checksum_valid = FIT_TYPE_BIOS_STARTUP;
	entry->version = FIT_TXT_VERSION;
	entry->checksum = 0;
	fit_entry_add_size(&fit->header, sizeof(struct fit_entry));
}

/*
 * There can be zero or one FIT_TYPE_BIOS_POLICY Record in the FIT.
 * If the platform uses the hash comparison method and employs a
 * failsafe bootblock, one FIT_TYPE_BIOS_POLICY entry is needed to
 * contain the failsafe hash.
 * If the platform uses the Signature verification method, one
 * FIT_TYPE_BIOS_POLICY entry is needed. In this case, the entry
 * contains the OEM key, hash of the BIOS and signature over the hash
 * using the OEM key.
 * In all other cases, the FIT_TYPE_BIOS_POLICY record is not required.
 *
 * The caller has to provide valid arguments as those aren't verfied.
 */
static void update_fit_bios_policy_entry(struct fit_table *fit,
					 struct fit_entry *entry,
					 const uint64_t lcp_policy_addr,
					 const uint32_t lcp_policy_size)
{
	entry->address = lcp_policy_addr;
	fit_entry_update_size(entry, lcp_policy_size);
	entry->type_checksum_valid = FIT_TYPE_BIOS_POLICY;
	entry->version = FIT_TXT_VERSION;
	entry->checksum = 0;
	fit_entry_add_size(&fit->header, sizeof(struct fit_entry));
}

/*
 * There can be zero or one FIT_TYPE_TXT_POLICY entries
 *
 * The caller has to provide valid arguments as those aren't verfied.
 */
static void update_fit_txt_policy_entry(struct fit_table *fit,
					struct fit_entry *entry,
					uint64_t txt_policy_addr)
{
	entry->address = txt_policy_addr;
	/*
	 * Points to the flag indicating if TXT is enabled on this platform.
	 * If not present, TXT is not disabled by FIT.
	 */
	entry->size_reserved = 0;
	entry->type_checksum_valid = FIT_TYPE_TXT_POLICY;
	entry->version = 0x1;
	entry->checksum = 0;
	fit_entry_add_size(&fit->header, sizeof(struct fit_entry));
}

/*
 * There can be zero or one FIT_TYPE_BOOT_POLICY entries
 *
 * The caller has to provide valid arguments as those aren't verified.
 */
static void update_fit_boot_policy_entry(struct fit_table *fit,
					struct fit_entry *entry,
					uint64_t boot_policy_addr,
					uint32_t boot_policy_size)
{
	entry->address = boot_policy_addr;
	entry->type_checksum_valid = FIT_TYPE_BOOT_POLICY;
	entry->size_reserved = boot_policy_size;
	entry->version = FIT_TXT_VERSION;
	entry->checksum = 0;
	fit_entry_add_size(&fit->header, sizeof(struct fit_entry));
}

/*
 * There can be zero or one FIT_TYPE_KEY_MANIFEST entries
 *
 * The caller has to provide valid arguments as those aren't verified.
 */
static void update_fit_key_manifest_entry(struct fit_table *fit,
					struct fit_entry *entry,
					uint64_t key_manifest_addr,
					uint32_t key_manifest_size)
{
	entry->address = key_manifest_addr;

	entry->type_checksum_valid = FIT_TYPE_KEY_MANIFEST;
	entry->size_reserved = key_manifest_size;
	entry->version = FIT_TXT_VERSION;
	entry->checksum = 0;
	fit_entry_add_size(&fit->header, sizeof(struct fit_entry));
}

/* Special case for ucode CBFS file, as it might contain more than one ucode */
int fit_add_microcode_file(struct fit_table *fit,
			   struct cbfs_image *image,
			   const char *blob_name,
			   fit_offset_converter_t offset_helper,
			   const size_t max_fit_entries)
{
	struct microcode_entry *mcus;

	size_t i;
	size_t mcus_found;

	mcus = malloc(sizeof(*mcus) * max_fit_entries);
	if (!mcus) {
		ERROR("Couldn't allocate memory for microcode entries.\n");
		return 1;
	}

	if (parse_microcode_blob(image, blob_name, &mcus_found, mcus,
				 max_fit_entries)) {
		ERROR("Couldn't parse microcode blob.\n");
		free(mcus);
		return 1;
	}

	if (mcus_found > fit_free_space(fit, max_fit_entries)) {
		ERROR("Maximum of FIT entries reached.\n");
		free(mcus);
		return 1;
	}

	for (i = 0; i < mcus_found; i++) {
		if (fit_add_entry(fit,
				  offset_to_ptr(offset_helper, &image->buffer,
						mcus[i].offset),
				  0,
				  FIT_TYPE_MICROCODE,
				  max_fit_entries)) {

			free(mcus);
			return 1;
		}
	}

	free(mcus);
	return 0;
}

static uint32_t *get_fit_ptr(struct buffer *bootblock, fit_offset_converter_t offset_fn,
		      uint32_t topswap_size)
{
	return rom_buffer_pointer(bootblock,
				  ptr_to_offset(offset_fn, bootblock,
						FIT_POINTER_LOCATION - topswap_size));
}

/* Set the FIT pointer to a FIT table. */
int set_fit_pointer(struct buffer *bootblock,
		    const uint32_t fit_address,
		    fit_offset_converter_t offset_fn,
		    uint32_t topswap_size)
{
	struct fit_table *fit;
	uint32_t *fit_pointer = get_fit_ptr(bootblock, offset_fn, topswap_size);

	fit = rom_buffer_pointer(bootblock, ptr_to_offset(offset_fn, bootblock, fit_address));

	if (fit_address < FIT_TABLE_LOWEST_ADDRESS) {
		ERROR("FIT must be reside in the top 16MiB.\n");
		return 1;
	}

	if (!fit_table_verified(fit)) {
		ERROR("FIT not found at address.\n");
		return 1;
	}

	fit_pointer[0] = fit_address;
	fit_pointer[1] = 0;
	return 0;
}

/*
 * Return a pointer to the active FIT.
 */
struct fit_table *fit_get_table(struct buffer *bootblock,
				fit_offset_converter_t offset_fn,
				uint32_t topswap_size)
{
	struct fit_table *fit;
	uint32_t *fit_pointer = get_fit_ptr(bootblock, offset_fn, topswap_size);

	/* Ensure pointer is below 4GiB and within 16MiB of 4GiB */
	if (fit_pointer[1] != 0 || fit_pointer[0] < FIT_TABLE_LOWEST_ADDRESS) {
		ERROR("FIT not found.\n");
		return NULL;
	}

	fit = rom_buffer_pointer(bootblock,
			   ptr_to_offset(offset_fn, bootblock, *fit_pointer));
	if (!fit_table_verified(fit)) {
		ERROR("FIT not found.\n");
		return NULL;
	}

	return fit;
}

/*
 * Dump the current FIT in human readable format to stdout.
 */
int fit_dump(struct fit_table *fit)
{
	size_t i;

	if (!fit)
		return 1;

	printf("\n");
	printf("    FIT table:\n");

	if (fit_table_entries(fit) < 1) {
		printf("    empty\n\n");
		return 0;
	}

	printf("    %-6s %-20s %-16s %-8s\n", "Index", "Type", "Addr", "Size");

	for (i = 0; i < fit_table_entries(fit); i++) {
		const char *name;

		switch (fit->entries[i].type_checksum_valid) {
		case FIT_TYPE_MICROCODE:
			name = "Microcode";
			break;
		case FIT_TYPE_BIOS_ACM:
			name = "BIOS ACM";
			break;
		case FIT_TYPE_BIOS_STARTUP:
			name = "BIOS Startup Module";
			break;
		case FIT_TYPE_TPM_POLICY:
			name = "TPM Policy";
			break;
		case FIT_TYPE_BIOS_POLICY:
			name = "BIOS Policy";
			break;
		case FIT_TYPE_TXT_POLICY:
			name = "TXT Policy";
			break;
		case FIT_TYPE_KEY_MANIFEST:
			name = "Key Manifest";
			break;
		case FIT_TYPE_BOOT_POLICY:
			name = "Boot Policy";
			break;
		case FIT_TYPE_CSE_SECURE_BOOT:
			name = "CSE SecureBoot";
			break;
		case FIT_TYPE_TXTSX_POLICY:
			name = "TXTSX policy";
			break;
		case FIT_TYPE_JMP_DEBUG_POLICY:
			name = "JMP debug policy";
			break;
		case FIT_TYPE_UNUSED:
			name = "unused";
			break;
		default:
			name = "unknown";
		}

		printf("    %6zd %-20s 0x%08"PRIx64"      0x%08zx\n", i, name,
			fit->entries[i].address,
			fit_entry_size_bytes(&fit->entries[i]));
	}
	printf("\n");
	return 0;
}

/*
 * Remove all entries from table.
 */
int fit_clear_table(struct fit_table *fit)
{
	if (!fit)
		return 1;

	memset(fit->entries, 0,
	       sizeof(struct fit_entry) * fit_table_entries(fit));

	/* Reset entry counter in header */
	fit_entry_update_size(&fit->header, sizeof(fit->header));

	update_fit_checksum(fit);

	return 0;
}

/*
 * Returns true if the FIT type is know and can be added to the table.
 */
int fit_is_supported_type(const enum fit_type type)
{
	switch (type) {
	case FIT_TYPE_MICROCODE:
	case FIT_TYPE_BIOS_ACM:
	case FIT_TYPE_BIOS_STARTUP:
	case FIT_TYPE_BIOS_POLICY:
	case FIT_TYPE_TXT_POLICY:
	case FIT_TYPE_KEY_MANIFEST:
	case FIT_TYPE_BOOT_POLICY:
		return 1;
	case FIT_TYPE_TPM_POLICY:
	default:
		return 0;
	}
}

/*
 * Adds an known entry to the FIT.
 * len is optional for same types and might be zero.
 * offset is an absolute address in 32-bit protected mode address space.
 */
int fit_add_entry(struct fit_table *fit,
		  const uint32_t offset,
		  const uint32_t len,
		  const enum fit_type type,
		  const size_t max_fit_entries)
{
	struct fit_entry *entry;

	if (!fit) {
		ERROR("Internal error.");
		return 1;
	}

	if (fit_free_space(fit, max_fit_entries) < 1) {
		ERROR("No space left in FIT.");
		return 1;
	}

	if (!fit_is_supported_type(type)) {
		ERROR("Unsupported FIT type %u\n", type);
		return 1;
	}

	DEBUG("Adding new entry type %u at offset %zd\n", type,
	      fit_table_entries(fit));

	entry = get_next_free_entry(fit);

	switch (type) {
	case FIT_TYPE_MICROCODE:
		update_fit_ucode_entry(fit, entry, offset);
		break;
	case FIT_TYPE_BIOS_ACM:
		update_fit_bios_acm_entry(fit, entry, offset);
		break;
	case FIT_TYPE_BIOS_STARTUP:
		update_fit_bios_startup_entry(fit, entry, offset, len);
		break;
	case FIT_TYPE_BIOS_POLICY:
		update_fit_bios_policy_entry(fit, entry, offset, len);
		break;
	case FIT_TYPE_TXT_POLICY:
		update_fit_txt_policy_entry(fit, entry, offset);
		break;
	case FIT_TYPE_KEY_MANIFEST:
		update_fit_key_manifest_entry(fit, entry, offset, len);
		break;
	case FIT_TYPE_BOOT_POLICY:
		update_fit_boot_policy_entry(fit, entry, offset, len);
		break;
	default:
		return 1;
	}

	sort_fit_table(fit);

	update_fit_checksum(fit);

	return 0;
}

/*
 * Delete one entry from table.
 */
int fit_delete_entry(struct fit_table *fit,
		     const size_t idx)
{
	if (!fit) {
		ERROR("Internal error.");
		return 1;
	}

	if (idx >= fit_table_entries(fit)) {
		ERROR("Index out of range.");
		return 1;
	}

	memset(&fit->entries[idx], 0, sizeof(struct fit_entry));

	fit->entries[idx].type_checksum_valid = FIT_TYPE_UNUSED;

	sort_fit_table(fit);

	/* The unused entry is now the last one */
	fit_entry_add_size(&fit->header, -(int)sizeof(struct fit_entry));

	update_fit_checksum(fit);

	return 0;
}
