/*
 * Copyright (C) 2014 Google, Inc.
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

#ifndef ELFPARSING_H
#define ELFPARSING_H

#include "elf.h"
#include "common.h"

struct parsed_elf {
	Elf64_Ehdr ehdr;
	Elf64_Phdr *phdr;
	Elf64_Shdr *shdr;
	/*
	 * The relocs array contains pointers to arrays of relocation
	 * structures.  Each index into the relocs array corresponds to its
	 * corresponding section index. i.e. if a section i is of type SHT_REL
	 * or SHT_RELA then the corresponding index into the relocs array will
	 * contain the associated relocations. Otherwise thee entry will be
	 * NULL.
	 */
	Elf64_Rela **relocs;
	/*
	 * Similarly to the relocs array the strtabs array consists of an
	 * array of pointers where each entry represents a potential struct
	 * buffer pointer. Only setions of type SHT_STRTAB will have a non-NULL
	 * entry.
	 */
	struct buffer **strtabs;
	/* Parsed symbols. */
	Elf64_Sym *syms;
};

#define ELF_PARSE_PHDR		(1 << 0)
#define ELF_PARSE_SHDR		(1 << 1)
#define ELF_PARSE_RELOC		(1 << 2)
#define ELF_PARSE_STRTAB	(1 << 3)
#define ELF_PARSE_SYMTAB	(1 << 4)

#define ELF_PARSE_ALL		(-1)

/*
 * Parse an ELF file contained within provide struct buffer. The ELF header
 * is always parsed while the flags value containing the ELF_PARSE_* values
 * determine if other parts of the ELF file will be parsed as well.
 * Returns 0 on success, < 0 error.
 */
int parse_elf(const struct buffer *pinput, struct parsed_elf *pelf, int flags);

/*
 * Clean up memory associated with parsed_elf.
 */
void parsed_elf_destroy(struct parsed_elf *pelf);


int
elf_headers(const struct buffer *pinput,
	    uint32_t arch,
	    Elf64_Ehdr *ehdr,
	    Elf64_Phdr **pphdr,
	    Elf64_Shdr **pshdr);

#endif /* ELFPARSING_H */
