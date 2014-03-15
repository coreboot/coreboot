/*
 * elf header parsing.
 *
 * Copyright (C) 2013 Google, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elfparsing.h"
#include "common.h"
#include "cbfs.h"

/*
 * Short form: this is complicated, but we've tried making it simple
 * and we keep hitting problems with our ELF parsing.
 *
 * The ELF parsing situation has always been a bit tricky.  In fact,
 * we (and most others) have been getting it wrong in small ways for
 * years. Recently this has caused real trouble for the ARM V8 build.
 * In this file we attempt to finally get it right for all variations
 * of endian-ness and word size and target architectures and
 * architectures we might get run on. Phew!. To do this we borrow a
 * page from the FreeBSD NFS xdr model (see elf_ehdr and elf_phdr),
 * the Plan 9 endianness functions (see xdr.c), and Go interfaces (see
 * how we use buffer structs in this file). This ends up being a bit
 * wordy at the lowest level, but greatly simplifies the elf parsing
 * code and removes a common source of bugs, namely, forgetting to
 * flip type endianness when referencing a struct member.
 *
 * ELF files can have four combinations of data layout: 32/64, and
 * big/little endian.  Further, to add to the fun, depending on the
 * word size, the size of the ELF structs varies. The coreboot SELF
 * format is simpler in theory: it's supposed to be always BE, and the
 * various struct members allow room for growth: the entry point is
 * always 64 bits, for example, so the size of a SELF struct is
 * constant, regardless of target architecture word size.  Hence, we
 * need to do some transformation of the ELF files.
 *
 * A given architecture, realistically, only supports one of the four
 * combinations at a time as the 'native' format. Hence, our code has
 * been sprinkled with every variation of [nh]to[hn][sll] over the
 * years. We've never quite gotten it all right, however, and a quick
 * pass over this code revealed another bug.  It's all worked because,
 * until now, all the working platforms that had CBFS were 32 LE. Even then,
 * however, bugs crept in: we recently realized that we're not
 * transforming the entry point to big format when we store into the
 * SELF image.
 *
 * The problem is essentially an XDR operation:
 * we have something in a foreign format and need to transform it.
 * It's most like XDR because:
 * 1) the byte order can be wrong
 * 2) the word size can be wrong
 * 3) the size of elements in the stream depends on the value
 *    of other elements in the stream
 * it's not like XDR because:
 * 1) the byte order can be right
 * 2) the word size can be right
 * 3) the struct members are all on a natural alignment
 *
 * Hence, this new approach.  To cover word size issues, we *always*
 * transform the two structs we care about, the file header and
 * program header, into a native struct in the 64 bit format:
 *
 * [32,little] -> [Elf64_Ehdr, Elf64_Phdr]
 * [64,little] -> [Elf64_Ehdr, Elf64_Phdr]
 * [32,big] -> [Elf64_Ehdr, Elf64_Phdr]
 * [64,big] -> [Elf64_Ehdr, Elf64_Phdr]
 * Then we just use those structs, and all the need for inline ntoh* goes away,
 * as well as all the chances for error.
 * This works because all the SELF structs have fields large enough for
 * the largest ELF 64 struct members, and all the Elf64 struct members
 * are at least large enough for all ELF 32 struct members.
 * We end up with one function to do all our ELF parsing, and two functions
 * to transform the headers. For the put case, we also have
 * XDR functions, and hopefully we'll never again spend 5 years with the
 * wrong endian-ness on an output value :-)
 * This should work for all word sizes and endianness we hope to target.
 * I *really* don't want to be here for 128 bit addresses.
 *
 * The parse functions are called with a pointer to an input buffer
 * struct. One might ask: are there enough bytes in the input buffer?
 * We know there need to be at *least* sizeof(Elf32_Ehdr) +
 * sizeof(Elf32_Phdr) bytes. Realistically, there has to be some data
 * too.  If we start to worry, though we have not in the past, we
 * might apply the simple test: the input buffer needs to be at least
 * sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) bytes because, even if it's
 * ELF 32, there's got to be *some* data! This is not theoretically
 * accurate but it is actually good enough in practice. It allows the
 * header transformation code to ignore the possibility of underrun.
 *
 * We also must accomodate different ELF files, and hence formats,
 * in the same cbfs invocation. We might load a 64-bit payload
 * on a 32-bit machine; we might even have a mixed armv7/armv8
 * SOC or even a system with an x86/ARM!
 *
 * A possibly problematic (though unlikely to be so) assumption
 * is that we expect the BIOS to remain in the lowest 32 bits
 * of the physical address space. Since ARMV8 has standardized
 * on that, and x86_64 also has, this seems a safe assumption.
 *
 * To repeat, ELF structs are different sizes because ELF struct
 * members are different sizes, depending on values in the ELF file
 * header. For this we use the functions defined in xdr.c, which
 * consume bytes, convert the endianness, and advance the data pointer
 * in the buffer struct.
 */


