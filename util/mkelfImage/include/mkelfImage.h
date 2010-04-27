#ifndef MKELFIMAGE_H
#define MKELFIMAGE_H

#include <sys/types.h>
#include <stdint.h>
#include <byteswap.h>
#define USE_BSD
#include <endian.h>
#define _GNU_SOURCE

struct memelfheader;
struct memelfphdr;
struct memelfnote;

extern void die(char *fmt, ...);
extern void usage(void);
extern void error(char *fmt, ...);
extern uint16_t ipchksum(const void *data, unsigned long length);
extern uint16_t add_ipchksums(unsigned long offset, uint16_t sum, uint16_t new);
extern void *xmalloc(size_t size, const char *name);
extern void *xrealloc(void *ptr, size_t size, const char *name);
extern char *slurp_file(const char *filename, off_t *r_size);
extern char *slurp_decompress_file(const char *filename, off_t *r_size);
extern struct memelfphdr *add_program_headers(struct memelfheader *ehdr, int count);
extern struct memelfnote *add_notes(struct memelfheader *ehdr, int count);

typedef char *(probe_t)(char *kernel_buf, off_t kernel_size);
typedef int (mkelf_t)(int argc, char **argv,
	struct memelfheader *hdr, char *kernel_buf, off_t kernel_size);
typedef void (usage_t)(void);
struct file_type {
	const char *name;
	probe_t *probe;
	mkelf_t *mkelf;
	usage_t *usage;
};

#if BYTE_ORDER == LITTLE_ENDIAN
#define cpu_to_le16(val) (val)
#define cpu_to_le32(val) (val)
#define cpu_to_le64(val) (val)
#define cpu_to_be16(val) bswap_16(val)
#define cpu_to_be32(val) bswap_32(val)
#define cpu_to_be64(val) bswap_64(val)
#define le16_to_cpu(val) (val)
#define le32_to_cpu(val) (val)
#define le64_to_cpu(val) (val)
#define be16_to_cpu(val) bswap_16(val)
#define be32_to_cpu(val) bswap_32(val)
#define be64_to_cpu(val) bswap_64(val)
#endif
#if BYTE_ORDER == BIG_ENDIAN
#define cpu_to_le16(val) bswap_16(val)
#define cpu_to_le32(val) bswap_32(val)
#define cpu_to_le64(val) bswap_64(val)
#define cpu_to_be16(val) (val)
#define cpu_to_be32(val) (val)
#define cpu_to_be64(val) (val)
#define le16_to_cpu(val) bswap_16(val)
#define le32_to_cpu(val) bswap_32(val)
#define le64_to_cpu(val) bswap_64(val)
#define be16_to_cpu(val) (val)
#define be32_to_cpu(val) (val)
#define be64_to_cpu(val) (val)
#endif

#define roundup(x, y)  ((((x)+((y)-1))/(y))*(y))

struct memelfheader {
	unsigned ei_class;
	unsigned ei_data;
	unsigned e_type;
	unsigned e_machine;
	unsigned e_flags;
	unsigned e_phnum;
	unsigned e_notenum;
	unsigned long e_entry;
	struct memelfphdr *e_phdr;
	struct memelfnote *e_notes;
};

struct memelfphdr {
	uint64_t p_vaddr;
	uint64_t p_paddr;
	uint64_t p_filesz;
	uint64_t p_memsz;
	void *p_data;
	unsigned p_type;
	unsigned p_flags;
};

struct memelfnote {
	unsigned n_type;
	char *n_name;
	void *n_desc;
	unsigned n_descsz;
};

#define OPT_HELP		'h'
#define OPT_VERSION		'v'
#define OPT_TYPE		't'
#define OPT_KERNEL		256
#define OPT_OUTPUT		257
#define OPT_MAX			258

#define MKELF_OPTIONS \
	{ "help",		0, 0, OPT_HELP }, \
	{ "version", 		0, 0, OPT_VERSION }, \
	{ "kernel",		1, 0, OPT_KERNEL }, \
	{ "output",		1, 0, OPT_OUTPUT }, \
	{ "type",		1, 0, OPT_TYPE },

#define MKELF_OPT_STR "hvt:"

extern probe_t vmlinux_i386_probe;
extern probe_t bzImage_i386_probe;
extern probe_t linux_i386_probe;
extern mkelf_t linux_i386_mkelf;
extern usage_t linux_i386_usage;

extern probe_t linux_ia64_probe;
extern mkelf_t linux_ia64_mkelf;
extern usage_t linux_ia64_usage;

#endif /* MKELFIMAGE_H */
