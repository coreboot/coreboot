#include "common.h"
#include <windows.h>

static inline size_t xsize_t(off_t len)
{
        if (len > (size_t) len)
                die("Cannot handle files this big");
        return (size_t)len;
}

void *win32_mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
	HANDLE hmap;
	void *temp;
	off_t len;
	struct stat st;
	uint64_t o = offset;
	uint32_t l = o & 0xFFFFFFFF;
	uint32_t h = (o >> 32) & 0xFFFFFFFF;

	if (!fstat(fd, &st))
		len = st.st_size;
	else
		printf("mmap: could not determine filesize");

	if ((length + offset) > len)
		length = xsize_t(len - offset);

	if (!(flags & MAP_PRIVATE))
		printf("Invalid usage of mmap when built with USE_WIN32_MMAP");

	hmap = CreateFileMapping((HANDLE)_get_osfhandle(fd), 0, PAGE_WRITECOPY,
		0, 0, 0);

	if (!hmap)
		return MAP_FAILED;

	temp = MapViewOfFileEx(hmap, FILE_MAP_COPY, h, l, length, start);

	if (!CloseHandle(hmap))
		printf("unable to close file mapping handle");

	return temp ? temp : MAP_FAILED;
}

int win32_munmap(void *start, size_t length)
{
	return !UnmapViewOfFile(start);
}
