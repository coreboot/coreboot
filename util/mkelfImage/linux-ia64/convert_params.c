#include "stdint.h"
#include "limits.h"
#include <stdarg.h>
#include "elf.h"
#include "elf_boot.h"
#include "convert.h"


/* NOTE be very careful with static variables.  No relocations are
 * being performed so static variables with initialized pointers will
 * point to the wrong locations, unless this file is loaded at just
 * the right location.
 */
/* Data and functions in head.S */
extern void uart_tx_byte(int c);

static void putchar(int c)
{
	if (c == '\n') {
		putchar('\r');
	}
#if 0
	uart_tx_byte(c);
#endif
}

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
			if (*fmt == 'l') {
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
				unsigned long h;
				int ncase;
				if (shift > INT_SHIFT) {
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
				if (shift > INT_SHIFT) {
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

void *memcpy(void *vdest, void *vsrc, size_t size)
{
	unsigned char *dest = vdest, *src = vsrc;
	size_t i;
	for(i = 0; i < size; i++) {
		*dest++ = *src++;
	}
	return dest;
}

int memcmp(void *vs1, void *vs2, size_t size)
{
	unsigned char *s1 =vs1, *s2=vs2;
	size_t i;
	for(i = 0; i < size; i++, s1++, s2++) {
		if (*s1 != *s2)
			return *s1 - *s2;
	}
	return 0;

}

void strappend(char *dest, const char *src, size_t max)
{
	size_t len, i;
	/* Walk to the end of the destination string */
	for(len = 0; len < max; len++) {
		if (dest[len] == '\0')
			break;
	}
	/* Walk through the source string and append it */
	for(i = 0; (i + len) < max; i++) {
		if (src[i] == '\0')
			break;
		dest[len + i] = src[i];
	}
	len = len + i;
	/* Now null terminate the string */
	if (len >= max) {
		len = max -1;
	}
	dest[len] = '\0';
}

static struct ia64_boot_param {
	uint64_t command_line;		/* physical address of command line arguments */
	uint64_t efi_systab;		/* physical address of EFI system table */
	uint64_t efi_memmap;		/* physical address of EFI memory map */
	uint64_t efi_memmap_size;		/* size of EFI memory map */
	uint64_t efi_memdesc_size;		/* size of an EFI memory map descriptor */
	uint32_t efi_memdesc_version;	/* memory descriptor version */
	struct {
		uint16_t num_cols;	/* number of columns on console output device */
		uint16_t num_rows;	/* number of rows on console output device */
		uint16_t orig_x;	/* cursor's x position */
		uint16_t orig_y;	/* cursor's y position */
	} console_info;
	uint64_t fpswa;		/* physical address of the fpswa interface */
	uint64_t initrd_start;
	uint64_t initrd_size;
} bp = { 0, 0, 0, 0, 0, 0, { 80, 24, 0, 0 }, 0, 0, 0 };

static void append_command_line(char *arg)
{
	strappend((char *)bp.command_line, " ", CMDLINE_MAX);
	strappend((char *)bp.command_line, arg, CMDLINE_MAX);
}

static void convert_ia64_boot_params(struct ia64_boot_param *orig_bp)
{
	/* Copy the parameters I have no clue about */
	bp.efi_systab            = orig_bp->efi_systab;
	bp.efi_memmap            = orig_bp->efi_memmap;
	bp.efi_memmap_size       = orig_bp->efi_memmap_size;
	bp.efi_memdesc_size      = orig_bp->efi_memdesc_size;
	bp.efi_memdesc_version   = orig_bp->efi_memdesc_version;
	bp.console_info.num_cols = orig_bp->console_info.num_cols;
	bp.console_info.num_rows = orig_bp->console_info.num_rows;
	bp.console_info.orig_x   = orig_bp->console_info.orig_x;
	bp.console_info.orig_y   = orig_bp->console_info.orig_y;
	bp.fpswa                 = orig_bp->fpswa;
	/* If a ramdisk was supplied and I didn't original have one,
	 * use it.
	 */
	if (orig_bp->initrd_size && (!bp.initrd_size)) {
		bp.initrd_start = orig_bp->initrd_start;
		bp.initrd_size = orig_bp->initrd_size;
	}
	/* If a command line was supplied append it */
	if (orig_bp->command_line) {
		append_command_line((char *)(orig_bp->command_line));
	}
}

static void convert_bhdr_params(Elf_Bhdr *bhdr)
{
	unsigned char *note, *end;
	char *ldr_name, *ldr_version, *firmware;

	ldr_name = ldr_version = firmware = 0;

	note = ((char *)bhdr) + sizeof(*bhdr);
	end  = ((char *)bhdr) + bhdr->b_size;
	while(note < end) {
		Elf_Nhdr *hdr;
		unsigned char *n_name, *n_desc, *next;
		hdr = (Elf_Nhdr *)note;
		n_name = note + sizeof(*hdr);
		n_desc = n_name + ((hdr->n_namesz + 3) & ~3);
		next = n_desc + ((hdr->n_descsz + 3) & ~3);
		if (next > end)
			break;
#if 0
		printf("n_type: %x n_name(%d): n_desc(%d): \n",
			hdr->n_type, hdr->n_namesz, hdr->n_descsz);
#endif

		if (hdr->n_namesz == 0) {
			switch(hdr->n_type) {
			case EBN_FIRMWARE_TYPE:
				firmware = n_desc;
				break;
			case EBN_BOOTLOADER_NAME:
				ldr_name = n_desc;
				break;
			case EBN_BOOTLOADER_VERSION:
				ldr_version = n_desc;
				break;
			case EBN_COMMAND_LINE:
				append_command_line(n_desc);
				break;
			}
		}
		else if ((hdr->n_namesz == 10) &&
			(memcmp(n_name, "Etherboot", 10) == 0)) {
			switch(hdr->n_type) {
			case EB_IA64_SYSTAB:
			{
				uint64_t *systabp = (void *)n_desc;
				bp.efi_systab = *systabp;
				break;
			}
			case EB_IA64_FPSWA:
			{
				uint64_t *fpswap = (void *)n_desc;
				bp.fpswa = *fpswap;
				break;
			}
			case EB_IA64_CONINFO:
				memcpy(&bp.console_info, n_desc, sizeof(bp.console_info));
				break;
			case EB_IA64_MEMMAP:
			{
				struct efi_mem_map {
					uint64_t	map_size;
					uint64_t        map_key;
					uint64_t        descriptor_size;
					uint64_t        descriptor_version;
					uint8_t         map[40];
				} *map = (void *)n_desc;
				bp.efi_memmap = (uint64_t)&map->map;
				bp.efi_memmap_size     = map->map_size;
				bp.efi_memdesc_size    = map->descriptor_size;
				bp.efi_memdesc_version = map->descriptor_version;
				break;
			}
			}
		}
		note = next;
	}
	if (ldr_name && ldr_version) {
		printf("Loader: %s version: %s\n",
			ldr_name, ldr_version);
	}
	if (firmware) {
		printf("Firmware: %s\n",
			firmware);
	}
}

void *convert_params(unsigned long arg1, unsigned long r28,
	struct image_parameters *params)
{
	struct ia64_boot_param *orig_bp;
	Elf_Bhdr *bhdr = (Elf_Bhdr*)arg1;

	/* handle the options I can easily deal with */
	bp.command_line = (unsigned long)&params->cmdline;
	bp.initrd_start = params->initrd_start;
	bp.initrd_size  = params->initrd_size;

	orig_bp = (struct ia64_boot_param *)r28;
	if (bhdr->b_signature == 0x0E1FB007) {
		convert_bhdr_params(bhdr);
	}
	else {
		convert_ia64_boot_params(orig_bp);
	}

	return &bp;
}