static int iself(const void *input)
{
	const Elf32_Ehdr *ehdr = input;
	return !memcmp(ehdr->e_ident, ELFMAG, 4);
}

/* Get the ident array, so we can figure out
 * endian-ness, word size, and in future other useful
 * parameters
 */
static void
elf_eident(struct buffer *input, Elf64_Ehdr *ehdr)
{
	bgets(input, ehdr->e_ident, sizeof(ehdr->e_ident));
}


static int
check_size(const struct buffer *b, size_t offset, size_t size, const char *desc)
{
	if (size == 0)
		return 0;

	if (offset >= buffer_size(b) || (offset + size) > buffer_size(b)) {
		ERROR("The file is not large enough for the '%s'. "
		      "%zu bytes @ offset %zu, input %zu bytes.\n",
		      desc, size, offset, buffer_size(b));
		return -1;
	}
	return 0;
}

static void
elf_ehdr(struct buffer *input, Elf64_Ehdr *ehdr, struct xdr *xdr, int bit64)
{
	ehdr->e_type = xdr->get16(input);
	ehdr->e_machine = xdr->get16(input);
	ehdr->e_version = xdr->get32(input);
	if (bit64){
		ehdr->e_entry = xdr->get64(input);
		ehdr->e_phoff = xdr->get64(input);
		ehdr->e_shoff = xdr->get64(input);
	} else {
		ehdr->e_entry = xdr->get32(input);
		ehdr->e_phoff = xdr->get32(input);
		ehdr->e_shoff = xdr->get32(input);
	}
	ehdr->e_flags = xdr->get32(input);
	ehdr->e_ehsize = xdr->get16(input);
	ehdr->e_phentsize = xdr->get16(input);
	ehdr->e_phnum = xdr->get16(input);
	ehdr->e_shentsize = xdr->get16(input);
	ehdr->e_shnum = xdr->get16(input);
	ehdr->e_shstrndx = xdr->get16(input);
}

static void
elf_phdr(struct buffer *pinput, Elf64_Phdr *phdr,
	 int entsize, struct xdr *xdr, int bit64)
{
	/*
	 * The entsize need not be sizeof(*phdr).
	 * Hence, it is easier to keep a copy of the input,
	 * as the xdr functions may not advance the input
	 * pointer the full entsize; rather than get tricky
	 * we just advance it below.
	 */
	struct buffer input;
	buffer_clone(&input, pinput);
	if (bit64){
		phdr->p_type = xdr->get32(&input);
		phdr->p_flags = xdr->get32(&input);
		phdr->p_offset = xdr->get64(&input);
		phdr->p_vaddr = xdr->get64(&input);
		phdr->p_paddr = xdr->get64(&input);
		phdr->p_filesz = xdr->get64(&input);
		phdr->p_memsz = xdr->get64(&input);
		phdr->p_align = xdr->get64(&input);
	} else {
		phdr->p_type = xdr->get32(&input);
		phdr->p_offset = xdr->get32(&input);
		phdr->p_vaddr = xdr->get32(&input);
		phdr->p_paddr = xdr->get32(&input);
		phdr->p_filesz = xdr->get32(&input);
		phdr->p_memsz = xdr->get32(&input);
		phdr->p_flags = xdr->get32(&input);
		phdr->p_align = xdr->get32(&input);
	}
	buffer_seek(pinput, entsize);
}

