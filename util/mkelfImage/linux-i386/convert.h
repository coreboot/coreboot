
/* Hard coded locations */
#define CONVERTLOC 		0x10000
#define REAL_MODE_DATA_LOC	0x20000
#define GDTLOC	   		0x21000
#define GDT64LOC		0x22000
#define PGTLOC			0x23000

#define DEFAULT_ROOT_DEV ((0x3<<8)| 0)

#define CMDLINE_MAX 256

#ifdef ASSEMBLY
#define CONVERT_MAGIC 0xA5A5A5A5
#else
#define CONVERT_MAGIC 0xA5A5A5A5UL
#endif

#ifndef ASSEMBLY
struct image_parameters {
	uint32_t convert_magic; /* a signature to verify mkelfImage was built properly */
	uint32_t gdt_size;
	uint32_t gdt64_size;
	uint32_t pgt_size;
	uint32_t bss_size;
	uint16_t ramdisk_flags;
	uint16_t root_dev;
	uint32_t entry;
	uint32_t switch_64;
	uint32_t initrd_start;
	uint32_t initrd_size;
	uint8_t  cmdline[CMDLINE_MAX];
};
#endif
