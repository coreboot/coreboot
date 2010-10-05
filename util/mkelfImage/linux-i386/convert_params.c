#include <stdarg.h>
#include <limits.h>
#include "arch/io.h"
#include "stdint.h"
#include "uniform_boot.h"
#include "linuxbios_tables.h"
#include "elf_boot.h"
#include "convert.h"
#define STACK_SIZE (4096)

#define __unused __attribute__ ((unused))

long user_stack [STACK_SIZE] = { 0 };

unsigned long * stack_start = & user_stack[STACK_SIZE];

/* FIXME expand on drive_info_)struct... */
struct drive_info_struct {
	uint8_t dummy[32];
};
struct sys_desc_table {
	uint16_t length;
	uint8_t  table[318];
};

/*
 * These are set up by the setup-routine at boot-time:
 */

struct screen_info {
	uint8_t  orig_x;		/* 0x00 */
	uint8_t  orig_y;		/* 0x01 */
	uint16_t dontuse1;		/* 0x02 -- EXT_MEM_K sits here */
	uint16_t orig_video_page;	/* 0x04 */
	uint8_t  orig_video_mode;	/* 0x06 */
	uint8_t  orig_video_cols;	/* 0x07 */
	uint16_t unused2;		/* 0x08 */
	uint16_t orig_video_ega_bx;	/* 0x0a */
	uint16_t unused3;		/* 0x0c */
	uint8_t	 orig_video_lines;	/* 0x0e */
	uint8_t	 orig_video_isVGA;	/* 0x0f */
	uint16_t orig_video_points;	/* 0x10 */

	/* VESA graphic mode -- linear frame buffer */
	uint16_t lfb_width;		/* 0x12 */
	uint16_t lfb_height;		/* 0x14 */
	uint16_t lfb_depth;		/* 0x16 */
	uint32_t lfb_base;		/* 0x18 */
	uint32_t lfb_size;		/* 0x1c */
	uint16_t dontuse2, dontuse3;	/* 0x20 -- CL_MAGIC and CL_OFFSET here */
	uint16_t lfb_linelength;	/* 0x24 */
	uint8_t	 red_size;		/* 0x26 */
	uint8_t	 red_pos;		/* 0x27 */
	uint8_t	 green_size;		/* 0x28 */
	uint8_t	 green_pos;		/* 0x29 */
	uint8_t	 blue_size;		/* 0x2a */
	uint8_t	 blue_pos;		/* 0x2b */
	uint8_t	 rsvd_size;		/* 0x2c */
	uint8_t	 rsvd_pos;		/* 0x2d */
	uint16_t vesapm_seg;		/* 0x2e */
	uint16_t vesapm_off;		/* 0x30 */
	uint16_t pages;			/* 0x32 */
};


#define PAGE_SIZE 4096


#define E820MAP	0x2d0		/* our map */
#define E820MAX	32		/* number of entries in E820MAP */
#define E820NR	0x1e8		/* # entries in E820MAP */


struct e820entry {
	unsigned long long addr;	/* start of memory segment */
	unsigned long long size;	/* size of memory segment */
	unsigned long type;		/* type of memory segment */
#define E820_RAM	1
#define E820_RESERVED	2
#define E820_ACPI	3 /* usable as RAM once ACPI tables have been read */
#define E820_NVS	4
};

struct e820map {
    int nr_map;
	struct e820entry map[E820MAX];
};


struct apm_bios_info {
	uint16_t version;       /* 0x40 */
	uint16_t cseg;		/* 0x42 */
	uint32_t offset;	/* 0x44 */
	uint16_t cseg_16;	/* 0x48 */
	uint16_t dseg;		/* 0x4a */
	uint16_t flags;		/* 0x4c */
	uint16_t cseg_len;	/* 0x4e */
	uint16_t cseg_16_len;	/* 0x50 */
	uint16_t dseg_len;	/* 0x52 */
	uint8_t  reserved[44];	/* 0x54 */
};


struct parameters {
	uint8_t  orig_x;			/* 0x00 */
	uint8_t  orig_y;			/* 0x01 */
	uint16_t ext_mem_k;			/* 0x02 -- EXT_MEM_K sits here */
	uint16_t orig_video_page;		/* 0x04 */
	uint8_t  orig_video_mode;		/* 0x06 */
	uint8_t  orig_video_cols;		/* 0x07 */
	uint16_t unused2;			/* 0x08 */
	uint16_t orig_video_ega_bx;		/* 0x0a */
	uint16_t unused3;			/* 0x0c */
	uint8_t	 orig_video_lines;		/* 0x0e */
	uint8_t	 orig_video_isVGA;		/* 0x0f */
	uint16_t orig_video_points;		/* 0x10 */

	/* VESA graphic mode -- linear frame buffer */
	uint16_t lfb_width;			/* 0x12 */
	uint16_t lfb_height;			/* 0x14 */
	uint16_t lfb_depth;			/* 0x16 */
	uint32_t lfb_base;			/* 0x18 */
	uint32_t lfb_size;			/* 0x1c */
	uint16_t cl_magic;			/* 0x20 */
#define CL_MAGIC_VALUE 0xA33F
	uint16_t cl_offset;			/* 0x22 */
	uint16_t lfb_linelength;		/* 0x24 */
	uint8_t	 red_size;			/* 0x26 */
	uint8_t	 red_pos;			/* 0x27 */
	uint8_t	 green_size;			/* 0x28 */
	uint8_t	 green_pos;			/* 0x29 */
	uint8_t	 blue_size;			/* 0x2a */
	uint8_t	 blue_pos;			/* 0x2b */
	uint8_t	 rsvd_size;			/* 0x2c */
	uint8_t	 rsvd_pos;			/* 0x2d */
	uint16_t vesapm_seg;			/* 0x2e */
	uint16_t vesapm_off;			/* 0x30 */
	uint16_t pages;				/* 0x32 */
	uint8_t  reserved4[12];			/* 0x34 -- 0x3f reserved for future expansion */

