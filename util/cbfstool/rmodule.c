/*
 ;* Copyright (C) 2014 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elfparsing.h"
#include "rmodule.h"
#include <commonlib/rmodule-defs.h>

/*
 * Architecture specific support operations.
 */
static int valid_reloc_386(Elf64_Rela *rel)
{
	int type;

	type = ELF64_R_TYPE(rel->r_info);

	/* Only these 2 relocations are expected to be found. */
	return (type == R_386_32 || type == R_386_PC32);
}

static int should_emit_386(Elf64_Rela *rel)
{
	int type;

	type = ELF64_R_TYPE(rel->r_info);

	/* R_386_32 relocations are absolute. Must emit these. */
	return (type == R_386_32);
}

static int valid_reloc_arm(Elf64_Rela *rel)
{
	int type;

	type = ELF64_R_TYPE(rel->r_info);

	/* Only these 6 relocations are expected to be found. */
	return (type == R_ARM_ABS32 || type == R_ARM_THM_PC22 ||
                type == R_ARM_THM_JUMP24 || type == R_ARM_V4BX ||
		type == R_ARM_CALL || type == R_ARM_JUMP24);
}

static int should_emit_arm(Elf64_Rela *rel)
{
	int type;

	type = ELF64_R_TYPE(rel->r_info);

	/* R_ARM_ABS32 relocations are absolute. Must emit these. */
	return (type == R_ARM_ABS32);
}

static int valid_reloc_aarch64(Elf64_Rela *rel)
{
	int type;

	type = ELF64_R_TYPE(rel->r_info);

	return (type == R_AARCH64_ADR_PREL_PG_HI21 ||
		type == R_AARCH64_ADD_ABS_LO12_NC  ||
		type == R_AARCH64_LDST8_ABS_LO12_NC ||
		type == R_AARCH64_CONDBR19 ||
		type == R_AARCH64_JUMP26 ||
		type == R_AARCH64_LDST32_ABS_LO12_NC ||
		type == R_AARCH64_LDST64_ABS_LO12_NC ||
		type == R_AARCH64_CALL26 ||
		type == R_AARCH64_ABS64 ||
		type == R_AARCH64_LD_PREL_LO19 ||
		type == R_AARCH64_ADR_PREL_LO21);
}

static int should_emit_aarch64(Elf64_Rela *rel)
{
	int type;

	type = ELF64_R_TYPE(rel->r_info);

	return (type == R_AARCH64_ABS64);
}

static const struct arch_ops reloc_ops[] = {
	{
		.arch = EM_386,
		.valid_type = valid_reloc_386,
		.should_emit = should_emit_386,
	},
	{
		.arch = EM_ARM,
		.valid_type = valid_reloc_arm,
		.should_emit = should_emit_arm,
	},
	{
		.arch = EM_AARCH64,
		.valid_type = valid_reloc_aarch64,
		.should_emit = should_emit_aarch64,
	},
};

/*
 * Relocation processing loops.
 */

static int for_each_reloc(struct rmod_context *ctx, struct reloc_filter *f,
				int do_emit)
{
	Elf64_Half i;
	struct parsed_elf *pelf = &ctx->pelf;

	for (i = 0; i < pelf->ehdr.e_shnum; i++) {
		Elf64_Shdr *shdr;
		Elf64_Rela *relocs;
		Elf64_Xword nrelocs;
		Elf64_Xword j;

		relocs = pelf->relocs[i];

		/* No relocations in this section. */
		if (relocs == NULL)
			continue;

		shdr = &pelf->shdr[i];
		nrelocs = shdr->sh_size / shdr->sh_entsize;

		for (j = 0; j < nrelocs; j++) {
			int filter_emit = 1;
			Elf64_Rela *r = &relocs[j];

			if (!ctx->ops->valid_type(r)) {
				ERROR("Invalid reloc type: %u\n",
				      (unsigned int)ELF64_R_TYPE(r->r_info));
				return -1;
			}

			/* Allow the provided filter to have precedence. */
			if (f != NULL) {
				filter_emit = f->filter(f, r);

				if (filter_emit < 0)
					return filter_emit;
			}

			if (filter_emit && ctx->ops->should_emit(r)) {
				int n = ctx->nrelocs;
				if (do_emit)
					ctx->emitted_relocs[n] = r->r_offset;
				ctx->nrelocs++;
			}
		}
	}

	return 0;
}

