#ifndef ROM_READ_BYTES_H
#define ROM_READ_BYTES_H

#include <stdint.h>

typedef long byte_offset_t;

typedef struct tag_t {
	char signature[5];
	unsigned char block_count;
	unsigned long length;
	char data[6];
} __attribute__ ((packed)) tag_head;

struct stream {
	int (*init)(void);
	int (*init_tags)(void);
	void (*get_tags)(void *buf);
	void (*load_tag)(int tag);
	byte_offset_t (*read)(void *vdest, byte_offset_t count);
	byte_offset_t (*skip)(byte_offset_t count);
	void (*fini)(void);
};

#define __stream	__attribute__ ((unused,__section__ (".rodata.streams")))

/* Defined by the linker... */
extern struct stream streams[];
extern struct stream estreams[];

#endif /* ROM_READ_BYTES_H */
