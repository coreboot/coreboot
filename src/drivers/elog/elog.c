/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <boot_device.h>
#include <bootstate.h>
#include <cbmem.h>
#include <commonlib/bsd/bcd.h>
#include <commonlib/bsd/elog.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <elog.h>
#include <fmap.h>
#include <lib.h>
#include <post.h>
#include <rtc.h>
#include <smbios.h>
#include <stdint.h>
#include <string.h>
#include <timestamp.h>

#define ELOG_MIN_AVAILABLE_ENTRIES	2  /* Shrink when this many can't fit */
#define ELOG_SHRINK_PERCENTAGE		25 /* Percent of total area to remove */

#if CONFIG(ELOG_DEBUG)
#define elog_debug(STR...) printk(BIOS_DEBUG, STR)
#else
#define elog_debug(STR...)
#endif

#define NV_NEEDS_ERASE (~(size_t)0)
enum elog_init_state {
	ELOG_UNINITIALIZED = 0,
	ELOG_INITIALIZED,
	ELOG_BROKEN,
};

struct elog_state {
	u16 full_threshold;
	u16 shrink_size;

	/*
	 * The non-volatile storage chases the mirrored copy. When nv_last_write
	 * is less than the mirrored last write the non-volatile storage needs
	 * to be updated.
	 */
	size_t mirror_last_write;
	size_t nv_last_write;

	struct region_device nv_dev;
	/* Device that mirrors the eventlog in memory. */
	struct region_device mirror_dev;

	enum elog_init_state elog_initialized;
};

static struct elog_state elog_state;

#define ELOG_SIZE (4 * KiB)
static uint8_t elog_mirror_buf[ELOG_SIZE];

static inline struct region_device *mirror_dev_get(void)
{
	return &elog_state.mirror_dev;
}

static size_t elog_events_start(void)
{
	/* Events are added directly after the header. */
	return sizeof(struct elog_header);
}

static size_t elog_events_total_space(void)
{
	return region_device_sz(&elog_state.nv_dev) - elog_events_start();
}

static struct event_header *elog_get_event_buffer(size_t offset, size_t size)
{
	return rdev_mmap(mirror_dev_get(), offset, size);
}

static struct event_header *elog_get_next_event_buffer(size_t size)
{
	elog_debug("ELOG: new event at offset 0x%zx\n",
		   elog_state.mirror_last_write);
	return elog_get_event_buffer(elog_state.mirror_last_write, size);
}

static void elog_put_event_buffer(struct event_header *event)
{
	rdev_munmap(mirror_dev_get(), event);
}

static size_t elog_mirror_reset_last_write(void)
{
	/* Return previous write value. */
	size_t prev = elog_state.mirror_last_write;

	elog_state.mirror_last_write = 0;
	return prev;
}

static void elog_mirror_increment_last_write(size_t size)
{
	elog_state.mirror_last_write += size;
}

static void elog_nv_reset_last_write(void)
{
	elog_state.nv_last_write = 0;
}

static void elog_nv_increment_last_write(size_t size)
{
	elog_state.nv_last_write += size;
}

static void elog_nv_needs_possible_erase(void)
{
	/* If last write is 0 it means it is already erased. */
	if (elog_state.nv_last_write != 0)
		elog_state.nv_last_write = NV_NEEDS_ERASE;
}

static bool elog_should_shrink(void)
{
	return elog_state.mirror_last_write >= elog_state.full_threshold;
}

static bool elog_nv_needs_erase(void)
{
	return elog_state.nv_last_write == NV_NEEDS_ERASE;
}

static bool elog_nv_needs_update(void)
{
	return elog_state.nv_last_write != elog_state.mirror_last_write;
}

static size_t elog_nv_region_to_update(size_t *offset)
{
	*offset = elog_state.nv_last_write;
	return elog_state.mirror_last_write - elog_state.nv_last_write;
}

/*
 * When parsing state from the NV one needs to adjust both the NV and mirror
 * write state. Therefore, provide helper functions which adjust both
 * at the same time.
 */
static void elog_tandem_reset_last_write(void)
{
	elog_mirror_reset_last_write();
	elog_nv_reset_last_write();
}

static void elog_tandem_increment_last_write(size_t size)
{
	elog_mirror_increment_last_write(size);
	elog_nv_increment_last_write(size);
}

