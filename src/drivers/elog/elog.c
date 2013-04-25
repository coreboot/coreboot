/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <pc80/mc146818rtc.h>
#include <smbios.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <stdint.h>
#include <string.h>
#include <elog.h>
#include "elog_internal.h"

#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/fmap.h>
#elif CONFIG_ELOG_FLASH_BASE == 0
#error "CONFIG_ELOG_FLASH_BASE is invalid"
#endif
#if CONFIG_ELOG_FULL_THRESHOLD >= CONFIG_ELOG_AREA_SIZE
#error "CONFIG_ELOG_FULL_THRESHOLD is larger than CONFIG_ELOG_AREA_SIZE"
#endif
#if (CONFIG_ELOG_AREA_SIZE - CONFIG_ELOG_FULL_THRESHOLD) < (MAX_EVENT_SIZE + 1)
#error "CONFIG_ELOG_FULL_THRESHOLD is too small"
#endif
#if CONFIG_ELOG_SHRINK_SIZE >= CONFIG_ELOG_AREA_SIZE
#error "CONFIG_ELOG_SHRINK_SIZE is larger than CONFIG_ELOG_AREA_SIZE"
#endif
#if (CONFIG_ELOG_AREA_SIZE - CONFIG_ELOG_SHRINK_SIZE) > \
	CONFIG_ELOG_FULL_THRESHOLD
#error "CONFIG_ELOG_SHRINK_SIZE is too large"
#endif

#if CONFIG_ELOG_DEBUG
#define elog_debug(STR...) printk(BIOS_DEBUG, STR)
#else
#define elog_debug(STR...)
#endif

/*
 * Static variables for ELOG state
 */
static int elog_initialized;
static struct spi_flash *elog_spi;
static struct elog_descriptor elog_flash_area;

static inline struct elog_descriptor* elog_get_flash(void)
{
	return &elog_flash_area;
}

/*
 * Convert a memory mapped flash address into a flash offset
 */
static inline u32 elog_flash_address_to_offset(u8 *address)
{
	if (!elog_spi)
		return 0;
	return (u32)address - ((u32)~0UL - elog_spi->size + 1);
}

/*
 * Convert a flash offset into a memory mapped flash address
 */
static inline u8* elog_flash_offset_to_address(u32 offset)
{
	if (!elog_spi)
		return NULL;
	return (u8*)((u32)~0UL - elog_spi->size + 1 + offset);
}

/*
 * The ELOG header is at the very beginning of the area
 */
static inline struct elog_header*
elog_get_header(struct elog_descriptor *elog)
{
	return elog->backing_store;
}

/*
 * Pointer to an event log header in the event data area
 */
static inline struct event_header*
elog_get_event_base(struct elog_descriptor *elog, u32 offset)
{
	return (struct event_header *)&elog->data[offset];
}

/*
 * Pointer to where the next event should be stored
 */
static inline struct event_header*
elog_get_next_event_base(struct elog_descriptor *elog)
{
	return elog_get_event_base(elog, elog->next_event_offset);
}

/*
 * Update the checksum at the last byte
 */
static void elog_update_checksum(struct event_header *event, u8 checksum)
{
	u8 *event_data = (u8*)event;
	event_data[event->length - 1] = checksum;
}

/*
 * Simple byte checksum for events
 */
static u8 elog_checksum_event(struct event_header *event)
{
	u8 index, checksum = 0;
	u8 *data = (u8*)event;

	for (index = 0; index < event->length; index++)
		checksum += data[index];
	return checksum;
}

/*
 * Check if a raw buffer is filled with ELOG_TYPE_EOL byte
 */
static int elog_is_buffer_clear(u8 *base, u32 size)
{
	u8 *current = base;
	u8 *end = current + size;

	elog_debug("elog_is_buffer_clear(base=0x%p size=%u)\n", base, size);

	for (; current != end; current++) {
		if (*current != ELOG_TYPE_EOL)
			return 0;
	}
	return 1;
}

/*
 * Verify whether ELOG area is filled with ELOG_TYPE_EOL byte
 */
static int elog_is_area_clear(struct elog_descriptor *elog)
{
	return elog_is_buffer_clear(elog->backing_store, elog->total_size);
}

/*
 * Check that the ELOG area has been initialized and is valid.
 */
static int elog_is_area_valid(struct elog_descriptor *elog)
{
	elog_debug("elog_is_area_valid()\n");

	if (elog->area_state != ELOG_AREA_HAS_CONTENT)
		return 0;
	if (elog->header_state != ELOG_HEADER_VALID)
		return 0;
	if (elog->event_buffer_state != ELOG_EVENT_BUFFER_OK)
		return 0;
	return 1;
}

