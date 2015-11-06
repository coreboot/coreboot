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
 */

#if CONFIG_HAVE_ACPI_RESUME == 1
#include <arch/acpi.h>
#endif
#include <cbmem.h>
#include <console/console.h>
#if CONFIG_ARCH_X86
#include <pc80/mc146818rtc.h>
#endif
#include <bcd.h>
#include <fmap.h>
#include <rtc.h>
#include <smbios.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <stdint.h>
#include <string.h>
#include <elog.h>
#include "elog_internal.h"


#if !IS_ENABLED(CONFIG_CHROMEOS) && CONFIG_ELOG_FLASH_BASE == 0
#error "CONFIG_ELOG_FLASH_BASE is invalid"
#endif

#if CONFIG_ELOG_DEBUG
#define elog_debug(STR...) printk(BIOS_DEBUG, STR)
#else
#define elog_debug(STR...)
#endif

/*
 * Static variables for ELOG state
 */
static struct elog_area *elog_area;
static u16 total_size;
static u16 log_size; /* excluding header */
static u32 flash_base;
static u16 full_threshold; /* from end of header */
static u16 shrink_size; /* from end of header */

static elog_area_state area_state;
static elog_header_state header_state;
static elog_event_buffer_state event_buffer_state;

static u16 next_event_offset; /* from end of header */
static u16 event_count;

static struct spi_flash *elog_spi;

static enum {
	ELOG_UNINITIALIZED = 0,
	ELOG_INITIALIZED,
	ELOG_BROKEN,
} elog_initialized = ELOG_UNINITIALIZED;

static inline u32 get_rom_size(void)
{
	u32 rom_size;

	/* Assume the used space of the ROM image starts from 0. The
	 * physical size of the device may not be completely used. */
	rom_size = elog_spi->size;
	if (rom_size > CONFIG_ROM_SIZE)
		rom_size = CONFIG_ROM_SIZE;

	return rom_size;
}

/*
 * Pointer to an event log header in the event data area
 */