static void elog_debug_dump_buffer(const char *msg)
{
	struct region_device *rdev;
	void *buffer;

	if (!CONFIG(ELOG_DEBUG))
		return;

	elog_debug("%s", msg);

	rdev = mirror_dev_get();

	buffer = rdev_mmap_full(rdev);

	if (buffer == NULL)
		return;

	hexdump(buffer, region_device_sz(rdev));

	rdev_munmap(rdev, buffer);
}

/*
 * Check if mirrored buffer is filled with ELOG_TYPE_EOL byte from the
 * provided offset to the end of the mirrored buffer.
 */
static int elog_is_buffer_clear(size_t offset)
{
	size_t i;
	const struct region_device *rdev = mirror_dev_get();
	size_t size = region_device_sz(rdev) - offset;
	uint8_t *buffer = rdev_mmap(rdev, offset, size);
	int ret = 1;

	elog_debug("%s(offset=%zu size=%zu)\n", __func__, offset, size);

	if (buffer == NULL)
		return 0;

	for (i = 0; i < size; i++) {
		if (buffer[i] != ELOG_TYPE_EOL) {
			ret = 0;
			break;
		}
	}
	rdev_munmap(rdev, buffer);
	return ret;
}

/*
 * Verify if the mirrored elog structure is valid.
 * Returns 1 if the header is valid, 0 otherwise
 */
static int elog_is_header_valid(void)
{
	struct elog_header *header;

	elog_debug("%s()\n", __func__);

	header = rdev_mmap(mirror_dev_get(), 0, sizeof(*header));

	if (elog_verify_header(header) != CB_SUCCESS) {
		printk(BIOS_ERR, "ELOG: failed to verify header.\n");
		return 0;
	}
	return 1;
}

/*
 * Validate the event header and data.
 */
static size_t elog_is_event_valid(size_t offset)
{
	uint8_t checksum;
	struct event_header *event;
	uint8_t len;
	const size_t len_offset = offsetof(struct event_header, length);
	const size_t size = sizeof(len);

	/* Read and validate length. */
	if (rdev_readat(mirror_dev_get(), &len, offset + len_offset, size) < 0)
		return 0;

	/* Event length must be at least header size + checksum */
	if (len < (sizeof(*event) + sizeof(checksum)))
		return 0;

	if (len > ELOG_MAX_EVENT_SIZE)
		return 0;

	event = elog_get_event_buffer(offset, len);
	if (!event)
		return 0;

	/* If event checksum is invalid the area is corrupt */
	checksum = elog_checksum_event(event);
	elog_put_event_buffer(event);

	if (checksum != 0)
		return 0;

	/* Event is valid */
	return len;
}

/*
 * Write 'size' bytes of data from provided 'offset' in the mirrored elog to
 * the flash backing store. This will not erase the flash and it assumes the
 * flash area has been erased appropriately.
 */
static void elog_nv_write(size_t offset, size_t size)
{
	void *address;
	const struct region_device *rdev = mirror_dev_get();
	if (!size)
		return;

	address = rdev_mmap(rdev, offset, size);

	elog_debug("%s(address=%p offset=0x%08zx size=%zu)\n", __func__,
		 address, offset, size);

	if (address == NULL)
		return;

	/* Write the data to flash */
	if (rdev_writeat(&elog_state.nv_dev, address, offset, size) != size)
		printk(BIOS_ERR, "ELOG: NV Write failed at 0x%zx, size 0x%zx\n",
			offset, size);

	rdev_munmap(rdev, address);
}

/*
 * Erase the first block specified in the address.
 * Only handles flash area within a single flash block.
 */
static void elog_nv_erase(void)
{
	size_t size = region_device_sz(&elog_state.nv_dev);
	elog_debug("%s()\n", __func__);

	/* Erase the sectors in this region */
	if (rdev_eraseat(&elog_state.nv_dev, 0, size) != size)
		printk(BIOS_ERR, "ELOG: erase failure.\n");
}

/*
 * Scan the event area and validate each entry and update the ELOG state.
 */
