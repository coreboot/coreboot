#include <sys/io.h>
#include <stdint.h>
#include "uniform_boot.h"
#include "linuxbios_tables.h"
#include "elf_boot.h"
#define STACK_SIZE (4096)

long user_stack [STACK_SIZE];

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
	uint8_t  reserved13[176];		/* 0x220 */
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



#undef memset
#undef memcpy
#define memzero(s, n)     memset ((s), 0, (n))

typedef unsigned size_t;


#define _NOTE __attribute__((__section__(".rodata")))
static const struct {
	Elf_Word namesz;
	Elf_Word descsz;
	Elf_Word type;
	unsigned char name[8];
	unsigned char desc[6];
	unsigned char dummy[2];
} program _NOTE = {
	sizeof(program.name),
	sizeof(program.desc),
	EIN_PROGRAM_NAME,
	"ElfBoot",
	"Linux"
};

static const struct {
	Elf_Word namesz;
	Elf_Word descsz;
	Elf_Word type;
	unsigned char name[8];
	unsigned char desc[64];
} program_version _NOTE = {
	sizeof(program.name),
	sizeof(program.desc),
	EIN_PROGRAM_NAME,
	"ElfBoot",
	"2.2.17 (eric@DLT) #21 Wed Jan 3 14:44:09 MST 2001"
};

static const struct {
	Elf_Word namesz;
	Elf_Word descsz;
	Elf_Word type;
	unsigned char name[6];
	unsigned char dummy[2];
	char command_line[256];
} note_command_line _NOTE = {
	sizeof(program.name),
	sizeof(program.desc),
	LIN_COMMAND_LINE,
	"Linux", { 0, 0 },
	DEFAULT_COMMAND_LINE,
};

static const struct {
	Elf_Word namesz;
	Elf_Word descsz;
	Elf_Word type;
	unsigned char name[6];
	unsigned char dummy[2];
	unsigned short root_dev;
	unsigned char dummy2[2];
} note_root_dev _NOTE = {
	sizeof(program.name),
	sizeof(program.desc),
	LIN_ROOT_DEV,
	"Linux", { 0, 0 },
	DEFAULT_ROOT_DEV, { 0, 0 },
};


static const struct {
	Elf_Word namesz;
	Elf_Word descsz;
	Elf_Word type;
	unsigned char name[6];
	unsigned char dummy[2];
	unsigned short ramdisk_flags;
	unsigned char dummy2[2];
} note_ramdisk_flags _NOTE = {
	sizeof(program.name),
	sizeof(program.desc),
	LIN_RAMDISK_FLAGS,
	"Linux", { 0, 0 },
	0 
#if defined(DEFAULT_RAMDISK_IMAGE_START)
	| (DEFAULT_RAMDISK_IMAGE_START & RAMDISK_IMAGE_START_MASK)
#endif
#if defined(DEFAULT_RAMDISK_PROMPT_FLAG)
	| RAMDISK_PROMPT_FLAG
#endif
#if defined(DEFAULT_RAMDISK_LOAD_FLAG)
	| RAMDISK_LOAD_FLAG
#endif
	, { 0 , 0 },
	
};


extern char ramdisk_data[], ramdisk_data_size[];

static const struct {
	Elf_Word namesz;
	Elf_Word descsz;
	Elf_Word type;
	unsigned char name[6];
	unsigned char dummy[2];
	unsigned initrd_start;
} note_initrd_start _NOTE = {
	sizeof(program.name),
	sizeof(program.desc),
	LIN_INITRD_START,
	"Linux", { 0, 0 },
	(unsigned)&ramdisk_data,
};

static const struct {
	Elf_Word namesz;
	Elf_Word descsz;
	Elf_Word type;
	unsigned char name[6];
	unsigned char dummy[2];
	unsigned initrd_size;
} note_initrd_size _NOTE = {
	sizeof(program.name),
	sizeof(program.desc),
	LIN_INITRD_SIZE,
	"Linux", { 0, 0 },
	((unsigned)&ramdisk_data_size),
};


/* FIXME handle systems with large EBDA's */
static struct parameters *faked_real_mode = (void *)0x90000;



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

static void put_char(int c)
{
#if 0
	put_char_video(c);
#endif
	put_char_serial(c);
}

static void puts(const char *str)
{
	int c;
	while(c = *str++) {
		put_char(c);
	}
}

