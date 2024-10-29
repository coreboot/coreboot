/* SPDX-License-Identifier: GPL-2.0-only */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elfparsing.h"
#include "common.h"
#include "cbfs.h"
#include "rmodule.h"

/* Checks if program segment contains the ignored sections */
static int is_phdr_ignored(Elf64_Phdr *phdr, Elf64_Shdr **shdrs)
{
	/* If no ignored section, return false. */
	if (shdrs == NULL)
		return 0;

	while (*shdrs) {
		Elf64_Addr sh_start = (*shdrs)->sh_addr;
		Elf64_Addr sh_end = (*shdrs)->sh_addr + (*shdrs)->sh_size;
		Elf64_Addr ph_start = phdr->p_vaddr;
		Elf64_Addr ph_end = phdr->p_vaddr + phdr->p_memsz;

		/* Return true only if section occupies whole of segment. */
		if ((sh_start == ph_start) && (sh_end == ph_end)) {
			DEBUG("Ignoring program segment at 0x%" PRIx64 "\n", ph_start);
			return 1;
		}

		/* If shdr intersects phdr at all, its a conflict */
		if (((sh_start >= ph_start) && (sh_start <= ph_end)) ||
		    ((sh_end >= ph_start) && (sh_end <= ph_end))) {
			ERROR("Conflicting sections in segment\n");
			exit(1);
		}
		shdrs++;
	}

	/* Program header doesn't need to be ignored. */
	return 0;
}

/* Sections to be ignored are comma separated  */
static bool is_ignored_sections(const char *section_name,
				const char *ignore_sections)
{
	const char *cur, *comma;

	for (cur = ignore_sections; (comma = strchr(cur, ',')); cur = comma + 1)
		if (!strncmp(cur, section_name, comma - cur))
			return true;
	return !strcmp(cur, section_name);
}

/* Find section headers based on ignored section names.
 * Returns a NULL-terminated list of section headers.
 */
static Elf64_Shdr **find_ignored_sections_header(struct parsed_elf *pelf,
						 const char *ignore_sections)
{
	int i;
	const char *shstrtab;
	Elf64_Shdr **headers = NULL;
	size_t size = 1;

	/* No section needs to be ignored */
	if (ignore_sections == NULL)
		return NULL;

	DEBUG("Sections to be ignored: %s\n", ignore_sections);

	/* Get pointer to string table */
	shstrtab = buffer_get(pelf->strtabs[pelf->ehdr.e_shstrndx]);

	for (i = 0; i < pelf->ehdr.e_shnum; i++) {
		Elf64_Shdr *shdr;
		const char *section_name;

		shdr = &pelf->shdr[i];
		section_name = &shstrtab[shdr->sh_name];

		/* If section name matches ignored string, add to list */
		if (is_ignored_sections(section_name, ignore_sections)) {
			headers = realloc(headers, sizeof(*headers) * ++size);
			if (!headers) {
				ERROR("Memory allocation failed\n");
				exit(1);
			}
			headers[size - 2] = shdr;
		}
	}

	if (headers)
		headers[size - 1] = NULL;
	return headers;
}

static int fill_cbfs_stageheader(struct cbfs_file_attr_stageheader *stageheader,
				 uint64_t entry, uint64_t loadaddr,
				 uint32_t memsize)
{
	if (entry - loadaddr >= memsize) {
		ERROR("stage entry point out of bounds!\n");
		return -1;
	}

	stageheader->loadaddr = htobe64(loadaddr);
	stageheader->memlen = htobe32(memsize);
	stageheader->entry_offset = htobe32(entry - loadaddr);

	return  0;
}

/* returns size of result, or -1 if error.
 * Note that, with the new code, this function
 * works for all elf files, not just the restricted set.
 */
int parse_elf_to_stage(const struct buffer *input, struct buffer *output,
		       const char *ignore_section,
		       struct cbfs_file_attr_stageheader *stageheader)
{
	struct parsed_elf pelf;
	Elf64_Phdr *phdr;
	Elf64_Ehdr *ehdr;
	Elf64_Shdr **shdrs_ignored;
	Elf64_Addr virt_to_phys;
	int ret = -1;

	int headers;
	int i;
	uint64_t data_start, data_end, mem_end;

	int flags = ELF_PARSE_PHDR | ELF_PARSE_SHDR | ELF_PARSE_STRTAB;

	if (parse_elf(input, &pelf, flags)) {
		ERROR("Couldn't parse ELF\n");
		return -1;
	}

	ehdr = &pelf.ehdr;
	phdr = &pelf.phdr[0];

	/* Find the section headers corresponding to ignored-sections */
	shdrs_ignored = find_ignored_sections_header(&pelf, ignore_section);

	if (ignore_section && (shdrs_ignored == NULL))
		WARN("Ignore section(s) not found\n");