	struct apm_bios_info apm_bios_info;	/* 0x40 */
	struct drive_info_struct drive_info;	/* 0x80 */
	struct sys_desc_table sys_desc_table;	/* 0xa0 */
	uint32_t alt_mem_k;			/* 0x1e0 */
	uint8_t  reserved5[4];			/* 0x1e4 */
	uint8_t  e820_map_nr;			/* 0x1e8 */
	uint8_t  reserved6[9];			/* 0x1e9 */
	uint16_t mount_root_rdonly;		/* 0x1f2 */
	uint8_t  reserved7[4];			/* 0x1f4 */
	uint16_t ramdisk_flags;			/* 0x1f8 */
#define RAMDISK_IMAGE_START_MASK  	0x07FF
#define RAMDISK_PROMPT_FLAG		0x8000
#define RAMDISK_LOAD_FLAG		0x4000
	uint8_t  reserved8[2];			/* 0x1fa */
	uint16_t orig_root_dev;			/* 0x1fc */
	uint8_t  reserved9[1];			/* 0x1fe */
	uint8_t  aux_device_info;		/* 0x1ff */
	uint8_t  reserved10[2];			/* 0x200 */
	uint8_t  param_block_signature[4];	/* 0x202 */
	uint16_t param_block_version;		/* 0x206 */
	uint8_t  reserved11[8];			/* 0x208 */
	uint8_t  loader_type;			/* 0x210 */
#define LOADER_TYPE_LOADLIN         1
#define LOADER_TYPE_BOOTSECT_LOADER 2
#define LOADER_TYPE_SYSLINUX        3
#define LOADER_TYPE_ETHERBOOT       4
#define LOADER_TYPE_KERNEL          5
	uint8_t  loader_flags;			/* 0x211 */
	uint8_t  reserved12[2];			/* 0x212 */
	uint32_t kernel_start;			/* 0x214 */
	uint32_t initrd_start;			/* 0x218 */
	uint32_t initrd_size;			/* 0x21c */
        uint8_t  reserved13[4];                 /* 0x220 */
        /* 2.01+ */
        uint16_t heap_end_ptr;                  /* 0x224 */
        uint8_t  reserved14[2];                 /* 0x226 */
        /* 2.02+ */
        uint32_t cmd_line_ptr;                  /* 0x228 */
        /* 2.03+ */
        uint32_t initrd_addr_max;               /* 0x22c */
	/* 2.05+ */
	uint32_t kernel_alignment;		/* 0x230 */
	uint8_t  relocateable_kernel;		/* 0x234 */
	uint8_t  reserved15[0x2d0 - 0x235];     /* 0x235 */

	struct e820entry e820_map[E820MAX];	/* 0x2d0 */
	uint8_t  reserved16[688];		/* 0x550 */
#define COMMAND_LINE_SIZE 256
	uint8_t  command_line[COMMAND_LINE_SIZE]; /* 0x800 */
	uint8_t  reserved17[1792];		/* 0x900 - 0x1000 */
};

/* Keep track of which information I need to acquire. */
struct param_info {
	unsigned type;
	void *data;
	Elf_Bhdr *param;
	struct image_parameters *image;
	struct parameters *real_mode;
	/* bootloader type */
	int has_multiboot;
	int has_uniform_boot;
	int has_elf_boot;
	/* firmware type */
	int has_pcbios;
	int has_linuxbios;
	struct lb_header *lb_table;
	/* machine information needed */
	int need_mem_sizes;
};
/*
 * This is set up by the setup-routine at boot-time
 */


#undef memcmp
#undef memset
#undef memcpy
#define memzero(s, n)     memset ((s), 0, (n))


/* FIXME handle systems with large EBDA's */
static struct parameters *faked_real_mode = (void *)REAL_MODE_DATA_LOC;



/*
 * Output
 * =============================================================================
 */

/* Base Address */
#define TTYS0 0x3f8
#define TTYS0_LSR (TTYS0+0x05)
#define TTYS0_TBR (TTYS0+0x00)

static void ttys0_tx_byte(unsigned byte)
{
	/* Wait until I can send a byte */
	while((inb(TTYS0_LSR) & 0x20) == 0)
		;
	outb(byte, TTYS0_TBR);
	/* Wait until the byte is transmitted */
	while(!(inb(TTYS0_LSR) & 0x40))
		;
}
static void put_char_serial(int c)
{
	if (c == '\n') {
		ttys0_tx_byte('\r');
	}
	ttys0_tx_byte(c);
}

static void putchar(int c)
{
#if 0
	put_char_video(c);
#endif
	put_char_serial(c);
}

#define LONG_LONG_SHIFT  ((int)((sizeof(unsigned long long)*CHAR_BIT) - 4))
#define LONG_SHIFT  ((int)((sizeof(unsigned long)*CHAR_BIT) - 4))
#define INT_SHIFT   ((int)((sizeof(unsigned int)*CHAR_BIT) - 4))
#define SHRT_SHIFT  ((int)((sizeof(unsigned short)*CHAR_BIT) - 4))
#define CHAR_SHIFT  ((int)((sizeof(unsigned char)*CHAR_BIT) - 4))

/**************************************************************************
PRINTF and friends

	Formats:
		%x	- 4 bytes int (8 hex digits, lower case)
		%X	- 4 bytes int (8 hex digits, upper case)
		%lx     - 8 bytes long (16 hex digits, lower case)
		%lX     - 8 bytes long (16 hex digits, upper case)
		%hx	- 2 bytes int (4 hex digits, lower case)
		%hX	- 2 bytes int (4 hex digits, upper case)
		%hhx	- 1 byte int (2 hex digits, lower case)
		%hhX	- 1 byte int (2 hex digits, upper case)
			- optional # prefixes 0x or 0X
		%d	- decimal int
		%c	- char
		%s	- string
	Note: width specification not supported
**************************************************************************/
static void printf(const char *fmt, ...)
{
	va_list args;
	char *p;
	va_start(args, fmt);
	for ( ; *fmt != '\0'; ++fmt) {
		if (*fmt != '%') {
			putchar(*fmt);
			continue;
		}
		if (*++fmt == 's') {
			for(p = va_arg(args, char *); *p != '\0'; p++)
				putchar(*p);
		}
		else {	/* Length of item is bounded */
			char tmp[40], *q = tmp;
			int shift = INT_SHIFT;
			if (*fmt == 'L') {
				shift = LONG_LONG_SHIFT;
				fmt++;
			}
			else if (*fmt == 'l') {
				shift = LONG_SHIFT;
				fmt++;
			}
			else if (*fmt == 'h') {
				shift = SHRT_SHIFT;
				fmt++;
				if (*fmt == 'h') {
					shift = CHAR_SHIFT;
					fmt++;
				}
			}

			/*
			 * Before each format q points to tmp buffer
			 * After each format q points past end of item
			 */
			if ((*fmt | 0x20) == 'x') {
				/* With x86 gcc, sizeof(long) == sizeof(int) */
				unsigned long long h;
				int ncase;
				if (shift > LONG_SHIFT) {
					h = va_arg(args, unsigned long long);
				}
				else if (shift > INT_SHIFT) {
					h = va_arg(args, unsigned long);
				} else {
					h = va_arg(args, unsigned int);
				}
				ncase = (*fmt & 0x20);
				for ( ; shift >= 0; shift -= 4)
					*q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
			}
			else if (*fmt == 'd') {
				char *r;
				long i;
				if (shift > LONG_SHIFT) {
					i = va_arg(args, long long);
				}
				else if (shift > INT_SHIFT) {
					i = va_arg(args, long);
				} else {
					i = va_arg(args, int);
				}
				if (i < 0) {
					*q++ = '-';
					i = -i;
				}
				p = q;		/* save beginning of digits */
				do {
					*q++ = '0' + (i % 10);
					i /= 10;
				} while (i);
				/* reverse digits, stop in middle */
				r = q;		/* don't alter q */
				while (--r > p) {
					i = *r;
					*r = *p;
					*p++ = i;
				}
			}
			else if (*fmt == 'c')
				*q++ = va_arg(args, int);
			else
				*q++ = *fmt;
			/* now output the saved string */
			for (p = tmp; p < q; ++p)
				putchar(*p);
		}
	}
	va_end(args);
}