static void
elf_shdr(struct buffer *pinput, Elf64_Shdr *shdr,
	 int entsize, struct xdr *xdr, int bit64)
{
	/*
	 * The entsize need not be sizeof(*shdr).
	 * Hence, it is easier to keep a copy of the input,
	 * as the xdr functions may not advance the input
	 * pointer the full entsize; rather than get tricky
	 * we just advance it below.
	 */
	struct buffer input = *pinput;
	if (bit64){
		shdr->sh_name = xdr->get32(&input);
		shdr->sh_type = xdr->get32(&input);
		shdr->sh_flags = xdr->get64(&input);
		shdr->sh_addr = xdr->get64(&input);
		shdr->sh_offset = xdr->get64(&input);
		shdr->sh_size= xdr->get64(&input);
		shdr->sh_link = xdr->get32(&input);
		shdr->sh_info = xdr->get32(&input);
		shdr->sh_addralign = xdr->get64(&input);
		shdr->sh_entsize = xdr->get64(&input);
	} else {
		shdr->sh_name = xdr->get32(&input);
		shdr->sh_type = xdr->get32(&input);
		shdr->sh_flags = xdr->get32(&input);
		shdr->sh_addr = xdr->get32(&input);
		shdr->sh_offset = xdr->get32(&input);
		shdr->sh_size = xdr->get32(&input);
		shdr->sh_link = xdr->get32(&input);
		shdr->sh_info = xdr->get32(&input);
		shdr->sh_addralign = xdr->get32(&input);
		shdr->sh_entsize = xdr->get32(&input);
	}
	buffer_seek(pinput, entsize);
}

static int
phdr_read(const struct buffer *in, struct parsed_elf *pelf,
          struct xdr *xdr, int bit64)
{
	struct buffer b;
	Elf64_Phdr *phdr;
	Elf64_Ehdr *ehdr;
	int i;

	ehdr = &pelf->ehdr;
	/* cons up an input buffer for the headers.
	 * Note that the program headers can be anywhere,
	 * per the ELF spec, You'd be surprised how many ELF
	 * readers miss this little detail.
	 */
	buffer_splice(&b, in, ehdr->e_phoff, ehdr->e_phentsize * ehdr->e_phnum);
	if (check_size(in, ehdr->e_phoff, buffer_size(&b), "program headers"))
		return -1;

	/* gather up all the phdrs.
	 * We do them all at once because there is more
	 * than one loop over all the phdrs.
	 */
	phdr = calloc(ehdr->e_phnum, sizeof(*phdr));
	for (i = 0; i < ehdr->e_phnum; i++) {
		DEBUG("Parsing segment %d\n", i);
		elf_phdr(&b, &phdr[i], ehdr->e_phentsize, xdr, bit64);

		/* Ensure the contents are valid within the elf file. */
		if (check_size(in, phdr[i].p_offset, phdr[i].p_filesz,
	                  "segment contents"))
			return -1;
	}

	pelf->phdr = phdr;

	return 0;
}

static int
shdr_read(const struct buffer *in, struct parsed_elf *pelf,
          struct xdr *xdr, int bit64)
{
	struct buffer b;
	Elf64_Shdr *shdr;
	Elf64_Ehdr *ehdr;
	int i;

	ehdr = &pelf->ehdr;

	/* cons up an input buffer for the section headers.
	 * Note that the section headers can be anywhere,
	 * per the ELF spec, You'd be surprised how many ELF
	 * readers miss this little detail.
	 */
	buffer_splice(&b, in, ehdr->e_shoff, ehdr->e_shentsize * ehdr->e_shnum);
	if (check_size(in, ehdr->e_shoff, buffer_size(&b), "section headers"))
		return -1;

	/* gather up all the shdrs. */
	shdr = calloc(ehdr->e_shnum, sizeof(*shdr));
	for (i = 0; i < ehdr->e_shnum; i++) {
		DEBUG("Parsing section %d\n", i);
		elf_shdr(&b, &shdr[i], ehdr->e_shentsize, xdr, bit64);
	}

	pelf->shdr = shdr;

	return 0;
}

static int
reloc_read(const struct buffer *in, struct parsed_elf *pelf,
           struct xdr *xdr, int bit64)
{
	struct buffer b;
	Elf64_Word i;
	Elf64_Ehdr *ehdr;

	ehdr = &pelf->ehdr;
	pelf->relocs = calloc(ehdr->e_shnum, sizeof(Elf64_Rela *));