static int elog_update_event_buffer_state(void)
{
	size_t offset = elog_events_start();

	elog_debug("%s()\n", __func__);

	/* Go through each event and validate it */
	while (1) {
		uint8_t type;
		const size_t type_offset = offsetof(struct event_header, type);
		size_t len;
		const size_t size = sizeof(type);

		if (rdev_readat(mirror_dev_get(), &type,
				offset + type_offset, size) < 0) {
			return -1;
		}

		/* The end of the event marker has been found */
		if (type == ELOG_TYPE_EOL)
			break;

		/* Validate the event */
		len = elog_is_event_valid(offset);

		if (!len) {
			printk(BIOS_ERR, "ELOG: Invalid event @ offset 0x%zx\n",
				offset);
			return -1;
		}

		/* Move to the next event */
		elog_tandem_increment_last_write(len);
		offset += len;
	}

	/* Ensure the remaining buffer is empty */
	if (!elog_is_buffer_clear(offset)) {
		printk(BIOS_ERR, "ELOG: buffer not cleared from 0x%zx\n",
			offset);
		return -1;
	}

	return 0;
}

static int elog_scan_flash(void)
{
	elog_debug("%s()\n", __func__);
	void *mirror_buffer;
	const struct region_device *rdev = mirror_dev_get();

	size_t size = region_device_sz(&elog_state.nv_dev);

	/* Fill memory buffer by reading from SPI */
	mirror_buffer = rdev_mmap_full(rdev);
	if (rdev_readat(&elog_state.nv_dev, mirror_buffer, 0, size) != size) {
		rdev_munmap(rdev, mirror_buffer);
		printk(BIOS_ERR, "ELOG: NV read failure.\n");
		return -1;
	}
	rdev_munmap(rdev, mirror_buffer);

	/* No writes have been done yet. */
	elog_tandem_reset_last_write();

	/* Check if the area is empty or not */
	if (elog_is_buffer_clear(0)) {
		printk(BIOS_ERR, "ELOG: NV Buffer Cleared.\n");
		return -1;
	}

	/* Indicate that header possibly written. */
	elog_tandem_increment_last_write(elog_events_start());

	/* Validate the header */
	if (!elog_is_header_valid()) {
		printk(BIOS_ERR, "ELOG: NV Buffer Invalid.\n");
		return -1;
	}

	return elog_update_event_buffer_state();
}

static void elog_write_header_in_mirror(void)
{
	static const struct elog_header header = {
		.magic = ELOG_SIGNATURE,
		.version = ELOG_VERSION,
		.header_size = sizeof(struct elog_header),
		.reserved = {
			[0] = ELOG_TYPE_EOL,
			[1] = ELOG_TYPE_EOL,
		},
	};

	rdev_writeat(mirror_dev_get(), &header, 0, sizeof(header));
	elog_mirror_increment_last_write(elog_events_start());
}

static void elog_move_events_to_front(size_t offset, size_t size)
{
	void *src;
	void *dest;
	size_t start_offset = elog_events_start();
	const struct region_device *rdev = mirror_dev_get();

	src = rdev_mmap(rdev, offset, size);
	dest = rdev_mmap(rdev, start_offset, size);

	if (src == NULL || dest == NULL) {
		printk(BIOS_ERR, "ELOG: failure moving events!\n");
		rdev_munmap(rdev, dest);
		rdev_munmap(rdev, src);
		return;
	}

	/* Move the events to the front. */
	memmove(dest, src, size);
	rdev_munmap(rdev, dest);
	rdev_munmap(rdev, src);

	/* Mark EOL for previously used buffer until the end. */
	offset = start_offset + size;
	size = region_device_sz(rdev) - offset;
	dest = rdev_mmap(rdev, offset, size);
	if (dest == NULL) {
		printk(BIOS_ERR, "ELOG: failure filling EOL!\n");
		return;
	}
	memset(dest, ELOG_TYPE_EOL, size);
	rdev_munmap(rdev, dest);
}

/* Perform the shrink and move events returning the size of bytes shrunk. */
static size_t elog_do_shrink(size_t requested_size, size_t last_write)
{
	const struct region_device *rdev = mirror_dev_get();
	size_t offset = elog_events_start();
	size_t remaining_size;

	while (1) {
		const size_t type_offset = offsetof(struct event_header, type);
		const size_t len_offset = offsetof(struct event_header, length);
		const size_t size = sizeof(uint8_t);
		uint8_t type;
		uint8_t len;

		/* Next event has exceeded constraints */
		if (offset > requested_size)
			break;

		if (rdev_readat(rdev, &type, offset + type_offset, size) < 0)
			break;

		/* Reached the end of the area */
		if (type == ELOG_TYPE_EOL)
			break;

		if (rdev_readat(rdev, &len, offset + len_offset, size) < 0)
			break;

		offset += len;
	}

	/*
	 * Move the events and update the last write. The last write before
	 * shrinking was captured prior to resetting the counter to determine
	 * actual size we're keeping.
	 */
	remaining_size = last_write - offset;
	elog_debug("ELOG: shrinking offset: 0x%zx remaining_size: 0x%zx\n",
		offset, remaining_size);
	elog_move_events_to_front(offset, remaining_size);
	elog_mirror_increment_last_write(remaining_size);

	/* Return the amount of data removed. */
	return offset - elog_events_start();
}

