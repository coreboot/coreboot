/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <amdblocks/apob_cache.h>
#include <assert.h>
#include <boot_device.h>
#include <bootstate.h>
#include <commonlib/helpers.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>
#include <fmap_config.h>
#include <spi_flash.h>
#include <stdint.h>
#include <string.h>
#include <thread.h>
#include <timestamp.h>
#include <xxhash.h>

#define DEFAULT_MRC_CACHE	"RW_MRC_CACHE"
#define DEFAULT_MRC_CACHE_SIZE	FMAP_SECTION_RW_MRC_CACHE_SIZE

#if CONFIG(SOC_AMD_COMMON_BLOCK_APOB_HASH)
#define MRC_HASH_SIZE		((uint32_t)sizeof(uint64_t))
#else
#define MRC_HASH_SIZE		0
#endif
#define MRC_HASH_OFFSET		(DEFAULT_MRC_CACHE_SIZE-MRC_HASH_SIZE)
#define MRC_HASH_UNINITIALIZED	0xffffffffull

#if !CONFIG_PSP_APOB_DRAM_ADDRESS
#error Incorrect APOB configuration setting(s)
#endif

_Static_assert(CONFIG_PSP_APOB_DRAM_SIZE == DEFAULT_MRC_CACHE_SIZE,
	"APOB DRAM reserved space != to MRC CACHE size - check your config");

#define APOB_SIGNATURE 0x424F5041	/* 'APOB' */

/* APOB_BASE_HEADER from AGESA */
struct apob_base_header {
	uint32_t   signature;			/* APOB signature */
	uint32_t   version;			/* Version */
	uint32_t   size;			/* APOB Size */
	uint32_t   offset_of_first_entry;	/* APOB Header Size */
};

static bool apob_header_valid(const struct apob_base_header *apob_header_ptr, const char *where)
{
	uint32_t size_plus_hash = apob_header_ptr->size + MRC_HASH_SIZE;

	if (apob_header_ptr->signature != APOB_SIGNATURE) {
		printk(BIOS_WARNING, "Invalid %s APOB signature %x\n",
			where, apob_header_ptr->signature);
		return false;
	}

	if (apob_header_ptr->size == 0 || size_plus_hash > DEFAULT_MRC_CACHE_SIZE) {
		printk(BIOS_WARNING, "%s APOB data is too large (%x + %x) > %x\n",
			where, apob_header_ptr->size, MRC_HASH_SIZE, DEFAULT_MRC_CACHE_SIZE);
		return false;
	}

	return true;
}

static void *get_apob_dram_address(void)
{
	/*
	 * TODO: Find the APOB destination by parsing the PSP's tables
	 * (once vboot is implemented).
	 */
	void *apob_src_ram = (void *)(uintptr_t)CONFIG_PSP_APOB_DRAM_ADDRESS;

	if (apob_header_valid(apob_src_ram, "RAM") == false)
		return NULL;

	return apob_src_ram;
}

static int get_nv_rdev(struct region_device *r)
{
	if  (fmap_locate_area_as_rdev(DEFAULT_MRC_CACHE, r) < 0) {
		printk(BIOS_ERR, "No APOB NV region is found in flash\n");
		return -1;
	}

	return 0;
}

static struct apob_thread_context {
	uint8_t buffer[DEFAULT_MRC_CACHE_SIZE] __attribute__((aligned(64)));
	struct thread_handle handle;
	struct region_device apob_rdev;
} global_apob_thread;

static enum cb_err apob_thread_entry(void *arg)
{
	ssize_t size;
	struct apob_thread_context *thread = arg;

	printk(BIOS_DEBUG, "APOB thread running\n");
	size = rdev_readat(&thread->apob_rdev, thread->buffer, 0,
		    region_device_sz(&thread->apob_rdev));

	printk(BIOS_DEBUG, "APOB thread done\n");

	if (size == region_device_sz(&thread->apob_rdev))
		return CB_SUCCESS;

	return CB_ERR;
}

void start_apob_cache_read(void)
{
	struct apob_thread_context *thread = &global_apob_thread;

	if (!CONFIG(COOP_MULTITASKING) || CONFIG(SOC_AMD_COMMON_BLOCK_APOB_HASH))
		return;

	/* We don't perform any comparison on S3 resume */
	if (acpi_is_wakeup_s3())
		return;

	if (get_nv_rdev(&thread->apob_rdev) != 0)
		return;

	assert(ARRAY_SIZE(thread->buffer) == region_device_sz(&thread->apob_rdev));

	printk(BIOS_DEBUG, "Starting APOB preload\n");
	if (thread_run(&thread->handle, apob_thread_entry, thread))
		printk(BIOS_ERR, "Failed to start APOB preload thread\n");
}

static void *get_apob_from_nv_rdev(struct region_device *read_rdev)
{
	struct apob_base_header apob_header;

	if (rdev_readat(read_rdev, &apob_header, 0, sizeof(apob_header)) < 0) {
		printk(BIOS_ERR, "Couldn't read APOB header!\n");
		return NULL;
	}

	if (apob_header_valid(&apob_header, "ROM") == false) {
		printk(BIOS_ERR, "No APOB NV data!\n");
		return NULL;
	}

	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));
	return rdev_mmap_full(read_rdev);
}