	/* Allocate array for each section that contains relocation entries. */
	for (i = 0; i < ehdr->e_shnum; i++) {
		Elf64_Shdr *shdr;
		Elf64_Rela *rela;
		Elf64_Xword j;
		Elf64_Xword nrelocs;
		int is_rela;

		shdr = &pelf->shdr[i];

		/* Only process REL and RELA sections. */
		if (shdr->sh_type != SHT_REL && shdr->sh_type != SHT_RELA)
			continue;

		DEBUG("Checking relocation section %u\n", i);

		/* Ensure the section that relocations apply is a valid. */
		if (shdr->sh_info >= ehdr->e_shnum ||
		    shdr->sh_info == SHN_UNDEF) {
			ERROR("Relocations apply to an invalid section: %u\n",
			      shdr[i].sh_info);
			return -1;
		}

		is_rela = shdr->sh_type == SHT_RELA;

		/* Determine the number relocations in this section. */
		nrelocs = shdr->sh_size / shdr->sh_entsize;

		pelf->relocs[i] = calloc(nrelocs, sizeof(Elf64_Rela));

		buffer_splice(&b, in, shdr->sh_offset, shdr->sh_size);
		if (check_size(in, shdr->sh_offset, buffer_size(&b),
		               "relocation section")) {
			ERROR("Relocation section %u failed.\n", i);
			return -1;
		}

		rela = pelf->relocs[i];
		for (j = 0; j < nrelocs; j++) {
			if (bit64) {
				rela->r_offset = xdr->get64(&b);
				rela->r_info = xdr->get64(&b);
				if (is_rela)
					rela->r_addend = xdr->get64(&b);
			} else {
				uint32_t r_info;

				rela->r_offset = xdr->get32(&b);
				r_info = xdr->get32(&b);
				rela->r_info = ELF64_R_INFO(ELF32_R_SYM(r_info),
				                          ELF32_R_TYPE(r_info));
				if (is_rela)
					rela->r_addend = xdr->get32(&b);
			}
			rela++;
		}
	}

	return 0;
}

static int strtab_read(const struct buffer *in, struct parsed_elf *pelf)
{
	Elf64_Ehdr *ehdr;
	Elf64_Word i;

	ehdr = &pelf->ehdr;

	if (ehdr->e_shstrndx >= ehdr->e_shnum) {
		ERROR("Section header string table index out of range: %d\n",
		      ehdr->e_shstrndx);
		return -1;
	}

	/* For each section of type SHT_STRTAB create a symtab buffer. */
	pelf->strtabs = calloc(ehdr->e_shnum, sizeof(struct buffer *));

	for (i = 0; i < ehdr->e_shnum; i++) {
		struct buffer *b;
		Elf64_Shdr *shdr = &pelf->shdr[i];

		if (shdr->sh_type != SHT_STRTAB)
			continue;

		b = calloc(1, sizeof(*b));
		buffer_splice(b, in, shdr->sh_offset, shdr->sh_size);
		if (check_size(in, shdr->sh_offset, buffer_size(b), "strtab")) {
			ERROR("STRTAB section not within bounds: %d\n", i);
			return -1;
		}
		pelf->strtabs[i] = b;
	}

	return 0;
}

static int
symtab_read(const struct buffer *in, struct parsed_elf *pelf,
            struct xdr *xdr, int bit64)
{
	Elf64_Ehdr *ehdr;
	Elf64_Shdr *shdr;
	Elf64_Half i;
	Elf64_Xword nsyms;
	Elf64_Sym *sym;
	struct buffer b;

	ehdr = &pelf->ehdr;

	shdr = NULL;
	for (i = 0; i < ehdr->e_shnum; i++) {
		if (pelf->shdr[i].sh_type != SHT_SYMTAB)
			continue;

		if (shdr != NULL) {
			ERROR("Multiple symbol sections found. %u and %u\n",
			      (unsigned int)(shdr - pelf->shdr), i);
			return -1;
		}

		shdr = &pelf->shdr[i];
	}

	if (shdr == NULL) {
		ERROR("No symbol table found.\n");
		return -1;
	}

	buffer_splice(&b, in, shdr->sh_offset, shdr->sh_size);
	if (check_size(in, shdr->sh_offset, buffer_size(&b), "symtab"))
		return -1;

	nsyms = shdr->sh_size / shdr->sh_entsize;

	pelf->syms = calloc(nsyms, sizeof(Elf64_Sym));

