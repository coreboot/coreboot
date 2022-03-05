/* cbfstool, CLI utility for CBFS file manipulation */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include "common.h"
#include "cbfs.h"
#include "cbfs_image.h"
#include "cbfs_sections.h"
#include "elfparsing.h"
#include "partitioned_file.h"
#include "lz4/lib/xxhash.h"
#include <commonlib/bsd/cbfs_private.h>
#include <commonlib/bsd/compression.h>
#include <commonlib/bsd/metadata_hash.h>
#include <commonlib/fsp.h>
#include <commonlib/endian.h>
#include <commonlib/helpers.h>
#include <commonlib/region.h>
#include <vboot_host.h>

struct command {
	const char *name;
	const char *optstring;
	int (*function) (void);
	// Whether to populate param.image_region before invoking function
	bool accesses_region;
	// This set to true means two things:
	// - in case of a command operating on a region, the region's contents
	//   will be written back to image_file at the end
	// - write access to the file is required
	bool modifies_region;
};

static struct param {
	partitioned_file_t *image_file;
	struct buffer *image_region;
	const char *name;
	const char *filename;
	const char *fmap;
	const char *region_name;
	const char *source_region;
	const char *bootblock;
	const char *ignore_section;
	const char *ucode_region;
	uint64_t u64val;
	uint32_t type;
	uint32_t baseaddress;
	/*
	 * Input can be negative. It will be transformed to offset from start of region (if
	 * negative) and stored in baseaddress.
	 */
	long long int baseaddress_input;
	uint32_t baseaddress_assigned;
	uint32_t loadaddress;
	uint32_t headeroffset;
	/*
	 * Input can be negative. It will be transformed to offset from start of region (if
	 * negative) and stored in baseaddress.
	 */
	long long int headeroffset_input;
	uint32_t headeroffset_assigned;
	uint32_t entrypoint;
	uint32_t size;
	uint32_t alignment;
	uint32_t pagesize;
	uint32_t cbfsoffset;
	/*
	 * Input can be negative. It will be transformed to corresponding region offset (if
	 * negative) and stored in baseaddress.
	 */
	long long int cbfsoffset_input;
	uint32_t cbfsoffset_assigned;
	uint32_t arch;
	uint32_t padding;
	uint32_t topswap_size;
	bool u64val_assigned;
	bool fill_partial_upward;
	bool fill_partial_downward;
	bool show_immutable;
	bool stage_xip;
	bool force_pow2_pagesize;
	bool autogen_attr;
	bool machine_parseable;
	bool unprocessed;
	bool ibb;
	enum cbfs_compression compression;
	int precompression;
	enum vb2_hash_algorithm hash;
	/* For linux payloads */
	char *initrd;
	char *cmdline;
	int force;
	/*
	 * Base and size of extended window for decoding SPI flash greater than 16MiB in host
	 * address space on x86 platforms. The assumptions here are:
	 * 1. Top 16MiB is still decoded in the fixed decode window just below 4G boundary.
	 * 2. Rest of the SPI flash below the top 16MiB is mapped at the top of extended
	 * window. Even though the platform might support a larger extended window, the SPI
	 * flash part used by the mainboard might not be large enough to be mapped in the entire
	 * window. In such cases, the mapping is assumed to be in the top part of the extended
	 * window with the bottom part remaining unused.
	 */
	uint32_t ext_win_base;
	uint32_t ext_win_size;
} param = {
	/* All variables not listed are initialized as zero. */
	.arch = CBFS_ARCHITECTURE_UNKNOWN,
	.compression = CBFS_COMPRESS_NONE,
	.hash = VB2_HASH_INVALID,
	.headeroffset = ~0,
	.region_name = SECTION_NAME_PRIMARY_CBFS,
	.u64val = -1,
};

/*
 * This "metadata_hash cache" caches the value and location of the CBFS metadata
 * hash embedded in the bootblock when CBFS verification is enabled. The first
 * call to get_mh_cache() searches for the cache by scanning the whole bootblock
 * for its 8-byte signature, later calls will just return the previously found
 * information again. If the cbfs_hash.algo member in the result is
 * VB2_HASH_INVALID, that means no metadata hash was found and this image does
 * not use CBFS verification.
 */
struct mh_cache {
	const char *region;
	size_t offset;
	struct vb2_hash cbfs_hash;
	platform_fixup_func fixup;
	bool initialized;
};

static struct mh_cache *get_mh_cache(void)
{
	static struct mh_cache mhc;

	if (mhc.initialized)
		return &mhc;

	mhc.initialized = true;

	const struct fmap *fmap = partitioned_file_get_fmap(param.image_file);
	if (!fmap)
		goto no_metadata_hash;

	/* Find the bootblock. If there is a "BOOTBLOCK" FMAP section, it's
	   there. If not, it's a normal file in the primary CBFS section. */
	size_t offset, size;
	struct buffer buffer;
	if (fmap_find_area(fmap, SECTION_NAME_BOOTBLOCK)) {
		if (!partitioned_file_read_region(&buffer, param.image_file,
						  SECTION_NAME_BOOTBLOCK))
			goto no_metadata_hash;
		mhc.region = SECTION_NAME_BOOTBLOCK;
		offset = 0;
		size = buffer.size;
	} else {
		struct cbfs_image cbfs;
		struct cbfs_file *bootblock;
		if (!partitioned_file_read_region(&buffer, param.image_file,
						  SECTION_NAME_PRIMARY_CBFS))
			goto no_metadata_hash;
		mhc.region = SECTION_NAME_PRIMARY_CBFS;
		if (cbfs_image_from_buffer(&cbfs, &buffer, param.headeroffset))
			goto no_metadata_hash;
		bootblock = cbfs_get_entry(&cbfs, "bootblock");
		if (!bootblock || be32toh(bootblock->type) != CBFS_TYPE_BOOTBLOCK)
			goto no_metadata_hash;
		offset = (void *)bootblock + be32toh(bootblock->offset) -
			 buffer_get(&cbfs.buffer);
		size = be32toh(bootblock->len);
	}

	/* Find and validate the metadata hash anchor inside the bootblock and
	   record its exact byte offset from the start of the FMAP region. */
	struct metadata_hash_anchor *anchor = memmem(buffer_get(&buffer) + offset,
			size, METADATA_HASH_ANCHOR_MAGIC, sizeof(anchor->magic));
	if (anchor) {
		if (!vb2_digest_size(anchor->cbfs_hash.algo)) {
			ERROR("Unknown CBFS metadata hash type: %d\n",
			      anchor->cbfs_hash.algo);
			goto no_metadata_hash;
		}
		mhc.cbfs_hash = anchor->cbfs_hash;
		mhc.offset = (void *)anchor - buffer_get(&buffer);
		mhc.fixup = platform_fixups_probe(&buffer, mhc.offset,
						  mhc.region);
		return &mhc;
	}

no_metadata_hash:
	mhc.cbfs_hash.algo = VB2_HASH_INVALID;
	return &mhc;
}

static void update_and_info(const char *name, void *dst, void *src, size_t size)
{
	if (!memcmp(dst, src, size))
		return;
	char *src_str = bintohex(src, size);
	char *dst_str = bintohex(dst, size);
	INFO("Updating %s from %s to %s\n", name, dst_str, src_str);
	memcpy(dst, src, size);
	free(src_str);
	free(dst_str);
}

static int update_anchor(struct mh_cache *mhc, uint8_t *fmap_hash)
{
	struct buffer buffer;
	if (!partitioned_file_read_region(&buffer, param.image_file,
					  mhc->region))
		return -1;
	struct metadata_hash_anchor *anchor = buffer_get(&buffer) + mhc->offset;
	/* The metadata hash anchor should always still be where we left it. */
	assert(!memcmp(anchor->magic, METADATA_HASH_ANCHOR_MAGIC,
		      sizeof(anchor->magic)) &&
	       anchor->cbfs_hash.algo == mhc->cbfs_hash.algo);
	update_and_info("CBFS metadata hash", anchor->cbfs_hash.raw,
		mhc->cbfs_hash.raw, vb2_digest_size(anchor->cbfs_hash.algo));
	if (fmap_hash) {
		update_and_info("FMAP hash",
				metadata_hash_anchor_fmap_hash(anchor), fmap_hash,
				vb2_digest_size(anchor->cbfs_hash.algo));
	}
	if (mhc->fixup && mhc->fixup(&buffer, mhc->offset) != 0)
		return -1;
	if (!partitioned_file_write_region(param.image_file, &buffer))
		return -1;
	return 0;

}

/* This should be called after every time CBFS metadata might have changed. It
   will recalculate and update the metadata hash in the bootblock if needed. */
static int maybe_update_metadata_hash(struct cbfs_image *cbfs)
{
	if (strcmp(param.region_name, SECTION_NAME_PRIMARY_CBFS))
		return 0;  /* Metadata hash only embedded in primary CBFS. */

	struct mh_cache *mhc = get_mh_cache();
	if (mhc->cbfs_hash.algo == VB2_HASH_INVALID)
		return 0;

	enum cb_err err = cbfs_walk(cbfs, NULL, NULL, &mhc->cbfs_hash,
				    CBFS_WALK_WRITEBACK_HASH);
	if (err != CB_CBFS_NOT_FOUND) {
		ERROR("Unexpected cbfs_walk() error %d\n", err);
		return -1;
	}

	return update_anchor(mhc, NULL);
}

/* This should be called after every time the FMAP or the bootblock itself might
   have changed, and will write the new FMAP hash into the metadata hash anchor
   in the bootblock if required (usually when the bootblock is first added). */
