/*
 * This file is part of the coreboot project.
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

#include <commonlib/region.h>
#include <boot_device.h>
#include <fmap.h>
#include <console/console.h>
#include <console/flash.h>
#include <stdlib.h>
#include <types.h>

#define LINE_BUFFER_SIZE 128
#define READ_BUFFER_SIZE 0x100

static const struct region_device *g_rdev_ptr;
static struct region_device g_rdev;
static uint8_t g_line_buffer[LINE_BUFFER_SIZE];
static size_t g_offset;
static size_t g_line_offset;

void flashconsole_init(void)
{
	uint8_t buffer[READ_BUFFER_SIZE];
	size_t size;
	size_t offset = 0;
	size_t len = READ_BUFFER_SIZE;
	size_t i;

	if (fmap_locate_area_as_rdev_rw("CONSOLE", &g_rdev)) {
		printk(BIOS_INFO, "Can't find 'CONSOLE' area in FMAP\n");
		return;
	}
	size = region_device_sz(&g_rdev);

	/*
	 * We need to check the region until we find a 0xff indicating
	 * the end of a previous log write.
	 * We can't erase the region because one stage would erase the
	 * data from the previous stage. Also, it looks like doing an
	 * erase could completely freeze the SPI controller and then
	 * we can't write anything anymore (apparently might happen if
	 * the sector is already erased, so we would need to read
	 * anyways to check if it's all 0xff).
	 */
	for (i = 0; i < len && offset < size;) {
		// Fill the buffer on first iteration
		if (i == 0) {
			len = min(READ_BUFFER_SIZE, size - offset);
			if (rdev_readat(&g_rdev, buffer, offset, len) != len)
				return;
		}
		if (buffer[i] == 0xff) {
			offset += i;
			break;
		}
		// If we're done, repeat the process for the next sector
		if (++i == READ_BUFFER_SIZE) {
			offset += len;
			i = 0;
		}
	}
	// Make sure there is still space left on the console
	if (offset >= size) {
		printk(BIOS_INFO, "No space left on 'console' region in SPI flash\n");
		return;
	}

	g_offset = offset;
	g_rdev_ptr = &g_rdev;
}

void flashconsole_tx_byte(unsigned char c)
{
	if (!g_rdev_ptr)
		return;

	size_t region_size = region_device_sz(g_rdev_ptr);

	g_line_buffer[g_line_offset++] = c;

	if (g_line_offset >= LINE_BUFFER_SIZE ||
	    g_offset + g_line_offset >= region_size || c == '\n') {
		flashconsole_tx_flush();
	}
}

void flashconsole_tx_flush(void)
{
	size_t offset = g_offset;
	size_t len = g_line_offset;
	size_t region_size;
	static int busy;

	/* Prevent any recursive loops in case the spi flash driver
	 * calls printk (in case of transaction timeout or
	 * any other error while writing) */
	if (busy)
		return;

	if (!g_rdev_ptr)
		return;

	busy = 1;
	region_size = region_device_sz(g_rdev_ptr);
	if (offset + len >= region_size)
		len = region_size - offset;

	if (rdev_writeat(&g_rdev, g_line_buffer, offset, len) != len)
		return;

	// If the region is full, stop future write attempts
	if (offset + len >= region_size)
		return;

	g_offset = offset + len;
	g_line_offset = 0;

	busy = 0;
}