	for (i = 0; i < nsyms; i++) {
		sym = &pelf->syms[i];

		if (bit64) {
			sym->st_name = xdr->get32(&b);
			sym->st_info = xdr->get8(&b);
			sym->st_other = xdr->get8(&b);
			sym->st_shndx = xdr->get16(&b);
			sym->st_value = xdr->get64(&b);
			sym->st_size = xdr->get64(&b);
		} else {
			sym->st_name = xdr->get32(&b);
			sym->st_value = xdr->get32(&b);
			sym->st_size = xdr->get32(&b);
			sym->st_info = xdr->get8(&b);
			sym->st_other = xdr->get8(&b);
			sym->st_shndx = xdr->get16(&b);
		}
	}

	return 0;
}

int parse_elf(const struct buffer *pinput, struct parsed_elf *pelf, int flags)
{
	struct xdr *xdr = &xdr_le;
	int bit64 = 0;
	struct buffer input;
	Elf64_Ehdr *ehdr;

	/* Zero out the parsed elf structure. */
	memset(pelf, 0, sizeof(*pelf));

	if (!iself(buffer_get(pinput))) {
		ERROR("The stage file is not in ELF format!\n");
		return -1;
	}

	buffer_clone(&input, pinput);
	ehdr = &pelf->ehdr;
	elf_eident(&input, ehdr);
	bit64 = ehdr->e_ident[EI_CLASS] == ELFCLASS64;
	/* Assume LE unless we are sure otherwise.
	 * We're not going to take on the task of
	 * fully validating the ELF file. That way
	 * lies madness.
	 */
	if (ehdr->e_ident[EI_DATA] == ELFDATA2MSB)
		xdr = &xdr_be;

	elf_ehdr(&input, ehdr, xdr, bit64);

	/* Relocation processing requires section header parsing. */
	if (flags & ELF_PARSE_RELOC)
		flags |= ELF_PARSE_SHDR;

	/* String table processing requires section header parsing. */
	if (flags & ELF_PARSE_STRTAB)
		flags |= ELF_PARSE_SHDR;

	/* Symbole table processing requires section header parsing. */
	if (flags & ELF_PARSE_SYMTAB)
		flags |= ELF_PARSE_SHDR;

	if ((flags & ELF_PARSE_PHDR) && phdr_read(pinput, pelf, xdr, bit64))
		goto fail;

	if ((flags & ELF_PARSE_SHDR) && shdr_read(pinput, pelf, xdr, bit64))
		goto fail;

	if ((flags & ELF_PARSE_RELOC) && reloc_read(pinput, pelf, xdr, bit64))
		goto fail;

	if ((flags & ELF_PARSE_STRTAB) && strtab_read(pinput, pelf))
		goto fail;

	if ((flags & ELF_PARSE_SYMTAB) && symtab_read(pinput, pelf, xdr, bit64))
		goto fail;

	return 0;

fail:
	parsed_elf_destroy(pelf);
	return -1;
}

void parsed_elf_destroy(struct parsed_elf *pelf)
{
	Elf64_Half i;

	free(pelf->phdr);
	free(pelf->shdr);
	if (pelf->relocs != NULL) {
		for (i = 0; i < pelf->ehdr.e_shnum; i++)
			free(pelf->relocs[i]);
	}
	free(pelf->relocs);

	if (pelf->strtabs != NULL) {
		for (i = 0; i < pelf->ehdr.e_shnum; i++)
			free(pelf->strtabs[i]);
	}
	free(pelf->strtabs);
	free(pelf->syms);
}

/* Get the headers from the buffer.
 * Return -1 in the event of an error.
 * The section headers are optional; if NULL
 * is passed in for pshdr they won't be parsed.
 * We don't (yet) make payload parsing optional
 * because we've never seen a use case.
 */