static void __put_hex(unsigned long long x, int bits)
{
	static const char digit[] = "0123456789ABCDEF";
	int i;
	for(i = bits -4; i >= 0; i -= 4) {
		put_char(digit[(x >> i) & 0xf]);
	}
}

static void put_hex(unsigned x)
{
	puts("0x"); 
	__put_hex(x, 32);
}

static void put_lhex(unsigned long long x)
{
	puts("0x"); 
	__put_hex(x, 64);
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
	int i;
	char *ss = (char*)s;

	for (i=0;i<n;i++) ss[i] = c;
	return s;
}

void* memcpy(void* __dest, const void* __src,
			    size_t __n)
{
	int i;
	char *d = (char *)__dest, *s = (char *)__src;

	for (i=0;i<__n;i++) d[i] = s[i];
	return __dest;
}

/*
 * Checksum functions
 * =============================================================================
 */


static unsigned long checksum_partial(unsigned long partial,
	void *addr, unsigned long length)
{
	/* Assumes the start is 2 byte aligned.
	 * This isn't exactly a problem on x86 but...
	 */
	unsigned short *ptr = addr;
	unsigned long len;
	unsigned long remainder;
	len = length >> 1;
	remainder = len & 1;
	while(len--) {
		partial += *(ptr++);
		if (partial > 0xFFFF) 
			partial -= 0xFFFF;
	}
	if (remainder) {
		unsigned char *ptr2 = (void *)ptr;
		partial += *ptr2;
		if (partial > 0xFFFF)
			partial -= 0xFFFF;
	}
	return partial;
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
#define multi_puts(x) puts(x)
#define multi_put_hex(x) put_hex(x)
#define multi_put_lhex(x) put_lhex(x)
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
	int i;
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
#define ube_puts(x) puts(x)
#define ube_put_hex(x) put_hex(x)
#define ube_put_lhex(x) put_lhex(x)
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
		unsigned long mem_k;
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
	outb_p(0x11, 0x20);		/*! initialization sequence to 8259A-1*/
	outb_p(0x11, 0xA0);		/*! and to 8259A-2*/

	outb_p(0x20, 0x21);		/*! start of hardware int's (0x20)*/
	outb_p(0x28, 0xA1);		/*! start of hardware int's 2 (0x28)*/

	outb_p(0x04, 0x21);		/*! 8259-1 is master*/
	outb_p(0x02, 0xA1);		/*! 8259-2 is slave*/

	outb_p(0x01, 0x21);		/*! 8086 mode for both*/
	outb_p(0x01, 0xA1);		

	outb_p(0xFF, 0xA1);		/*! mask off all interrupts for now*/
	outb_p(0xFB, 0x21);		/*! mask all irq's but irq2 which is cascaded*/
}

static void hardware_setup(struct param_info *info)
{
	/* Disable nmi */
	outb(0x80, 0x70);

	/* Make sure any coprocessor is properly reset.. */
	outb_p(0, 0xf0);
	outb_p(0, 0xf1);

	setup_i8259();
}


/*
 * ELF Boot loader
 * =============================================================================
 */

static inline unsigned long elf_note_size(Elf_Nhdr *hdr)
{
	unsigned long size;
	size = sizeof(*hdr);
	size += hdr->n_namesz;
	if (size & 3) {
		size += 4 - (size & 3);
	}
	size = hdr->n_descsz;
	if (size & 3) {
		size += 4 - (size & 3);
	}
	return size;
}
static inline Elf_Nhdr *next_elf_note(Elf_Nhdr *hdr)
{
	return (void *)(((char *)hdr) + elf_note_size(hdr));
}

static inline unsigned char *elf_note_name(Elf_Nhdr *hdr)
{
	return (void *)(((char *)hdr) + sizeof(*hdr));
}

static inline unsigned char *elf_note_desc(Elf_Nhdr *hdr)
{
	int offset;
	offset = sizeof(*hdr);
	offset = hdr->n_namesz;
	if (offset & 3) {
		offset += 4 - (offset & 3);
	}
	return (void *)(((char *)hdr) + offset);
}

static int count_elf_notes(Elf_Bhdr *bhdr)
{
	Elf_Nhdr *hdr;
	void *start = (void *)bhdr;
	void *end = ((char *)start) + bhdr->b_size;
	int count;
	count = 0;
	for(hdr = start; ((void *)hdr < end) &&
		(elf_note_size(hdr) <= (end - (void *)hdr));
		hdr = next_elf_note(hdr)) {
		count++;
	}
	return count;
}

