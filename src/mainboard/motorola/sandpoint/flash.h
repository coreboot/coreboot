/* $Id$ */
/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

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