int
elf_headers(const struct buffer *pinput,
	    uint32_t arch,
	    Elf64_Ehdr *ehdr,
	    Elf64_Phdr **pphdr,
	    Elf64_Shdr **pshdr)
{

	struct parsed_elf pelf;
	int flags;

	flags = ELF_PARSE_PHDR;

	if (pshdr != NULL)
		flags |= ELF_PARSE_SHDR;

	if (parse_elf(pinput, &pelf, flags))
		return -1;

	/* Copy out the parsed elf header. */
	memcpy(ehdr, &pelf.ehdr, sizeof(*ehdr));

	// The tool may work in architecture-independent way.
	if (arch != CBFS_ARCHITECTURE_UNKNOWN &&
	    !((ehdr->e_machine == EM_ARM) && (arch == CBFS_ARCHITECTURE_ARMV7)) &&
	    !((ehdr->e_machine == EM_386) && (arch == CBFS_ARCHITECTURE_X86))) {
		ERROR("The stage file has the wrong architecture\n");
		return -1;
	}

	*pphdr = calloc(ehdr->e_phnum, sizeof(Elf64_Phdr));
	memcpy(*pphdr, pelf.phdr, ehdr->e_phnum * sizeof(Elf64_Phdr));

	if (pshdr != NULL) {
		*pshdr = calloc(ehdr->e_shnum, sizeof(Elf64_Shdr));
		memcpy(*pshdr, pelf.shdr, ehdr->e_shnum * sizeof(Elf64_Shdr));
	}

	parsed_elf_destroy(&pelf);

	return 0;
}

/* ELF Writing  Support
 *
 * The ELF file is written according to the following layout:
 * +------------------+
 * |    ELF Header    |
 * +------------------+
 * | Section  Headers |
 * +------------------+
 * | Program  Headers |
 * +------------------+
 * |   String table   |
 * +------------------+ <- 4KiB Aligned
 * |     Code/Data    |
 * +------------------+
 */

/* Arbitray maximum number of sections. */
#define MAX_SECTIONS 16
struct elf_writer_section {
	Elf64_Shdr shdr;
	struct buffer content;
	const char *name;
};

struct elf_writer
{
	Elf64_Ehdr ehdr;
	struct xdr *xdr;
	size_t num_secs;
	struct elf_writer_section sections[MAX_SECTIONS];
	Elf64_Phdr *phdrs;
	struct elf_writer_section *shstrtab;
	int bit64;
};

struct elf_writer *elf_writer_init(const Elf64_Ehdr *ehdr)
{
	struct elf_writer *ew;
	Elf64_Shdr shdr;
	struct buffer empty_buffer;

	if (!iself(ehdr))
		return NULL;

	ew = calloc(1, sizeof(*ew));

	memcpy(&ew->ehdr, ehdr, sizeof(ew->ehdr));

	ew->bit64 = ew->ehdr.e_ident[EI_CLASS] == ELFCLASS64;

	/* Set the endinan ops. */
	if (ew->ehdr.e_ident[EI_DATA] == ELFDATA2MSB)
		ew->xdr = &xdr_be;
	else
		ew->xdr = &xdr_le;

	/* Reset count and offsets */
	ew->ehdr.e_phoff = 0;
	ew->ehdr.e_shoff = 0;
	ew->ehdr.e_shnum = 0;
	ew->ehdr.e_phnum = 0;

	memset(&empty_buffer, 0, sizeof(empty_buffer));
	memset(&shdr, 0, sizeof(shdr));

	/* Add SHT_NULL section header. */
	shdr.sh_type = SHT_NULL;
	elf_writer_add_section(ew, &shdr, &empty_buffer, NULL);

	/* Add section header string table and maintain reference to it.  */
	shdr.sh_type = SHT_STRTAB;
	elf_writer_add_section(ew, &shdr, &empty_buffer, ".shstrtab");
	ew->ehdr.e_shstrndx = ew->num_secs - 1;
	ew->shstrtab = &ew->sections[ew->ehdr.e_shstrndx];

	return ew;
}

/*
 * Clean up any internal state represented by ew. Aftewards the elf_writer
 * is invalid.
 */
void elf_writer_destroy(struct elf_writer *ew)
{
	if (ew->phdrs != NULL)
		free(ew->phdrs);
	free(ew);
}

/*
 * Add a section to the ELF file. Section type, flags, and memsize are
 * maintained from the passed in Elf64_Shdr. The buffer represents the
 * content of the section while the name is the name of section itself.
 * Returns < 0 on error, 0 on success.
 */
int elf_writer_add_section(struct elf_writer *ew, const Elf64_Shdr *shdr,
                           struct buffer *contents, const char *name)
{
	struct elf_writer_section *newsh;

	if (ew->num_secs == MAX_SECTIONS)
		return -1;

	newsh = &ew->sections[ew->num_secs];
	ew->num_secs++;

	memcpy(&newsh->shdr, shdr, sizeof(newsh->shdr));
	newsh->shdr.sh_offset = 0;

