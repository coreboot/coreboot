#include <console/console.h>
#include <stdlib.h>
#include <stddef.h>
#include <stream/read_bytes.h>
#include <string.h>
#include <fs/fs.h>

char *boot_file;

int stream_init(void)
{
	return file_open(boot_file);
}


void stream_fini(void)
{
	return;
}

byte_offset_t stream_skip(byte_offset_t count)
{
	unsigned long pos = file_pos();
	if (file_seek(count+pos) != count+pos)
		return 0;
	return count;
}

byte_offset_t stream_read(void *vdest, byte_offset_t count)
{
	return file_read(vdest, count);
}