static int maybe_update_fmap_hash(void)
{
	if (strcmp(param.region_name, SECTION_NAME_BOOTBLOCK) &&
	    strcmp(param.region_name, SECTION_NAME_FMAP) &&
	    param.type != CBFS_TYPE_BOOTBLOCK)
		return 0;	/* FMAP and bootblock didn't change. */

	struct mh_cache *mhc = get_mh_cache();
	if (mhc->cbfs_hash.algo == VB2_HASH_INVALID)
		return 0;

	uint8_t fmap_hash[VB2_MAX_DIGEST_SIZE];
	const struct fmap *fmap = partitioned_file_get_fmap(param.image_file);
	if (!fmap || vb2_digest_buffer((const void *)fmap, fmap_size(fmap),
			mhc->cbfs_hash.algo, fmap_hash, sizeof(fmap_hash)))
		return -1;
	return update_anchor(mhc, fmap_hash);
}

static bool region_is_flashmap(const char *region)
{
	return partitioned_file_region_check_magic(param.image_file, region,
					FMAP_SIGNATURE, strlen(FMAP_SIGNATURE));
}

/* @return Same as cbfs_is_valid_cbfs(), but for a named region. */
static bool region_is_modern_cbfs(const char *region)
{
	return partitioned_file_region_check_magic(param.image_file, region,
				CBFS_FILE_MAGIC, strlen(CBFS_FILE_MAGIC));
}

/* This describes a window from the SPI flash address space into the host address space. */
struct mmap_window {
	struct region flash_space;
	struct region host_space;
};

enum mmap_window_type {
	X86_DEFAULT_DECODE_WINDOW, /* Decode window just below 4G boundary */
	X86_EXTENDED_DECODE_WINDOW, /* Extended decode window for mapping greater than 16MiB
				       flash */
	MMAP_MAX_WINDOWS,
};

/* Table of all the decode windows supported by the platform. */
static struct mmap_window mmap_window_table[MMAP_MAX_WINDOWS];

static void add_mmap_window(enum mmap_window_type idx, size_t flash_offset, size_t host_offset,
			    size_t window_size)
{
	if (idx >= MMAP_MAX_WINDOWS) {
		ERROR("Incorrect mmap window index(%d)\n", idx);
		return;
	}

	mmap_window_table[idx].flash_space.offset = flash_offset;
	mmap_window_table[idx].host_space.offset = host_offset;
	mmap_window_table[idx].flash_space.size = window_size;
	mmap_window_table[idx].host_space.size = window_size;
}

#define DEFAULT_DECODE_WINDOW_TOP	(4ULL * GiB)
#define DEFAULT_DECODE_WINDOW_MAX_SIZE	(16 * MiB)

static bool create_mmap_windows(void)
{
	static bool done;

	if (done)
		return done;

	const size_t image_size = partitioned_file_total_size(param.image_file);
	const size_t std_window_size = MIN(DEFAULT_DECODE_WINDOW_MAX_SIZE, image_size);
	const size_t std_window_flash_offset = image_size - std_window_size;

	/*
	 * Default decode window lives just below 4G boundary in host space and maps up to a
	 * maximum of 16MiB. If the window is smaller than 16MiB, the SPI flash window is mapped
	 * at the top of the host window just below 4G.
	 */
	add_mmap_window(X86_DEFAULT_DECODE_WINDOW, std_window_flash_offset,
			DEFAULT_DECODE_WINDOW_TOP - std_window_size, std_window_size);

	if (param.ext_win_size && (image_size > DEFAULT_DECODE_WINDOW_MAX_SIZE)) {
		/*
		 * If the platform supports extended window and the SPI flash size is greater
		 * than 16MiB, then create a mapping for the extended window as well.
		 * The assumptions here are:
		 * 1. Top 16MiB is still decoded in the fixed decode window just below 4G
		 * boundary.
		 * 2. Rest of the SPI flash below the top 16MiB is mapped at the top of extended
		 * window. Even though the platform might support a larger extended window, the
		 * SPI flash part used by the mainboard might not be large enough to be mapped
		 * in the entire window. In such cases, the mapping is assumed to be in the top
		 * part of the extended window with the bottom part remaining unused.
		 *
		 * Example:
		 * ext_win_base = 0xF8000000
		 * ext_win_size = 32 * MiB
		 * ext_win_limit = ext_win_base + ext_win_size - 1 = 0xF9FFFFFF
		 *
		 * If SPI flash is 32MiB, then top 16MiB is mapped from 0xFF000000 - 0xFFFFFFFF
		 * whereas the bottom 16MiB is mapped from 0xF9000000 - 0xF9FFFFFF. The extended
		 * window 0xF8000000 - 0xF8FFFFFF remains unused.
		 */
		const size_t ext_window_mapped_size = MIN(param.ext_win_size,
							  image_size - std_window_size);
		const size_t ext_window_top = param.ext_win_base + param.ext_win_size;
		add_mmap_window(X86_EXTENDED_DECODE_WINDOW,
				std_window_flash_offset - ext_window_mapped_size,
				ext_window_top - ext_window_mapped_size,
				ext_window_mapped_size);

		if (region_overlap(&mmap_window_table[X86_EXTENDED_DECODE_WINDOW].host_space,
				   &mmap_window_table[X86_DEFAULT_DECODE_WINDOW].host_space)) {
			const struct region *ext_region;

			ext_region = &mmap_window_table[X86_EXTENDED_DECODE_WINDOW].host_space;
			ERROR("Extended window(base=0x%zx, limit=0x%zx) overlaps with default window!\n",
			      region_offset(ext_region), region_end(ext_region));

			return false;
		}
	}

	done = true;
	return done;
}

static unsigned int convert_address(const struct region *to, const struct region *from,
				    unsigned int addr)
{
	/*
	 * Calculate the offset in the "from" region and use that offset to calculate
	 * corresponding address in the "to" region.
	 */
	size_t offset = addr - region_offset(from);
	return region_offset(to) + offset;
}

enum mmap_addr_type {
	HOST_SPACE_ADDR,
	FLASH_SPACE_ADDR,
};

static int find_mmap_window(enum mmap_addr_type addr_type, unsigned int addr)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(mmap_window_table); i++) {
		const struct region *reg;

		if (addr_type == HOST_SPACE_ADDR)
			reg = &mmap_window_table[i].host_space;
		else
			reg = &mmap_window_table[i].flash_space;

		if (region_offset(reg) <= addr &&
		   ((uint64_t)region_offset(reg) + (uint64_t)region_sz(reg) - 1) >= addr)
			return i;
	}

	return -1;
}

static unsigned int convert_host_to_flash(const struct buffer *region, unsigned int addr)
{
	int idx;
	const struct region *to, *from;

	idx = find_mmap_window(HOST_SPACE_ADDR, addr);
	if (idx == -1) {
		ERROR("Host address(%x) not in any mmap window!\n", addr);
		return 0;
	}

	to = &mmap_window_table[idx].flash_space;
	from = &mmap_window_table[idx].host_space;

	/* region->offset is subtracted because caller expects offset in the given region. */
	return convert_address(to, from, addr) - region->offset;
}

static unsigned int convert_flash_to_host(const struct buffer *region, unsigned int addr)
{
	int idx;
	const struct region *to, *from;

	/*
	 * region->offset is added because caller provides offset in the given region. This is
	 * converted to an absolute address in the SPI flash space. This is done before the
	 * conversion as opposed to after in convert_host_to_flash() above because the address
	 * is actually an offset within the region. So, it needs to be converted into an
	 * absolute address in the SPI flash space before converting into an address in host
	 * space.
	 */
	addr += region->offset;
	idx = find_mmap_window(FLASH_SPACE_ADDR, addr);

	if (idx == -1) {
		ERROR("SPI flash address(%x) not in any mmap window!\n", addr);
		return 0;
	}

	to = &mmap_window_table[idx].host_space;
	from = &mmap_window_table[idx].flash_space;

	return convert_address(to, from, addr);
}

static unsigned int convert_addr_space(const struct buffer *region, unsigned int addr)
{
	assert(region);

	assert(create_mmap_windows());

	if (IS_HOST_SPACE_ADDRESS(addr))
		return convert_host_to_flash(region, addr);
	else
		return convert_flash_to_host(region, addr);
}

/*
 * This function takes offset value which represents the offset from one end of the region and
 * converts it to offset from the other end of the region. offset is expected to be positive.
 */
static int convert_region_offset(unsigned int offset, uint32_t *region_offset)
{
	size_t size;

	if (param.size) {
		size = param.size;
	} else {
		assert(param.image_region);
		size = param.image_region->size;
	}

	if (size < offset) {
		ERROR("Cannot convert region offset (size=0x%zx, offset=0x%x)\n", size, offset);
		return 1;
	}

	*region_offset = size - offset;
	return 0;
}

static int do_cbfs_locate(uint32_t *cbfs_addr, size_t data_size)
{
	uint32_t metadata_size = 0;

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	struct cbfs_image image;
	if (cbfs_image_from_buffer(&image, param.image_region,
							param.headeroffset))
		return 1;

	if (cbfs_get_entry(&image, param.name))
		WARN("'%s' already in CBFS.\n", param.name);

	if (!data_size) {
		ERROR("File '%s' is empty?\n", param.name);
		return 1;
	}

	/* Compute required page size */
	if (param.force_pow2_pagesize) {
		param.pagesize = 1;
		while (param.pagesize < data_size)
			param.pagesize <<= 1;
		DEBUG("Page size is %d (0x%x)\n", param.pagesize, param.pagesize);
	}

	/* Include cbfs_file size along with space for with name. */
	metadata_size += cbfs_calculate_file_header_size(param.name);
	/* Adjust metadata_size if additional attributes were added */
	if (param.autogen_attr) {
		if (param.alignment)
			metadata_size += sizeof(struct cbfs_file_attr_align);
		if (param.baseaddress_assigned || param.stage_xip)
			metadata_size += sizeof(struct cbfs_file_attr_position);
	}
	if (param.precompression || param.compression != CBFS_COMPRESS_NONE)
		metadata_size += sizeof(struct cbfs_file_attr_compression);
	if (param.type == CBFS_TYPE_STAGE)
		metadata_size += sizeof(struct cbfs_file_attr_stageheader);

	/* Take care of the hash attribute if it is used */
	if (param.hash != VB2_HASH_INVALID)
		metadata_size += cbfs_file_attr_hash_size(param.hash);

	int32_t address = cbfs_locate_entry(&image, data_size, param.pagesize,
						param.alignment, metadata_size);

	if (address < 0) {
		ERROR("'%s'(%u + %zu) can't fit in CBFS for page-size %#x, align %#x.\n",
		      param.name, metadata_size, data_size, param.pagesize, param.alignment);
		return 1;
	}

	*cbfs_addr = address;
	return 0;
}