	newsh->name = name;
	if (contents != NULL)
		buffer_clone(&newsh->content, contents);

	return 0;
}

static void ehdr_write(struct elf_writer *ew, struct buffer *m)
{
	int i;

	for (i = 0; i < EI_NIDENT; i++)
		ew->xdr->put8(m, ew->ehdr.e_ident[i]);
	ew->xdr->put16(m, ew->ehdr.e_type);
	ew->xdr->put16(m, ew->ehdr.e_machine);
	ew->xdr->put32(m, ew->ehdr.e_version);
	if (ew->bit64) {
		ew->xdr->put64(m, ew->ehdr.e_entry);
		ew->xdr->put64(m, ew->ehdr.e_phoff);
		ew->xdr->put64(m, ew->ehdr.e_shoff);
	} else {
		ew->xdr->put32(m, ew->ehdr.e_entry);
		ew->xdr->put32(m, ew->ehdr.e_phoff);
		ew->xdr->put32(m, ew->ehdr.e_shoff);
	}
	ew->xdr->put32(m, ew->ehdr.e_flags);
	ew->xdr->put16(m, ew->ehdr.e_ehsize);
	ew->xdr->put16(m, ew->ehdr.e_phentsize);
	ew->xdr->put16(m, ew->ehdr.e_phnum);
	ew->xdr->put16(m, ew->ehdr.e_shentsize);
	ew->xdr->put16(m, ew->ehdr.e_shnum);
	ew->xdr->put16(m, ew->ehdr.e_shstrndx);
}

static void shdr_write(struct elf_writer *ew, size_t n, struct buffer *m)
{
	struct xdr *xdr = ew->xdr;
	int bit64 = ew->bit64;
	struct elf_writer_section *sec = &ew->sections[n];
	Elf64_Shdr *shdr = &sec->shdr;

	xdr->put32(m, shdr->sh_name);
	xdr->put32(m, shdr->sh_type);
	xdr->put32(m, shdr->sh_flags);
	if (bit64) {
		xdr->put64(m, shdr->sh_addr);
		xdr->put64(m, shdr->sh_offset);
		xdr->put64(m, shdr->sh_size);
		xdr->put32(m, shdr->sh_link);
		xdr->put32(m, shdr->sh_info);
		xdr->put64(m, shdr->sh_addralign);
		xdr->put64(m, shdr->sh_entsize);
	} else {
		xdr->put32(m, shdr->sh_addr);
		xdr->put32(m, shdr->sh_offset);
		xdr->put32(m, shdr->sh_size);
		xdr->put32(m, shdr->sh_link);
		xdr->put32(m, shdr->sh_info);
		xdr->put32(m, shdr->sh_addralign);
		xdr->put32(m, shdr->sh_entsize);
	}
}

static void
phdr_write(struct elf_writer *ew, struct buffer *m, Elf64_Phdr *phdr)
{
	if (ew->bit64) {
		ew->xdr->put32(m, phdr->p_type);
		ew->xdr->put32(m, phdr->p_flags);
		ew->xdr->put64(m, phdr->p_offset);
		ew->xdr->put64(m, phdr->p_vaddr);
		ew->xdr->put64(m, phdr->p_paddr);
		ew->xdr->put64(m, phdr->p_filesz);
		ew->xdr->put64(m, phdr->p_memsz);
		ew->xdr->put64(m, phdr->p_align);
	} else {
		ew->xdr->put32(m, phdr->p_type);
		ew->xdr->put32(m, phdr->p_offset);
		ew->xdr->put32(m, phdr->p_vaddr);
		ew->xdr->put32(m, phdr->p_paddr);
		ew->xdr->put32(m, phdr->p_filesz);
		ew->xdr->put32(m, phdr->p_memsz);
		ew->xdr->put32(m, phdr->p_flags);
		ew->xdr->put32(m, phdr->p_align);
	}

}

/*
 * Serialize the ELF file to the output buffer. Return < 0 on error,
 * 0 on success.
 */