	headers = ehdr->e_phnum;

	/* Ignore the program header containing ignored section */
	for (i = 0; i < headers; i++) {
		if (is_phdr_ignored(&phdr[i], shdrs_ignored))
			phdr[i].p_type = PT_NULL;
	}

	data_start = ~0;
	data_end = 0;
	mem_end = 0;
	virt_to_phys = 0;

	for (i = 0; i < headers; i++) {
		uint64_t start, mend, rend;

		if (phdr[i].p_type != PT_LOAD)
			continue;

		/* Empty segments are never interesting */
		if (phdr[i].p_memsz == 0)
			continue;

		/* BSS */

		start = phdr[i].p_paddr;

		mend = start + phdr[i].p_memsz;
		rend = start + phdr[i].p_filesz;

		if (start < data_start)
			data_start = start;

		if (rend > data_end)
			data_end = rend;

		if (mend > mem_end)
			mem_end = mend;

		if (virt_to_phys == 0)
			virt_to_phys = phdr[i].p_paddr - phdr[i].p_vaddr;
	}

	if (data_end <= data_start) {
		ERROR("data ends (%08lx) before it starts (%08lx). Make sure "
		      "the ELF file is correct and resides in ROM space.\n",
		      (unsigned long)data_end, (unsigned long)data_start);
		exit(1);
	}

	if (buffer_create(output, data_end - data_start, input->name) != 0) {
		ERROR("Unable to allocate memory: %m\n");
		goto err;
	}
	memset(output->data, 0, output->size);

	/* Copy the file data into the output buffer */

	for (i = 0; i < headers; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		if (phdr[i].p_memsz == 0)
			continue;

		/* A legal ELF file can have a program header with
		 * non-zero length but zero-length file size and a
		 * non-zero offset which, added together, are > than
		 * input->size (i.e. the total file size).  So we need
		 * to not even test in the case that p_filesz is zero.
		 */
		if (!phdr[i].p_filesz)
			continue;
		if (input->size < (phdr[i].p_offset + phdr[i].p_filesz)){
			ERROR("Underflow copying out the segment."
			      "File has %zu bytes left, segment end is %zu\n",
			      input->size, (size_t)(phdr[i].p_offset + phdr[i].p_filesz));
			goto err;
		}
		memcpy(&output->data[phdr[i].p_paddr - data_start],
		       &input->data[phdr[i].p_offset],
		       phdr[i].p_filesz);
	}

	/* coreboot expects entry point to be physical address. Thus, adjust the
	   entry point accordingly. */
	ret = fill_cbfs_stageheader(stageheader, ehdr->e_entry + virt_to_phys,
				    data_start, mem_end - data_start);
err:
	parsed_elf_destroy(&pelf);
	return ret;
}

struct xip_context {
	struct rmod_context rmodctx;
	Elf64_Shdr **ignored_sections;
};

static int rmod_filter(struct reloc_filter *f, const Elf64_Rela *r)
{
	size_t symbol_index;
	int reloc_type;
	struct parsed_elf *pelf;
	Elf64_Sym *sym;
	struct xip_context *xipctx;
	Elf64_Shdr **sections;

	xipctx = f->context;
	pelf = &xipctx->rmodctx.pelf;

	/* Allow everything through if there isn't an ignored section. */
	if (xipctx->ignored_sections == NULL)
		return 1;

	reloc_type = ELF64_R_TYPE(r->r_info);
	symbol_index = ELF64_R_SYM(r->r_info);
	sym = &pelf->syms[symbol_index];

	/* Nothing to filter. Relocation is not being applied to the
	 * ignored sections. */
	for (sections = xipctx->ignored_sections; *sections; sections++)
		if (sym->st_shndx == *sections - pelf->shdr)
			break;
	if (!*sections)
		return 1;

	/* If there is any relocation to the ignored section that isn't
	 * absolute fail as current assumptions are that all relocations
	 * are absolute. */
	if ((reloc_type != R_386_32) &&
	    (reloc_type != R_AMD64_64) &&
	    (reloc_type != R_AMD64_32)) {
		ERROR("Invalid reloc to ignored section: %x\n", reloc_type);
		return -1;
	}

	/* Relocation referencing ignored sections. Don't emit it. */
	return 0;
}

/* Returns a NULL-terminated list of loadable segments.  Returns NULL if no
 * loadable segments were found or if two consecutive segments are not
 * consecutive in their physical address space.
 */