static Elf_Nhdr *find_elf_note(Elf_Bhdr *bhdr, 
	Elf_Word namesz, unsigned char *name, Elf_Word type)
{

	Elf_Nhdr *hdr;
	void *start = (void *)bhdr;
	void *end = ((char *)start) + bhdr->b_size;
	for(hdr = start; ((void *)hdr < end) &&
		(elf_note_size(hdr) <= (end - (void *)hdr));
		hdr = next_elf_note(hdr)) {
		unsigned char *n_name;
		unsigned char *n_desc;
		n_name = elf_note_name(hdr);
		n_desc = elf_note_desc(hdr);
		if ((hdr->n_type == type) &&
			(hdr->n_namesz == namesz) &&
			(memcmp(n_name, name, namesz) == 0)) {
			return hdr;
		}
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
	Elf_Nhdr *hdr;
	int i;
	void *start = (void *)bhdr;
	void *end = ((char *)start) + bhdr->b_size;
	for(hdr = start; ((void *)hdr < end) &&
		(elf_note_size(hdr) <= (end - (void *)hdr));
		hdr = next_elf_note(hdr)) {
		unsigned char *n_name;
		unsigned char *n_desc;
		n_name = elf_note_name(hdr);
		n_desc = elf_note_desc(hdr);
		for(i = 0; i < sizeof(elf_notes)/sizeof(elf_notes[0]); i++) {
			if ((hdr->n_type == elf_notes[i].type) &&
				(hdr->n_namesz == elf_notes[i].namesz) &&
				(memcmp(n_name, elf_notes[i].name, elf_notes[i].namesz) == 0)) {
				elf_notes[i].convert(info, hdr->n_descsz, n_desc);
				break;
			}
		}
	}
}

/*
 * LinuxBIOS
 * =============================================================================
 */

#define LB_MEM_DEBUG 0
#if LB_MEM_DEBUG
#define lb_puts(x) puts(x)
#define lb_put_hex(x) put_hex(x)
#define lb_put_lhex(x) put_lhex(x)
#else
#define lb_puts(x)
#define lb_put_hex(x)
#define lb_put_lhex(x)
#endif /* LB_MEM_DEBUG */

static int count_lb_records(void *start, unsigned long length)
{
	struct lb_record *rec;
	void *end;
	int count;
	count = 0;
	end = ((char *)start) + length;
	for(rec = start; ((void *)rec < end) &&
		(rec->size <= (end - (void *)rec)); 
		rec = (void *)(((char *)rec) + rec->size)) {
		count++;
	}
	return count;
}

static int find_lb_table(struct param_info *info, void *start, void *end)
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
			info->has_linuxbios = 1;
			info->lb_table = (void *)ptr;
			return 1;
		}
	};
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
		(rec->size <= (end - (void *)rec)); 
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
#define pc_puts(x) puts(x)
#define pc_put_hex(x) put_hex(x)
#define pc_put_lhex(x) put_lhex(x)
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
			puts("Bad uniform boot header checksum!\n");
		}
	}
	if (!has_bootloader_type && (info->type == 0x0E1FB007)) {
		/* Good ELF boot proposal... */
		Elf_Bhdr *hdr;
		unsigned long checksum;
		int count;
		hdr = info->data;
		checksum = compute_checksum(hdr, hdr->b_size);
		count = count_elf_notes(hdr);
		if ((hdr->b_signature == ELF_BOOT_MAGIC) &&
			(checksum == 0) &&
			hdr->b_records == count) {
			info->has_elf_boot = 1;
			has_bootloader_type = 1;
		}
		else {
			puts("Bad ELF parameter table!\n");
		}
	}
	if (!has_bootloader_type) {
		puts("Unknown bootloader class!\n");
		puts("type="); put_hex(info->type); puts("\n");
		puts("data="); put_hex((unsigned)info->data); puts("\n");
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
		convert_elf_boot(info, info->data);
	}
}

/*
 * Firmware
 * =============================================================================
 */