/*
 * Shrink the log, deleting old entries and moving the
 * remaining ones to the front of the log.
 */
static int elog_shrink_by_size(size_t requested_size)
{
	size_t shrunk_size;
	size_t captured_last_write;
	size_t total_event_space = elog_events_total_space();

	elog_debug("%s()\n", __func__);

	/* Indicate possible erase required. */
	elog_nv_needs_possible_erase();

	/* Capture the last write to determine data size in buffer to shrink. */
	captured_last_write = elog_mirror_reset_last_write();

	/* Prepare new header. */
	elog_write_header_in_mirror();

	/* Determine if any actual shrinking is required. */
	if (requested_size >= total_event_space)
		shrunk_size = total_event_space;
	else
		shrunk_size = elog_do_shrink(requested_size,
						captured_last_write);

	/* Add clear event */
	return elog_add_event_word(ELOG_TYPE_LOG_CLEAR, shrunk_size);
}

static int elog_prepare_empty(void)
{
	elog_debug("%s()\n", __func__);
	return elog_shrink_by_size(elog_events_total_space());
}

static int elog_shrink(void)
{
	if (elog_should_shrink())
		return elog_shrink_by_size(elog_state.shrink_size);
	return 0;
}

/*
 * Convert a flash offset into a memory mapped flash address
 */
static inline u8 *elog_flash_offset_to_address(void)
{
	/* Only support memory-mapped devices. */
	if (!CONFIG(BOOT_DEVICE_MEMORY_MAPPED))
		return NULL;

	if (!region_device_sz(&elog_state.nv_dev))
		return NULL;

	/* Get a view into the read-only boot device. */
	return rdev_mmap(boot_device_ro(),
			 region_device_offset(&elog_state.nv_dev),
			 region_device_sz(&elog_state.nv_dev));
}

/*
 * Fill out SMBIOS Type 15 table entry so the
 * event log can be discovered at runtime.
 */
