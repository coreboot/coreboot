/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Free Software Foundation, Inc.
 * Copyright (C) 2008 Robert Millan
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * multiboot.h - multiboot header file.
 */

#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

/* How many bytes from the start of the file we search for the header.  */
#define MB_SEARCH                 8192

/* The magic field should contain this.  */
#define MB_MAGIC                  0x1BADB002

/* This should be in %eax.  */
#define MB_MAGIC2                 0x2BADB002

/* The bits in the required part of flags field we don't support.  */
#define MB_UNSUPPORTED            0x0000fffc

/* Alignment of multiboot modules.  */
#define MB_MOD_ALIGN              0x00001000

/*
 * Flags set in the 'flags' member of the multiboot header.
 */

/* Align all boot modules on i386 page (4KB) boundaries.  */
#define MB_PAGE_ALIGN		0x00000001

/* Must pass memory information to OS.  */
#define MB_MEMORY_INFO		0x00000002

/* Must pass video information to OS.  */
#define MB_VIDEO_MODE		0x00000004

/* This flag indicates the use of the address fields in the header.  */
#define MB_AOUT_KLUDGE		0x00010000

/*
 *  Flags to be set in the 'flags' member of the multiboot info structure.
 */

/* is there basic lower/upper memory information? */
#define MB_INFO_MEMORY		0x00000001
/* is there a boot device set? */
#define MB_INFO_BOOTDEV		0x00000002
/* is the command-line defined? */
#define MB_INFO_CMDLINE		0x00000004
/* are there modules to do something with? */
#define MB_INFO_MODS		0x00000008

/* These next two are mutually exclusive */

/* is there a symbol table loaded? */
#define MB_INFO_AOUT_SYMS		0x00000010
/* is there an ELF section header table? */
#define MB_INFO_ELF_SHDR		0x00000020

/* is there a full memory map? */
#define MB_INFO_MEM_MAP		0x00000040

/* Is there drive info?  */
#define MB_INFO_DRIVE_INFO		0x00000080

/* Is there a config table?  */
#define MB_INFO_CONFIG_TABLE	0x00000100

/* Is there a boot loader name?  */
#define MB_INFO_BOOT_LOADER_NAME	0x00000200

/* Is there a APM table?  */
#define MB_INFO_APM_TABLE		0x00000400

/* Is there video information?  */
#define MB_INFO_VIDEO_INFO		0x00000800

struct multiboot_header {
	/* Must be MB_MAGIC - see above.  */
	uint32_t magic;

	/* Feature flags.  */
	uint32_t flags;

	/* The above fields plus this one must equal 0 mod 2^32. */
	uint32_t checksum;

	/* These are only valid if MB_AOUT_KLUDGE is set.  */
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;

	/* These are only valid if MB_VIDEO_MODE is set.  */
	uint32_t mode_type;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
};

struct multiboot_info {
	/* Multiboot info version number */
	uint32_t flags;

	/* Available memory from BIOS */
	uint32_t mem_lower;
	uint32_t mem_upper;

	/* "root" partition */
	uint32_t boot_device;

	/* Kernel command line */
	uint32_t cmdline;

	/* Boot-Module list */
	uint32_t mods_count;
	uint32_t mods_addr;

	uint32_t syms[4];

	/* Memory Mapping buffer */
	uint32_t mmap_length;
	uint32_t mmap_addr;

	/* Drive Info buffer */
	uint32_t drives_length;
	uint32_t drives_addr;

	/* ROM configuration table */
	uint32_t config_table;

	/* Boot Loader Name */
	uint32_t boot_loader_name;

	/* APM table */
	uint32_t apm_table;

	/* Video */
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};

#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2

struct multiboot_mmap_entry {
	uint32_t size;
	uint64_t addr;
	uint64_t len;
	uint32_t type;
} __attribute__ ((packed));

extern struct multiboot_info *mbi;

unsigned long  write_multiboot_info(unsigned long rom_table_end);

#endif
