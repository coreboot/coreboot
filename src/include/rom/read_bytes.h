#ifndef ROM_READ_BYTES_H
#define ROM_READ_BYTES_H

#include <stdint.h>

typedef long byte_offset_t;

struct stream {
	int (*init)(void);
	byte_offset_t (*read)(void *vdest, byte_offset_t count);
	byte_offset_t (*skip)(byte_offset_t count);
	void (*fini)(void);
};

#define __stream	__attribute__ ((unused,__section__ (".rodata.streams")))

/* Defined by the linker... */
extern struct stream streams[];
extern struct stream estreams[];

#endif /* ROM_READ_BYTES_H */
