/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 PC Engines GmbH
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

#include <console/console.h>
#include <string.h>
#include <stdlib.h>
#include "s1_button.h"
#include <spi_flash.h>
#include <spi-generic.h>
#include <boot_device.h>
#include <cbfs.h>
#include <commonlib/region.h>
#include <commonlib/cbfs.h>

#define BOOTORDER_FILE "bootorder"

static int find_knob_index(const char *s, const char *pattern)
{

       int pattern_index = 0;
       char *result = (char *) s;
       char *lpattern = (char *) pattern;

       while (*result && *pattern ) {
               if ( *lpattern == 0)  // the pattern matches return the pointer
                       return pattern_index;
               if ( *result == 0)  // We're at the end of the file content but don't have a patter match yet
                       return -1;
               if (*result == *lpattern ) {
                       // The string matches, simply advance
                       result++;
                       pattern_index++;
                       lpattern++;
               } else {
                       // The string doesn't match restart the pattern
                       result++;
                       pattern_index++;
                       lpattern = (char *) pattern;
               }
       }

       return -1;
}

static size_t get_bootorder_cbfs_offset(const char *name, uint32_t type)
{
	struct region_device rdev;
	const struct region_device *boot_dev;
	struct cbfs_props props;
	struct cbfsf fh;

	boot_dev = boot_device_ro();

	if (boot_dev == NULL) {
		printk(BIOS_WARNING, "Can't init CBFS boot device\n");
		return 0;
	}

	if (cbfs_boot_region_properties(&props)) {
		printk(BIOS_WARNING, "Can't locate CBFS\n");
		return 0;
	}

	if (rdev_chain(&rdev, boot_dev, props.offset, props.size)) {
		printk(BIOS_WARNING, "Rdev chain failed\n");
		return 0;
	}

	if (cbfs_locate(&fh, &rdev, name, &type)) {
		printk(BIOS_WARNING, "Can't locate file in CBFS\n");
		return 0;
	}

	return (size_t) rdev_relative_offset(boot_dev, &fh.data);
}

static int flash_bootorder(size_t offset, size_t fsize, char *buffer)
{
        const struct spi_flash *flash;

        flash = boot_device_spi_flash();

        if (flash == NULL) {
                printk(BIOS_WARNING, "Can't get boot flash device\n");
                return -1;
        }

        if (spi_flash_erase(flash, (u32) offset, fsize)) {
                printk(BIOS_WARNING, "SPI erase failed\n");
                return -1;
        }

        if (spi_flash_write(flash, offset, fsize, buffer)) {
                printk(BIOS_WARNING, "SPI write failed\n");
                return -1;
        }

        printk(BIOS_INFO, "Bootorder write successed\n");
        return 0;
}

void enable_console(void)
{
        size_t fsize, offset;
        char* bootorder_file = NULL;
        int knob_index;
        char *bootorder_copy;

        bootorder_file = cbfs_boot_map_with_leak("bootorder", CBFS_TYPE_RAW, &fsize);

        if (bootorder_file == NULL){
                printk(BIOS_WARNING, "Could not mmap bootorder\n");
                return;
        }

        if (fsize & 0xFFF) {
                printk(BIOS_WARNING,"The bootorder file is not 4k aligned\n");
                return;
        }

        offset = get_bootorder_cbfs_offset("bootorder", CBFS_TYPE_RAW);

        if(offset == -1) {
                printk(BIOS_WARNING,"Failed to retrieve bootorder file offset\n");
                return;
        }

        bootorder_copy = (char *) malloc(fsize);

        if(bootorder_copy == NULL) {
                printk(BIOS_WARNING,"Failed to allocate memory for bootorder\n");
                return;
        }

        if(memcpy(bootorder_copy, bootorder_file, fsize) == NULL) {
                printk(BIOS_WARNING,"Copying bootorder failed\n");
                free(bootorder_copy);
                return;
        }

        knob_index = find_knob_index(bootorder_copy, "scon");

        if(knob_index == -1){
                printk(BIOS_WARNING,"scon knob not found in bootorder\n");
                free(bootorder_copy);
                return;
        }

        *(bootorder_copy + knob_index) = '1';

        if(flash_bootorder(offset, fsize, bootorder_copy)) {
                printk(BIOS_WARNING, "Failed to flash bootorder\n");
        }
}