static int find_program_segment(struct rmod_context *ctx)
{
	int i;
	int nsegments;
	struct parsed_elf *pelf;
	Elf64_Phdr *phdr = NULL;

	pelf = &ctx->pelf;

	/* There should only be a single loadable segment. */
	nsegments = 0;
	for (i = 0; i < pelf->ehdr.e_phnum; i++) {
		if (pelf->phdr[i].p_type != PT_LOAD)
			continue;
		phdr = &pelf->phdr[i];
		nsegments++;
	}

	if (nsegments != 1) {
		ERROR("Unexepcted number of loadable segments: %d.\n",
		      nsegments);
		return -1;
	}

	INFO("Segment at 0x%0llx, file size 0x%0llx, mem size 0x%0llx.\n",
	     (long long)phdr->p_vaddr, (long long)phdr->p_filesz,
	     (long long)phdr->p_memsz);

	ctx->phdr = phdr;

	return 0;
}

static int
filter_relocation_sections(struct rmod_context *ctx)
{
	int i;
	const char *shstrtab;
	struct parsed_elf *pelf;
	const Elf64_Phdr *phdr;

	pelf = &ctx->pelf;
	phdr = ctx->phdr;
	shstrtab = buffer_get(pelf->strtabs[pelf->ehdr.e_shstrndx]);

	/*
	 * Find all relocation sections that contain relocation entries
	 * for sections that fall within the bounds of the segment. For
	 * easier processing the pointer to the relocation array for the
	 * sections that don't fall within the loadable program are NULL'd
	 * out.
	 */
	for (i = 0; i < pelf->ehdr.e_shnum; i++) {
		Elf64_Shdr *shdr;
		Elf64_Word sh_info;
		const char *section_name;

		shdr = &pelf->shdr[i];

		/* Ignore non-relocation sections. */
		if (shdr->sh_type != SHT_RELA && shdr->sh_type != SHT_REL)
			continue;

		/* Obtain section which relocations apply. */
		sh_info = shdr->sh_info;
		shdr = &pelf->shdr[sh_info];

		section_name = &shstrtab[shdr->sh_name];
		DEBUG("Relocation section found for '%s' section.\n",
		      section_name);

		/* Do not process relocations for debug sections. */
		if (strstr(section_name, ".debug") != NULL) {
			pelf->relocs[i] = NULL;
			continue;
		}

		/*
		 * If relocations apply to a non program section ignore the
		 * relocations for future processing.
		 */
		if (shdr->sh_type != SHT_PROGBITS) {
			pelf->relocs[i] = NULL;
			continue;
		}

		if (shdr->sh_addr < phdr->p_vaddr ||
		    ((shdr->sh_addr + shdr->sh_size) >
		     (phdr->p_vaddr + phdr->p_memsz))) {
			ERROR("Relocations being applied to section %d not "
			      "within segment region.\n", sh_info);
			return -1;
		}
	}

	return 0;
}

static int vaddr_cmp(const void *a, const void *b)
{
	const Elf64_Addr *pa = a;
	const Elf64_Addr *pb = b;

	if (*pa < *pb)
		return -1;
	if (*pa > *pb)
		return 1;
	return 0;
}

int rmodule_collect_relocations(struct rmod_context *ctx,
				struct reloc_filter *f)
{
	Elf64_Xword nrelocs;

	/*
	 * The relocs array in the pelf should only contain relocations that
	 * apply to the program. Count the number relocations. Then collect
	 * them into the allocated buffer.
	 */
	if (for_each_reloc(ctx, f, 0))
		return -1;

	nrelocs = ctx->nrelocs;
	INFO("%" PRIu64 " relocations to be emitted.\n", nrelocs);
	if (!nrelocs)
		return 0;

	/* Reset the counter for indexing into the array. */
	ctx->nrelocs = 0;
	ctx->emitted_relocs = calloc(nrelocs, sizeof(Elf64_Addr));
	/* Write out the relocations into the emitted_relocs array. */
	if (for_each_reloc(ctx, f, 1))
		return -1;

	if (ctx->nrelocs != nrelocs) {
		ERROR("Mismatch counted and emitted relocations: %zu vs %zu.\n",
		      (size_t)nrelocs, (size_t)ctx->nrelocs);
		return -1;
	}

	/* Sort the relocations by their address. */
	qsort(ctx->emitted_relocs, nrelocs, sizeof(Elf64_Addr), vaddr_cmp);

	return 0;
}

