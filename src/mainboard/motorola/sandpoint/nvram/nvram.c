/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <stdlib.h>
#include "../nvram.h"

/* NVRAM layout
 * 
 * Environment variable record runs:
 * [length]NAME=value[length]NAME=value[0]\0
 * A deleted variable is:
 * [length]\0AME=value
 * 
 * When memory is full, we compact.
 * 
 */
static nvram_device *nvram_dev = 0;
static unsigned char *nvram_buffer = 0;
static unsigned nvram_size = 0;
static uint8_t nvram_csum = 0;
#define NVRAM_INVALID (! nvram_dev)

static void update_device(unsigned i, unsigned char data)
{
    if (i < nvram_size)
    {
	nvram_csum -= nvram_buffer[i];
	nvram_buffer[i] = data;
	nvram_dev->write_byte(nvram_dev, i, data);
	nvram_csum += data;
    }
    else
	printk_info("Offset %d out of range in nvram\n", i);
}

static void update_csum(void)
{
    nvram_dev->write_byte(nvram_dev, nvram_size, nvram_csum);
    if (nvram_dev->commit)
	nvram_dev->commit(nvram_dev);
}

static void update_string_device(unsigned i, const unsigned char *data,
	    	    	    unsigned len)
{
    if (i + len < nvram_size)
    {
	unsigned j;
	for(j = 0; j < len; j++)
	{
	    nvram_csum -= nvram_buffer[i];
    	    nvram_buffer[i] = *data;
    	    nvram_dev->write_byte(nvram_dev, i, *data);
    	    nvram_csum += *data;
	    data++;
	    i++;
	}   
    }
    else
	printk_info("Offset %d out of range in nvram\n", i + len);
}

int nvram_init (struct nvram_device *dev)
{
    nvram_dev = dev;
    
    if (! nvram_size)
	nvram_size = dev->size(dev) - 1;
    printk_info("NVRAM size is %d\n", nvram_size);
    if (!nvram_buffer)
    {
	unsigned i;
	
	nvram_buffer = malloc (nvram_size);
	if (!nvram_buffer)
	    return -1;

	nvram_csum = 0;
	dev->read_block(dev, 0, nvram_buffer, nvram_size+1);
	for(i = 0; i < nvram_size; i++)
	    nvram_csum += nvram_buffer[i];

	if (nvram_csum != nvram_buffer[nvram_size])
	{
	    printk_info("NVRAM checksum invalid - erasing\n");
	    //update_device(0, 0);
	    //update_csum();
	}
    }
    printk_info("Initialised nvram\n");
    return 0;
}

void nvram_clear(void)
{
    printk_info("Erasing NVRAM\n");
    update_device(0, 0);
    update_csum();    
}