typedef int (*convert_buffer_t)(struct buffer *buffer, uint32_t *offset,
	struct cbfs_file *header);

static int cbfs_add_integer_component(const char *name,
			      uint64_t u64val,
			      uint32_t offset,
			      uint32_t headeroffset) {
	struct cbfs_image image;
	struct cbfs_file *header = NULL;
	struct buffer buffer;
	int i, ret = 1;

	if (!name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (buffer_create(&buffer, 8, name) != 0)
		return 1;

	for (i = 0; i < 8; i++)
		buffer.data[i] = (u64val >> i*8) & 0xff;

	if (cbfs_image_from_buffer(&image, param.image_region, headeroffset)) {
		ERROR("Selected image region is not a CBFS.\n");
		goto done;
	}

	if (cbfs_get_entry(&image, name)) {
		ERROR("'%s' already in ROM image.\n", name);
		goto done;
	}

	header = cbfs_create_file_header(CBFS_TYPE_RAW,
		buffer.size, name);

	enum vb2_hash_algorithm algo = get_mh_cache()->cbfs_hash.algo;
	if (algo != VB2_HASH_INVALID)
		if (cbfs_add_file_hash(header, &buffer, algo)) {
			ERROR("couldn't add hash for '%s'\n", name);
			goto done;
		}

	if (cbfs_add_entry(&image, &buffer, offset, header, 0) != 0) {
		ERROR("Failed to add %llu into ROM image as '%s'.\n",
					(long long unsigned)u64val, name);
		goto done;
	}

	ret = maybe_update_metadata_hash(&image);

done:
	free(header);
	buffer_delete(&buffer);
	return ret;
}

static int is_valid_topswap(void)
{
	switch (param.topswap_size) {
	case (64 * KiB):
	case (128 * KiB):
	case (256 * KiB):
	case (512 * KiB):
	case (1 * MiB):
		break;
	default:
		ERROR("Invalid topswap_size %d, topswap can be 64K|128K|256K|512K|1M\n",
							param.topswap_size);
		return 0;
	}
	return 1;
}

static void fill_header_offset(void *location, uint32_t offset)
{
	// TODO: When we have a BE target, we'll need to store this as BE
	write_le32(location, offset);
}

static int update_master_header_loc_topswap(struct cbfs_image *image,
				void *h_loc, uint32_t header_offset)
{
	struct cbfs_file *entry;
	void *ts_h_loc = h_loc;

	entry = cbfs_get_entry(image, "bootblock");
	if (entry == NULL) {
		ERROR("Bootblock not in ROM image?!?\n");
		return 1;
	}

	/*
	 * Check if the existing topswap boundary matches with
	 * the one provided.
	 */
	if (param.topswap_size != be32toh(entry->len)/2) {
		ERROR("Top swap boundary does not match\n");
		return 1;
	}

	ts_h_loc -= param.topswap_size;
	fill_header_offset(ts_h_loc, header_offset);

	return 0;
}

static int cbfs_add_master_header(void)
{
	const char * const name = "cbfs master header";
	struct cbfs_image image;
	struct cbfs_file *header = NULL;
	struct buffer buffer;
	int ret = 1;
	size_t offset;
	size_t size;
	void *h_loc;

	if (cbfs_image_from_buffer(&image, param.image_region,
		param.headeroffset)) {
		ERROR("Selected image region is not a CBFS.\n");
		return 1;
	}

	if (cbfs_get_entry(&image, name)) {
		ERROR("'%s' already in ROM image.\n", name);
		return 1;
	}

	if (buffer_create(&buffer, sizeof(struct cbfs_header), name) != 0)
		return 1;

	struct cbfs_header *h = (struct cbfs_header *)buffer.data;
	h->magic = htobe32(CBFS_HEADER_MAGIC);
	h->version = htobe32(CBFS_HEADER_VERSION);
	/* The 4 bytes are left out for two reasons:
	 * 1. the cbfs master header pointer resides there
	 * 2. some cbfs implementations assume that an image that resides
	 *    below 4GB has a bootblock and get confused when the end of the
	 *    image is at 4GB == 0.
	 */
	h->bootblocksize = htobe32(4);
	h->align = htobe32(CBFS_ALIGNMENT);
	/* The offset and romsize fields within the master header are absolute
	 * values within the boot media. As such, romsize needs to relfect
	 * the end 'offset' for a CBFS. To achieve that the current buffer
	 * representing the CBFS region's size is added to the offset of
	 * the region within a larger image.
	 */
	offset = buffer_get(param.image_region) -
		buffer_get_original_backing(param.image_region);
	size = buffer_size(param.image_region);
	h->romsize = htobe32(size + offset);
	h->offset = htobe32(offset);
	h->architecture = htobe32(CBFS_ARCHITECTURE_UNKNOWN);

	/* Never add a hash attribute to the master header. */
	header = cbfs_create_file_header(CBFS_TYPE_CBFSHEADER,
		buffer_size(&buffer), name);
	if (cbfs_add_entry(&image, &buffer, 0, header, 0) != 0) {
		ERROR("Failed to add cbfs master header into ROM image.\n");
		goto done;
	}

	struct cbfs_file *entry;
	if ((entry = cbfs_get_entry(&image, name)) == NULL) {
		ERROR("'%s' not in ROM image?!?\n", name);
		goto done;
	}

	uint32_t header_offset = CBFS_SUBHEADER(entry) -
		buffer_get(&image.buffer);
	header_offset = -(buffer_size(&image.buffer) - header_offset);

	h_loc = (void *)(buffer_get(&image.buffer) +
				buffer_size(&image.buffer) - 4);
	fill_header_offset(h_loc, header_offset);
	/*
	 * If top swap present, update the header
	 * location in secondary bootblock
	 */
	if (param.topswap_size) {
		if (update_master_header_loc_topswap(&image, h_loc,
							header_offset))
			return 1;
	}

	ret = maybe_update_metadata_hash(&image);

done:
	free(header);
	buffer_delete(&buffer);
	return ret;
}

static int add_topswap_bootblock(struct buffer *buffer, uint32_t *offset)
{
	size_t bb_buf_size = buffer_size(buffer);

	if (bb_buf_size > param.topswap_size) {
		ERROR("Bootblock bigger than the topswap boundary\n");
		ERROR("size = %zd, ts = %d\n", bb_buf_size,
							param.topswap_size);
		return 1;
	}

	/*
	 * Allocate topswap_size*2 bytes for bootblock to
	 * accommodate the second bootblock.
	 */
	struct buffer new_bootblock, bb1, bb2;
	if (buffer_create(&new_bootblock, 2 * param.topswap_size,
							buffer->name))
		return 1;

	buffer_splice(&bb1, &new_bootblock, param.topswap_size - bb_buf_size,
							bb_buf_size);
	buffer_splice(&bb2, &new_bootblock,
				buffer_size(&new_bootblock) - bb_buf_size,
							bb_buf_size);

	/* Copy to first bootblock */
	memcpy(buffer_get(&bb1), buffer_get(buffer), bb_buf_size);
	/* Copy to second bootblock */
	memcpy(buffer_get(&bb2), buffer_get(buffer), bb_buf_size);

	buffer_delete(buffer);
	buffer_clone(buffer, &new_bootblock);

	 /* Update the location (offset) of bootblock in the region */
	return convert_region_offset(buffer_size(buffer), offset);
}

static int cbfs_add_component(const char *filename,
			      const char *name,
			      uint32_t headeroffset,
			      convert_buffer_t convert)
{
	/*
	 * The steps used to determine the final placement offset in CBFS, in order:
	 *
	 * 1. If --base-address was passed, that value is used. If it was passed in the host
	 *    address space, convert it to flash address space. (After that, |*offset| is always
	 *    in the flash address space.)
	 *
	 * 2. The convert() function may write a location back to |offset|, usually by calling
	 *    do_cbfs_locate(). In this case, it needs to ensure that the location found can fit
	 *    the CBFS file in its final form (after any compression and conversion).
	 *
	 * 3. If --align was passed and the offset is still undecided at this point,
	 *    do_cbfs_locate() is called to find an appropriately aligned location.
	 *
	 * 4. If |offset| is still 0 at the end, cbfs_add_entry() will find the first available
	 *    location that fits.
	 */
	uint32_t offset = param.baseaddress_assigned ? param.baseaddress : 0;
	size_t len_align = 0;

	if (param.alignment && param.baseaddress_assigned) {
		ERROR("Cannot specify both alignment and base address\n");
		return 1;
	}

	if (param.stage_xip && param.compression != CBFS_COMPRESS_NONE) {
		ERROR("Cannot specify compression for XIP.\n");
		return 1;
	}

	if (!filename) {
		ERROR("You need to specify -f/--filename.\n");
		return 1;
	}

	if (!name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (param.type == 0) {
		ERROR("You need to specify a valid -t/--type.\n");
		return 1;
	}

	struct cbfs_image image;
	if (cbfs_image_from_buffer(&image, param.image_region, headeroffset))
		return 1;

	if (cbfs_get_entry(&image, name)) {
		ERROR("'%s' already in ROM image.\n", name);
		return 1;
	}

	struct buffer buffer;
	if (buffer_from_file(&buffer, filename) != 0) {
		ERROR("Could not load file '%s'.\n", filename);
		return 1;
	}

	struct cbfs_file *header =
		cbfs_create_file_header(param.type, buffer.size, name);

	/* Bootblock and CBFS header should never have file hashes. When adding
	   the bootblock it is important that we *don't* look up the metadata
	   hash yet (before it is added) or we'll cache an outdated result. */
	if (param.type != CBFS_TYPE_BOOTBLOCK && param.type != CBFS_TYPE_CBFSHEADER) {
		enum vb2_hash_algorithm mh_algo = get_mh_cache()->cbfs_hash.algo;
		if (mh_algo != VB2_HASH_INVALID && param.hash != mh_algo) {
			if (param.hash == VB2_HASH_INVALID) {
				param.hash = mh_algo;
			} else {
				ERROR("Cannot specify hash %s that's different from metadata hash algorithm %s\n",
				      vb2_get_hash_algorithm_name(param.hash),
				      vb2_get_hash_algorithm_name(mh_algo));
				goto error;
			}
		}
	}

	/*
	 * Check if Intel CPU topswap is specified this will require a
	 * second bootblock to be added.
	 */
	if (param.type == CBFS_TYPE_BOOTBLOCK && param.topswap_size)
		if (add_topswap_bootblock(&buffer, &offset))
			goto error;

	/* With --base-address we allow host space addresses -- if so, convert it here. */
	if (IS_HOST_SPACE_ADDRESS(offset))
		offset = convert_addr_space(param.image_region, offset);

	if (convert && convert(&buffer, &offset, header) != 0) {
		ERROR("Failed to parse file '%s'.\n", filename);
		goto error;
	}

	/* This needs to run after convert() to take compression into account. */
	if (!offset && param.alignment)
		if (do_cbfs_locate(&offset, buffer_size(&buffer)))
			goto error;

	/* This needs to run after convert() to hash the actual final file data. */
	if (param.hash != VB2_HASH_INVALID &&
	    cbfs_add_file_hash(header, &buffer, param.hash) == -1) {
		ERROR("couldn't add hash for '%s'\n", name);
		goto error;
	}

	if (param.autogen_attr) {
		/* Add position attribute if assigned */
		if (param.baseaddress_assigned || param.stage_xip) {
			struct cbfs_file_attr_position *attrs =
				(struct cbfs_file_attr_position *)
				cbfs_add_file_attr(header,
					CBFS_FILE_ATTR_TAG_POSITION,
					sizeof(struct cbfs_file_attr_position));
			if (attrs == NULL)
				goto error;
			attrs->position = htobe32(offset);
		}
		/* Add alignment attribute if used */
		if (param.alignment) {
			struct cbfs_file_attr_align *attrs =
				(struct cbfs_file_attr_align *)
				cbfs_add_file_attr(header,
					CBFS_FILE_ATTR_TAG_ALIGNMENT,
					sizeof(struct cbfs_file_attr_align));
			if (attrs == NULL)
				goto error;
			attrs->alignment = htobe32(param.alignment);
		}
	}

	if (param.ibb) {
		/* Mark as Initial Boot Block */
		struct cbfs_file_attribute *attrs = cbfs_add_file_attr(header,
				CBFS_FILE_ATTR_TAG_IBB,
				sizeof(struct cbfs_file_attribute));
		if (attrs == NULL)
			goto error;
		/* For Intel TXT minimum align is 16 */
		len_align = 16;
	}

	if (param.padding) {
		const uint32_t hs = sizeof(struct cbfs_file_attribute);
		uint32_t size = ALIGN_UP(MAX(hs, param.padding),
					 CBFS_ATTRIBUTE_ALIGN);
		INFO("Padding %d bytes\n", size);
		struct cbfs_file_attribute *attr =
			(struct cbfs_file_attribute *)cbfs_add_file_attr(
					header, CBFS_FILE_ATTR_TAG_PADDING,
					size);
		if (attr == NULL)
			goto error;
	}

	if (cbfs_add_entry(&image, &buffer, offset, header, len_align) != 0) {
		ERROR("Failed to add '%s' into ROM image.\n", filename);
		goto error;
	}

	free(header);
	buffer_delete(&buffer);

	return maybe_update_metadata_hash(&image) || maybe_update_fmap_hash();

error:
	free(header);
	buffer_delete(&buffer);
	return 1;
}

static int cbfstool_convert_raw(struct buffer *buffer,
	unused uint32_t *offset, struct cbfs_file *header)
{
	char *compressed;
	int decompressed_size, compressed_size;
	comp_func_ptr compress;

	decompressed_size = buffer->size;
	if (param.precompression) {
		param.compression = read_le32(buffer->data);
		decompressed_size = read_le32(buffer->data + sizeof(uint32_t));
		compressed_size = buffer->size - 8;
		compressed = malloc(compressed_size);
		if (!compressed)
			return -1;
		memcpy(compressed, buffer->data + 8, compressed_size);
	} else {
		if (param.compression == CBFS_COMPRESS_NONE)
			goto out;

		compress = compression_function(param.compression);
		if (!compress)
			return -1;
		compressed = calloc(buffer->size, 1);
		if (!compressed)
			return -1;

		if (compress(buffer->data, buffer->size,
			     compressed, &compressed_size)) {
			WARN("Compression failed - disabled\n");
			free(compressed);
			goto out;
		}
	}

	struct cbfs_file_attr_compression *attrs =
		(struct cbfs_file_attr_compression *)
		cbfs_add_file_attr(header,
			CBFS_FILE_ATTR_TAG_COMPRESSION,
			sizeof(struct cbfs_file_attr_compression));
	if (attrs == NULL) {
		free(compressed);
		return -1;
	}
	attrs->compression = htobe32(param.compression);
	attrs->decompressed_size = htobe32(decompressed_size);

	free(buffer->data);
	buffer->data = compressed;
	buffer->size = compressed_size;

out:
	header->len = htobe32(buffer->size);
	return 0;
}

static int cbfstool_convert_fsp(struct buffer *buffer,
				uint32_t *offset, struct cbfs_file *header)
{
	uint32_t address;
	struct buffer fsp;

	/*
	 * There are 4 different cases here:
	 *
	 * 1. --xip and --base-address: we need to place the binary at the given base address
	 *    in the CBFS image and relocate it to that address. *offset was already filled in,
	 *    but we need to convert it to the host address space for relocation.
	 *
	 * 2. --xip but no --base-address: we implicitly force a 4K minimum alignment so that
	 *    relocation can occur. Call do_cbfs_locate() here to find an appropriate *offset.
	 *    This also needs to be converted to the host address space for relocation.
	 *
	 * 3. No --xip but a --base-address: special case where --base-address does not have its
	 *    normal meaning, instead we use it as the relocation target address. We explicitly
	 *    reset *offset to 0 so that the file will be placed wherever it fits in CBFS.
	 *
	 * 4. No --xip and no --base-address: this means that the FSP was pre-linked and should
	 *    not be relocated. Just chain directly to convert_raw() for compression.
	 */

	if (param.stage_xip) {
		if (!param.baseaddress_assigned) {
			param.alignment = 4*1024;
			if (do_cbfs_locate(offset, buffer_size(buffer)))
				return -1;
		}
		assert(!IS_HOST_SPACE_ADDRESS(*offset));
		address = convert_addr_space(param.image_region, *offset);
	} else {
		if (param.baseaddress_assigned == 0) {
			INFO("Honoring pre-linked FSP module, no relocation.\n");
			return cbfstool_convert_raw(buffer, offset, header);
		} else {
			address = param.baseaddress;
			*offset = 0;
		}
	}

	/* Create a copy of the buffer to attempt relocation. */
	if (buffer_create(&fsp, buffer_size(buffer), "fsp"))
		return -1;

	memcpy(buffer_get(&fsp), buffer_get(buffer), buffer_size(buffer));

	/* Replace the buffer contents w/ the relocated ones on success. */
	if (fsp_component_relocate(address, buffer_get(&fsp), buffer_size(&fsp))
	    > 0) {
		buffer_delete(buffer);
		buffer_clone(buffer, &fsp);
	} else {
		buffer_delete(&fsp);
		WARN("Invalid FSP variant.\n");
	}

	/* Let the raw path handle all the cbfs metadata logic. */
	return cbfstool_convert_raw(buffer, offset, header);
}

static int cbfstool_convert_mkstage(struct buffer *buffer, uint32_t *offset,
	struct cbfs_file *header)
{
	struct buffer output;
	size_t data_size;
	int ret;

	if (elf_program_file_size(buffer, &data_size) < 0) {
		ERROR("Could not obtain ELF size\n");
		return 1;
	}

	/*
	 * We need a final location for XIP parsing, so we need to call do_cbfs_locate() early
	 * here. That is okay because XIP stages may not be compressed, so their size cannot
	 * change anymore at a later point.
	 */
	if (param.stage_xip &&
	    do_cbfs_locate(offset, data_size))  {
		ERROR("Could not find location for stage.\n");
		return 1;
	}

	struct cbfs_file_attr_stageheader *stageheader = (void *)
		cbfs_add_file_attr(header, CBFS_FILE_ATTR_TAG_STAGEHEADER,
				   sizeof(struct cbfs_file_attr_stageheader));
	if (!stageheader)
		return -1;

	if (param.stage_xip) {
		uint32_t host_space_address = convert_addr_space(param.image_region, *offset);
		assert(IS_HOST_SPACE_ADDRESS(host_space_address));
		ret = parse_elf_to_xip_stage(buffer, &output, host_space_address,
					     param.ignore_section, stageheader);
	} else {
		ret = parse_elf_to_stage(buffer, &output, param.ignore_section,
					 stageheader);
	}
	if (ret != 0)
		return -1;

	/* Store a hash of original uncompressed stage to compare later. */
	size_t decmp_size = buffer_size(&output);
	uint32_t decmp_hash = XXH32(buffer_get(&output), decmp_size, 0);

	/* Chain to base conversion routine to handle compression. */
	ret = cbfstool_convert_raw(&output, offset, header);
	if (ret != 0)
		goto fail;

	/* Special care must be taken for LZ4-compressed stages that the BSS is
	   large enough to provide scratch space for in-place decompression. */
	if (!param.precompression && param.compression == CBFS_COMPRESS_LZ4) {
		size_t memlen = be32toh(stageheader->memlen);
		size_t compressed_size = buffer_size(&output);
		uint8_t *compare_buffer = malloc(memlen);
		uint8_t *start = compare_buffer + memlen - compressed_size;
		if (!compare_buffer) {
			ERROR("Out of memory\n");
			goto fail;
		}
		memcpy(start, buffer_get(&output), compressed_size);
		ret = ulz4fn(start, compressed_size, compare_buffer, memlen);
		if  (ret == 0) {
			ERROR("Not enough scratch space to decompress LZ4 in-place -- increase BSS size or disable compression!\n");
			free(compare_buffer);
			goto fail;
		} else if (ret != (int)decmp_size ||
			   decmp_hash != XXH32(compare_buffer, decmp_size, 0)) {
			ERROR("LZ4 compression BUG! Report to mailing list.\n");
			free(compare_buffer);
			goto fail;
		}
		free(compare_buffer);
	}

	buffer_delete(buffer);
	buffer_clone(buffer, &output);
	return 0;

fail:
	buffer_delete(&output);
	return -1;
}

static int cbfstool_convert_mkpayload(struct buffer *buffer,
	unused uint32_t *offset, struct cbfs_file *header)
{
	struct buffer output;
	int ret;
	/* Per default, try and see if payload is an ELF binary */
	ret = parse_elf_to_payload(buffer, &output, param.compression);

	/* If it's not an ELF, see if it's a FIT */
	if (ret != 0) {
		ret = parse_fit_to_payload(buffer, &output, param.compression);
		if (ret == 0)
			header->type = htobe32(CBFS_TYPE_FIT);
	}

	/* If it's not an FIT, see if it's a UEFI FV */
	if (ret != 0)
		ret = parse_fv_to_payload(buffer, &output, param.compression);

	/* If it's neither ELF nor UEFI Fv, try bzImage */
	if (ret != 0)
		ret = parse_bzImage_to_payload(buffer, &output,
				param.initrd, param.cmdline, param.compression);

	/* Not a supported payload type */
	if (ret != 0) {
		ERROR("Not a supported payload type (ELF / FV).\n");
		buffer_delete(buffer);
		return -1;
	}

	buffer_delete(buffer);
	// Direct assign, no dupe.
	memcpy(buffer, &output, sizeof(*buffer));
	header->len = htobe32(output.size);
	return 0;
}

static int cbfstool_convert_mkflatpayload(struct buffer *buffer,
	unused uint32_t *offset, struct cbfs_file *header)
{
	struct buffer output;
	if (parse_flat_binary_to_payload(buffer, &output,
					 param.loadaddress,
					 param.entrypoint,
					 param.compression) != 0) {
		return -1;
	}
	buffer_delete(buffer);
	// Direct assign, no dupe.
	memcpy(buffer, &output, sizeof(*buffer));
	header->len = htobe32(output.size);
	return 0;
}

static int cbfs_add(void)
{
	convert_buffer_t convert = cbfstool_convert_raw;

	if (param.type == CBFS_TYPE_FSP) {
		convert = cbfstool_convert_fsp;
	} else if (param.type == CBFS_TYPE_STAGE) {
		ERROR("stages can only be added with cbfstool add-stage\n");
		return 1;
	} else if (param.stage_xip) {
		ERROR("cbfstool add supports xip only for FSP component type\n");
		return 1;
	}

	return cbfs_add_component(param.filename,
				  param.name,
				  param.headeroffset,
				  convert);
}

static int cbfs_add_stage(void)
{
	if (param.stage_xip && param.baseaddress_assigned) {
		ERROR("Cannot specify base address for XIP.\n");
		return 1;
	}
	param.type = CBFS_TYPE_STAGE;

	return cbfs_add_component(param.filename,
				  param.name,
				  param.headeroffset,
				  cbfstool_convert_mkstage);
}

static int cbfs_add_payload(void)
{
	param.type = CBFS_TYPE_SELF;
	return cbfs_add_component(param.filename,
				  param.name,
				  param.headeroffset,
				  cbfstool_convert_mkpayload);
}

static int cbfs_add_flat_binary(void)
{
	if (param.loadaddress == 0) {
		ERROR("You need to specify a valid "
			"-l/--load-address.\n");
		return 1;
	}
	if (param.entrypoint == 0) {
		ERROR("You need to specify a valid "
			"-e/--entry-point.\n");
		return 1;
	}
	param.type = CBFS_TYPE_SELF;
	return cbfs_add_component(param.filename,
				  param.name,
				  param.headeroffset,
				  cbfstool_convert_mkflatpayload);
}

static int cbfs_add_integer(void)
{
	if (!param.u64val_assigned) {
		ERROR("You need to specify a value to write.\n");
		return 1;
	}
	return cbfs_add_integer_component(param.name,
				  param.u64val,
				  param.baseaddress,
				  param.headeroffset);
}

static int cbfs_remove(void)
{
	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	struct cbfs_image image;
	if (cbfs_image_from_buffer(&image, param.image_region,
							param.headeroffset))
		return 1;

	if (cbfs_remove_entry(&image, param.name) != 0) {
		ERROR("Removing file '%s' failed.\n",
		      param.name);
		return 1;
	}

	return maybe_update_metadata_hash(&image);
}

static int cbfs_create(void)
{
	struct cbfs_image image;
	memset(&image, 0, sizeof(image));
	buffer_clone(&image.buffer, param.image_region);

	if (param.fmap) {
		if (param.arch != CBFS_ARCHITECTURE_UNKNOWN || param.size ||
						param.baseaddress_assigned ||
						param.headeroffset_assigned ||
						param.cbfsoffset_assigned ||
							param.bootblock) {
			ERROR("Since -M was provided, -m, -s, -b, -o, -H, and -B should be omitted\n");
			return 1;
		}

		return cbfs_image_create(&image, image.buffer.size);
	}

	if (param.arch == CBFS_ARCHITECTURE_UNKNOWN) {
		ERROR("You need to specify -m/--machine arch.\n");
		return 1;
	}

	struct buffer bootblock;
	if (!param.bootblock) {
		DEBUG("-B not given, creating image without bootblock.\n");
		if (buffer_create(&bootblock, 0, "(dummy)") != 0)
			return 1;
	} else if (buffer_from_file(&bootblock, param.bootblock)) {
		return 1;
	}

	if (!param.alignment)
		param.alignment = CBFS_ALIGNMENT;

	// Set default offsets. x86, as usual, needs to be a special snowflake.
	if (!param.baseaddress_assigned) {
		if (param.arch == CBFS_ARCHITECTURE_X86) {
			// Make sure there's at least enough room for rel_offset
			param.baseaddress = param.size -
					MAX(bootblock.size, sizeof(int32_t));
			DEBUG("x86 -> bootblock lies at end of ROM (%#x).\n",
			      param.baseaddress);
		} else {
			param.baseaddress = 0;
			DEBUG("bootblock starts at address 0x0.\n");
		}
	}
	if (!param.headeroffset_assigned) {
		if (param.arch == CBFS_ARCHITECTURE_X86) {
			param.headeroffset = param.baseaddress -
					     sizeof(struct cbfs_header);
			DEBUG("x86 -> CBFS header before bootblock (%#x).\n",
				param.headeroffset);
		} else {
			param.headeroffset = align_up(param.baseaddress +
				bootblock.size, sizeof(uint32_t));
			DEBUG("CBFS header placed behind bootblock (%#x).\n",
				param.headeroffset);
		}
	}
	if (!param.cbfsoffset_assigned) {
		if (param.arch == CBFS_ARCHITECTURE_X86) {
			param.cbfsoffset = 0;
			DEBUG("x86 -> CBFS entries start at address 0x0.\n");
		} else {
			param.cbfsoffset = align_up(param.headeroffset +
						    sizeof(struct cbfs_header),
						    CBFS_ALIGNMENT);
			DEBUG("CBFS entries start beind master header (%#x).\n",
			      param.cbfsoffset);
		}
	}

	int ret = cbfs_legacy_image_create(&image,
					   param.arch,
					   CBFS_ALIGNMENT,
					   &bootblock,
					   param.baseaddress,
					   param.headeroffset,
					   param.cbfsoffset);
	buffer_delete(&bootblock);
	return ret;
}

static int cbfs_layout(void)
{
	const struct fmap *fmap = partitioned_file_get_fmap(param.image_file);
	if (!fmap) {
		LOG("This is a legacy image composed entirely of a single CBFS.\n");
		return 1;
	}

	printf("This image contains the following sections that can be %s with this tool:\n",
			param.show_immutable ? "accessed" : "manipulated");
	puts("");
	for (unsigned i = 0; i < fmap->nareas; ++i) {
		const struct fmap_area *current = fmap->areas + i;

		bool readonly = partitioned_file_fmap_count(param.image_file,
			partitioned_file_fmap_select_children_of, current) ||
				region_is_flashmap((const char *)current->name);
		if (!param.show_immutable && readonly)
			continue;

		printf("'%s'", current->name);

		// Detect consecutive sections that describe the same region and
		// show them as aliases. This cannot find equivalent entries
		// that aren't adjacent; however, fmaptool doesn't generate
		// FMAPs with such sections, so this convenience feature works
		// for all but the strangest manually created FMAP binaries.
		// TODO: This could be done by parsing the FMAP into some kind
		// of tree that had duplicate lists in addition to child lists,
		// which would allow covering that weird, unlikely case as well.
		unsigned lookahead;
		for (lookahead = 1; i + lookahead < fmap->nareas;
								++lookahead) {
			const struct fmap_area *consecutive =
					fmap->areas + i + lookahead;
			if (consecutive->offset != current->offset ||
					consecutive->size != current->size)
				break;
			printf(", '%s'", consecutive->name);
		}
		if (lookahead > 1)
			fputs(" are aliases for the same region", stdout);

		const char *qualifier = "";
		if (readonly)
			qualifier = "read-only, ";
		else if (region_is_modern_cbfs((const char *)current->name))
			qualifier = "CBFS, ";
		else if (current->flags & FMAP_AREA_PRESERVE)
			qualifier = "preserve, ";
		printf(" (%ssize %u, offset %u)\n", qualifier, current->size,
				current->offset);

		i += lookahead - 1;
	}
	puts("");

	if (param.show_immutable) {
		puts("It is at least possible to perform the read action on every section listed above.");
	} else {
		puts("It is possible to perform either the write action or the CBFS add/remove actions on every section listed above.");
		puts("To see the image's read-only sections as well, rerun with the -w option.");
	}

	return 0;
}

static int cbfs_print(void)
{
	struct cbfs_image image;
	if (cbfs_image_from_buffer(&image, param.image_region,
							param.headeroffset))
		return 1;
	if (param.machine_parseable) {
		if (verbose)
			printf("[FMAP REGION]\t%s\n", param.region_name);
		cbfs_print_parseable_directory(&image);
	} else {
		printf("FMAP REGION: %s\n", param.region_name);
		cbfs_print_directory(&image);
	}

	if (verbose) {
		struct mh_cache *mhc = get_mh_cache();
		if (mhc->cbfs_hash.algo == VB2_HASH_INVALID)
			return 0;

		struct vb2_hash real_hash = { .algo = mhc->cbfs_hash.algo };
		enum cb_err err = cbfs_walk(&image, NULL, NULL, &real_hash,
					    CBFS_WALK_WRITEBACK_HASH);
		if (err != CB_CBFS_NOT_FOUND) {
			ERROR("Unexpected cbfs_walk() error %d\n", err);
			return 1;
		}
		char *hash_str = bintohex(real_hash.raw,
				vb2_digest_size(real_hash.algo));
		printf("[METADATA HASH]\t%s:%s",
		       vb2_get_hash_algorithm_name(real_hash.algo), hash_str);
		if (!strcmp(param.region_name, SECTION_NAME_PRIMARY_CBFS)) {
			if (!memcmp(mhc->cbfs_hash.raw, real_hash.raw,
				    vb2_digest_size(real_hash.algo)))
				printf(":valid");
			else
				printf(":invalid");
		}
		printf("\n");
		free(hash_str);
	}

	return 0;
}

static int cbfs_extract(void)
{
	if (!param.filename) {
		ERROR("You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	struct cbfs_image image;
	if (cbfs_image_from_buffer(&image, param.image_region,
							param.headeroffset))
		return 1;

	return cbfs_export_entry(&image, param.name, param.filename,
				param.arch, !param.unprocessed);
}

static int cbfs_write(void)
{
	if (!param.filename) {
		ERROR("You need to specify a valid input -f/--file.\n");
		return 1;
	}
	if (!partitioned_file_is_partitioned(param.image_file)) {
		ERROR("This operation isn't valid on legacy images having CBFS master headers\n");
		return 1;
	}

	if (!param.force && region_is_modern_cbfs(param.region_name)) {
		ERROR("Target image region '%s' is a CBFS and must be manipulated using add and remove\n",
							param.region_name);
		return 1;
	}

	struct buffer new_content;
	if (buffer_from_file(&new_content, param.filename))
		return 1;

	if (buffer_check_magic(&new_content, FMAP_SIGNATURE,
						strlen(FMAP_SIGNATURE))) {
		ERROR("File '%s' appears to be an FMAP and cannot be added to an existing image\n",
								param.filename);
		buffer_delete(&new_content);
		return 1;
	}
	if (!param.force && buffer_check_magic(&new_content, CBFS_FILE_MAGIC,
						strlen(CBFS_FILE_MAGIC))) {
		ERROR("File '%s' appears to be a CBFS and cannot be inserted into a raw region\n",
								param.filename);
		buffer_delete(&new_content);
		return 1;
	}

	unsigned offset = 0;
	if (param.fill_partial_upward && param.fill_partial_downward) {
		ERROR("You may only specify one of -u and -d.\n");
		buffer_delete(&new_content);
		return 1;
	} else if (!param.fill_partial_upward && !param.fill_partial_downward) {
		if (new_content.size != param.image_region->size) {
			ERROR("File to add is %zu bytes and would not fill %zu-byte target region (did you mean to pass either -u or -d?)\n",
				new_content.size, param.image_region->size);
			buffer_delete(&new_content);
			return 1;
		}
	} else {
		if (new_content.size > param.image_region->size) {
			ERROR("File to add is %zu bytes and would overflow %zu-byte target region\n",
				new_content.size, param.image_region->size);
			buffer_delete(&new_content);
			return 1;
		}
		if (param.u64val == (uint64_t)-1) {
			WARN("Written area will abut %s of target region: any unused space will keep its current contents\n",
					param.fill_partial_upward ? "bottom" : "top");
		} else if (param.u64val > 0xff) {
			ERROR("given fill value (%x) is larger than a byte\n", (unsigned)(param.u64val & 0xff));
			buffer_delete(&new_content);
			return 1;
		} else {
			memset(buffer_get(param.image_region),
				param.u64val & 0xff,
				buffer_size(param.image_region));
		}
		if (param.fill_partial_downward)
			offset = param.image_region->size - new_content.size;
	}

	memcpy(param.image_region->data + offset, new_content.data,
							new_content.size);
	buffer_delete(&new_content);

	return maybe_update_fmap_hash();
}

static int cbfs_read(void)
{
	if (!param.filename) {
		ERROR("You need to specify a valid output -f/--file.\n");
		return 1;
	}
	if (!partitioned_file_is_partitioned(param.image_file)) {
		ERROR("This operation isn't valid on legacy images having CBFS master headers\n");
		return 1;
	}

	return buffer_write_file(param.image_region, param.filename);
}

static int cbfs_copy(void)
{
	struct cbfs_image src_image;
	struct buffer src_buf;

	if (!param.source_region) {
		ERROR("You need to specify -R/--source-region.\n");
		return 1;
	}

	/* Obtain the source region and convert it to a cbfs_image. */
	if (!partitioned_file_read_region(&src_buf, param.image_file,
						param.source_region)) {
		ERROR("Region not found in image: %s\n", param.source_region);
		return 1;
	}

	if (cbfs_image_from_buffer(&src_image, &src_buf, param.headeroffset))
		return 1;

	return cbfs_copy_instance(&src_image, param.image_region);
}

static int cbfs_compact(void)
{
	struct cbfs_image image;
	if (cbfs_image_from_buffer(&image, param.image_region,
							param.headeroffset))
		return 1;
	WARN("Compacting a CBFS doesn't honor alignment or fixed addresses!\n");
	return cbfs_compact_instance(&image);
}

static int cbfs_expand(void)
{
	struct buffer src_buf;

	/* Obtain the source region. */
	if (!partitioned_file_read_region(&src_buf, param.image_file,
						param.region_name)) {
		ERROR("Region not found in image: %s\n", param.source_region);
		return 1;
	}

	return cbfs_expand_to_region(param.image_region);
}

static int cbfs_truncate(void)
{
	struct buffer src_buf;

	/* Obtain the source region. */
	if (!partitioned_file_read_region(&src_buf, param.image_file,
						param.region_name)) {
		ERROR("Region not found in image: %s\n", param.source_region);
		return 1;
	}

	uint32_t size;
	int result = cbfs_truncate_space(param.image_region, &size);
	printf("0x%x\n", size);
	return result;
}

static const struct command commands[] = {
	{"add", "H:r:f:n:t:c:b:a:p:yvA:j:gh?", cbfs_add, true, true},
	{"add-flat-binary", "H:r:f:n:l:e:c:b:p:vA:gh?", cbfs_add_flat_binary,
				true, true},
	{"add-payload", "H:r:f:n:c:b:a:C:I:p:vA:gh?", cbfs_add_payload,
				true, true},
	{"add-stage", "a:H:r:f:n:t:c:b:P:QS:p:yvA:gh?", cbfs_add_stage,
				true, true},
	{"add-int", "H:r:i:n:b:vgh?", cbfs_add_integer, true, true},
	{"add-master-header", "H:r:vh?j:", cbfs_add_master_header, true, true},
	{"compact", "r:h?", cbfs_compact, true, true},
	{"copy", "r:R:h?", cbfs_copy, true, true},
	{"create", "M:r:s:B:b:H:o:m:vh?", cbfs_create, true, true},
	{"extract", "H:r:m:n:f:Uvh?", cbfs_extract, true, false},
	{"layout", "wvh?", cbfs_layout, false, false},
	{"print", "H:r:vkh?", cbfs_print, true, false},
	{"read", "r:f:vh?", cbfs_read, true, false},
	{"remove", "H:r:n:vh?", cbfs_remove, true, true},
	{"write", "r:f:i:Fudvh?", cbfs_write, true, true},
	{"expand", "r:h?", cbfs_expand, true, true},
	{"truncate", "r:h?", cbfs_truncate, true, true},
};

enum {
	/* begin after ASCII characters */
	LONGOPT_START = 256,
	LONGOPT_IBB = LONGOPT_START,
	LONGOPT_EXT_WIN_BASE,
	LONGOPT_EXT_WIN_SIZE,
	LONGOPT_END,
};

static struct option long_options[] = {
	{"alignment",     required_argument, 0, 'a' },
	{"base-address",  required_argument, 0, 'b' },
	{"bootblock",     required_argument, 0, 'B' },
	{"cmdline",       required_argument, 0, 'C' },
	{"compression",   required_argument, 0, 'c' },
	{"topswap-size",  required_argument, 0, 'j' },
	{"empty-fits",    required_argument, 0, 'x' },
	{"entry-point",   required_argument, 0, 'e' },
	{"file",          required_argument, 0, 'f' },
	{"fill-downward", no_argument,       0, 'd' },
	{"fill-upward",   no_argument,       0, 'u' },
	{"flashmap",      required_argument, 0, 'M' },
	{"fmap-regions",  required_argument, 0, 'r' },
	{"force",         no_argument,       0, 'F' },
	{"source-region", required_argument, 0, 'R' },
	{"hash-algorithm",required_argument, 0, 'A' },
	{"header-offset", required_argument, 0, 'H' },
	{"help",          no_argument,       0, 'h' },
	{"ignore-sec",    required_argument, 0, 'S' },
	{"initrd",        required_argument, 0, 'I' },
	{"int",           required_argument, 0, 'i' },
	{"load-address",  required_argument, 0, 'l' },
	{"machine",       required_argument, 0, 'm' },
	{"name",          required_argument, 0, 'n' },
	{"offset",        required_argument, 0, 'o' },
	{"padding",       required_argument, 0, 'p' },
	{"pow2page",      no_argument,       0, 'Q' },
	{"ucode-region",  required_argument, 0, 'q' },
	{"size",          required_argument, 0, 's' },
	{"type",          required_argument, 0, 't' },
	{"verbose",       no_argument,       0, 'v' },
	{"with-readonly", no_argument,       0, 'w' },
	{"xip",           no_argument,       0, 'y' },
	{"gen-attribute", no_argument,       0, 'g' },
	{"mach-parseable",no_argument,       0, 'k' },
	{"unprocessed",   no_argument,       0, 'U' },
	{"ibb",           no_argument,       0, LONGOPT_IBB },
	{"ext-win-base",  required_argument, 0, LONGOPT_EXT_WIN_BASE },
	{"ext-win-size",  required_argument, 0, LONGOPT_EXT_WIN_SIZE },
	{NULL,            0,                 0,  0  }
};

static int get_region_offset(long long int offset, uint32_t *region_offset)
{
	/* If offset is not negative, no transformation required. */
	if (offset >= 0) {
		*region_offset = offset;
		return 0;
	}

	/* Calculate offset from start of region. */
	return convert_region_offset(-offset, region_offset);
}

static int calculate_region_offsets(void)
{
	int ret = 0;

	if (param.baseaddress_assigned)
		ret |= get_region_offset(param.baseaddress_input, &param.baseaddress);
	if (param.headeroffset_assigned)
		ret |= get_region_offset(param.headeroffset_input, &param.headeroffset);
	if (param.cbfsoffset_assigned)
		ret |= get_region_offset(param.cbfsoffset_input, &param.cbfsoffset);

	return ret;
}

static int dispatch_command(struct command command)
{
	if (command.accesses_region) {
		assert(param.image_file);

		if (partitioned_file_is_partitioned(param.image_file)) {
			INFO("Performing operation on '%s' region...\n",
					param.region_name);
		}
		if (!partitioned_file_read_region(param.image_region,
					param.image_file, param.region_name)) {
			ERROR("The image will be left unmodified.\n");
			return 1;
		}

		if (command.modifies_region) {
			// We (intentionally) don't support overwriting the FMAP
			// section. If you find yourself wanting to do this,
			// consider creating a new image rather than performing
			// whatever hacky transformation you were planning.
			if (region_is_flashmap(param.region_name)) {
				ERROR("Image region '%s' is read-only because it contains the FMAP.\n",
							param.region_name);
				ERROR("The image will be left unmodified.\n");
				return 1;
			}
			// We don't allow writing raw data to regions that
			// contain nested regions, since doing so would
			// overwrite all such subregions.
			if (partitioned_file_region_contains_nested(
					param.image_file, param.region_name)) {
				ERROR("Image region '%s' is read-only because it contains nested regions.\n",
							param.region_name);
				ERROR("The image will be left unmodified.\n");
				return 1;
			}
		}

		/*
		 * Once image region is read, input offsets can be adjusted accordingly if the
		 * inputs are provided as negative integers i.e. offsets from end of region.
		 */
		if (calculate_region_offsets())
			return 1;
	}

	if (command.function()) {
		if (partitioned_file_is_partitioned(param.image_file)) {
			ERROR("Failed while operating on '%s' region!\n",
							param.region_name);
			ERROR("The image will be left unmodified.\n");
		}
		return 1;
	}

	return 0;
}

static void usage(char *name)
{
	printf
	    ("cbfstool: Management utility for CBFS formatted ROM images\n\n"
	     "USAGE:\n" " %s [-h]\n"
	     " %s FILE COMMAND [-v] [PARAMETERS]...\n\n" "OPTIONs:\n"
	     "  -H header_offset Do not search for header; use this offset*\n"
	     "  -T               Output top-aligned memory address\n"
	     "  -u               Accept short data; fill upward/from bottom\n"
	     "  -d               Accept short data; fill downward/from top\n"
	     "  -F               Force action\n"
	     "  -g               Generate position and alignment arguments\n"
	     "  -U               Unprocessed; don't decompress or make ELF\n"
	     "  -v               Provide verbose output\n"
	     "  -h               Display this help message\n\n"
	     "  --ext-win-base   Base of extended decode window in host address\n"
	     "                   space(x86 only)\n"
	     "  --ext-win-size   Size of extended decode window in host address\n"
	     "                   space(x86 only)\n"
	     "COMMANDs:\n"
	     " add [-r image,regions] -f FILE -n NAME -t TYPE [-A hash] \\\n"
	     "        [-c compression] [-b base-address | -a alignment] \\\n"
	     "        [-p padding size] [-y|--xip if TYPE is FSP]       \\\n"
	     "        [-j topswap-size] (Intel CPUs only) [--ibb]       \\\n"
	     "        [--ext-win-base win-base --ext-win-size win-size]     "
			"Add a component\n"
	     "                                                         "
	     "    -j valid size: 0x10000 0x20000 0x40000 0x80000 0x100000 \n"
	     " add-payload [-r image,regions] -f FILE -n NAME [-A hash] \\\n"
	     "        [-c compression] [-b base-address] \\\n"
	     "        (linux specific: [-C cmdline] [-I initrd])           "
			"Add a payload to the ROM\n"
	     " add-stage [-r image,regions] -f FILE -n NAME [-A hash] \\\n"
	     "        [-c compression] [-b base] [-S section-to-ignore] \\\n"
	     "        [-a alignment] [-Q|--pow2page] \\\n"
	     "        [-y|--xip] [--ibb]                                \\\n"
	     "        [--ext-win-base win-base --ext-win-size win-size]     "
			"Add a stage to the ROM\n"
	     " add-flat-binary [-r image,regions] -f FILE -n NAME \\\n"
	     "        [-A hash] -l load-address -e entry-point \\\n"
	     "        [-c compression] [-b base]                           "
			"Add a 32bit flat mode binary\n"
	     " add-int [-r image,regions] -i INTEGER -n NAME [-b base]     "
			"Add a raw 64-bit integer value\n"
	     " add-master-header [-r image,regions] \\                   \n"
	     "        [-j topswap-size] (Intel CPUs only)                  "
			"Add a legacy CBFS master header\n"
	     " remove [-r image,regions] -n NAME                           "
			"Remove a component\n"
	     " compact -r image,regions                                    "
			"Defragment CBFS image.\n"
	     " copy -r image,regions -R source-region                      "
			"Create a copy (duplicate) cbfs instance in fmap\n"
	     " create -m ARCH -s size [-b bootblock offset] \\\n"
	     "        [-o CBFS offset] [-H header offset] [-B bootblock]   "
			"Create a legacy ROM file with CBFS master header*\n"
	     " create -M flashmap [-r list,of,regions,containing,cbfses]   "
			"Create a new-style partitioned firmware image\n"
	     " layout [-w]                                                 "
			"List mutable (or, with -w, readable) image regions\n"
	     " print [-r image,regions] [-k]                               "
			"Show the contents of the ROM\n"
	     " extract [-r image,regions] [-m ARCH] -n NAME -f FILE [-U]   "
			"Extracts a file from ROM\n"
	     " write [-F] -r image,regions -f file [-u | -d] [-i int]      "
			"Write file into same-size [or larger] raw region\n"
	     " read [-r fmap-region] -f file                               "
			"Extract raw region contents into binary file\n"
	     " truncate [-r fmap-region]                                   "
			"Truncate CBFS and print new size on stdout\n"
	     " expand [-r fmap-region]                                     "
			"Expand CBFS to span entire region\n"
	     "OFFSETs:\n"
	     "  Numbers accompanying -b, -H, and -o switches* may be provided\n"
	     "  in two possible formats: if their value is greater than\n"
	     "  0x80000000, they are interpreted as a top-aligned x86 memory\n"
	     "  address; otherwise, they are treated as an offset into flash.\n"
	     "ARCHes:\n", name, name
	    );
	print_supported_architectures();

	printf("TYPEs:\n");
	print_supported_filetypes();
	printf(
	     "\n* Note that these actions and switches are only valid when\n"
	     "  working with legacy images whose structure is described\n"
	     "  primarily by a CBFS master header. New-style images, in\n"
	     "  contrast, exclusively make use of an FMAP to describe their\n"
	     "  layout: this must minimally contain an '%s' section\n"
	     "  specifying the location of this FMAP itself and a '%s'\n"
	     "  section describing the primary CBFS. It should also be noted\n"
	     "  that, when working with such images, the -F and -r switches\n"
	     "  default to '%s' for convenience, and the -b switch becomes\n"
	     "  relative to the selected CBFS region's lowest address.\n"
	     "  The one exception to this rule is the top-aligned address,\n"
	     "  which is always relative to the end of the entire image\n"
	     "  rather than relative to the local region; this is true for\n"
	     "  for both input (sufficiently large) and output (-T) data.\n",
	     SECTION_NAME_FMAP, SECTION_NAME_PRIMARY_CBFS,
	     SECTION_NAME_PRIMARY_CBFS
	     );
}

static bool valid_opt(size_t i, int c)
{
	/* Check if it is one of the optstrings supported by the command. */
	if (strchr(commands[i].optstring, c))
		return true;

	/*
	 * Check if it is one of the non-ASCII characters. Currently, the
	 * non-ASCII characters are only checked against the valid list
	 * irrespective of the command.
	 */
	if (c >= LONGOPT_START && c < LONGOPT_END)
		return true;

	return false;
}

int main(int argc, char **argv)
{
	size_t i;
	int c;

	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}

	char *image_name = argv[1];
	char *cmd = argv[2];
	optind += 2;

	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(cmd, commands[i].name) != 0)
			continue;

		while (1) {
			char *suffix = NULL;
			int option_index = 0;

			c = getopt_long(argc, argv, commands[i].optstring,
						long_options, &option_index);
			if (c == -1) {
				if (optind < argc) {
					ERROR("%s: excessive argument -- '%s'"
						"\n", argv[0], argv[optind]);
					return 1;
				}
				break;
			}

			/* Filter out illegal long options */
			if (!valid_opt(i, c)) {
				ERROR("%s: invalid option -- '%d'\n",
				      argv[0], c);
				c = '?';
			}

			switch(c) {
			case 'n':
				param.name = optarg;
				break;
			case 't':
				if (intfiletype(optarg) != ((uint64_t) - 1))
					param.type = intfiletype(optarg);
				else
					param.type = strtoul(optarg, NULL, 0);
				if (param.type == 0)
					WARN("Unknown type '%s' ignored\n",
							optarg);
				break;
			case 'c': {
				if (strcmp(optarg, "precompression") == 0) {
					param.precompression = 1;
					break;
				}
				int algo = cbfs_parse_comp_algo(optarg);
				if (algo >= 0)
					param.compression = algo;
				else
					WARN("Unknown compression '%s' ignored.\n",
									optarg);
				break;
			}
			case 'A': {
				if (!vb2_lookup_hash_alg(optarg, &param.hash)) {
					ERROR("Unknown hash algorithm '%s'.\n",
						optarg);
					return 1;
				}
				break;
			}
			case 'M':
				param.fmap = optarg;
				break;
			case 'r':
				param.region_name = optarg;
				break;
			case 'R':
				param.source_region = optarg;
				break;
			case 'b':
				param.baseaddress_input = strtoll(optarg, &suffix, 0);
				if (!*optarg || (suffix && *suffix)) {
					ERROR("Invalid base address '%s'.\n",
						optarg);
					return 1;
				}
				// baseaddress may be zero on non-x86, so we
				// need an explicit "baseaddress_assigned".
				param.baseaddress_assigned = 1;
				break;
			case 'l':
				param.loadaddress = strtoul(optarg, &suffix, 0);
				if (!*optarg || (suffix && *suffix)) {
					ERROR("Invalid load address '%s'.\n",
						optarg);
					return 1;
				}
				break;
			case 'e':
				param.entrypoint = strtoul(optarg, &suffix, 0);
				if (!*optarg || (suffix && *suffix)) {
					ERROR("Invalid entry point '%s'.\n",
						optarg);
					return 1;
				}
				break;
			case 's':
				param.size = strtoul(optarg, &suffix, 0);
				if (!*optarg) {
					ERROR("Empty size specified.\n");
					return 1;
				}
				switch (tolower((int)suffix[0])) {
				case 'k':
					param.size *= 1024;
					break;
				case 'm':
					param.size *= 1024 * 1024;
					break;
				case '\0':
					break;
				default:
					ERROR("Invalid suffix for size '%s'.\n",
						optarg);
					return 1;
				}
				break;
			case 'B':
				param.bootblock = optarg;
				break;
			case 'H':
				param.headeroffset_input = strtoll(optarg, &suffix, 0);
				if (!*optarg || (suffix && *suffix)) {
					ERROR("Invalid header offset '%s'.\n",
						optarg);
					return 1;
				}
				param.headeroffset_assigned = 1;
				break;
			case 'a':
				param.alignment = strtoul(optarg, &suffix, 0);
				if (!*optarg || (suffix && *suffix)) {
					ERROR("Invalid alignment '%s'.\n",
						optarg);
					return 1;
				}
				break;
			case 'p':
				param.padding = strtoul(optarg, &suffix, 0);
				if (!*optarg || (suffix && *suffix)) {
					ERROR("Invalid pad size '%s'.\n",
						optarg);
					return 1;
				}
				break;
			case 'Q':
				param.force_pow2_pagesize = 1;
				break;
			case 'o':
				param.cbfsoffset_input = strtoll(optarg, &suffix, 0);
				if (!*optarg || (suffix && *suffix)) {
					ERROR("Invalid cbfs offset '%s'.\n",
						optarg);
					return 1;
				}
				param.cbfsoffset_assigned = 1;
				break;
			case 'f':
				param.filename = optarg;
				break;
			case 'F':
				param.force = 1;
				break;
			case 'i':
				param.u64val = strtoull(optarg, &suffix, 0);
				param.u64val_assigned = 1;
				if (!*optarg || (suffix && *suffix)) {
					ERROR("Invalid int parameter '%s'.\n",
						optarg);
					return 1;
				}
				break;
			case 'u':
				param.fill_partial_upward = true;
				break;
			case 'd':
				param.fill_partial_downward = true;
				break;
			case 'w':
				param.show_immutable = true;
				break;
			case 'j':
				param.topswap_size = strtol(optarg, NULL, 0);
				if (!is_valid_topswap())
					return 1;
				break;
			case 'q':
				param.ucode_region = optarg;
				break;
			case 'v':
				verbose++;
				break;
			case 'm':
				param.arch = string_to_arch(optarg);
				break;
			case 'I':
				param.initrd = optarg;
				break;
			case 'C':
				param.cmdline = optarg;
				break;
			case 'S':
				param.ignore_section = optarg;
				break;
			case 'y':
				param.stage_xip = true;
				break;
			case 'g':
				param.autogen_attr = true;
				break;
			case 'k':
				param.machine_parseable = true;
				break;
			case 'U':
				param.unprocessed = true;
				break;
			case LONGOPT_IBB:
				param.ibb = true;
				break;
			case LONGOPT_EXT_WIN_BASE:
				param.ext_win_base = strtoul(optarg, &suffix, 0);
				if (!*optarg || (suffix && *suffix)) {
					ERROR("Invalid ext window base '%s'.\n", optarg);
					return 1;
				}
				break;
			case LONGOPT_EXT_WIN_SIZE:
				param.ext_win_size = strtoul(optarg, &suffix, 0);
				if (!*optarg || (suffix && *suffix)) {
					ERROR("Invalid ext window size '%s'.\n", optarg);
					return 1;
				}
				break;
			case 'h':
			case '?':
				usage(argv[0]);
				return 1;
			default:
				break;
			}
		}

		if (commands[i].function == cbfs_create) {
			if (param.fmap) {
				struct buffer flashmap;
				if (buffer_from_file(&flashmap, param.fmap))
					return 1;
				param.image_file = partitioned_file_create(
							image_name, &flashmap);
				buffer_delete(&flashmap);
			} else if (param.size) {
				param.image_file = partitioned_file_create_flat(
							image_name, param.size);
			} else {
				ERROR("You need to specify a valid -M/--flashmap or -s/--size.\n");
				return 1;
			}
		} else {
			bool write_access = commands[i].modifies_region;

			param.image_file =
				partitioned_file_reopen(image_name,
							write_access);
		}
		if (!param.image_file)
			return 1;

		unsigned num_regions = 1;
		for (const char *list = strchr(param.region_name, ','); list;
						list = strchr(list + 1, ','))
			++num_regions;

		// If the action needs to read an image region, as indicated by
		// having accesses_region set in its command struct, that
		// region's buffer struct will be stored here and the client
		// will receive a pointer to it via param.image_region. It
		// need not write the buffer back to the image file itself,
		// since this behavior can be requested via its modifies_region
		// field. Additionally, it should never free the region buffer,
		// as that is performed automatically once it completes.
		struct buffer image_regions[num_regions];
		memset(image_regions, 0, sizeof(image_regions));

		bool seen_primary_cbfs = false;
		char region_name_scratch[strlen(param.region_name) + 1];
		strcpy(region_name_scratch, param.region_name);
		param.region_name = strtok(region_name_scratch, ",");
		for (unsigned region = 0; region < num_regions; ++region) {
			if (!param.region_name) {
				ERROR("Encountered illegal degenerate region name in -r list\n");
				ERROR("The image will be left unmodified.\n");
				partitioned_file_close(param.image_file);
				return 1;
			}

			if (strcmp(param.region_name, SECTION_NAME_PRIMARY_CBFS)
									== 0)
				seen_primary_cbfs = true;

			param.image_region = image_regions + region;
			if (dispatch_command(commands[i])) {
				partitioned_file_close(param.image_file);
				return 1;
			}

			param.region_name = strtok(NULL, ",");
		}

		if (commands[i].function == cbfs_create && !seen_primary_cbfs) {
			ERROR("The creation -r list must include the mandatory '%s' section.\n",
						SECTION_NAME_PRIMARY_CBFS);
			ERROR("The image will be left unmodified.\n");
			partitioned_file_close(param.image_file);
			return 1;
		}

		if (commands[i].modifies_region) {
			assert(param.image_file);
			for (unsigned region = 0; region < num_regions;
								++region) {

				if (!partitioned_file_write_region(
							param.image_file,
						image_regions + region)) {
					partitioned_file_close(
							param.image_file);
					return 1;
				}
			}
		}

		partitioned_file_close(param.image_file);
		return 0;
	}

	ERROR("Unknown command '%s'.\n", cmd);
	usage(argv[0]);
	return 1;
}
