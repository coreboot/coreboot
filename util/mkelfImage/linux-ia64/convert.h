#define CMDLINE_MAX 1024

#ifdef ASSEMBLY
#define CONVERT_MAGIC 0xA5A5A5A5A5A5A5A5
#else
#define CONVERT_MAGIC 0xA5A5A5A5A5A5A5A5ULL
#endif

#ifndef ASSEMBLY
struct image_parameters {
	uint64_t convert_magic;
	uint64_t entry;
	uint64_t initrd_start;
	uint64_t initrd_size;
	uint8_t  cmdline[CMDLINE_MAX];
};
#endif