/*
 * String Functions
 * =============================================================================
 */


size_t strnlen(const char *s, size_t max)
{
	size_t len = 0;
	while(len < max && *s) {
		len++;
		s++;
	}
	return len;
}

void* memset(void* s, int c, size_t n)
{
	size_t i;
	char *ss = (char*)s;

	for (i=0;i<n;i++) ss[i] = c;
	return s;
}

void* memcpy(void *dest, const void *src, size_t len)
{
	size_t i;
	unsigned char *d;
	const unsigned char *s;
	d = dest;
	s = src;

	for (i=0; i < len; i++)
		d[i] = s[i];

	return dest;
}

int memcmp(void *src1, void *src2, size_t len)
{
	unsigned char *s1, *s2;
	size_t i;
	s1 = src1;
	s2 = src2;
	for(i = 0; i < len; i++) {
		if (*s1 != *s2) {
			return *s2 - *s1;
		}
	}
	return 0;

}

/*
 * Checksum functions
 * =============================================================================
 */


static unsigned long checksum_partial(unsigned long sum,
	void *addr, unsigned long length)
{
	uint8_t *ptr;
	volatile union {
		uint8_t  byte[2];
		uint16_t word;
	} value;
	unsigned long i;
	/* In the most straight forward way possible,
	 * compute an ip style checksum.
	 */
	sum = 0;
	ptr = addr;
	for(i = 0; i < length; i++) {
		unsigned long value;
		value = ptr[i];
		if (i & 1) {
			value <<= 8;
		}
		/* Add the new value */
		sum += value;
		/* Wrap around the carry */
		if (sum > 0xFFFF) {
			sum = (sum + (sum >> 16)) & 0xFFFF;
		}
	}
	value.byte[0] = sum & 0xff;
	value.byte[1] = (sum >> 8) & 0xff;
	return value.word & 0xFFFF;

}

static unsigned long checksum_final(unsigned long partial)
{
	return (~partial) & 0xFFFF;
}

static unsigned long compute_checksum(void *vaddr, unsigned long length)
{
	return checksum_final(checksum_partial(0, vaddr, length));
}

/*
 * Helper functions
 * =============================================================================
 */


void append_command_line(struct parameters *real_mode, char *arg, int arg_bytes)
{
	int len, max;
	char *dest;
	/* skip over what has already been set */
	len = strnlen(real_mode->command_line, sizeof(real_mode->command_line));
	dest = real_mode->command_line + len;
	max = sizeof(real_mode->command_line) - 1;
	if (max < 1) {
		/* No room to append anything :( */
		return;
	}
	/* Add a space in between strings */
	*dest++ = ' ';
	/* Append the added command line */
	max = sizeof(real_mode->command_line) - 1;
	if (max > arg_bytes) {
		max = arg_bytes;
	}
	len = strnlen(arg, max);
	memcpy(dest, arg, len);
	dest += len;
	/* Null terminate the string */
	*dest++ = '\0';
}

static void set_memsize_k(struct parameters *real_mode, unsigned long mem_k)
{
	/* ALT_MEM_K maxes out at 4GB */
	if (mem_k > 0x3fffff) {
		mem_k = 0x3fffff;
	}
	if (mem_k > (real_mode->alt_mem_k + (1 << 10))) {
		/* Use our memory size less 1M */
		real_mode->alt_mem_k = mem_k - (1 << 10);
		real_mode->ext_mem_k = mem_k - (1 << 10);
		if ((mem_k - (1 << 10)) > 0xFFFF) {
			real_mode->ext_mem_k = 0xFC00; /* 64 M */
		}
	}
}

static void add_e820_map(struct parameters *real_mode,
	unsigned long long addr, unsigned long long size,
	unsigned long type)
{
	unsigned long long high;
	unsigned long mem_k;
	int i;
	i = real_mode->e820_map_nr;
	if (i < E820MAX) {
		real_mode->e820_map[i].addr = addr;
		real_mode->e820_map[i].size = size;
		real_mode->e820_map[i].type = type;
		real_mode->e820_map_nr++;
	}
	/* policy I assume that for the legacy memory
	 * variables memory is contiguous.
	 */
	if (type == E820_RAM) {
		high = addr + size;
		if (high >= 0x40000000000ULL) {
			mem_k = 0xFFFFFFFF;
		} else {
			mem_k = high >> 10;
		}
		set_memsize_k(real_mode, mem_k);
	}
}

/*
 * Multiboot
 * =============================================================================
 */

#define MULTI_MEM_DEBUG 0
#if MULTI_MEM_DEBUG
#define multi_puts(x) printf("%s", x)
#define multi_put_hex(x) printf("%x", x)
#define multi_put_lhex(x) printf("%Lx", x)
#else
#define multi_puts(x)
#define multi_put_hex(x)
#define multi_put_lhex(x)
#endif /* MULTI_MEM_DEBUG */

/* Multiboot Specification */
struct multiboot_mods {
	unsigned mod_start;
	unsigned mod_end;
	unsigned char *string;
	unsigned reserved;
};

struct memory_segment {
	unsigned long long addr;
	unsigned long long size;
	unsigned type;
};

struct multiboot_info {
        unsigned flags;
#define MULTIBOOT_MEM_VALID       0x01
#define MULTIBOOT_BOOT_DEV_VALID  0x02
#define MULTIBOOT_CMDLINE_VALID   0x04
#define MULTIBOOT_MODS_VALID      0x08
#define MULTIBOOT_AOUT_SYMS_VALID 0x10
#define MULTIBOOT_ELF_SYMS_VALID  0x20
#define MULTIBOOT_MMAP_VALID      0x40
	unsigned mem_lower;
	unsigned mem_upper;
	unsigned char boot_device[4];
	void *command_line;
	unsigned mods_count;
	struct multiboot_mods *mods_addr;
	unsigned syms_num;
	unsigned syms_size;
	unsigned syms_addr;
	unsigned syms_shndx;
	unsigned mmap_length;
	struct memory_segment *mmap_addr;
};

#define MULTIBOOT_MAX_COMMAND_LINE 0xFFFFFFFF

static void convert_multiboot_memmap(
	struct parameters *real_mode,
	struct multiboot_info *info)
{
	unsigned size;
	unsigned *size_addr;
#define next_seg(seg, size) ((struct memory_segment *)((char *)(seg) + (size)))
	struct memory_segment *seg, *end;

