#ifndef _VFS_H
#define _VFS_H

#include <stdint.h>

#define DISK_IDE 1
#define DISK_MEM 2

int devopen(const char *name, int *reopen);
int devread(unsigned long sector, unsigned long byte_offset,
	unsigned long byte_len, void *buf);

int file_open(const char *filename);
int file_read(void *buf, unsigned long len);
int file_seek(unsigned long offset);
unsigned long file_pos(void);
unsigned long file_size(void);

#define PARTITION_UNKNOWN 0xbad6a7

#ifdef CONFIG_FS_ELTORITO
int open_eltorito_image(int part, unsigned long *start, unsigned long *length);
#else
# define open_eltorito_image(x,y,z) PARTITION_UNKNOWN
#endif

extern int using_devsize;

#endif /* _VFS_H */