static int
populate_sym(struct rmod_context *ctx, const char *sym_name, Elf64_Addr *addr,
             int nsyms, const char *strtab, int optional)
{
	int i;
	Elf64_Sym *syms;

	syms = ctx->pelf.syms;

	for (i = 0; i < nsyms; i++) {
		if (syms[i].st_name == 0)
			continue;
		if (strcmp(sym_name, &strtab[syms[i].st_name]))
			continue;
		DEBUG("%s -> 0x%llx\n", sym_name, (long long)syms[i].st_value);
		*addr = syms[i].st_value;
		return 0;
	}

	if (optional) {
		DEBUG("optional symbol '%s' not found.\n", sym_name);
		*addr = 0;
		return 0;
	}

	ERROR("symbol '%s' not found.\n", sym_name);
	return -1;
}

static int populate_rmodule_info(struct rmod_context *ctx)
{
	int i;
	const char *strtab;
	struct parsed_elf *pelf;
	Elf64_Ehdr *ehdr;
	int nsyms;

	pelf = &ctx->pelf;
	ehdr = &pelf->ehdr;

	/* Obtain the string table. */
	strtab = NULL;
	for (i = 0; i < ehdr->e_shnum; i++) {
		if (ctx->pelf.strtabs[i] == NULL)
			continue;
		/* Don't use the section headers' string table. */
		if (i == ehdr->e_shstrndx)
			continue;
		strtab = buffer_get(ctx->pelf.strtabs[i]);
		break;
	}

	if (strtab == NULL) {
		ERROR("No string table found.\n");
		return -1;
	}

	/* Determine number of symbols. */
	nsyms = 0;
	for (i = 0; i < ehdr->e_shnum; i++) {
		if (pelf->shdr[i].sh_type != SHT_SYMTAB)
			continue;

		nsyms = pelf->shdr[i].sh_size / pelf->shdr[i].sh_entsize;
		break;
	}

	if (populate_sym(ctx, "_rmodule_params", &ctx->parameters_begin,
	                 nsyms, strtab, 1))
		return -1;

	if (populate_sym(ctx, "_ermodule_params", &ctx->parameters_end,
	                 nsyms, strtab, 1))
		return -1;

	if (populate_sym(ctx, "_bss", &ctx->bss_begin, nsyms, strtab, 0))
		return -1;

	if (populate_sym(ctx, "_ebss", &ctx->bss_end, nsyms, strtab, 0))
		return -1;

	return 0;
}

static int
add_section(struct elf_writer *ew, struct buffer *data, const char *name,
            Elf64_Addr addr, Elf64_Word size)
{
	Elf64_Shdr shdr;
	int ret;

	memset(&shdr, 0, sizeof(shdr));
	if (data != NULL) {
		shdr.sh_type = SHT_PROGBITS;
		shdr.sh_flags = SHF_ALLOC | SHF_WRITE | SHF_EXECINSTR;
	} else {
		shdr.sh_type = SHT_NOBITS;
		shdr.sh_flags = SHF_ALLOC;
	}
	shdr.sh_addr = addr;
	shdr.sh_offset = addr;
	shdr.sh_size = size;

	ret = elf_writer_add_section(ew, &shdr, data, name);

	if (ret)
		ERROR("Could not add '%s' section.\n", name);

	return ret;
}

static int
write_elf(const struct rmod_context *ctx, const struct buffer *in,
          struct buffer *out)
{
	int ret;
	int bit64;
	size_t loc;
	size_t rmod_data_size;
	struct elf_writer *ew;
	struct buffer rmod_data;
	struct buffer rmod_header;
	struct buffer program;
	struct buffer relocs;
	Elf64_Xword total_size;
	Elf64_Addr addr;
	Elf64_Ehdr ehdr;

	bit64 = ctx->pelf.ehdr.e_ident[EI_CLASS] == ELFCLASS64;

	/*
	 * 3 sections will be added  to the ELF file.
	 * +------------------+
	 * |  rmodule header  |
	 * +------------------+
	 * |     program      |
	 * +------------------+
	 * |   relocations    |
	 * +------------------+
	 */

	/* Create buffer for header and relocations. */
	rmod_data_size = sizeof(struct rmodule_header);
	if (bit64)
		rmod_data_size += ctx->nrelocs * sizeof(Elf64_Addr);
	else
		rmod_data_size += ctx->nrelocs * sizeof(Elf32_Addr);

	if (buffer_create(&rmod_data, rmod_data_size, "rmod"))
		return -1;

