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

#ifndef _FLASH_H
#define _FLASH_H

struct flash_device;

typedef struct flash_fn
{
    const char *(* identify)(struct flash_device *flash);
    void *(* ptr)(void *data);
    int (* erase_all)(void *data);
    int (* erase)(void *data, unsigned offset, unsigned length);
    int (* program)(void *data, unsigned offset, const void *source, unsigned length);    
    uint8_t ( *read_byte)(void *data, unsigned offset);
} flash_fn;

typedef struct flash_device
{
    const flash_fn *fn;
    char *tag;
    void *data;
    unsigned long base;
    unsigned size;
    unsigned erase_size;
    unsigned store_size;
    struct flash_device *next;
} flash_device;

int register_flash_device(const flash_fn *fn, char *tag, void *data);
flash_device *find_flash_device(const char *tag);
int init_flash_amd800(char *tag, unsigned base, unsigned spacing);

#endif