	seg = info->mmap_addr;
	end  = (void *)(((char *)seg) + info->mmap_length);
	size_addr = (unsigned *)(((char *)seg) - 4);
	size = *size_addr;
	multi_puts("mmap_addr: "); multi_put_hex((unsigned)info->mmap_addr); multi_puts("\n");
	multi_puts("mmap_length: "); multi_put_hex(info->mmap_length); multi_puts("\n");
	multi_puts("size_addr: "); multi_put_hex((unsigned)size_addr); multi_puts("\n");
	multi_puts("size: "); multi_put_hex(size); multi_puts("\n");
	multi_puts("end: "); multi_put_hex((unsigned)end); multi_puts("\n");
	for(seg = info->mmap_addr; (seg < end); seg = next_seg(seg,size)) {
		multi_puts("multi-mem: ");
		multi_put_lhex(seg->size);
		multi_puts(" @ ");
		multi_put_lhex(seg->addr);
		multi_puts(" (");
		switch(seg->type) {
		case E820_RAM:
			multi_puts("ram");
			break;
		case E820_ACPI:
			multi_puts("ACPI data");
			break;
		case E820_NVS:
			multi_puts("ACPI NVS");
			break;
		case E820_RESERVED:
		default:
			multi_puts("reserved");
			break;
		}
		multi_puts(")\n");
		add_e820_map(real_mode, seg->addr, seg->size, seg->type);
	}
#undef next_seg
}

static void convert_multiboot(
	struct param_info *info, struct multiboot_info *mb_info)
{
	if (info->need_mem_sizes && (mb_info->flags & MULTIBOOT_MEM_VALID)) {
		/* info->memory is short 1M */
		set_memsize_k(info->real_mode, mb_info->mem_upper + (1 << 10));
	}
	if (mb_info->flags & MULTIBOOT_CMDLINE_VALID) {
		append_command_line(info->real_mode, mb_info->command_line,
			MULTIBOOT_MAX_COMMAND_LINE);
	}
	if (info->need_mem_sizes && (mb_info->flags & MULTIBOOT_MMAP_VALID)) {
		convert_multiboot_memmap(info->real_mode, mb_info);
	}
	if (mb_info->flags & (MULTIBOOT_MEM_VALID | MULTIBOOT_MMAP_VALID)) {
		info->need_mem_sizes = 0;
	}
}


/*
 * Uniform Boot Environment
 * =============================================================================
 */

#define UBE_MEM_DEBUG 0
#if UBE_MEM_DEBUG
#define ube_puts(x) printf("%s", x)
#define ube_put_hex(x) printf("%x", x)
#define ube_put_lhex(x) printf("%Lx", x)
#else
#define ube_puts(x)
#define ube_put_hex(x)
#define ube_put_lhex(x)
#endif /* UBE_MEM_DEBUG */
static void convert_uniform_boot_memory(
	struct parameters *real_mode, struct ube_memory *mem)
{
	int i;
	int entries;
	unsigned long mem_k;
	mem_k = 0;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	for(i = 0; (i < entries) && (i < E820MAX); i++) {
		unsigned long type;
		ube_puts("ube-mem: ");
		ube_put_lhex(mem->map[i].size);
		ube_puts(" @ ");
		ube_put_lhex(mem->map[i].start);
		ube_puts(" (");
		switch(mem->map[i].type) {
		case UBE_MEM_RAM:
			type = E820_RAM;
			ube_puts("ram");
			break;
		case UBE_MEM_ACPI:
			type = E820_ACPI;
			ube_puts("ACPI data");
			break;
		case UBE_MEM_NVS:
			type = E820_NVS;
			ube_puts("ACPI NVS");
			break;
		case UBE_MEM_RESERVED:
		default:
			type = E820_RESERVED;
			ube_puts("reserved");
			break;
		}
		ube_puts(")\n");
		add_e820_map(real_mode,
			mem->map[i].start, mem->map[i].size, type);
	}
}

static void convert_uniform_boot(struct param_info *info,
	struct uniform_boot_header *header)
{
	/* Uniform boot environment */
	unsigned long env_bytes;
	char *env;
	if (header->arg_bytes) {
		append_command_line(info->real_mode, (void *)(header->arg), header->arg_bytes);
	}
	env = (void *)(header->env);
	env_bytes = header->env_bytes;
	while(env_bytes) {
		struct ube_record *record;
		record = (void *)env;
		if (record->tag == UBE_TAG_MEMORY) {
			if (info->need_mem_sizes) {
				convert_uniform_boot_memory(info->real_mode, (void *)record);
				info->need_mem_sizes = 0;
			}
		}
		env += record->size;
		env_bytes -= record->size;
	}
}




/*
 * Hardware
 * =============================================================================
 */

/* we're getting screwed again and again by this problem of the 8259.
 * so we're going to leave this lying around for inclusion into
 * crt0.S on an as-needed basis.
 * well, that went ok, I hope. Now we have to reprogram the interrupts :-(
 * we put them right after the intel-reserved hardware interrupts, at
 * int 0x20-0x2F. There they won't mess up anything. Sadly IBM really
 * messed this up with the original PC, and they haven't been able to
 * rectify it afterwards. Thus the bios puts interrupts at 0x08-0x0f,
 * which is used for the internal hardware interrupts as well. We just
 * have to reprogram the 8259's, and it isn't fun.
 */

static void setup_i8259(void)
{
	outb(0x11, 0x20);		/*! initialization sequence to 8259A-1*/
	outb(0x11, 0xA0);		/*! and to 8259A-2*/

	outb(0x20, 0x21);		/*! start of hardware int's (0x20)*/
	outb(0x28, 0xA1);		/*! start of hardware int's 2 (0x28)*/

	outb(0x04, 0x21);		/*! 8259-1 is master*/
	outb(0x02, 0xA1);		/*! 8259-2 is slave*/

	outb(0x01, 0x21);		/*! 8086 mode for both*/
	outb(0x01, 0xA1);

	outb(0xFF, 0xA1);		/*! mask off all interrupts for now*/
	outb(0xFB, 0x21);		/*! mask all irq's but irq2 which is cascaded*/
}

static void hardware_setup(struct param_info *info __unused)
{
	/* Disable nmi */
	outb(0x80, 0x70);

	/* Make sure any coprocessor is properly reset.. */
	outb(0, 0xf0);
	outb(0, 0xf1);

	setup_i8259();
}


/*
 * ELF Boot loader
 * =============================================================================
 */

