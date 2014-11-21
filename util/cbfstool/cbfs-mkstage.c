/*
 * cbfs-mkstage
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
 *               2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
 * Copyright (C) 2012 Google, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elfparsing.h"
#include "common.h"
#include "cbfs.h"

/* Checks if program segment contains the ignored section */
static int is_phdr_ignored(Elf64_Phdr *phdr, Elf64_Shdr *shdr)
{
	/* If no ignored section, return false. */
	if (shdr == NULL)
		return 0;

	Elf64_Addr sh_start = shdr->sh_addr;
	Elf64_Addr sh_end = shdr->sh_addr + shdr->sh_size;
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

	/* Program header doesn't need to be ignored. */
	return 0;
}

/* Find section header based on ignored section name */
static Elf64_Shdr *find_ignored_section_header(struct parsed_elf *pelf,
					       const char *ignore_section)
{
	int i;
	const char *shstrtab;

	/* No section needs to be ignored */
	if (ignore_section == NULL)
		return NULL;

	DEBUG("Section to be ignored: %s\n", ignore_section);

	/* Get pointer to string table */
	shstrtab = buffer_get(pelf->strtabs[pelf->ehdr.e_shstrndx]);

	for (i = 0; i < pelf->ehdr.e_shnum; i++) {
		Elf64_Shdr *shdr;
		const char *section_name;

		shdr = &pelf->shdr[i];
		section_name = &shstrtab[shdr->sh_name];

		/* If section name matches ignored string, return shdr */
		if (strcmp(section_name, ignore_section) == 0)
			return shdr;
	}

	/* No section matches ignore string */
	return NULL;
}

/* returns size of result, or -1 if error.
 * Note that, with the new code, this function
 * works for all elf files, not just the restricted set.
 */
int parse_elf_to_stage(const struct buffer *input, struct buffer *output,
		       uint32_t arch, comp_algo algo, uint32_t *location,
		       const char *ignore_section)
{
	struct parsed_elf pelf;
	Elf64_Phdr *phdr;
	Elf64_Ehdr *ehdr;
	Elf64_Shdr *shdr_ignored;
	char *buffer;
	struct buffer outheader;
	int ret = -1;

	int headers;
	int i, outlen;
	uint32_t data_start, data_end, mem_end;

	comp_func_ptr compress = compression_function(algo);
	if (!compress)
		return -1;

	DEBUG("start: parse_elf_to_stage(location=0x%x)\n", *location);

	int flags = ELF_PARSE_PHDR | ELF_PARSE_SHDR | ELF_PARSE_STRTAB;

	if (parse_elf(input, &pelf, flags)) {
		ERROR("Couldn't parse ELF\n");
		return -1;
	}

	ehdr = &pelf.ehdr;
	phdr = &pelf.phdr[0];

	/* Find the section header corresponding to ignored-section */
	shdr_ignored = find_ignored_section_header(&pelf, ignore_section);

	if (ignore_section && (shdr_ignored == NULL))
		WARN("Ignore section not found\n");

	headers = ehdr->e_phnum;

	/* Ignore the program header containing ignored section */
	for (i = 0; i < headers; i++) {
		if (is_phdr_ignored(&phdr[i], shdr_ignored))
			phdr[i].p_type = PT_NULL;
	}

	data_start = ~0;
	data_end = 0;
	mem_end = 0;

	for (i = 0; i < headers; i++) {
		unsigned int start, mend, rend;

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
	}

	if (data_start < *location) {
		data_start = *location;
	}

	if (data_end <= data_start) {
		ERROR("data ends (%08lx) before it starts (%08lx). Make sure "
		      "the ELF file is correct and resides in ROM space.\n",
		      (unsigned long)data_end, (unsigned long)data_start);
		exit(1);
	}

	/* allocate an intermediate buffer for the data */
	buffer = calloc(data_end - data_start, 1);

	if (buffer == NULL) {
		ERROR("Unable to allocate memory: %m\n");
		goto err;
	}

	/* Copy the file data into the buffer */

	for (i = 0; i < headers; i++) {
		unsigned int l_start, l_offset = 0;

		if (phdr[i].p_type != PT_LOAD)
			continue;

		if (phdr[i].p_memsz == 0)
			continue;

		l_start = phdr[i].p_paddr;
		if (l_start < *location) {
			l_offset = *location - l_start;
			l_start = *location;
		}

		/* A legal ELF file can have a program header with
		 * non-zero length but zero-length file size and a
		 * non-zero offset which, added together, are > than
		 * input->size (i.e. the total file size).  So we need
		 * to not even test in the case that p_filesz is zero.
		 */
		if (! phdr[i].p_filesz)
			continue;
		if (input->size < (phdr[i].p_offset + phdr[i].p_filesz)){
			ERROR("Underflow copying out the segment."
			      "File has %zu bytes left, segment end is %zu\n",
			      input->size, (size_t)(phdr[i].p_offset + phdr[i].p_filesz));
			free(buffer);
			goto err;
		}
		memcpy(buffer + (l_start - data_start),
		       &input->data[phdr[i].p_offset + l_offset],
		       phdr[i].p_filesz - l_offset);
	}

	/* Now make the output buffer */
	if (buffer_create(output, sizeof(struct cbfs_stage) + data_end - data_start,
			  input->name) != 0) {
		ERROR("Unable to allocate memory: %m\n");
		free(buffer);
		goto err;
	}
	memset(output->data, 0, output->size);

	/* Compress the data, at which point we'll know information
	 * to fill out the header. This seems backward but it works because
	 * - the output header is a known size (not always true in many xdr's)
	 * - we do need to know the compressed output size first
	 * If compression fails or makes the data bigger, we'll warn about it
	 * and use the original data.
	 */
	if (compress(buffer, data_end - data_start,
		     (output->data + sizeof(struct cbfs_stage)),
		     &outlen) < 0 || outlen > data_end - data_start) {
		WARN("Compression failed or would make the data bigger "
		     "- disabled.\n");
		memcpy(output->data + sizeof(struct cbfs_stage),
		       buffer, data_end - data_start);
		algo = CBFS_COMPRESS_NONE;
	}
	free(buffer);

	/* Set up for output marshaling. */
	outheader.data = output->data;
	outheader.size = 0;
	/* N.B. The original plan was that SELF data was B.E.
	 * but: this is all L.E.
	 * Maybe we should just change the spec.
	 */
	xdr_le.put32(&outheader, algo);
	xdr_le.put64(&outheader, ehdr->e_entry);
	xdr_le.put64(&outheader, data_start);
	xdr_le.put32(&outheader, outlen);
	xdr_le.put32(&outheader, mem_end - data_start);

	if (*location)
		*location -= sizeof(struct cbfs_stage);
	output->size = sizeof(struct cbfs_stage) + outlen;
	ret = 0;

err:
	parsed_elf_destroy(&pelf);
	return ret;
}