static int bootloader_query_firmware_class(struct param_info *info)
{
	Elf_Nhdr *hdr;
	if (!info->has_elf_boot) {
		/* Only the elf boot tables gives us a firmware type */
		return 0;
	}
	hdr = find_elf_note(info->data, 0, 0, EBN_FIRMWARE_TYPE);
	if (!hdr) {
		info->has_pcbios = 1;
	}
	else if ((hdr->n_descsz == 7) &&
		(memcmp(elf_note_desc(hdr), "PCBIOS", 7) == 0)) {
		info->has_pcbios = 1;
	}
	else if ((hdr->n_descsz == 10) &&
		(memcmp(elf_note_desc(hdr), "LinuxBIOS", 10) == 0)) {
		info->has_linuxbios = 1;
	}
	else if (hdr->n_descsz == 0) {
		/* No firmware is present */
	}
	else if ((hdr->n_descsz == 1) && 
		(memcmp(elf_note_desc(hdr), "", 1) == 0)) {
		/* No firmware is present */
	}
	else {
		puts("Unknow firmware type!");
	}
	return 1;
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
		/* First try at address 0 */
		detected_firmware_type = 
			find_lb_table(info, (void*)0x00000, (void*)0x1000);
	}

	if (!detected_firmware_type) {
		/* Then try at address 0xf0000 */
		detected_firmware_type = 
			find_lb_table(info, (void*)0xf0000, (void*)0x100000);
	}

	if (!detected_firmware_type) {
		/* if all else fails assume a standard pcbios... */
		info->has_pcbios = 1;
	}

	/* Now print out the firmware type... */
	puts("Firmware type:");
	if (info->has_linuxbios) {
		puts(" linuxBIOS");
	}
	if (info->has_pcbios) {
		puts(" PCBIOS");
	}
	puts("\n");
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
static print_offsets(void)
{
	struct parameters *real_mode = 0;
	puts("print_offsets\n");

	puts("orig_x="); put_hex((uint32_t)&real_mode->orig_x); puts("\n");			
	puts("orig_y="); put_hex((uint32_t)&real_mode->orig_y); puts("\n");			
	puts("ext_mem_k="); put_hex((uint32_t)&real_mode->ext_mem_k); puts("\n");
	puts("orig_video_page="); put_hex((uint32_t)&real_mode->orig_video_page); puts("\n");		
	puts("orig_video_mode="); put_hex((uint32_t)&real_mode->orig_video_mode); puts("\n");		
	puts("orig_video_cols="); put_hex((uint32_t)&real_mode->orig_video_cols); puts("\n");		
	puts("unused2="); put_hex((uint32_t)&real_mode->unused2); puts("\n");			
	puts("orig_video_ega_bx="); put_hex((uint32_t)&real_mode->orig_video_ega_bx); puts("\n");		
	puts("unused3="); put_hex((uint32_t)&real_mode->unused3); puts("\n");			
	puts("orig_video_lines="); put_hex((uint32_t)&real_mode->orig_video_lines); puts("\n");		
	puts("orig_video_isVGA="); put_hex((uint32_t)&real_mode->orig_video_isVGA); puts("\n");		
	puts("orig_video_points="); put_hex((uint32_t)&real_mode->orig_video_points); puts("\n");		
	puts("lfb_width="); put_hex((uint32_t)&real_mode->lfb_width); puts("\n");			
	puts("lfb_height="); put_hex((uint32_t)&real_mode->lfb_height); puts("\n");			
	puts("lfb_depth="); put_hex((uint32_t)&real_mode->lfb_depth); puts("\n");			
	puts("lfb_base="); put_hex((uint32_t)&real_mode->lfb_base); puts("\n");			
	puts("lfb_size="); put_hex((uint32_t)&real_mode->lfb_size); puts("\n");			
	puts("cl_magic="); put_hex((uint32_t)&real_mode->cl_magic); puts("\n");			
	puts("cl_offset="); put_hex((uint32_t)&real_mode->cl_offset); puts("\n");			
	puts("lfb_linelength="); put_hex((uint32_t)&real_mode->lfb_linelength); puts("\n");		
	puts("red_size="); put_hex((uint32_t)&real_mode->red_size); puts("\n");			
	puts("red_pos="); put_hex((uint32_t)&real_mode->red_pos); puts("\n");			
	puts("green_size="); put_hex((uint32_t)&real_mode->green_size); puts("\n");			
	puts("green_pos="); put_hex((uint32_t)&real_mode->green_pos); puts("\n");			
	puts("blue_size="); put_hex((uint32_t)&real_mode->blue_size); puts("\n");			
	puts("blue_pos="); put_hex((uint32_t)&real_mode->blue_pos); puts("\n");			
	puts("rsvd_size="); put_hex((uint32_t)&real_mode->rsvd_size); puts("\n");			
	puts("rsvd_pos="); put_hex((uint32_t)&real_mode->rsvd_pos); puts("\n");			
	puts("vesapm_seg="); put_hex((uint32_t)&real_mode->vesapm_seg); puts("\n");			
	puts("vesapm_off="); put_hex((uint32_t)&real_mode->vesapm_off); puts("\n");			
	puts("pages="); put_hex((uint32_t)&real_mode->pages); puts("\n");				
	puts("reserved4="); put_hex((uint32_t)&real_mode->reserved4); puts("\n");
	puts("apm_bios_info="); put_hex((uint32_t)&real_mode->apm_bios_info); puts("\n");	
	puts("drive_info="); put_hex((uint32_t)&real_mode->drive_info); puts("\n");	
	puts("sys_desc_table="); put_hex((uint32_t)&real_mode->sys_desc_table); puts("\n");	
	puts("alt_mem_k="); put_hex((uint32_t)&real_mode->alt_mem_k); puts("\n");			
	puts("reserved5="); put_hex((uint32_t)&real_mode->reserved5); puts("\n");
	puts("e820_map_nr="); put_hex((uint32_t)&real_mode->e820_map_nr); puts("\n");			
	puts("reserved6="); put_hex((uint32_t)&real_mode->reserved6); puts("\n");
	puts("mount_root_rdonly="); put_hex((uint32_t)&real_mode->mount_root_rdonly); puts("\n");		
	puts("reserved7="); put_hex((uint32_t)&real_mode->reserved7); puts("\n");			
	puts("ramdisk_flags="); put_hex((uint32_t)&real_mode->ramdisk_flags); puts("\n");			
	puts("reserved8="); put_hex((uint32_t)&real_mode->reserved8); puts("\n");			
	puts("orig_root_dev="); put_hex((uint32_t)&real_mode->orig_root_dev); puts("\n");			
	puts("reserved9="); put_hex((uint32_t)&real_mode->reserved9); puts("\n");			
	puts("aux_device_info="); put_hex((uint32_t)&real_mode->aux_device_info); puts("\n");		
	puts("reserved10="); put_hex((uint32_t)&real_mode->reserved10); puts("\n");			
	puts("param_block_signature="); put_hex((uint32_t)&real_mode->param_block_signature); puts("\n");	
	puts("param_block_version="); put_hex((uint32_t)&real_mode->param_block_version); puts("\n");		
	puts("reserved11="); put_hex((uint32_t)&real_mode->reserved11); puts("\n");			
	puts("loader_type="); put_hex((uint32_t)&real_mode->loader_type); puts("\n");			
	puts("loader_flags="); put_hex((uint32_t)&real_mode->loader_flags); puts("\n");			
	puts("reserved12="); put_hex((uint32_t)&real_mode->reserved12); puts("\n");			
	puts("kernel_start="); put_hex((uint32_t)&real_mode->kernel_start); puts("\n");			
	puts("initrd_start="); put_hex((uint32_t)&real_mode->initrd_start); puts("\n");			
	puts("initrd_size="); put_hex((uint32_t)&real_mode->initrd_size); puts("\n");			
	puts("reserved13="); put_hex((uint32_t)&real_mode->reserved13); puts("\n");		
	puts("e820_map="); put_hex((uint32_t)&real_mode->e820_map); puts("\n");
	puts("reserved16="); put_hex((uint32_t)&real_mode->reserved16); puts("\n");      	
	puts("command_line="); put_hex((uint32_t)&real_mode->command_line); puts("\n");
	puts("reserved17="); put_hex((uint32_t)&real_mode->reserved17); puts("\n");		
}