static int count_elf_notes(Elf_Bhdr *bhdr)
{
	unsigned char *note, *end;
	int count;
	count = 0;
	note = ((char *)bhdr) + sizeof(*bhdr);
	end = ((char *)bhdr) + bhdr->b_size;
#if 0
	printf("count_elf_notes %lx\n", (unsigned long)bhdr);
#endif
	while (note < end) {
		Elf_Nhdr *hdr;
		unsigned char *n_name, *n_desc, *next;
		hdr = (Elf_Nhdr *)note;
		n_name = note + sizeof(*hdr);
		n_desc = n_name + ((hdr->n_namesz + 3) & ~3);
		next = n_desc + ((hdr->n_descsz + 3) & ~3);
#if 0
		printf("elf_note = %lx\n", (unsigned long)note);
		printf("elf_namesz = %x\n", hdr->n_namesz);
		printf("elf_descsz = %x\n", hdr->n_descsz);
		printf("elf_type   = %x\n", hdr->n_type);
		printf("elf_name = %lx\n", (unsigned long)n_name);
		printf("elf_desc = %lx\n", (unsigned long)n_desc);
#endif
		if (next > end)
			break;
		count++;
		note = next;
	}
	return count;
}

static Elf_Nhdr *find_elf_note(Elf_Bhdr *bhdr,
	Elf_Word namesz, unsigned char *name, Elf_Word type)
{
	unsigned char *note, *end;
	note = ((char *)bhdr) + sizeof(*bhdr);
	end = ((char *)bhdr) + bhdr->b_size;
	while(note < end) {
		Elf_Nhdr *hdr;
		unsigned char *n_name, *n_desc, *next;
		hdr = (Elf_Nhdr *)note;
		n_name = note + sizeof(*hdr);
		n_desc = n_name + ((hdr->n_namesz + 3) & ~3);
		next = n_desc + ((hdr->n_descsz + 3) & ~3);
		if (next > end)
			break;
		if ((hdr->n_type == type) &&
			(hdr->n_namesz == namesz) &&
			(memcmp(n_name, name, namesz) == 0)) {
			return hdr;
		}
		note = next;
	}
	return 0;
}

static void convert_elf_command_line(struct param_info *info,
	Elf_Word descsz, unsigned char *desc)
{
	append_command_line(info->real_mode, desc, descsz);
}

struct {
	Elf_Word namesz;
	unsigned char *name;
	Elf_Word type;
	void (*convert)(struct param_info *info, Elf_Word descsz, unsigned char *desc);
} elf_notes[] =
{
	{ 0, "", EBN_COMMAND_LINE, convert_elf_command_line },
};

static void convert_elf_boot(struct param_info *info, Elf_Bhdr *bhdr)
{
	unsigned char *note, *end;
	note = ((char *)bhdr) + sizeof(*bhdr);
	end = ((char *)bhdr) + bhdr->b_size;
	while(note < end) {
		Elf_Nhdr *hdr;
		unsigned char *n_name, *n_desc, *next;
		size_t i;
		hdr = (Elf_Nhdr *)note;
		n_name = note + sizeof(*hdr);
		n_desc = n_name + ((hdr->n_namesz + 3) & ~3);
		next = n_desc + ((hdr->n_descsz + 3) & ~3);
		if (next > end)
			break;
		for(i = 0; i < sizeof(elf_notes)/sizeof(elf_notes[0]); i++) {
			if ((hdr->n_type == elf_notes[i].type) &&
				(hdr->n_namesz == elf_notes[i].namesz) &&
				(memcmp(n_name, elf_notes[i].name, elf_notes[i].namesz) == 0)) {
				elf_notes[i].convert(info, hdr->n_descsz, n_desc);
				break;
			}
		}
		note = next;
	}
}

/*
 * LinuxBIOS
 * =============================================================================
 */

#define LB_MEM_DEBUG 0
#if LB_MEM_DEBUG
#define lb_puts(x) printf("%s", x)
#define lb_put_hex(x) printf("%x", x)
#define lb_put_lhex(x) printf("%Lx", x)
#else
#define lb_puts(x)
#define lb_put_hex(x)
#define lb_put_lhex(x)
#endif /* LB_MEM_DEBUG */

static unsigned count_lb_records(void *start, unsigned long length)
{
	struct lb_record *rec;
	void *end;
	unsigned count;
	count = 0;
	end = ((char *)start) + length;
	for(rec = start; ((void *)rec < end) &&
		(rec->size <= (unsigned long)(end - (void *)rec));
		rec = (void *)(((char *)rec) + rec->size)) {
		count++;
	}
	return count;
}

static struct lb_header *__find_lb_table(void *start, void *end)
{
	unsigned char *ptr;
	/* For now be stupid.... */
	for(ptr = start; (void *)ptr < end; ptr += 16) {
		struct lb_header *head = (void *)ptr;
		if ((head->signature[0] == 'L') &&
			(head->signature[1] == 'B') &&
			(head->signature[2] == 'I') &&
			(head->signature[3] == 'O') &&
			(head->header_bytes == sizeof(*head)) &&
			(compute_checksum(head, sizeof(*head)) == 0) &&
			(compute_checksum(ptr + sizeof(*head), head->table_bytes) ==
				head->table_checksum) &&
			(count_lb_records(ptr + sizeof(*head), head->table_bytes) ==
				head->table_entries)
			) {
			return head;
		}
	};
	return 0;
}

static int find_lb_table(struct param_info *info)
{
	struct lb_header *head;
	head = 0;
	if (!head) {
		/* First try at address 0 */
		head = __find_lb_table((void *)0x00000, (void *)0x1000);
	}
	if (!head) {
		/* Then try at address 0xf0000 */
		head = __find_lb_table((void *)0xf0000, (void *)0x100000);
	}
	if (head) {
		struct lb_forward *forward = (struct lb_forward *)(((char *)head) + head->header_bytes);
		if (forward->tag == LB_TAG_FORWARD) {
			head = __find_lb_table(forward->forward,
					forward->forward + 0x1000);
		}
	}
	if (head) {
		info->has_linuxbios = 1;
		info->lb_table = head;
		return 1;
	}
	return 0;
}

static void convert_lb_memory(struct param_info *info, struct lb_memory *mem)
{
	int i;
	int entries;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	for(i = 0; (i < entries) && (i < E820MAX); i++) {
		unsigned long type;
		unsigned long long end;
		end = mem->map[i].start + mem->map[i].size;
		lb_puts("lb-mem: ");
		lb_put_lhex(mem->map[i].start);
		lb_puts(" - ");
		lb_put_lhex(end);
		lb_puts(" (");
		switch(mem->map[i].type) {
		case LB_MEM_RAM:
			type = E820_RAM;
			lb_puts("ram");
			break;
		default:
			type = E820_RESERVED;
			lb_puts("reserved");
			break;
		}
		lb_puts(")\n");
		add_e820_map(info->real_mode,
			mem->map[i].start, mem->map[i].size, type);
	}
	info->need_mem_sizes = 0;
}

static void query_lb_values(struct param_info *info)
{
	struct lb_header *head;
	struct lb_record *rec;
	void *start, *end;
	head = info->lb_table;
	start = ((unsigned char *)head) + sizeof(*head);
	end = ((char *)start) + head->table_bytes;
	for(rec = start; ((void *)rec < end) &&
		(rec->size <= (unsigned long)(end - (void *)rec));
		rec = (void *)(((char *)rec) + rec->size)) {
		switch(rec->tag) {
		case LB_TAG_MEMORY:
		{
			struct lb_memory *mem;
			mem = (struct lb_memory *) rec;
			convert_lb_memory(info, mem);
			break;
		}
		default:
			break;
		};
	}
}

