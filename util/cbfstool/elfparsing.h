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
	    Elf64_Ehdr *ehdr,
	    Elf64_Phdr **pphdr,
	    Elf64_Shdr **pshdr);

/* ELF writing support. */
struct elf_writer;

/*
 * Initialize a 64-bit ELF header provided the inputs. While the structure
 * is a 64-bit header one can specify a 32-bit machine. The 64-bit version
 * is just used as a common structure. If one wants to specify the entry
 * point, for example, the caller can set it after filling in the common
 * bits. The machine, nbits, and endian values should be from the ELF
 * definitions (e.g. EM_386, ELFCLASS32, and ELFDATA2LSB) found in elf.h
 * with no endian conversion required.
 */
void elf_init_eheader(Elf64_Ehdr *ehdr, int machine, int nbits, int endian);

/*
 * Initialize a new ELF writer. Deafult machine type, endianness, etc is
 * copied from the passed in Elf64_Ehdr. Returns NULL on failure, valid
 * pointer on success.
 */
struct elf_writer *elf_writer_init(const Elf64_Ehdr *ehdr);

/*
 * Clean up any internal state represented by ew. Aftewards the elf_writer
 * is invalid.
 * It is safe to call elf_writer_destroy with ew as NULL. It returns without
 * performing any action.
 */
void elf_writer_destroy(struct elf_writer *ew);

/*
 * Add a section to the ELF file. Section type, flags, and memsize are
 * maintained from the passed in Elf64_Shdr. The buffer represents the
 * content of the section while the name is the name of section itself.
 * Returns < 0 on error, 0 on success.
 */
int elf_writer_add_section(struct elf_writer *ew, const Elf64_Shdr *shdr,
                           struct buffer *contents, const char *name);

/* Add an absolute symbol to the ELF file returning < 0 on error, index of
 * symbol otherwise. */
int elf_writer_add_symbol(struct elf_writer *ew, const char *name,
				const char *section_name,
				Elf64_Addr value, Elf64_Word size,
				int binding, int type);

/* Add an absolute relocation referencing the provided symbol name. Returns < 0
 * on error, 0 on success. */
int elf_writer_add_rel(struct elf_writer *ew, const char *sym, Elf64_Addr addr);

/*
 * Serialize the ELF file to the output buffer. Return < 0 on error,
 * 0 on success.
 */
int elf_writer_serialize(struct elf_writer *ew, struct buffer *out);

#endif /* ELFPARSING_H */