/*
 * Verify the contents of an ELOG Header structure
 * Returns 1 if the header is valid, 0 otherwise
 */
static int elog_is_header_valid(struct elog_header *header)
{
	elog_debug("elog_is_header_valid()\n");

	if (header->magic != ELOG_SIGNATURE) {
		printk(BIOS_ERR, "ELOG: header magic 0x%X != 0x%X\n",
		       header->magic, ELOG_SIGNATURE);
		return 0;
	}
	if (header->version != ELOG_VERSION) {
		printk(BIOS_ERR, "ELOG: header version %u != %u\n",
		       header->version, ELOG_VERSION);
		return 0;
	}
	if (header->header_size != sizeof(*header)) {
		printk(BIOS_ERR, "ELOG: header size mismatch %u != %zu\n",
		       header->header_size, sizeof(*header));
		return 0;
	}
	return 1;
}

/*
 * Validate the event header and data.
 */
static int elog_is_event_valid(struct elog_descriptor *elog, u32 offset)
{
	struct event_header *event;

	event = elog_get_event_base(elog, offset);
	if (!event)
		return 0;

	/* Validate event length */
	if ((offsetof(struct event_header, type) +
	     sizeof(event->type) - 1 + offset) >= elog->data_size)
		return 0;

	/* End of event marker has been found */
	if (event->type == ELOG_TYPE_EOL)
		return 0;

	/* Check if event fits in area */
	if ((offsetof(struct event_header, length) +
	     sizeof(event->length) - 1 + offset) >= elog->data_size)
		return 0;

	/*
	 * If the current event length + the current offset exceeds
	 * the area size then the event area is corrupt.
	 */
	if ((event->length + offset) >= elog->data_size)
		return 0;

	/* Event length must be at least header size + checksum */
	if (event->length < (sizeof(*event) + 1))
		return 0;

	/* If event checksum is invalid the area is corrupt */
	if (elog_checksum_event(event) != 0)
		return 0;

	/* Event is valid */
	return 1;
}

/*
 * Write 'size' bytes of data pointed to by 'address' in the flash backing
 * store into flash. This will not erase the flash and it assumes the flash
 * area has been erased appropriately.
 */
static void elog_flash_write(u8 *address, u32 size)
{
	struct elog_descriptor *flash = elog_get_flash();
	u32 offset;

	if (!address || !size || !elog_spi)
		return;

	offset = flash->flash_base;
	offset += address - (u8*)flash->backing_store;

	elog_debug("elog_flash_write(address=0x%p offset=0x%08x size=%u)\n",
		   address, offset, size);

	/* Write the data to flash */
	elog_spi->write(elog_spi, offset, size, address);
}

/*
 * Erase the first block specified in the address.
 * Only handles flash area within a single flash block.
 */
static void elog_flash_erase(u8 *address, u32 size)
{
	struct elog_descriptor *flash = elog_get_flash();
	u32 offset;

	if (!address || !size || !elog_spi)
		return;

	offset = flash->flash_base;
	offset += address - (u8*)flash->backing_store;

	elog_debug("elog_flash_erase(address=0x%p offset=0x%08x size=%u)\n",
		   address, offset, size);

	/* Erase the sectors in this region */
	elog_spi->erase(elog_spi, offset, size);
}

/*
 * Scan the event area and validate each entry and
 * update the ELOG descriptor state.
 */
static void elog_update_event_buffer_state(struct elog_descriptor *elog)
{
	u32 count = 0;
	u32 offset = 0;
	struct event_header *event;

	elog_debug("elog_update_event_buffer_state()\n");

	/* Go through each event and validate it */
	while (1) {
		event = elog_get_event_base(elog, offset);

		/* Do not de-reference anything past the area length */
		if ((offsetof(struct event_header, type) +
		     sizeof(event->type) - 1 + offset) >= elog->data_size) {
			elog->event_buffer_state = ELOG_EVENT_BUFFER_CORRUPTED;
			break;
		}

		/* The end of the event marker has been found */
		if (event->type == ELOG_TYPE_EOL)
			break;

		/* Validate the event */
		if (!elog_is_event_valid(elog, offset)) {
			elog->event_buffer_state = ELOG_EVENT_BUFFER_CORRUPTED;
			break;
		}

		/* Move to the next event */
		count++;
		offset += event->length;
	}

	/* Ensure the remaining buffer is empty */
	if (!elog_is_buffer_clear(&elog->data[offset],
				  elog->data_size - offset))
		elog->event_buffer_state = ELOG_EVENT_BUFFER_CORRUPTED;

	/* Update data into elog descriptor */
	elog->event_count = count;
	elog->next_event_offset = offset;
}