static inline struct event_header*
elog_get_event_base(u32 offset)
{
	return (struct event_header *)&elog_area->data[offset];
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
static int elog_is_buffer_clear(void *base, u32 size)
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
 * Check that the ELOG area has been initialized and is valid.
 */
static int elog_is_area_valid(void)
{
	elog_debug("elog_is_area_valid()\n");

	if (area_state != ELOG_AREA_HAS_CONTENT)
		return 0;
	if (header_state != ELOG_HEADER_VALID)
		return 0;
	if (event_buffer_state != ELOG_EVENT_BUFFER_OK)
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
static int elog_is_event_valid(u32 offset)
{
	struct event_header *event;

	event = elog_get_event_base(offset);
	if (!event)
		return 0;

	/* Validate event length */
	if ((offsetof(struct event_header, type) +
	     sizeof(event->type) - 1 + offset) >= log_size)
		return 0;

	/* End of event marker has been found */
	if (event->type == ELOG_TYPE_EOL)
		return 0;

	/* Check if event fits in area */
	if ((offsetof(struct event_header, length) +
	     sizeof(event->length) - 1 + offset) >= log_size)
		return 0;

	/*
	 * If the current event length + the current offset exceeds
	 * the area size then the event area is corrupt.
	 */
	if ((event->length + offset) >= log_size)
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
static void elog_flash_write(void *address, u32 size)
{
	u32 offset;

	if (!address || !size || !elog_spi)
		return;

	offset = flash_base;
	offset += (u8 *)address - (u8 *)elog_area;

	elog_debug("elog_flash_write(address=0x%p offset=0x%08x size=%u)\n",
		   address, offset, size);

	/* Write the data to flash */
	elog_spi->write(elog_spi, offset, size, address);
}

/*
 * Erase the first block specified in the address.
 * Only handles flash area within a single flash block.
 */
static void elog_flash_erase(void *address, u32 size)
{
	u32 offset;

	if (!address || !size || !elog_spi)
		return;

	offset = flash_base;
	offset += (u8 *)address - (u8*)elog_area;

	elog_debug("elog_flash_erase(address=0x%p offset=0x%08x size=%u)\n",
		   address, offset, size);

	/* Erase the sectors in this region */
	elog_spi->erase(elog_spi, offset, size);
}

/*
 * Scan the event area and validate each entry and update the ELOG state.
 */
static void elog_update_event_buffer_state(void)
{
	u32 count = 0;
	u32 offset = 0;
	struct event_header *event;

	elog_debug("elog_update_event_buffer_state()\n");

	/* Go through each event and validate it */
	while (1) {
		event = elog_get_event_base(offset);

		/* Do not de-reference anything past the area length */
		if ((offsetof(struct event_header, type) +
		     sizeof(event->type) - 1 + offset) >= log_size) {
			event_buffer_state = ELOG_EVENT_BUFFER_CORRUPTED;
			break;
		}

		/* The end of the event marker has been found */
		if (event->type == ELOG_TYPE_EOL)
			break;

		/* Validate the event */
		if (!elog_is_event_valid(offset)) {
			event_buffer_state = ELOG_EVENT_BUFFER_CORRUPTED;
			break;
		}

		/* Move to the next event */
		count++;
		offset += event->length;
	}

	/* Ensure the remaining buffer is empty */
	if (!elog_is_buffer_clear(&elog_area->data[offset], log_size - offset))
		event_buffer_state = ELOG_EVENT_BUFFER_CORRUPTED;

	/* Update ELOG state */
	event_count = count;
	next_event_offset = offset;
}

static void elog_scan_flash(void)
{
	elog_debug("elog_scan_flash()\n");

	area_state = ELOG_AREA_UNDEFINED;
	header_state = ELOG_HEADER_INVALID;
	event_buffer_state = ELOG_EVENT_BUFFER_OK;

	/* Fill memory buffer by reading from SPI */
	elog_spi->read(elog_spi, flash_base, total_size, elog_area);

	next_event_offset = 0;
	event_count = 0;

	/* Check if the area is empty or not */
	if (elog_is_buffer_clear(elog_area, total_size)) {
		area_state = ELOG_AREA_EMPTY;
		return;
	}

	area_state = ELOG_AREA_HAS_CONTENT;

	/* Validate the header */
	if (!elog_is_header_valid(&elog_area->header)) {
		header_state = ELOG_HEADER_INVALID;
		return;
	}

	header_state = ELOG_HEADER_VALID;
	elog_update_event_buffer_state();
}

static void elog_prepare_empty(void)
{
	struct elog_header *header;

	elog_debug("elog_prepare_empty()\n");

	/* Write out the header */
	header = &elog_area->header;
	header->magic = ELOG_SIGNATURE;
	header->version = ELOG_VERSION;
	header->header_size = sizeof(struct elog_header);
	header->reserved[0] = ELOG_TYPE_EOL;
	header->reserved[1] = ELOG_TYPE_EOL;
	elog_flash_write(elog_area, header->header_size);

	elog_scan_flash();
}

/*
 * Shrink the log, deleting old entries and moving the
 * remaining ones to the front of the log.
 */
static int elog_shrink(void)
{
	struct event_header *event;
	u16 discard_count = 0;
	u16 offset = 0;
	u16 new_size = 0;

	elog_debug("elog_shrink()\n");

	if (next_event_offset < shrink_size)
		return 0;

	while (1) {
		/* Next event has exceeded constraints */
		if (offset > shrink_size)
			break;

		event = elog_get_event_base(offset);

		/* Reached the end of the area */
		if (!event || event->type == ELOG_TYPE_EOL)
			break;

		offset += event->length;
		discard_count++;
	}

	new_size = next_event_offset - offset;
	memmove(&elog_area->data[0], &elog_area->data[offset], new_size);
	memset(&elog_area->data[new_size], ELOG_TYPE_EOL, log_size - new_size);

	elog_flash_erase(elog_area, total_size);
	elog_flash_write(elog_area, total_size);
	elog_scan_flash();

	/* Ensure the area was successfully erased */
	if (next_event_offset >= full_threshold) {
		printk(BIOS_ERR, "ELOG: Flash area was not erased!\n");
		return -1;
	}

	/* Add clear event */
	elog_add_event_word(ELOG_TYPE_LOG_CLEAR, offset);

	return 0;
}

#ifndef __SMM__
#if IS_ENABLED(CONFIG_ARCH_X86)

/*
 * Convert a flash offset into a memory mapped flash address
 */
static inline u8 *elog_flash_offset_to_address(u32 offset)
{
	u32 rom_size;

	if (!elog_spi)
		return NULL;

	rom_size = get_rom_size();

	return (u8 *)((u32)~0UL - rom_size + 1 + offset);
}

/*
 * Fill out SMBIOS Type 15 table entry so the
 * event log can be discovered at runtime.
 */
int elog_smbios_write_type15(unsigned long *current, int handle)
{
	struct smbios_type15 *t = (struct smbios_type15 *)*current;
	int len = sizeof(struct smbios_type15);

#if CONFIG_ELOG_CBMEM
	/* Save event log buffer into CBMEM for the OS to read */
	void *cbmem = cbmem_add(CBMEM_ID_ELOG, total_size);
	if (!cbmem)
		return 0;
	memcpy(cbmem, elog_area, total_size);
#endif

	memset(t, 0, len);
	t->type = SMBIOS_EVENT_LOG;
	t->length = len - 2;
	t->handle = handle;
	t->area_length = total_size - 1;
	t->header_offset = 0;
	t->data_offset = sizeof(struct elog_header);
	t->access_method = SMBIOS_EVENTLOG_ACCESS_METHOD_MMIO32;
	t->log_status = SMBIOS_EVENTLOG_STATUS_VALID;
	t->change_token = 0;
#if CONFIG_ELOG_CBMEM
	t->address = (u32)cbmem;
#else
	t->address = (u32)elog_flash_offset_to_address(flash_base);
#endif
	t->header_format = ELOG_HEADER_TYPE_OEM;
	t->log_type_descriptors = 0;
	t->log_type_descriptor_length = 2;

	*current += len;
	return len;
}
#endif
#endif

/*
 * Clear the entire event log
 */
int elog_clear(void)
{
	elog_debug("elog_clear()\n");

	/* Make sure ELOG structures are initialized */
	if (elog_init() < 0)
		return -1;

	/* Erase flash area */
	elog_flash_erase(elog_area, total_size);
	elog_prepare_empty();

	if (!elog_is_area_valid())
		return -1;

	/* Log the clear event */
	elog_add_event_word(ELOG_TYPE_LOG_CLEAR, total_size);

	return 0;
}

static void elog_find_flash(void)
{
	elog_debug("elog_find_flash()\n");

	if (IS_ENABLED(CONFIG_CHROMEOS)) {
		/* Find the ELOG base and size in FMAP */
		struct region r;

		if (fmap_locate_area("RW_ELOG", &r) < 0) {
			printk(BIOS_WARNING,
				"ELOG: Unable to find RW_ELOG in FMAP\n");
			flash_base = total_size = 0;
		} else {
			flash_base = region_offset(&r);
			total_size = MIN(region_sz(&r), CONFIG_ELOG_AREA_SIZE);
		}
	} else {
		flash_base = CONFIG_ELOG_FLASH_BASE;
		total_size = CONFIG_ELOG_AREA_SIZE;
	}
	log_size = total_size - sizeof(struct elog_header);
	full_threshold = log_size - ELOG_MIN_AVAILABLE_ENTRIES * MAX_EVENT_SIZE;
	shrink_size = MIN(total_size * ELOG_SHRINK_PERCENTAGE / 100,
								full_threshold);
}

/*
 * Event log main entry point
 */
int elog_init(void)
{
	switch (elog_initialized) {
	case ELOG_UNINITIALIZED:
		break;
	case ELOG_INITIALIZED:
		return 0;
	case ELOG_BROKEN:
		return -1;
	}
	elog_initialized = ELOG_BROKEN;

	elog_debug("elog_init()\n");

	/* Probe SPI chip. SPI controller must already be initialized. */
	elog_spi = spi_flash_probe(CONFIG_BOOT_MEDIA_SPI_BUS, 0);
	if (!elog_spi) {
		printk(BIOS_ERR, "ELOG: Unable to find SPI flash\n");
		return -1;
	}

	/* Set up the backing store */
	elog_find_flash();
	if (flash_base == 0) {
		printk(BIOS_ERR, "ELOG: Invalid flash base\n");
		return -1;
	} else if (total_size < sizeof(struct elog_header) + MAX_EVENT_SIZE) {
		printk(BIOS_ERR, "ELOG: Region too small to hold any events\n");
		return -1;
	} else if (log_size - shrink_size >= full_threshold) {
		printk(BIOS_ERR,
			"ELOG: SHRINK_PERCENTAGE set too small for MIN_AVAILABLE_ENTRIES\n");
		return -1;
	}

	elog_area = malloc(total_size);
	if (!elog_area) {
		printk(BIOS_ERR, "ELOG: Unable to allocate backing store\n");
		return -1;
	}

	/* Load the log from flash */
	elog_scan_flash();

	/* Prepare the flash if necessary */
	if (header_state == ELOG_HEADER_INVALID ||
		event_buffer_state == ELOG_EVENT_BUFFER_CORRUPTED) {
		/* If the header is invalid or the events are corrupted,
		 * no events can be salvaged so erase the entire area. */
		printk(BIOS_ERR, "ELOG: flash area invalid\n");
		elog_flash_erase(elog_area, total_size);
		elog_prepare_empty();
	}

	if (area_state == ELOG_AREA_EMPTY)
		elog_prepare_empty();

	if (!elog_is_area_valid()) {
		printk(BIOS_ERR, "ELOG: Unable to prepare flash\n");
		return -1;
	}

	printk(BIOS_INFO, "ELOG: FLASH @0x%p [SPI 0x%08x]\n",
	       elog_area, flash_base);

	printk(BIOS_INFO, "ELOG: area is %d bytes, full threshold %d,"
	       " shrink size %d\n", total_size, full_threshold, shrink_size);

	elog_initialized = ELOG_INITIALIZED;

	/* Shrink the log if we are getting too full */
	if (next_event_offset >= full_threshold)
		if (elog_shrink() < 0)
			return -1;

	/* Log a clear event if necessary */
	if (event_count == 0)
		elog_add_event_word(ELOG_TYPE_LOG_CLEAR, total_size);

#if !defined(__SMM__)
	/* Log boot count event except in S3 resume */
#if CONFIG_ELOG_BOOT_COUNT == 1
#if CONFIG_HAVE_ACPI_RESUME == 1
		if (!acpi_is_wakeup_s3())
#endif
		elog_add_event_dword(ELOG_TYPE_BOOT, boot_count_read());
#else
		/* If boot count is not implemented, fake it. */
		elog_add_event_dword(ELOG_TYPE_BOOT, 0);
#endif

#if CONFIG_ARCH_X86
	/* Check and log POST codes from previous boot */
	if (CONFIG_CMOS_POST)
		cmos_post_log();
#endif
#endif

	elog_initialized = ELOG_INITIALIZED;

	return 0;
}

/*
 * Populate timestamp in event header with current time
 */
static void elog_fill_timestamp(struct event_header *event)
{
#if IS_ENABLED(CONFIG_RTC)
	struct rtc_time time;

	rtc_get(&time);
	event->second = bin2bcd(time.sec);
	event->minute = bin2bcd(time.min);
	event->hour = bin2bcd(time.hour);
	event->day = bin2bcd(time.mday);
	event->month = bin2bcd(time.mon);
	event->year = bin2bcd(time.year % 100);

	/* Basic sanity check of expected ranges */
	if (event->month > 0x12 || event->day > 0x31 || event->hour > 0x23 ||
	    event->minute > 0x59 || event->second > 0x59)
#endif
	{
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
	if ((next_event_offset + event_size) >= log_size) {
		printk(BIOS_ERR, "ELOG: Event(%X) does not fit\n",
		       event_type);
		return;
	}

	/* Fill out event data */
	event = elog_get_event_base(next_event_offset);
	event->type = event_type;
	event->length = event_size;
	elog_fill_timestamp(event);

	if (data_size)
		memcpy(&event[1], data, data_size);

	/* Zero the checksum byte and then compute checksum */
	elog_update_checksum(event, 0);
	elog_update_checksum(event, -(elog_checksum_event(event)));

	/* Update the ELOG state */
	event_count++;

	elog_flash_write((void *)event, event_size);

	next_event_offset += event_size;

	printk(BIOS_INFO, "ELOG: Event(%X) added with size %d\n",
	       event_type, event_size);

	/* Shrink the log if we are getting too full */
	if (next_event_offset >= full_threshold)
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
