#include <types.h>
#include <cbfs.h>
#include <string.h>
#include <arch/byteorder.h>


#define boot_cpu(x) 1

#ifdef CONFIG_BOOTBLOCK_CPU_INIT
#include CONFIG_BOOTBLOCK_CPU_INIT
#else
static void bootblock_cpu_init(void) { }
#endif
#ifdef CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT
#include CONFIG_BOOTBLOCK_NORTHBRIDGE_INIT
#else
static void bootblock_northbridge_init(void) { }
#endif
#ifdef CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT
#include CONFIG_BOOTBLOCK_SOUTHBRIDGE_INIT
#else
static void bootblock_southbridge_init(void) { }
#endif

static int cbfs_check_magic(struct cbfs_file *file)
{
	return !strcmp(file->magic, CBFS_FILE_MAGIC) ? 1 : 0;
}

static unsigned long findstage(const char* target)
{
	unsigned long offset;

	void *ptr = (void *)*((unsigned long *) CBFS_HEADPTR_ADDR);
	struct cbfs_header *header = (struct cbfs_header *) ptr;
	// if (ntohl(header->magic) != CBFS_HEADER_MAGIC)
	// 	printk(BIOS_ERR, "ERROR: No valid CBFS header found!\n");

	offset = 0 - ntohl(header->romsize) + ntohl(header->offset);
	int align = ntohl(header->align);
	while(1) {
		struct cbfs_file *file = (struct cbfs_file *) offset;
		if (!cbfs_check_magic(file))
			return 0;
		if (!strcmp(CBFS_NAME(file), target))
			return (unsigned long)CBFS_SUBHEADER(file);
		int flen = ntohl(file->len);
		int foffset = ntohl(file->offset);
		unsigned long oldoffset = offset;
		offset = ALIGN(offset + foffset + flen, align);
		if (offset <= oldoffset)
			return 0;
		if (offset < 0xFFFFFFFF - ntohl(header->romsize))
			return 0;
	}
}


static void call(unsigned long addr, unsigned long bist)
{
	asm volatile ("mov r0, %1\nbx %0\n" : : "r" (addr), "r" (bist));
}

static void hlt(void)
{
	/* is there such a thing as hlt on ARM? */
	// asm volatile ("1:\n\thlt\n\tjmp 1b\n\t");
	asm volatile ("1:\nb 1b\n\t");
}