static Elf64_Phdr **find_loadable_segments(struct parsed_elf *pelf)
{
	Elf64_Phdr **phdrs = NULL;
	Elf64_Phdr *prev = NULL, *cur = NULL;
	size_t size = 1, i;

	for (i = 0; i < pelf->ehdr.e_phnum; i++) {
		cur = &pelf->phdr[i];

		if (cur->p_type != PT_LOAD || cur->p_memsz == 0)
			continue;

		phdrs = realloc(phdrs, sizeof(*phdrs) * ++size);
		if (!phdrs) {
			ERROR("Memory allocation failed\n");
			return NULL;
		}
		phdrs[size - 2] = cur;

		if (prev && (prev->p_paddr + prev->p_memsz != cur->p_paddr ||
			     prev->p_filesz != prev->p_memsz)) {
			ERROR("Loadable segments physical addresses should "
			      "be consecutive\n");
			free(phdrs);
			return NULL;
		}
		prev = cur;
	}

	if (phdrs)
		phdrs[size - 1] = NULL;
	return phdrs;
}

int parse_elf_to_xip_stage(const struct buffer *input, struct buffer *output,
			   uint32_t location, const char *ignore_sections,
			   struct cbfs_file_attr_stageheader *stageheader)
{
	struct xip_context xipctx;
	struct rmod_context *rmodctx;
	struct reloc_filter filter;
	struct parsed_elf *pelf;
	uint32_t adjustment, memsz = 0;
	struct buffer binput;
	struct buffer boutput;
	Elf64_Phdr **toload, **phdr;
	Elf64_Xword i;
	int ret = -1;
	size_t filesz = 0;

	rmodctx = &xipctx.rmodctx;
	pelf = &rmodctx->pelf;

	if (rmodule_init(rmodctx, input))
		return -1;

	/* Only support x86 / x86_64 XIP currently. */
	if ((rmodctx->pelf.ehdr.e_machine != EM_386) &&
	    (rmodctx->pelf.ehdr.e_machine != EM_X86_64)) {
		ERROR("Only support XIP stages for x86/x86_64\n");
		goto out;
	}

	xipctx.ignored_sections =
		find_ignored_sections_header(pelf, ignore_sections);

	filter.filter = rmod_filter;
	filter.context = &xipctx;

	if (rmodule_collect_relocations(rmodctx, &filter))
		goto out;

	toload = find_loadable_segments(pelf);
	if (!toload)
		goto out;

	for (phdr = toload; *phdr; phdr++)
		filesz += (*phdr)->p_filesz;
	if (buffer_create(output, filesz, input->name) != 0) {
		ERROR("Unable to allocate memory: %m\n");
		goto out;
	}
	buffer_clone(&boutput, output);
	memset(buffer_get(&boutput), 0, filesz);
	buffer_set_size(&boutput, 0);

	/* The program segment moves to final location from based on virtual
	 * address of loadable segment. */
	adjustment = location - pelf->phdr->p_vaddr;
	DEBUG("Relocation adjustment: %08x\n", adjustment);

	for (phdr = toload; *phdr; phdr++)
		memsz += (*phdr)->p_memsz;
	fill_cbfs_stageheader(stageheader,
			      (uint32_t)pelf->ehdr.e_entry + adjustment,
			      (uint32_t)pelf->phdr->p_vaddr + adjustment,
			      memsz);
	for (phdr = toload; *phdr; phdr++) {
		/* Need an adjustable buffer. */
		buffer_clone(&binput, input);
		buffer_seek(&binput, (*phdr)->p_offset);
		bputs(&boutput, buffer_get(&binput), (*phdr)->p_filesz);
	}

	buffer_clone(&boutput, output);

	/* Make adjustments to all the relocations within the program. */
	for (i = 0; i < rmodctx->nrelocs; i++) {
		size_t reloc_offset;
		uint32_t val;
		struct buffer in, out;
		Elf64_Addr reloc = rmodctx->emitted_relocs[i];

		/* The relocations represent in-program addresses of the
		 * linked program. Obtain the offset into the program to do
		 * the adjustment. */
		reloc_offset = 0;
		for (phdr = toload; *phdr; phdr++) {
			if (reloc >= (*phdr)->p_vaddr &&
			    reloc < (*phdr)->p_vaddr + (*phdr)->p_memsz)
				break;
			reloc_offset += (*phdr)->p_filesz;
		}
		if (!*phdr) {
			ERROR("Relocation outside of loadable segments\n");
			goto out;
		}
		reloc_offset += reloc - (*phdr)->p_vaddr;

		buffer_clone(&out, &boutput);
		buffer_seek(&out, reloc_offset);
		buffer_clone(&in, &out);
		/* Appease around xdr semantics: xdr decrements buffer
		 * size when get()ing and appends to size when put()ing. */
		buffer_set_size(&out, 0);

		val = xdr_le.get32(&in);
		DEBUG("reloc %zx %08x -> %08x\n", reloc_offset, val,
			val + adjustment);
		xdr_le.put32(&out, val + adjustment);
	}

	ret = 0;

out:
	rmodule_cleanup(rmodctx);
	return ret;
}
