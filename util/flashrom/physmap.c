#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include "flash.h"

#if defined (__sun) && (defined(__i386) || defined(__amd64))
#  define MEM_DEV "/dev/xsvc"
#else
#  define MEM_DEV "/dev/mem"
#endif

static int fd_mem = -1;

void *sys_physmap(unsigned long phys_addr, size_t len)
{
	void *virt_addr;

	if (-1 == fd_mem) {
		/* Open the memory device UNCACHED. Important for MMIO. */
		if (-1 == (fd_mem = open(MEM_DEV, O_RDWR|O_SYNC))) {
			perror("Critical error: open(" MEM_DEV ")");
			exit(1);
		}
	}

	virt_addr = mmap(0, len, PROT_WRITE|PROT_READ, MAP_SHARED, fd_mem, (off_t)phys_addr);
	return MAP_FAILED == virt_addr ? NULL : virt_addr;
}

void physunmap(void *virt_addr, size_t len)
{
	munmap(virt_addr, len);
}

void *physmap(const char *descr, unsigned long phys_addr, size_t len)
{
	void *virt_addr = sys_physmap(phys_addr, len);

	if (NULL == virt_addr) {
		if (NULL == descr)
			descr = "memory";
		fprintf(stderr, "Error accessing %s, 0x%lx bytes at 0x%08lx\n", descr, (unsigned long)len, phys_addr);
		perror(MEM_DEV " mmap failed");
		if (EINVAL == errno) {
			fprintf(stderr, "In Linux this error can be caused by the CONFIG_NONPROMISC_DEVMEM (<2.6.27),\n");
			fprintf(stderr, "CONFIG_STRICT_DEVMEM (>=2.6.27) and CONFIG_X86_PAT kernel options.\n");
			fprintf(stderr, "Please check if either is enabled in your kernel before reporting a failure.\n");
			fprintf(stderr, "You can override CONFIG_X86_PAT at boot with the nopat kernel parameter but\n");
			fprintf(stderr, "disabling the other option unfortunately requires a kernel recompile. Sorry!\n");
		}
		exit(1);
	}

	return virt_addr;
}