/*
 * PCBIOS
 * =============================================================================
 */
#define PC_MEM_DEBUG 0
#if PC_MEM_DEBUG
#define pc_puts(x) printf("%s", x)
#define pc_put_hex(x) printf("%x", x)
#define pc_put_lhex(x) printf("%Lx", x)
#else
#define pc_puts(x)
#define pc_put_hex(x)
#define pc_put_lhex(x)
#endif /* PC_MEM_DEBUG */

/* functions for querying the pcbios */
extern void noop(void); /* for testing purposes only */
extern int meme820(struct e820entry *buf, int count);
extern unsigned int meme801(void);
extern unsigned short mem88(void);
extern unsigned short basememsize(void);

struct meminfo {
	int map_count;
	struct e820entry map[E820MAX];
};

static struct meminfo meminfo;
static void get_meminfo(struct param_info *info)
{
	int i;
	pc_puts("getting meminfo...\n");
	meminfo.map_count = meme820(meminfo.map, E820MAX);
	pc_puts("got meminfo count="); pc_put_hex(meminfo.map_count); pc_puts("\n");
	for(i = 0; i < meminfo.map_count; i++) {
		unsigned long long end;
		struct e820entry *seg = meminfo.map + i;
		end = seg->addr + seg->size;
		pc_puts("BIOS-e820: ");
		pc_put_lhex(seg->addr);
		pc_puts(" - ");
		pc_put_lhex(end);
		pc_puts(" (");
		switch(seg->type) {
		case E820_RAM:
			pc_puts("ram");
			info->need_mem_sizes = 0;
			break;
		case E820_ACPI:
			pc_puts("ACPI data");
			break;
		case E820_NVS:
			pc_puts("ACPI NVS");
			break;
		case E820_RESERVED:
		default:
			pc_puts("reserved");
			break;
		}
		pc_puts(")\n");
		add_e820_map(info->real_mode,
			seg->addr, seg->size, seg->type);
	}
	info->real_mode->alt_mem_k = meme801();
	info->real_mode->ext_mem_k = mem88();
	if (info->real_mode->alt_mem_k || info->real_mode->ext_mem_k) {
		info->need_mem_sizes = 0;
	}
}

static void query_pcbios_values(struct param_info *info)
{
	get_meminfo(info);
}

/*
 * Bootloaders
 * =============================================================================
 */


static void query_bootloader_param_class(struct param_info *info)
{
	int has_bootloader_type = 0;
	Elf_Bhdr *hdr = 0;
	if (!has_bootloader_type && (info->type == 0x2BADB002)) {
		/* Orignal multiboot specification */
		info->has_multiboot = 1;
		has_bootloader_type = 1;
	}
	if (!has_bootloader_type && (info->type == 0x0A11B007)) {
		/* Uniform boot proposal */
		unsigned long checksum;
		struct uniform_boot_header *header;
		header = info->data;
		checksum = compute_checksum(header, header->header_bytes);
		if (checksum == 0) {
			info->has_uniform_boot = 1;
			has_bootloader_type = 1;
		} else{
			printf("Bad uniform boot header checksum!\n");
		}
	}
	if (info->type == ELF_BHDR_MAGIC) {
		hdr = info->data;
	}
	if (info->param && (info->param->b_signature == ELF_BHDR_MAGIC)) {
		hdr = info->param;
	}
	if (!has_bootloader_type && hdr) {
		/* Good ELF boot proposal... */
		unsigned long checksum;
		int count;
		checksum = 0;
		if (hdr->b_checksum != 0) {
			checksum = compute_checksum(hdr, hdr->b_size);
		}
		count = count_elf_notes(hdr);
		if ((hdr->b_signature == ELF_BHDR_MAGIC) &&
			(checksum == 0) &&
			hdr->b_records == count) {
			info->has_elf_boot = 1;
			info->param = hdr;
			has_bootloader_type = 1;
		}
		else {
			printf("Bad ELF parameter table!\n");
			printf("   checksum = %x\n", checksum);
			printf("      count = %x\n", count);
			printf("        hdr = %x\n", (unsigned long)hdr);
			printf("     b_size = %x\n", hdr->b_size);
			printf("b_signature = %x\n", hdr->b_signature);
			printf("  b_records = %x\n", hdr->b_records);
		}
	}
	if (!has_bootloader_type) {
		printf("Unknown bootloader class!\n");
		printf("type=%x\n", info->type);
		printf("data=%x\n", (unsigned)info->data);
		printf("param=%x\n", (unsigned)info->param);
	}
}

static void query_bootloader_values(struct param_info *info)
{
	if (info->has_multiboot) {
		convert_multiboot(info, info->data);
	}
	else if (info->has_uniform_boot) {
		convert_uniform_boot(info, info->data);
	}
	else if (info->has_elf_boot) {
		convert_elf_boot(info, info->param);
	}
}

/*
 * Firmware
 * =============================================================================
 */

static int bootloader_query_firmware_class(struct param_info *info)
{
	Elf_Nhdr *hdr;
	unsigned char *note, *n_name, *n_desc;
	int detected_firmware_type;
	if (!info->has_elf_boot) {
		/* Only the elf boot tables gives us a firmware type */
		return 0;
	}
	detected_firmware_type = 0;
	n_desc = 0;

	hdr = find_elf_note(info->param, 0, 0, EBN_FIRMWARE_TYPE);
	if (!hdr) {
		/* If I'm not explicitly told the firmware type
		 * do my best to guess it for myself.
		 */
		detected_firmware_type = 0;
	} else {
		note = (char *)hdr;
		n_name = note + sizeof(*hdr);
		n_desc = n_name + ((hdr->n_namesz + 3) & ~3);
	}
	if (!detected_firmware_type && hdr &&
		(hdr->n_descsz == 7) &&
		(memcmp(n_desc, "PCBIOS", 7) == 0)) {
		info->has_pcbios = 1;
		detected_firmware_type = 1;
	}
	if (!detected_firmware_type && hdr &&
		(hdr->n_descsz == 10) &&
		(memcmp(n_desc, "LinuxBIOS", 10) == 0)) {
		/* Don't believe I'm linuxBIOS unless I can
		 * find the linuxBIOS table..
		 */
		detected_firmware_type = find_lb_table(info);
	}
	if (!detected_firmware_type && hdr &&
		(hdr->n_descsz == 0)) {
		/* No firmware is present */
		detected_firmware_type = 1;
	}
	if (!detected_firmware_type && hdr &&
		(hdr->n_descsz == 1) &&
		(memcmp(n_desc, "", 1) == 0)) {
		/* No firmware is present */
		detected_firmware_type = 1;
	}
	if (!detected_firmware_type && hdr) {
		printf("Unknown firmware type: %s\n", n_desc);
	}
	return detected_firmware_type;
}