static uint64_t get_apob_hash_from_nv_rdev(const struct region_device *read_rdev)
{
	uint64_t hash;

	if (rdev_readat(read_rdev, &hash, MRC_HASH_OFFSET, MRC_HASH_SIZE) < 0) {
		printk(BIOS_ERR, "Couldn't read APOB hash!\n");
		return MRC_HASH_UNINITIALIZED;
	}

	return hash;
}

static void update_apob_nv_hash(uint64_t hash, struct region_device *write_rdev)
{
	if (rdev_writeat(write_rdev, &hash, MRC_HASH_OFFSET, MRC_HASH_SIZE) < 0) {
		printk(BIOS_ERR, "APOB hash flash region update failed\n");
	}
}

/* Save APOB buffer to flash */
static void soc_update_apob_cache(void *unused)
{
	struct apob_base_header *apob_rom = NULL;
	struct region_device read_rdev, write_rdev;
	bool update_needed = false;
	const struct apob_base_header *apob_src_ram;
	uint64_t ram_hash, nv_hash;

	/* Nothing to update in case of S3 resume. */
	if (acpi_is_wakeup_s3())
		return;

	apob_src_ram = get_apob_dram_address();
	if (apob_src_ram == NULL)
		return;

	if (get_nv_rdev(&read_rdev) != 0)
		return;

	timestamp_add_now(TS_AMD_APOB_READ_START);

	if (CONFIG(SOC_AMD_COMMON_BLOCK_APOB_HASH)) {
		nv_hash = get_apob_hash_from_nv_rdev(&read_rdev);
		ram_hash = xxh64(apob_src_ram, apob_src_ram->size, 0);

		if (nv_hash != ram_hash) {
			printk(BIOS_INFO, "APOB RAM hash differs from flash\n");
			update_needed = true;
		} else {
			printk(BIOS_DEBUG, "APOB hash matches flash\n");
			timestamp_add_now(TS_AMD_APOB_END);
			return;
		}
	}

	if (CONFIG(COOP_MULTITASKING) && thread_join(&global_apob_thread.handle) == CB_SUCCESS)
		apob_rom = (struct apob_base_header *)global_apob_thread.buffer;
	else if (!update_needed)
		apob_rom = get_apob_from_nv_rdev(&read_rdev);

	if (apob_rom == NULL) {
		update_needed = true;
	} else if (memcmp(apob_src_ram, apob_rom, apob_src_ram->size)) {
		printk(BIOS_INFO, "APOB RAM copy differs from flash\n");
		update_needed = true;
	} else
		printk(BIOS_DEBUG, "APOB valid copy is already in flash\n");

	if (!update_needed) {
		timestamp_add_now(TS_AMD_APOB_END);
		return;
	}

	printk(BIOS_SPEW, "Copy APOB from RAM %p/%#x to flash %#zx/%#zx\n",
		apob_src_ram, apob_src_ram->size,
		region_device_offset(&read_rdev), region_device_sz(&read_rdev));

	if  (fmap_locate_area_as_rdev_rw(DEFAULT_MRC_CACHE, &write_rdev) < 0) {
		printk(BIOS_ERR, "No RW APOB NV region is found in flash\n");
		return;
	}

	timestamp_add_now(TS_AMD_APOB_ERASE_START);

	/* write data to flash region */
	if (rdev_eraseat(&write_rdev, 0, DEFAULT_MRC_CACHE_SIZE) < 0) {
		printk(BIOS_ERR, "APOB flash region erase failed\n");
		return;
	}

	timestamp_add_now(TS_AMD_APOB_WRITE_START);

	if (rdev_writeat(&write_rdev, apob_src_ram, 0, apob_src_ram->size) < 0) {
		printk(BIOS_ERR, "APOB flash region update failed\n");
		return;
	}

	if (CONFIG(SOC_AMD_COMMON_BLOCK_APOB_HASH))
		update_apob_nv_hash(ram_hash, &write_rdev);

	timestamp_add_now(TS_AMD_APOB_END);

	printk(BIOS_INFO, "Updated APOB in flash\n");
}

static void *get_apob_nv_address(void)
{
	struct region_device rdev;

	if (get_nv_rdev(&rdev) != 0)
		return NULL;

	return get_apob_from_nv_rdev(&rdev);
}

void *soc_fill_apob_cache(void)
{
	/* If this is non-S3 boot, then use the APOB data placed by PSP in DRAM. */
	if (!acpi_is_wakeup_s3())
		return get_apob_dram_address();

	/*
	 * In case of S3 resume, PSP does not copy APOB data to DRAM. Thus, coreboot needs to
	 * provide the APOB NV data from RW_MRC_CACHE on SPI flash so that FSP can use it
	 * without having to traverse the BIOS directory table.
	 */
	return get_apob_nv_address();
}

/*
 * BS_POST_DEVICE was chosen because this gives start_apob_cache_read plenty of time to read
 * the APOB from SPI.
 */
BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, soc_update_apob_cache, NULL);