static print_linux_params(struct param_info *info)
{
	int i;

	puts("print_linux_params\n");
	/* Default screen size */
	puts("orig_x           ="); put_hex(info->real_mode->orig_x); puts("\n");
	puts("orig_y           ="); put_hex(info->real_mode->orig_y); puts("\n");
	puts("orig_video_page  ="); put_hex(info->real_mode->orig_video_page); puts("\n");
	puts("orig_video_mode  ="); put_hex(info->real_mode->orig_video_mode); puts("\n");
	puts("orig_video_cols  ="); put_hex(info->real_mode->orig_video_cols); puts("\n");
	puts("orig_video_lines ="); put_hex(info->real_mode->orig_video_lines); puts("\n");
	puts("orig_video_ega_bx="); put_hex(info->real_mode->orig_video_ega_bx); puts("\n");
	puts("orig_video_isVGA ="); put_hex(info->real_mode->orig_video_isVGA); puts("\n");
	puts("orig_video_points="); put_hex(info->real_mode->orig_video_points); puts("\n");
	

	/* System descriptor table... */
	puts("sys_dest_table_len="); put_hex(info->real_mode->sys_desc_table.length); puts("\n");

	/* Memory sizes */
	puts("ext_mem_k        ="); put_hex(info->real_mode->ext_mem_k); puts("\n");
	puts("alt_mem_k        ="); put_hex(info->real_mode->alt_mem_k); puts("\n");
	puts("e820_map_nr      ="); put_hex(info->real_mode->e820_map_nr); puts("\n");
	for(i = 0; i < E820MAX; i++) {
		puts("addr["); put_hex(i); puts("]         =");
		put_lhex(info->real_mode->e820_map[i].addr);
		puts("\n");
		puts("size["); put_hex(i); puts("]         =");
		put_lhex(info->real_mode->e820_map[i].size);
		puts("\n");
		puts("type["); put_hex(i); puts("]         =");
		put_hex(info->real_mode->e820_map[i].type);
		puts("\n");
	}
	puts("mount_root_rdonly="); put_hex(info->real_mode->mount_root_rdonly); puts("\n");
	puts("ramdisk_flags    ="); put_hex(info->real_mode->ramdisk_flags); puts("\n");
	puts("orig_root_dev    ="); put_hex(info->real_mode->orig_root_dev); puts("\n");
	puts("aux_device_info  ="); put_hex(info->real_mode->aux_device_info); puts("\n");
	puts("param_block_signature="); put_hex(*((uint32_t *)info->real_mode->param_block_signature)); puts("\n");
	puts("loader_type      ="); put_hex(info->real_mode->loader_type); puts("\n");
	puts("loader_flags     ="); put_hex(info->real_mode->loader_flags); puts("\n");
	puts("initrd_start     ="); put_hex(info->real_mode->initrd_start); puts("\n");
	puts("initrd_size      ="); put_hex(info->real_mode->initrd_size); puts("\n");

	/* Where I'm putting the command line */
	puts("cl_magic         ="); put_hex(info->real_mode->cl_magic); puts("\n");
	puts("cl_offset        ="); put_hex(info->real_mode->cl_offset); puts("\n");
	
	/* Now print the command line */
	puts("command_line     ="); puts(info->real_mode->command_line); puts("\n");

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
	
	/* Now set the command line */
	len = strnlen(note_command_line.command_line, sizeof(info->real_mode->command_line) -1);
	memcpy(info->real_mode->command_line, note_command_line.command_line, len);
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
	info->real_mode->ramdisk_flags = note_ramdisk_flags.ramdisk_flags; 

	/* default to /dev/hda.
	 * Override this on the command line if necessary 
	 */
	info->real_mode->orig_root_dev = note_root_dev.root_dev;
	
	/* Originally from the bios? */
	info->real_mode->aux_device_info = 0;
	
	/* Boot block magic */
	memcpy(info->real_mode->param_block_signature, "HdrS", 4);
	info->real_mode->param_block_version = 0x0201;
	
	/* Say I'm a kernel boot loader */
	info->real_mode->loader_type = (LOADER_TYPE_KERNEL << 4) + 0 /* version */;
	
	/* No loader flags */
	info->real_mode->loader_flags = 0;
	
	/* Ramdisk address and size ... */
	info->real_mode->initrd_start = 0;
	info->real_mode->initrd_size = 0;
	if (note_initrd_size.initrd_size) {
		info->real_mode->initrd_start = note_initrd_start.initrd_start;
		info->real_mode->initrd_size = note_initrd_size.initrd_size;
	}	

	/* Now remember those things that I need */
	info->need_mem_sizes = 1;
}




void *convert_params(unsigned type, void *data)
{
	struct param_info info;
	size_t len;
	int have_realmode;
#if 0
	puts("hello world\n");
#endif
	info.real_mode = faked_real_mode;
	info.type = type;
	info.data = data;
	initialize_linux_params(&info);
	query_bootloader_param_class(&info);
	query_firmware_class(&info);
	query_firmware_values(&info);
	query_bootloader_values(&info);

	/* Do the hardware setup that linux might forget... */
	hardware_setup(&info);

	/* Print some debugging information */
#if 0
	puts("EXT_MEM_K="); put_hex(info.real_mode->ext_mem_k); puts("\n");
	puts("ALT_MEM_K="); put_hex(info.real_mode->alt_mem_k); puts("\n");
#endif
#if 0
	print_offsets();
	print_linux_params(&info);
#endif
	return info.real_mode;
}

