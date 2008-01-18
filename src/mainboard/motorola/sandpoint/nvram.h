/*
 * This file is part of the coreboot project.
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

/* Definitions for nvram devices - these are flash or eeprom devices used to
   store information across power cycles and resets. Though they are byte
   addressable, writes must be committed to allow flash devices to write
   complete sectors. */

#ifndef _NVRAM_H
#define _NVRAM_H

typedef struct nvram_device
{
    unsigned (*size)(struct nvram_device *data);
    int (*read_block)(struct nvram_device *dev, unsigned offset,
	    unsigned char *data, unsigned length);
    int (*write_byte)(struct nvram_device *dev, unsigned offset, unsigned char byte);
    void (*commit)(struct nvram_device *data);
    void *data;
} nvram_device;

int nvram_init (nvram_device *dev);
void nvram_clear(void);

extern nvram_device pcrtc_nvram;
extern void nvram_putenv(const char *name, const char *value);
extern  int nvram_getenv(const char *name, char *buffer, unsigned size);

typedef const struct nvram_constant 
{
    const char *name;
    const char *value;
} nvram_constant;
    
extern nvram_constant hardcoded_environment[];

#endif