/*
 * (Re)initialize a new ELOG descriptor
 */
static void elog_scan_flash(struct elog_descriptor *elog)
{
	elog_debug("elog_scan_flash()\n");

	elog->area_state = ELOG_AREA_UNDEFINED;
	elog->header_state = ELOG_HEADER_INVALID;
	elog->event_buffer_state = ELOG_EVENT_BUFFER_OK;

	/* Fill memory buffer by reading from SPI */
	elog_spi->read(elog_spi, elog->flash_base, elog->total_size,
		       elog->backing_store);

	elog->next_event_offset = 0;
	elog->event_count = 0;

	/* Check if the area is empty or not */
	if (elog_is_area_clear(elog)) {
		elog->area_state = ELOG_AREA_EMPTY;
		return;
	}

	elog->area_state = ELOG_AREA_HAS_CONTENT;

	/* Validate the header */
	if (!elog_is_header_valid(elog_get_header(elog))) {
		elog->header_state = ELOG_HEADER_INVALID;
		return;
	}

	elog->header_state = ELOG_HEADER_VALID;
	elog_update_event_buffer_state(elog);
}

static void elog_prepare_empty(struct elog_descriptor *elog)
{
	struct elog_header *header;

	elog_debug("elog_prepare_empty(%u bytes)\n", data_size);

	/* Write out the header */
	header = elog_get_header(elog);
	header->magic = ELOG_SIGNATURE;
	header->version = ELOG_VERSION;
	header->header_size = sizeof(struct elog_header);
	header->reserved[0] = ELOG_TYPE_EOL;
	header->reserved[1] = ELOG_TYPE_EOL;
	elog_flash_write(elog->backing_store, header->header_size);

	elog_scan_flash(elog);
}

/*
 * Shrink the log, deleting old entries and moving the
 * remaining ones to the front of the log.
 */
static int elog_shrink(void)
{
	struct elog_descriptor *flash = elog_get_flash();
	struct event_header *event;
	u16 discard_count = 0;
	u16 offset = 0;
	u16 new_size = 0;

	elog_debug("elog_shrink()\n");

	if (flash->next_event_offset < CONFIG_ELOG_SHRINK_SIZE)
		return 0;

	while (1) {
		/* Next event has exceeded constraints */
		if (offset > CONFIG_ELOG_SHRINK_SIZE)
			break;

		event = elog_get_event_base(flash, offset);

		/* Reached the end of the area */
		if (!event || event->type == ELOG_TYPE_EOL)
			break;

		offset += event->length;
		discard_count++;
	}

	new_size = flash->next_event_offset - offset;
	memmove(&flash->data[0], &flash->data[offset], new_size);
	memset(&flash->data[new_size], ELOG_TYPE_EOL,
	       flash->data_size - new_size);

	elog_flash_erase(flash->backing_store, flash->total_size);
	elog_flash_write(flash->backing_store, flash->total_size);
	elog_scan_flash(flash);

	/* Ensure the area was successfully erased */
	if (flash->next_event_offset >= CONFIG_ELOG_FULL_THRESHOLD) {
		printk(BIOS_ERR, "ELOG: Flash area was not erased!\n");
		return -1;
	}

	/* Add clear event */
	elog_add_event_word(ELOG_TYPE_LOG_CLEAR, offset);

	return 0;
}

#ifndef __SMM__
/*
 * Fill out SMBIOS Type 15 table entry so the
 * event log can be discovered at runtime.
 */
int elog_smbios_write_type15(unsigned long *current, int handle)
{
	struct elog_descriptor *flash = elog_get_flash();
	struct smbios_type15 *t = (struct smbios_type15 *)*current;
	int len = sizeof(struct smbios_type15);

#if CONFIG_ELOG_CBMEM
	/* Save event log buffer into CBMEM for the OS to read */
	void *cbmem = cbmem_add(CBMEM_ID_ELOG, flash->total_size);
	if (!cbmem)
		return 0;
	memcpy(cbmem, flash->backing_store, flash->total_size);
#endif

	memset(t, 0, len);
	t->type = SMBIOS_EVENT_LOG;
	t->length = len - 2;
	t->handle = handle;
	t->area_length = flash->total_size - 1;
	t->header_offset = 0;
	t->data_offset = sizeof(struct elog_header);
	t->access_method = SMBIOS_EVENTLOG_ACCESS_METHOD_MMIO32;
	t->log_status = SMBIOS_EVENTLOG_STATUS_VALID;
	t->change_token = 0;
#if CONFIG_ELOG_CBMEM
	t->address = (u32)cbmem;
#else
	t->address = (u32)elog_flash_offset_to_address(flash->flash_base);
#endif
	t->header_format = ELOG_HEADER_TYPE_OEM;
	t->log_type_descriptors = 0;
	t->log_type_descriptor_length = 2;

	*current += len;
	return len;
}
#endif

