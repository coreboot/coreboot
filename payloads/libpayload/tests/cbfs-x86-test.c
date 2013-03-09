/* system headers */
#include <stdlib.h>
#include <stdio.h>

/* libpayload headers */
#include "cbfs.h"

int fail(const char* str)
{
	fprintf(stderr, "%s", str);
	exit(1);
}

int main(int argc, char** argv)
{
	FILE *cbfs = fopen("data/cbfs-x86.bin", "rb");
	if (!cbfs) fail("could not open test file\n");
	if (fseek(cbfs, 0, SEEK_END) != 0) fail("seek to end failed\n");

	long size = ftell(cbfs);
	if (size == -1) fail("could not determine file size\n");
	if (fseek(cbfs, 0, SEEK_SET) != 0) fail("seek to start failed\n");

	void *data = malloc(size);
	if (!data) fail("could not allocate buffer\n");

	if (fread(data, size, 1, cbfs) != 1) fail("could not read data\n");
	if (fclose(cbfs)) fail("could not close file\n");

	if (setup_cbfs_from_ram(data, size) != 0) fail("could not setup CBFS in RAM\n");
	struct cbfs_file *file = cbfs_find("foo");
	if (file == NULL) fail("could not find file in CBFS\n");
	exit(0);
}