	buffer_splice(&rmod_header, &rmod_data,
	              0, sizeof(struct rmodule_header));
	buffer_clone(&relocs, &rmod_data);
	buffer_seek(&relocs, sizeof(struct rmodule_header));

	/* Reset current location. */
	buffer_set_size(&rmod_header, 0);
	buffer_set_size(&relocs, 0);

	/* Program contents. */
	buffer_splice(&program, in, ctx->phdr->p_offset, ctx->phdr->p_filesz);

	/* Create ELF writer with modified entry point. */
	memcpy(&ehdr, &ctx->pelf.ehdr, sizeof(ehdr));
	ew = elf_writer_init(&ehdr);

	if (ew == NULL) {
		ERROR("Failed to create ELF writer.\n");
		buffer_delete(&rmod_data);
		return -1;
	}

	/* Write out rmodule_header. */
	ctx->xdr->put16(&rmod_header, RMODULE_MAGIC);
	ctx->xdr->put8(&rmod_header, RMODULE_VERSION_1);
	ctx->xdr->put8(&rmod_header, 0);
	/* payload_begin_offset */
	loc = sizeof(struct rmodule_header);
	ctx->xdr->put32(&rmod_header, loc);
	/* payload_end_offset */
	loc += ctx->phdr->p_filesz;
	ctx->xdr->put32(&rmod_header, loc);
	/* relocations_begin_offset */
	ctx->xdr->put32(&rmod_header, loc);
	/* relocations_end_offset */
	if (bit64)
		loc += ctx->nrelocs * sizeof(Elf64_Addr);
	else
		loc += ctx->nrelocs * sizeof(Elf32_Addr);
	ctx->xdr->put32(&rmod_header, loc);
	/* module_link_start_address */
	ctx->xdr->put32(&rmod_header, ctx->phdr->p_vaddr);
	/* module_program_size */
	ctx->xdr->put32(&rmod_header, ctx->phdr->p_memsz);
	/* module_entry_point */
	ctx->xdr->put32(&rmod_header, ctx->pelf.ehdr.e_entry);
	/* parameters_begin */
	ctx->xdr->put32(&rmod_header, ctx->parameters_begin);
	/* parameters_end */
	ctx->xdr->put32(&rmod_header, ctx->parameters_end);
	/* bss_begin */
	ctx->xdr->put32(&rmod_header, ctx->bss_begin);
	/* bss_end */
	ctx->xdr->put32(&rmod_header, ctx->bss_end);
	/* padding[4] */
	ctx->xdr->put32(&rmod_header, 0);
	ctx->xdr->put32(&rmod_header, 0);
	ctx->xdr->put32(&rmod_header, 0);
	ctx->xdr->put32(&rmod_header, 0);

	/* Write the relocations. */
	for (unsigned i = 0; i < ctx->nrelocs; i++) {
		if (bit64)
			ctx->xdr->put64(&relocs, ctx->emitted_relocs[i]);
		else
			ctx->xdr->put32(&relocs, ctx->emitted_relocs[i]);
	}

	total_size = 0;
	addr = 0;

	/*
	 * There are 2 cases to deal with. The program has a large NOBITS
	 * section and the relocations can fit entirely within occupied memory
	 * region for the program. The other is that the relocations increase
	 * the memory footprint of the program if it was loaded directly into
	 * the region it would run. The rmdoule header is a fixed cost that
	 * is considered a part of the program.
	 */
	total_size += buffer_size(&rmod_header);
	if (buffer_size(&relocs) + ctx->phdr->p_filesz > ctx->phdr->p_memsz) {
		total_size += buffer_size(&relocs);
		total_size += ctx->phdr->p_filesz;
	} else {
		total_size += ctx->phdr->p_memsz;
	}

	ret = add_section(ew, &rmod_header, ".header", addr,
	                  buffer_size(&rmod_header));
	if (ret < 0)
		goto out;
	addr += buffer_size(&rmod_header);

	ret = add_section(ew, &program, ".program", addr, ctx->phdr->p_filesz);
	if (ret < 0)
		goto out;
	addr += ctx->phdr->p_filesz;

	if (ctx->nrelocs) {
		ret = add_section(ew, &relocs, ".relocs", addr,
				  buffer_size(&relocs));
		if (ret < 0)
			goto out;
		addr += buffer_size(&relocs);
	}

	if (total_size != addr) {
		ret = add_section(ew, NULL, ".empty", addr, total_size - addr);
		if (ret < 0)
			goto out;
	}