/*
 * Clear the entire event log
 */
int elog_clear(void)
{
	struct elog_descriptor *flash = elog_get_flash();

	elog_debug("elog_clear()\n");

	/* Make sure ELOG structures are initialized */
	if (elog_init() < 0)
		return -1;

	/* Erase flash area */
	elog_flash_erase(flash->backing_store, flash->total_size);
	elog_prepare_empty(flash);

	if (!elog_is_area_valid(flash))
		return -1;

	/* Log the clear event */
	elog_add_event_word(ELOG_TYPE_LOG_CLEAR, flash->total_size);

	return 0;
}

static void elog_find_flash(u32 *base, int *size)
{
#if CONFIG_CHROMEOS
	u8 *flash_base_ptr;
#endif

	elog_debug("elog_find_flash(base = %p, size = %p)\n", base, size);

#if CONFIG_CHROMEOS
	/* Find the ELOG base and size in FMAP */
	*size = find_fmap_entry("RW_ELOG", (void **)&flash_base_ptr);
	if (*size < 0) {
		printk(BIOS_WARNING, "ELOG: Unable to find RW_ELOG in FMAP, "
		       "using CONFIG_ELOG_FLASH_BASE instead\n");
		*size = CONFIG_ELOG_AREA_SIZE;
	} else {
		*base = elog_flash_address_to_offset(flash_base_ptr);

		/* Use configured size if smaller than FMAP size */
		if (*size > CONFIG_ELOG_AREA_SIZE)
			*size = CONFIG_ELOG_AREA_SIZE;
	}
#else
	*base = CONFIG_ELOG_FLASH_BASE;
	*size = CONFIG_ELOG_AREA_SIZE;
#endif
}

/*
 * Event log main entry point
 */
int elog_init(void)
{
	struct elog_descriptor *flash = elog_get_flash();
	u32 flash_base = CONFIG_ELOG_FLASH_BASE;
	int flash_size = CONFIG_ELOG_AREA_SIZE;

	if (elog_initialized)
		return 0;

	elog_debug("elog_init()\n");

	/* Prepare SPI */
	spi_init();
	elog_spi = spi_flash_probe(0, 0, 0, 0);
	if (!elog_spi) {
		printk(BIOS_ERR, "ELOG: Unable to find SPI flash\n");
		return -1;
	}

	/* Set up the backing store */
	elog_find_flash(&flash_base, &flash_size);
	if (flash_base == 0) {
		printk(BIOS_ERR, "ELOG: Invalid flash base\n");
		return -1;
	}

	flash->backing_store = malloc(flash_size);
	if (!flash->backing_store) {
		printk(BIOS_ERR, "ELOG: Unable to allocate backing store\n");
		return -1;
	}
	flash->flash_base = flash_base;
	flash->total_size = flash_size;

	/* Data starts immediately after header */
	flash->data = flash->backing_store + sizeof(struct elog_header);
	flash->data_size = flash_size - sizeof(struct elog_header);

	/* Load the log from flash */
	elog_scan_flash(flash);

	/* Prepare the flash if necessary */
	if (flash->header_state == ELOG_HEADER_INVALID ||
		flash->event_buffer_state == ELOG_EVENT_BUFFER_CORRUPTED) {
		/* If the header is invalid or the events are corrupted,
		 * no events can be salvaged so erase the entire area. */
		printk(BIOS_ERR, "ELOG: flash area invalid\n");
		elog_flash_erase(flash->backing_store, flash->total_size);
		elog_prepare_empty(flash);
	}

	if (flash->area_state == ELOG_AREA_EMPTY)
		elog_prepare_empty(flash);

	if (!elog_is_area_valid(flash)) {
		printk(BIOS_ERR, "ELOG: Unable to prepare flash\n");
		return -1;
	}

	elog_initialized = 1;

	printk(BIOS_INFO, "ELOG: FLASH @0x%p [SPI 0x%08x]\n",
	       flash->backing_store, flash->flash_base);

	printk(BIOS_INFO, "ELOG: area is %d bytes, full threshold %d,"
	       " shrink size %d\n", flash_size,
	       CONFIG_ELOG_FULL_THRESHOLD, CONFIG_ELOG_SHRINK_SIZE);

	/* Log a clear event if necessary */
	if (flash->event_count == 0)
		elog_add_event_word(ELOG_TYPE_LOG_CLEAR, flash->total_size);

	/* Shrink the log if we are getting too full */
	if (flash->next_event_offset >= CONFIG_ELOG_FULL_THRESHOLD)
		if (elog_shrink() < 0)
			return -1;

#if !defined(__SMM__)
	/* Log boot count event except in S3 resume */
	if (CONFIG_ELOG_BOOT_COUNT && acpi_slp_type != 3)
		elog_add_event_dword(ELOG_TYPE_BOOT, boot_count_read());

	/* Check and log POST codes from previous boot */
	if (CONFIG_CMOS_POST)
		cmos_post_log();
#endif

	return 0;
}

