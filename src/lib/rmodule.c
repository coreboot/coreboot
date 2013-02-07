/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 ChromeOS Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <stdint.h>
#include <string.h>
#include <console/console.h>
#include <rmodule.h>

/* Change this define to get more verbose debugging for module loading. */
#define PK_ADJ_LEVEL BIOS_NEVER

#if CONFIG_ARCH_X86
/*
 * On X86, the only relocations currently allowed are R_386_RELATIVE which
 * have '0' for the symbol info in the relocation metadata (in r_info).
 * The reason is that the module is fully linked and just has the relocations'
 * locations.
 */
typedef struct {
	u32 r_offset;
	u32 r_info;
} Elf32_Rel;

#define R_386_RELATIVE 8

#define RELOCTION_ENTRY_SIZE sizeof(Elf32_Rel)
static inline int rmodule_reloc_offset(const void *reloc)
{
	const Elf32_Rel *rel = reloc;
	return rel->r_offset;
}

static inline int rmodule_reloc_valid(const void *reloc)
{
	const Elf32_Rel *rel = reloc;
	return (rel->r_info == R_386_RELATIVE);
}

static inline void *remodule_next_reloc(const void *reloc)
{
	const Elf32_Rel *rel = reloc;
	rel++;
	return (void *)rel;
}

#else
#error Arch needs to add relocation information support for RMODULE
#endif

static inline int rmodule_is_loaded(const struct rmodule *module)
{
	return module->location != NULL;
}

/* Calculate a loaded program address based on the blob address. */
static inline void *rmodule_load_addr(const struct rmodule *module,
                                      u32 blob_addr)
{
	char *loc = module->location;
	return &loc[blob_addr - module->header->module_link_start_address];
}

/* Initialize a rmodule structure based on raw data. */
int rmodule_parse(void *ptr, struct rmodule *module)
{
	char *base;
	struct rmodule_header *rhdr;

	base = ptr;
	rhdr = ptr;

	if (rhdr == NULL)
		return -1;

	/* Sanity check the raw data. */
	if (rhdr->magic != RMODULE_MAGIC)
		return -1;
	if (rhdr->version != RMODULE_VERSION_1)
		return -1;

	/* Indicate the module hasn't been loaded yet. */
	module->location = NULL;

	/* The rmodule only needs a reference to the reloc_header. */
	module->header = rhdr;

	/* The payload lives after the header. */
	module->payload = &base[rhdr->payload_begin_offset];
	module->payload_size = rhdr->payload_end_offset -
	                       rhdr->payload_begin_offset;
	module->relocations = &base[rhdr->relocations_begin_offset];

	return 0;
}

int rmodule_memory_size(const struct rmodule *module)
{
	return module->header->module_program_size;
}

void *rmodule_parameters(const struct rmodule *module)
{
	if (!rmodule_is_loaded(module))
		return NULL;

	/* Indicate if there are no parameters. */
	if (module->header->parameters_begin == module->header->parameters_end)
		return NULL;

	return rmodule_load_addr(module, module->header->parameters_begin);
}

int rmodule_entry_offset(const struct rmodule *module)
{
	return module->header->module_entry_point -
	       module->header->module_link_start_address;
}

void *rmodule_entry(const struct rmodule *module)
{
	if (!rmodule_is_loaded(module))
		return NULL;

	return rmodule_load_addr(module, module->header->module_entry_point);
}

static void rmodule_clear_bss(struct rmodule *module)
{
	char *begin;
	int size;

	begin = rmodule_load_addr(module, module->header->bss_begin);
	size = module->header->bss_end - module->header->bss_begin;
	memset(begin, 0, size);
}

static inline int rmodule_number_relocations(const struct rmodule *module)
{
	int r;

	r = module->header->relocations_end_offset;
	r -= module->header->relocations_begin_offset;
	r /= RELOCTION_ENTRY_SIZE;
	return r;
}

static void rmodule_copy_payload(const struct rmodule *module)
{
	printk(BIOS_DEBUG, "Loading module at %p with entry %p. "
	       "filesize: 0x%x memsize: 0x%x\n",
	       module->location, rmodule_entry(module),
	       module->payload_size, rmodule_memory_size(module));
	memcpy(module->location, module->payload, module->payload_size);
}

static inline u32 *rmodule_adjustment_location(const struct rmodule *module,
                                               const void *reloc)
{
	int reloc_offset;

	/* Don't relocate header field entries -- only program relocations. */
	reloc_offset = rmodule_reloc_offset(reloc);
	if (reloc_offset < module->header->module_link_start_address)
		return NULL;

	return rmodule_load_addr(module, reloc_offset);
}

static int rmodule_relocate(const struct rmodule *module)
{
	int num_relocations;
	const void *reloc;
	u32 adjustment;

	/* Each relocation needs to be adjusted relative to the beginning of
	 * the loaded program. */
	adjustment = (u32)rmodule_load_addr(module, 0);

	reloc = module->relocations;
	num_relocations = rmodule_number_relocations(module);

	printk(BIOS_DEBUG, "Processing %d relocs with adjust value of 0x%08x\n",
	       num_relocations, adjustment);

	while (num_relocations > 0) {
		u32 *adjust_loc;

		if (!rmodule_reloc_valid(reloc))
			return -1;

		/* If the adjustment location is non-NULL adjust it. */
		adjust_loc = rmodule_adjustment_location(module, reloc);
		if (adjust_loc != NULL) {
			printk(PK_ADJ_LEVEL, "Adjusting %p: 0x%08x -> 0x%08x\n",
			       adjust_loc, *adjust_loc,
			       *adjust_loc + adjustment);
			*adjust_loc += adjustment;
		}

		reloc = remodule_next_reloc(reloc);
		num_relocations--;
	}

	return 0;
}

int rmodule_load_alignment(const struct rmodule *module)
{
	/* The load alignment is the start of the program's linked address.
	 * The base address where the program is loaded needs to be a multiple
	 * of the program's starting link address. That way all data alignment
	 * in the program is presered. */
	return module->header->module_link_start_address;
}

static int __rmodule_load(void *base, struct rmodule *module, int clear_bss)
{
	/*
	 * In order to load the module at a given address, the following steps
	 * take place:
	 *  1. Copy payload to base address.
	 *  2. Clear the bss segment.
	 *  3. Adjust relocations within the module to new base address.
	 */
	module->location = base;
	rmodule_copy_payload(module);
	if (clear_bss)
		rmodule_clear_bss(module);
	return rmodule_relocate(module);
}

int rmodule_load(void *base, struct rmodule *module)
{
	return __rmodule_load(base, module, 1);
}

int rmodule_load_no_clear_bss(void *base, struct rmodule *module)
{
	return __rmodule_load(base, module, 0);
}
