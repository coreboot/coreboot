#ifndef STREAM_READ_BYTES_H
#define STREAM_READ_BYTES_H

#include <stdint.h>

typedef long byte_offset_t;

extern int stream_init(void);
extern byte_offset_t stream_read(void *vdest, byte_offset_t count);
extern byte_offset_t stream_skip(byte_offset_t count);
extern void stream_fini(void);

#endif /* STREAM_READ_BYTES_H */