	/*
	 * Ensure last section has a memory usage that meets the required
	 * total size of the program in memory.
	 */

	ret = elf_writer_serialize(ew, out);
	if (ret < 0)
		ERROR("Failed to serialize ELF to buffer.\n");

out:
	buffer_delete(&rmod_data);
	elf_writer_destroy(ew);

	return ret;
}

int rmodule_init(struct rmod_context *ctx, const struct buffer *elfin)
{
	struct parsed_elf *pelf;
	size_t i;
	int ret;

	ret = -1;
	memset(ctx, 0, sizeof(*ctx));
	pelf = &ctx->pelf;

	if (parse_elf(elfin, pelf, ELF_PARSE_ALL)) {
		ERROR("Couldn't parse ELF!\n");
		return -1;
	}

	/* Only allow executables to be turned into rmodules. */
	if (pelf->ehdr.e_type != ET_EXEC) {
		ERROR("ELF is not an executable: %u.\n", pelf->ehdr.e_type);
		goto out;
	}

	/* Determine if architecture is supported. */
	for (i = 0; i < ARRAY_SIZE(reloc_ops); i++) {
		if (reloc_ops[i].arch == pelf->ehdr.e_machine) {
			ctx->ops = &reloc_ops[i];
			break;
		}
	}

	if (ctx->ops == NULL) {
		ERROR("ELF is unsupported arch: %u.\n", pelf->ehdr.e_machine);
		goto out;
	}

	/* Set the endian ops. */
	if (ctx->pelf.ehdr.e_ident[EI_DATA] == ELFDATA2MSB)
		ctx->xdr = &xdr_be;
	else
		ctx->xdr = &xdr_le;

	if (find_program_segment(ctx))
		goto out;

	if (filter_relocation_sections(ctx))
		goto out;

	ret = 0;

out:
	return ret;
}

void rmodule_cleanup(struct rmod_context *ctx)
{
	free(ctx->emitted_relocs);
	parsed_elf_destroy(&ctx->pelf);
}

int rmodule_create(const struct buffer *elfin, struct buffer *elfout)
{
	struct rmod_context ctx;
	int ret = -1;

	if (rmodule_init(&ctx, elfin))
		goto out;

	if (rmodule_collect_relocations(&ctx, NULL))
		goto out;

	if (populate_rmodule_info(&ctx))
		goto out;

	if (write_elf(&ctx, elfin, elfout))
		goto out;

	ret = 0;

out:
	rmodule_cleanup(&ctx);
	return ret;
}

static void rmod_deserialize(struct rmodule_header *rmod, struct buffer *buff,
				struct xdr *xdr)
{
	rmod->magic = xdr->get16(buff);
	rmod->version = xdr->get8(buff);
	rmod->type = xdr->get8(buff);
	rmod->payload_begin_offset = xdr->get32(buff);
	rmod->payload_end_offset = xdr->get32(buff);
	rmod->relocations_begin_offset = xdr->get32(buff);
	rmod->relocations_end_offset = xdr->get32(buff);
	rmod->module_link_start_address = xdr->get32(buff);
	rmod->module_program_size = xdr->get32(buff);
	rmod->module_entry_point = xdr->get32(buff);
	rmod->parameters_begin = xdr->get32(buff);
	rmod->parameters_end = xdr->get32(buff);
	rmod->bss_begin = xdr->get32(buff);
	rmod->bss_end = xdr->get32(buff);
	rmod->padding[0] = xdr->get32(buff);
	rmod->padding[1] = xdr->get32(buff);
	rmod->padding[2] = xdr->get32(buff);
	rmod->padding[3] = xdr->get32(buff);
}