/*
 * Populate timestamp in event header with current time
 */
static void elog_fill_timestamp(struct event_header *event)
{
	event->second = cmos_read(RTC_CLK_SECOND);
	event->minute = cmos_read(RTC_CLK_MINUTE);
	event->hour   = cmos_read(RTC_CLK_HOUR);
	event->day    = cmos_read(RTC_CLK_DAYOFMONTH);
	event->month  = cmos_read(RTC_CLK_MONTH);
	event->year   = cmos_read(RTC_CLK_YEAR);

	/* Basic sanity check of expected ranges */
	if (event->month > 0x12 || event->day > 0x31 || event->hour > 0x23 ||
	    event->minute > 0x59 || event->second > 0x59) {
		event->year   = 0;
		event->month  = 0;
		event->day    = 0;
		event->hour   = 0;
		event->minute = 0;
		event->second = 0;
	}
}

/*
 * Add an event to the log
 */
void elog_add_event_raw(u8 event_type, void *data, u8 data_size)
{
	struct event_header *event;
	struct elog_descriptor *flash = elog_get_flash();
	u8 event_size;

	elog_debug("elog_add_event_raw(type=%X)\n", event_type);

	/* Make sure ELOG structures are initialized */
	if (elog_init() < 0)
		return;

	/* Header + Data + Checksum */
	event_size = sizeof(*event) + data_size + 1;
	if (event_size > MAX_EVENT_SIZE) {
		printk(BIOS_ERR, "ELOG: Event(%X) data size too "
		       "big (%d)\n", event_type, event_size);
		return;
	}

	/* Make sure event data can fit */
	if ((flash->next_event_offset + event_size) >= flash->data_size) {
		printk(BIOS_ERR, "ELOG: Event(%X) does not fit\n",
		       event_type);
		return;
	}

	/* Fill out event data */
	event = elog_get_next_event_base(flash);
	event->type = event_type;
	event->length = event_size;
	elog_fill_timestamp(event);

	if (data_size)
		memcpy(&event[1], data, data_size);

	/* Zero the checksum byte and then compute checksum */
	elog_update_checksum(event, 0);
	elog_update_checksum(event, -(elog_checksum_event(event)));

	/* Update memory descriptor parameters */
	flash->event_count++;

	elog_flash_write((void *)event, event_size);

	flash->next_event_offset += event_size;

	printk(BIOS_INFO, "ELOG: Event(%X) added with size %d\n",
	       event_type, event_size);

	/* Shrink the log if we are getting too full */
	if (flash->next_event_offset >= CONFIG_ELOG_FULL_THRESHOLD)
		elog_shrink();
}

void elog_add_event(u8 event_type)
{
	elog_add_event_raw(event_type, NULL, 0);
}

void elog_add_event_byte(u8 event_type, u8 data)
{
	elog_add_event_raw(event_type, &data, sizeof(data));
}

void elog_add_event_word(u8 event_type, u16 data)
{
	elog_add_event_raw(event_type, &data, sizeof(data));
}

void elog_add_event_dword(u8 event_type, u32 data)
{
	elog_add_event_raw(event_type, &data, sizeof(data));
}

void elog_add_event_wake(u8 source, u32 instance)
{
	struct elog_event_data_wake wake = {
		.source = source,
		.instance = instance
	};
	elog_add_event_raw(ELOG_TYPE_WAKE_SOURCE, &wake, sizeof(wake));
}
