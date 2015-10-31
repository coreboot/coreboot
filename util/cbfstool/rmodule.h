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

#ifndef TOOL_RMODULE_H
#define TOOL_RMODULE_H

#include "elfparsing.h"
#include "common.h"

struct arch_ops {
	int arch;
	/* Determine if relocation is a valid type for the architecture. */
	int (*valid_type)(Elf64_Rela *rel);
	/* Determine if relocation should be emitted. */
	int (*should_emit)(Elf64_Rela *rel);
};

/*
 * The fields in rmod_context are read-only to the user. These are
 * exposed for easy shareability.
 */
struct rmod_context {
	/* Ops to process relocations. */
	const struct arch_ops *ops;

	/* endian conversion ops */
	struct xdr *xdr;

	/* Parsed ELF sturcture. */
	struct parsed_elf pelf;
	/* Program segment. */
	Elf64_Phdr *phdr;

	/* Collection of relocation addresses fixup in the module. */
	Elf64_Xword nrelocs;
	Elf64_Addr *emitted_relocs;

	/* The following fields are addresses within the linked program.  */
	Elf64_Addr parameters_begin;
	Elf64_Addr parameters_end;
	Elf64_Addr bss_begin;
	Elf64_Addr bss_end;
};

struct reloc_filter {
	/* Return < 0 on error. 0 to ignore relocation and 1 to include
	 * relocation. */
	int (*filter)(struct reloc_filter *f, const Elf64_Rela *r);
	/* Pointer for filter provides */
	void *context;
};

/*
 * Parse an ELF file within the elfin buffer and fill in the elfout buffer
 * with a created rmodule in ELF format. Return 0 on success, < 0 on error.
 */
int rmodule_create(const struct buffer *elfin, struct buffer *elfout);

/*
 * Initialize an rmodule context from an ELF buffer. Returns 0 on scucess, < 0
 * on error.
 */
int rmodule_init(struct rmod_context *ctx, const struct buffer *elfin);

/*
 * Collect all the relocations that apply to the program in
 * nrelocs/emitted_relocs. One can optionally provide a reloc_filter object
 * to help in relocation filtering. The filter function will be called twice:
 * once for counting and once for emitting. The same response should be
 * provided for each call. Returns 0 on success, < 0 on error.
 */
int rmodule_collect_relocations(struct rmod_context *c, struct reloc_filter *f);

/* Clean up the memory consumed by the rmdoule context. */
void rmodule_cleanup(struct rmod_context *ctx);

/*
 * Create an ELF file from the passed in rmodule in the buffer. The buffer
 * contents will be replaced with an ELF file. Returns 1 if buff doesn't
 * contain an rmodule and < 0 on failure, 0 on success.
 */
int rmodule_stage_to_elf(Elf64_Ehdr *ehdr, struct buffer *buff);

#endif /* TOOL_RMODULE_H */