int rmodule_stage_to_elf(Elf64_Ehdr *ehdr, struct buffer *buff)
{
	struct buffer reader;
	struct buffer elf_out;
	struct rmodule_header rmod;
	struct xdr *xdr;
	struct elf_writer *ew;
	Elf64_Shdr shdr;
	int bit64;
	size_t payload_sz;
	const char *section_name = ".program";
	const size_t input_sz = buffer_size(buff);

	buffer_clone(&reader, buff);

	xdr = (ehdr->e_ident[EI_DATA] == ELFDATA2MSB) ? &xdr_be : &xdr_le;
	bit64 = ehdr->e_ident[EI_CLASS] == ELFCLASS64;

	rmod_deserialize(&rmod, &reader, xdr);

	/* Indicate that file is not an rmodule if initial checks fail. */
	if (rmod.magic != RMODULE_MAGIC)
		return 1;
	if (rmod.version != RMODULE_VERSION_1)
		return 1;

	if (rmod.payload_begin_offset > input_sz ||
	    rmod.payload_end_offset > input_sz ||
	    rmod.relocations_begin_offset > input_sz ||
	    rmod.relocations_end_offset > input_sz) {
		ERROR("Rmodule fields out of bounds.\n");
		return -1;
	}

	ehdr->e_entry = rmod.module_entry_point;
	ew = elf_writer_init(ehdr);

	if (ew == NULL)
		return -1;

	payload_sz = rmod.payload_end_offset - rmod.payload_begin_offset;
	memset(&shdr, 0, sizeof(shdr));
	shdr.sh_type = SHT_PROGBITS;
	shdr.sh_flags = SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR;
	shdr.sh_addr = rmod.module_link_start_address;
	shdr.sh_size = payload_sz;
	buffer_splice(&reader, buff, rmod.payload_begin_offset, payload_sz);

	if (elf_writer_add_section(ew, &shdr, &reader, section_name)) {
		ERROR("Unable to add ELF section: %s\n", section_name);
		elf_writer_destroy(ew);
		return -1;
	}

	if (payload_sz != rmod.module_program_size) {
		struct buffer b;

		buffer_init(&b, NULL, NULL, 0);
		memset(&shdr, 0, sizeof(shdr));
		shdr.sh_type = SHT_NOBITS;
		shdr.sh_flags = SHF_WRITE | SHF_ALLOC;
		shdr.sh_addr = rmod.module_link_start_address + payload_sz;
		shdr.sh_size = rmod.module_program_size - payload_sz;
		if (elf_writer_add_section(ew, &shdr, &b, ".empty")) {
			ERROR("Unable to add ELF section: .empty\n");
			elf_writer_destroy(ew);
			return -1;
		}
	}

	/* Provide a section symbol so the relcoations can reference that. */
	if (elf_writer_add_symbol(ew, section_name, section_name, shdr.sh_addr,
					0, STB_LOCAL, STT_SECTION)) {
		ERROR("Unable to add section symbol to ELF.\n");
		elf_writer_destroy(ew);
		return -1;
	}

	/* Add symbols for the parameters if they are non-zero. */
	if (rmod.parameters_begin != rmod.parameters_end) {
		int ret = 0;

		ret |= elf_writer_add_symbol(ew, "_rmodule_params",
						section_name,
						rmod.parameters_begin, 0,
						STB_GLOBAL, STT_NOTYPE);
		ret |= elf_writer_add_symbol(ew, "_ermodule_params",
						section_name,
						rmod.parameters_end, 0,
						STB_GLOBAL, STT_NOTYPE);

		if (ret != 0) {
			ERROR("Unable to add module params symbols to ELF\n");
			elf_writer_destroy(ew);
			return -1;
		}
	}

	if (elf_writer_add_symbol(ew, "_bss", section_name, rmod.bss_begin, 0,
					STB_GLOBAL, STT_NOTYPE) ||
	    elf_writer_add_symbol(ew, "_ebss", section_name, rmod.bss_end, 0,
					STB_GLOBAL, STT_NOTYPE)) {
		ERROR("Unable to add bss symbols to ELF\n");
		elf_writer_destroy(ew);
		return -1;
	}

	ssize_t relocs_sz = rmod.relocations_end_offset;
	relocs_sz -= rmod.relocations_begin_offset;
	buffer_splice(&reader, buff, rmod.relocations_begin_offset, relocs_sz);
	while (relocs_sz > 0) {
		Elf64_Addr addr;

		if (bit64) {
			relocs_sz -= sizeof(Elf64_Addr);
			addr = xdr->get64(&reader);
		} else {
			relocs_sz -= sizeof(Elf32_Addr);
			addr = xdr->get32(&reader);
		}

		/* Skip any relocations that are below the link address. */
		if (addr < rmod.module_link_start_address)
			continue;

		if (elf_writer_add_rel(ew, section_name, addr)) {
			ERROR("Relocation addition failure.\n");
			elf_writer_destroy(ew);
			return -1;
		}
	}

	if (elf_writer_serialize(ew, &elf_out)) {
		ERROR("ELF writer serialize failure.\n");
		elf_writer_destroy(ew);
		return -1;
	}

	elf_writer_destroy(ew);

	/* Flip buffer with the created ELF one. */
	buffer_delete(buff);
	*buff = elf_out;

	return 0;
}