static void query_firmware_class(struct param_info *info)
{
	int detected_firmware_type = 0;

	/* First say I have no firmware at all... */
	info->has_pcbios = 0;
	info->has_linuxbios = 0;

	/* See if the bootloader has told us what
	 * kind of firmware we are running on.
	 */
	detected_firmware_type = bootloader_query_firmware_class(info);

	/* See if we can detect linuxbios. */
	if (!detected_firmware_type) {
		detected_firmware_type = find_lb_table(info);
	}

	if (!detected_firmware_type) {
		/* if all else fails assume a standard pcbios... */
		info->has_pcbios = 1;
	}

	/* Now print out the firmware type... */
	printf("Firmware type:");
	if (info->has_linuxbios) {
		printf(" LinuxBIOS");
	}
	if (info->has_pcbios) {
		printf(" PCBIOS");
	}
	printf("\n");
}

static void query_firmware_values(struct param_info *info)
{
	if (info->has_linuxbios) {
		query_lb_values(info);
	}
	if (info->has_pcbios) {
		query_pcbios_values(info);
	}

}

/*
 * Debug
 * =============================================================================
 */

#if 0
static void print_offsets(void)
{
	struct parameters *real_mode = 0;
	printf("print_offsets\n");

	printf("orig_x               =%x\n", (uint32_t)&real_mode->orig_x);
	printf("orig_y               =%x\n", (uint32_t)&real_mode->orig_y);
	printf("ext_mem_k            =%x\n", (uint32_t)&real_mode->ext_mem_k);
	printf("orig_video_page      =%x\n", (uint32_t)&real_mode->orig_video_page);
	printf("orig_video_mode      =%x\n", (uint32_t)&real_mode->orig_video_mode);
	printf("orig_video_cols      =%x\n", (uint32_t)&real_mode->orig_video_cols);
	printf("unused2              =%x\n", (uint32_t)&real_mode->unused2);
	printf("orig_video_ega_bx    =%x\n", (uint32_t)&real_mode->orig_video_ega_bx);
	printf("unused3              =%x\n", (uint32_t)&real_mode->unused3);
	printf("orig_video_lines     =%x\n", (uint32_t)&real_mode->orig_video_lines);
	printf("orig_video_isVGA     =%x\n", (uint32_t)&real_mode->orig_video_isVGA);
	printf("orig_video_points    =%x\n", (uint32_t)&real_mode->orig_video_points);
	printf("lfb_width            =%x\n", (uint32_t)&real_mode->lfb_width);
	printf("lfb_height           =%x\n", (uint32_t)&real_mode->lfb_height);
	printf("lfb_depth            =%x\n", (uint32_t)&real_mode->lfb_depth);
	printf("lfb_base             =%x\n", (uint32_t)&real_mode->lfb_base);
	printf("lfb_size             =%x\n", (uint32_t)&real_mode->lfb_size);
	printf("cl_magic             =%x\n", (uint32_t)&real_mode->cl_magic);
	printf("cl_offset            =%x\n", (uint32_t)&real_mode->cl_offset);
	printf("lfb_linelength       =%x\n", (uint32_t)&real_mode->lfb_linelength);
	printf("red_size             =%x\n", (uint32_t)&real_mode->red_size);
	printf("red_pos              =%x\n", (uint32_t)&real_mode->red_pos);
	printf("green_size           =%x\n", (uint32_t)&real_mode->green_size);
	printf("green_pos            =%x\n", (uint32_t)&real_mode->green_pos);
	printf("blue_size            =%x\n", (uint32_t)&real_mode->blue_size);
	printf("blue_pos             =%x\n", (uint32_t)&real_mode->blue_pos);
	printf("rsvd_size            =%x\n", (uint32_t)&real_mode->rsvd_size);
	printf("rsvd_pos             =%x\n", (uint32_t)&real_mode->rsvd_pos);
	printf("vesapm_seg           =%x\n", (uint32_t)&real_mode->vesapm_seg);
	printf("vesapm_off           =%x\n", (uint32_t)&real_mode->vesapm_off);
	printf("pages                =%x\n", (uint32_t)&real_mode->pages);
	printf("reserved4            =%x\n", (uint32_t)&real_mode->reserved4);
	printf("apm_bios_info        =%x\n", (uint32_t)&real_mode->apm_bios_info);
	printf("drive_info           =%x\n", (uint32_t)&real_mode->drive_info);
	printf("sys_desc_table       =%x\n", (uint32_t)&real_mode->sys_desc_table);
	printf("alt_mem_k            =%x\n", (uint32_t)&real_mode->alt_mem_k);
	printf("reserved5            =%x\n", (uint32_t)&real_mode->reserved5);
	printf("e820_map_nr          =%x\n", (uint32_t)&real_mode->e820_map_nr);
	printf("reserved6            =%x\n", (uint32_t)&real_mode->reserved6);
	printf("mount_root_rdonly    =%x\n", (uint32_t)&real_mode->mount_root_rdonly);
	printf("reserved7            =%x\n", (uint32_t)&real_mode->reserved7);
	printf("ramdisk_flags        =%x\n", (uint32_t)&real_mode->ramdisk_flags);
	printf("reserved8            =%x\n", (uint32_t)&real_mode->reserved8);
	printf("orig_root_dev        =%x\n", (uint32_t)&real_mode->orig_root_dev);
	printf("reserved9            =%x\n", (uint32_t)&real_mode->reserved9);
	printf("aux_device_info      =%x\n", (uint32_t)&real_mode->aux_device_info);
	printf("reserved10           =%x\n", (uint32_t)&real_mode->reserved10);
	printf("param_block_signature=%x\n", (uint32_t)&real_mode->param_block_signature);
	printf("param_block_version  =%x\n", (uint32_t)&real_mode->param_block_version);
	printf("reserved11           =%x\n", (uint32_t)&real_mode->reserved11);
	printf("loader_type          =%x\n", (uint32_t)&real_mode->loader_type);
	printf("loader_flags         =%x\n", (uint32_t)&real_mode->loader_flags);
	printf("reserved12           =%x\n", (uint32_t)&real_mode->reserved12);
	printf("kernel_start         =%x\n", (uint32_t)&real_mode->kernel_start);
	printf("initrd_start         =%x\n", (uint32_t)&real_mode->initrd_start);
	printf("initrd_size          =%x\n", (uint32_t)&real_mode->initrd_size);
	printf("reserved13           =%x\n", (uint32_t)&real_mode->reserved13);
	printf("e820_map             =%x\n", (uint32_t)&real_mode->e820_map);
	printf("reserved16           =%x\n", (uint32_t)&real_mode->reserved16);
	printf("command_line         =%x\n", (uint32_t)&real_mode->command_line);
	printf("reserved17           =%x\n", (uint32_t)&real_mode->reserved17);
}

