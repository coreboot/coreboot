/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */
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