int elog_smbios_write_type15(unsigned long *current, int handle)
{
	uintptr_t log_address;

	size_t elog_size = region_device_sz(&elog_state.nv_dev);

	if (CONFIG(ELOG_CBMEM)) {
		/* Save event log buffer into CBMEM for the OS to read */
		void *cbmem = cbmem_add(CBMEM_ID_ELOG, elog_size);
		if (cbmem)
			rdev_readat(mirror_dev_get(), cbmem, 0, elog_size);
		log_address = (uintptr_t)cbmem;
	} else {
		log_address = (uintptr_t)elog_flash_offset_to_address();
	}

	if (!log_address) {
		printk(BIOS_WARNING, "SMBIOS type 15 log address invalid.\n");
		return 0;
	}

	struct smbios_type15 *t = smbios_carve_table(*current, SMBIOS_EVENT_LOG,
						     sizeof(*t), handle);

	t->area_length = elog_size - 1;
	t->header_offset = 0;
	t->data_offset = sizeof(struct elog_header);
	t->access_method = SMBIOS_EVENTLOG_ACCESS_METHOD_MMIO32;
	t->log_status = SMBIOS_EVENTLOG_STATUS_VALID;
	t->change_token = 0;
	t->address = log_address;
	t->header_format = ELOG_HEADER_TYPE_OEM;
	t->log_type_descriptors = 0;
	t->log_type_descriptor_length = 2;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

/*
 * Clear the entire event log
 */
int elog_clear(void)
{
	elog_debug("%s()\n", __func__);

	/* Make sure ELOG structures are initialized */
	if (elog_init() < 0)
		return -1;

	return elog_prepare_empty();
}

static int elog_find_flash(void)
{
	size_t total_size;
	size_t reserved_space = ELOG_MIN_AVAILABLE_ENTRIES * ELOG_MAX_EVENT_SIZE;
	struct region_device *rdev = &elog_state.nv_dev;

	elog_debug("%s()\n", __func__);

	/* Find the ELOG base and size in FMAP */
	if (fmap_locate_area_as_rdev_rw(ELOG_RW_REGION_NAME, rdev) < 0) {
		printk(BIOS_WARNING, "ELOG: Unable to find RW_ELOG in FMAP\n");
		return -1;
	}

	if (region_device_sz(rdev) < ELOG_SIZE) {
		printk(BIOS_WARNING, "ELOG: Needs a minimum size of %dKiB: %zu\n",
			ELOG_SIZE / KiB, region_device_sz(rdev));
		return -1;
	}

	printk(BIOS_INFO, "ELOG: NV offset 0x%zx size 0x%zx\n",
		region_device_offset(rdev), region_device_sz(rdev));

	/* Keep 4KiB max size until large malloc()s have been fixed. */
	total_size = MIN(ELOG_SIZE, region_device_sz(rdev));
	rdev_chain(rdev, rdev, 0, total_size);

	elog_state.full_threshold = total_size - reserved_space;
	elog_state.shrink_size = total_size * ELOG_SHRINK_PERCENTAGE / 100;

	if (reserved_space > elog_state.shrink_size) {
		printk(BIOS_ERR, "ELOG: SHRINK_PERCENTAGE too small\n");
		return -1;
	}

	return 0;
}

static int elog_sync_to_nv(void)
{
	size_t offset;
	size_t size;
	bool erase_needed;
	/* Determine if any updates are required. */
	if (!elog_nv_needs_update())
		return 0;

	erase_needed = elog_nv_needs_erase();

	/* Erase if necessary. */
	if (erase_needed) {
		elog_nv_erase();
		elog_nv_reset_last_write();
	}

	size = elog_nv_region_to_update(&offset);

	elog_nv_write(offset, size);
	elog_nv_increment_last_write(size);

	/*
	 * If erase wasn't performed then don't rescan. Assume the appended
	 * write was successful.
	 */
	if (!erase_needed)
		return 0;

	elog_debug_dump_buffer("ELOG: in-memory mirror:\n");

	/* Mark broken if the scan failed after a sync. */
	if (elog_scan_flash() < 0) {
		printk(BIOS_ERR, "ELOG: Sync back from NV storage failed.\n");
		elog_debug_dump_buffer("ELOG: Buffer from NV:\n");
		elog_state.elog_initialized = ELOG_BROKEN;
		return -1;
	}

	return 0;
}

/*
 * Do not log boot count events in S3 resume or SMM.
 */
static bool elog_do_add_boot_count(void)
{
	if (ENV_SMM)
		return false;

	return !acpi_is_wakeup_s3();
}

/* Check and log POST codes from previous boot */
static void log_last_boot_post(void)
{
#if ENV_X86
	u8 code;
	u32 extra;

	if (!CONFIG(CMOS_POST))
		return;

	if (cmos_post_previous_boot(&code, &extra) == 0)
		return;

	printk(BIOS_WARNING, "POST: Unexpected post code/extra "
	       "in previous boot: 0x%02x/0x%04x\n", code, extra);

	elog_add_event_word(ELOG_TYPE_LAST_POST_CODE, code);
	if (extra)
		elog_add_event_dword(ELOG_TYPE_POST_EXTRA, extra);
#endif
}

static void elog_add_boot_count(void)
{
	if (elog_do_add_boot_count()) {
		elog_add_event_dword(ELOG_TYPE_BOOT, boot_count_read());

		log_last_boot_post();
	}
}

/*
 * Event log main entry point
 */
int elog_init(void)
{
	void *mirror_buffer;
	size_t elog_size;
	switch (elog_state.elog_initialized) {
	case ELOG_UNINITIALIZED:
		break;
	case ELOG_INITIALIZED:
		return 0;
	case ELOG_BROKEN:
		return -1;
	}
	elog_state.elog_initialized = ELOG_BROKEN;

	if (!ENV_SMM)
		timestamp_add_now(TS_ELOG_INIT_START);

	elog_debug("%s()\n", __func__);

	/* Set up the backing store */
	if (elog_find_flash() < 0)
		return -1;

	elog_size = region_device_sz(&elog_state.nv_dev);
	mirror_buffer = elog_mirror_buf;
	if (!mirror_buffer) {
		printk(BIOS_ERR, "ELOG: Unable to allocate backing store\n");
		return -1;
	}
	rdev_chain_mem_rw(&elog_state.mirror_dev, mirror_buffer, elog_size);

	/*
	 * Mark as initialized to allow elog_init() to be called and deemed
	 * successful in the prepare/shrink path which adds events.
	 */
	elog_state.elog_initialized = ELOG_INITIALIZED;

	/* Load the log from flash and prepare the flash if necessary. */
	if (elog_scan_flash() < 0 && elog_prepare_empty() < 0) {
		printk(BIOS_ERR, "ELOG: Unable to prepare flash\n");
		return -1;
	}

	printk(BIOS_INFO, "ELOG: area is %zu bytes, full threshold %d,"
	       " shrink size %d\n", region_device_sz(&elog_state.nv_dev),
	       elog_state.full_threshold, elog_state.shrink_size);

	if (ENV_PAYLOAD_LOADER)
		elog_add_boot_count();

	if (!ENV_SMM)
		timestamp_add_now(TS_ELOG_INIT_END);

	return 0;
}

/*
 * Add an event to the log
 */
int elog_add_event_raw(u8 event_type, void *data, u8 data_size)
{
	struct event_header *event;
	struct rtc_time time = { 0 };
	u8 event_size;

	elog_debug("%s(type=%X)\n", __func__, event_type);

	/* Make sure ELOG structures are initialized */
	if (elog_init() < 0)
		return -1;

	/* Header + Data + Checksum */
	event_size = sizeof(*event) + data_size + 1;
	if (event_size > ELOG_MAX_EVENT_SIZE) {
		printk(BIOS_ERR, "ELOG: Event(%X) data size too "
		       "big (%d)\n", event_type, event_size);
		return -1;
	}

	/* Make sure event data can fit */
	event = elog_get_next_event_buffer(event_size);
	if (event == NULL) {
		printk(BIOS_ERR, "ELOG: Event(%X) does not fit\n",
		       event_type);
		return -1;
	}

	/* Fill out event data */
	event->type = event_type;
	event->length = event_size;
	if (CONFIG(RTC))
		rtc_get(&time);

	elog_fill_timestamp(event, time.sec, time.min, time.hour,
			    time.mday, time.mon, time.year);

	if (data_size)
		memcpy(&event[1], data, data_size);

	/* Zero the checksum byte and then compute checksum */
	elog_update_checksum(event, 0);
	elog_update_checksum(event, -(elog_checksum_event(event)));
	elog_put_event_buffer(event);

	elog_mirror_increment_last_write(event_size);

	printk(BIOS_INFO, "ELOG: Event(%X) added with size %d ",
	       event_type, event_size);
	if (event->day != 0) {
		printk(BIOS_INFO, "at 20%02x-%02x-%02x %02x:%02x:%02x UTC\n",
		       event->year, event->month, event->day,
		       event->hour, event->minute, event->second);
	} else {
		printk(BIOS_INFO, "(timestamp unavailable)\n");
	}

	/* Shrink the log if we are getting too full */
	if (elog_shrink() < 0)
		return -1;

	/* Ensure the updates hit the non-volatile storage. */
	return elog_sync_to_nv();
}

int elog_add_event(u8 event_type)
{
	return elog_add_event_raw(event_type, NULL, 0);
}

int elog_add_event_byte(u8 event_type, u8 data)
{
	return elog_add_event_raw(event_type, &data, sizeof(data));
}

int elog_add_event_word(u8 event_type, u16 data)
{
	return elog_add_event_raw(event_type, &data, sizeof(data));
}

int elog_add_event_dword(u8 event_type, u32 data)
{
	return elog_add_event_raw(event_type, &data, sizeof(data));
}

int elog_add_event_wake(u8 source, u32 instance)
{
	struct elog_event_data_wake wake = {
		.source = source,
		.instance = instance
	};
	return elog_add_event_raw(ELOG_TYPE_WAKE_SOURCE, &wake, sizeof(wake));
}

int elog_add_extended_event(u8 type, u32 complement)
{
	struct elog_event_extended_event event = {
		.event_type = type,
		.event_complement = complement
	};
	return elog_add_event_raw(ELOG_TYPE_EXTENDED_EVENT,
				  &event,
				  sizeof(event));
}

/* Make sure elog_init() runs at least once to log System Boot event. */
static void elog_bs_init(void *unused) { elog_init(); }
BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, elog_bs_init, NULL);