static void print_linux_params(struct param_info *info)
{
	int i;

	printf("print_linux_params\n");
	/* Default screen size */
	printf("orig_x           =%x\n", info->real_mode->orig_x);
	printf("orig_y           =%x\n", info->real_mode->orig_y);
	printf("orig_video_page  =%x\n", info->real_mode->orig_video_page);
	printf("orig_video_mode  =%x\n", info->real_mode->orig_video_mode);
	printf("orig_video_cols  =%x\n", info->real_mode->orig_video_cols);
	printf("orig_video_lines =%x\n", info->real_mode->orig_video_lines);
	printf("orig_video_ega_bx=%x\n", info->real_mode->orig_video_ega_bx);
	printf("orig_video_isVGA =%x\n", info->real_mode->orig_video_isVGA);
	printf("orig_video_points=%x\n", info->real_mode->orig_video_points);


	/* System descriptor table... */
	printf("sys_dest_table_len=%x\n", info->real_mode->sys_desc_table.length);

	/* Memory sizes */
	printf("ext_mem_k        =%x\n", info->real_mode->ext_mem_k);
	printf("alt_mem_k        =%x\n", info->real_mode->alt_mem_k);
	printf("e820_map_nr      =%x\n", info->real_mode->e820_map_nr);
	for(i = 0; i < E820MAX; i++) {
		printf("addr[%x]         =%Lx\n",
			i,  info->real_mode->e820_map[i].addr);
		printf("size[%x]         =%Lx\n",
			i, info->real_mode->e820_map[i].size);
		printf("type[%x]         =%Lx\n",
			i, info->real_mode->e820_map[i].type);
	}
	printf("mount_root_rdonly=%x\n", info->real_mode->mount_root_rdonly);
	printf("ramdisk_flags    =%x\n", info->real_mode->ramdisk_flags);
	printf("orig_root_dev    =%x\n", info->real_mode->orig_root_dev);
	printf("aux_device_info  =%x\n", info->real_mode->aux_device_info);
	printf("param_block_signature=%x\n", *((uint32_t *)info->real_mode->param_block_signature));
	printf("loader_type      =%x\n", info->real_mode->loader_type);
	printf("loader_flags     =%x\n", info->real_mode->loader_flags);
	printf("initrd_start     =%x\n", info->real_mode->initrd_start);
	printf("initrd_size      =%x\n", info->real_mode->initrd_size);

	/* Where I'm putting the command line */
	printf("cl_magic         =%x\n", info->real_mode->cl_magic);
	printf("cl_offset        =%x\n", info->real_mode->cl_offset);

	/* Now print the command line */
	printf("command_line     =%s\n", info->real_mode->command_line);
}


#endif

/*
 * main
 * =============================================================================
 */

void initialize_linux_params(struct param_info *info)
{
	int len;
	/* First the defaults */
	memset(info->real_mode, 0, PAGE_SIZE);

	/* Default screen size */
	info->real_mode->orig_x = 0;
	info->real_mode->orig_y = 25;
	info->real_mode->orig_video_page = 0;
	info->real_mode->orig_video_mode = 0;
	info->real_mode->orig_video_cols = 80;
	info->real_mode->orig_video_lines = 25;
	info->real_mode->orig_video_ega_bx = 0;
	info->real_mode->orig_video_isVGA = 1;
	info->real_mode->orig_video_points = 16;

	/* Fill this in later */
	info->real_mode->ext_mem_k = 0;

	/* Fill in later... */
	info->real_mode->e820_map_nr = 0;

	/* Where I'm putting the command line */
	info->real_mode->cl_magic = CL_MAGIC_VALUE;
	info->real_mode->cl_offset = 2048;

	info->real_mode->cmd_line_ptr = info->real_mode->cl_offset + (unsigned long) info->real_mode;

	/* Now set the command line */
	len = strnlen(info->image->cmdline, sizeof(info->real_mode->command_line) -1);
	memcpy(info->real_mode->command_line, info->image->cmdline, len);
	info->real_mode->command_line[len] = '\0';

	/* from the bios initially */
	memset(&info->real_mode->apm_bios_info, 0, sizeof(info->real_mode->apm_bios_info));

	memset(&info->real_mode->drive_info, 0, sizeof(info->real_mode->drive_info));

	/* forget it for now... */
	info->real_mode->sys_desc_table.length = 0;

	/* Fill this in later */
	info->real_mode->alt_mem_k = 0;
	info->real_mode->ext_mem_k = 0;

	/* default yes: this can be overridden on the command line */
	info->real_mode->mount_root_rdonly = 0xFFFF;

	/* old ramdisk options, These really should be command line
	 * things...
	 */
	info->real_mode->ramdisk_flags = info->image->ramdisk_flags;

	/* default to /dev/hda.
	 * Override this on the command line if necessary
	 */
	info->real_mode->orig_root_dev = info->image->root_dev;

	/* Originally from the bios? */
	info->real_mode->aux_device_info = 0;

	/* Boot block magic */
	memcpy(info->real_mode->param_block_signature, "HdrS", 4);
	info->real_mode->param_block_version = 0x0201;

	/* Say I'm a kernel boot loader */
	info->real_mode->loader_type = (LOADER_TYPE_KERNEL << 4) + 0 /* version */;

	/* No loader flags */
	info->real_mode->loader_flags = 0;

	/* Set it to 16M, instead of 0 which means 4G */
	info->real_mode->kernel_alignment = 16*1024*1024;

	/* Ramdisk address and size ... */
	info->real_mode->initrd_start = 0;
	info->real_mode->initrd_size = 0;
	if (info->image->initrd_size) {
		info->real_mode->initrd_start = info->image->initrd_start;
		info->real_mode->initrd_size = info->image->initrd_size;
	}

	/* Now remember those things that I need */
	info->need_mem_sizes = 1;
}

void *convert_params(unsigned type, void *data, void *param, void *image)
{
	struct param_info info;
#if 0
	printf("hello world\n");
#endif
	info.real_mode = faked_real_mode;
	info.type  = type;
	info.data  = data;
	info.param = param;
	info.image = image;
	initialize_linux_params(&info);
	query_bootloader_param_class(&info);
	query_firmware_class(&info);
	query_firmware_values(&info);
	query_bootloader_values(&info);

	/* Do the hardware setup that linux might forget... */
	hardware_setup(&info);

	/* Print some debugging information */
#if 0
	printf("EXT_MEM_K=%x\n", info.real_mode->ext_mem_k);
	printf("ALT_MEM_K=%x\n", info.real_mode->alt_mem_k);
#endif
#if 0
	print_offsets();
	print_linux_params(&info);
#endif
#if 0
	printf("info.real_mode = 0x%x\n", info.real_mode );
	printf("Jumping to Linux\n");
#endif
	return info.real_mode;
}