int elf_writer_serialize(struct elf_writer *ew, struct buffer *out)
{
	Elf64_Half i;
	Elf64_Xword metadata_size;
	Elf64_Xword program_size;
	Elf64_Off shstroffset;
	size_t shstrlen;
	struct buffer metadata;
	struct buffer phdrs;
	struct buffer data;
	struct buffer *strtab;

	INFO("Writing %zu sections.\n", ew->num_secs);

	/* Determine size of sections to be written. */
	program_size = 0;
	/* Start with 1 byte for first byte of section header string table. */
	shstrlen = 1;
	for (i = 0; i < ew->num_secs; i++) {
		struct elf_writer_section *sec = &ew->sections[i];

		if (sec->shdr.sh_flags & SHF_ALLOC)
			ew->ehdr.e_phnum++;

		program_size += buffer_size(&sec->content);

		/* Keep track of the length sections' names. */
		if (sec->name != NULL) {
			sec->shdr.sh_name = shstrlen;
			shstrlen += strlen(sec->name) + 1;
		}
	}
	ew->ehdr.e_shnum = ew->num_secs;
	metadata_size = 0;
	metadata_size += ew->ehdr.e_ehsize;
	metadata_size += ew->ehdr.e_shnum * ew->ehdr.e_shentsize;
	metadata_size += ew->ehdr.e_phnum * ew->ehdr.e_phentsize;
	shstroffset = metadata_size;
	/* Align up section header string size and metadata size to 4KiB */
	metadata_size = ALIGN(metadata_size + shstrlen, 4096);

	if (buffer_create(out, metadata_size + program_size, "elfout")) {
		ERROR("Could not create output buffer for ELF.\n");
		return -1;
	}

	INFO("Created %zu output buffer for ELF file.\n", buffer_size(out));

	/*
	 * Write out ELF header. Section headers come right after ELF header
	 * followed by the program headers. Buffers need to be created first
	 * to do the writing.
	 */
	ew->ehdr.e_shoff = ew->ehdr.e_ehsize;
	ew->ehdr.e_phoff = ew->ehdr.e_shoff +
	                   ew->ehdr.e_shnum * ew->ehdr.e_shentsize;

	buffer_splice(&metadata, out, 0, metadata_size);
	buffer_splice(&phdrs, out, ew->ehdr.e_phoff,
	              ew->ehdr.e_phnum * ew->ehdr.e_phentsize);
	buffer_splice(&data, out, metadata_size, program_size);
	/* Set up the section header string table contents. */
	strtab = &ew->shstrtab->content;
	buffer_splice(strtab, out, shstroffset, shstrlen);
	ew->shstrtab->shdr.sh_size = shstrlen;

	/* Reset current locations. */
	buffer_set_size(&metadata, 0);
	buffer_set_size(&data, 0);
	buffer_set_size(&phdrs, 0);
	buffer_set_size(strtab, 0);

	/* ELF Header */
	ehdr_write(ew, &metadata);

	/* Write out section headers, section strings, section content, and
	 * program headers. */
	ew->xdr->put8(strtab, 0);
	for (i = 0; i < ew->num_secs; i++) {
		Elf64_Phdr phdr;
		struct elf_writer_section *sec = &ew->sections[i];

		/* Update section offsets. Be sure to not update SHT_NULL. */
		if (sec == ew->shstrtab)
			sec->shdr.sh_offset = shstroffset;
		else if (i != 0)
			sec->shdr.sh_offset = buffer_size(&data) +
			                      metadata_size;
		shdr_write(ew, i, &metadata);

		/* Add section name to string table. */
		if (sec->name != NULL)
			bputs(strtab, sec->name, strlen(sec->name) + 1);

		if (!(sec->shdr.sh_flags & SHF_ALLOC))
			continue;

		bputs(&data, buffer_get(&sec->content),
		      buffer_size(&sec->content));

		phdr.p_type = PT_LOAD;
		phdr.p_offset = sec->shdr.sh_offset;
		phdr.p_vaddr = sec->shdr.sh_addr;
		phdr.p_paddr = sec->shdr.sh_addr;
		phdr.p_filesz = buffer_size(&sec->content);
		phdr.p_memsz = sec->shdr.sh_size;
		phdr.p_flags = 0;
		if (sec->shdr.sh_flags & SHF_EXECINSTR)
			phdr.p_flags |= PF_X | PF_R;
		if (sec->shdr.sh_flags & SHF_WRITE)
			phdr.p_flags |= PF_W;
		phdr.p_align = sec->shdr.sh_addralign;
		phdr_write(ew, &phdrs, &phdr);
	}

	return 0;
}
