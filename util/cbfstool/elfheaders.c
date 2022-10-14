/* elf header parsing */
/* SPDX-License-Identifier: GPL-2.0-only */

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
 * We also must accommodate different ELF files, and hence formats,
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
	buffer_splice(&b, in, ehdr->e_phoff,
		      (uint32_t)ehdr->e_phentsize * ehdr->e_phnum);
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
	                  "segment contents")) {
			free(phdr);
			return -1;
		}
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
	buffer_splice(&b, in, ehdr->e_shoff,
		      (uint32_t)ehdr->e_shentsize * ehdr->e_shnum);
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
			free(b);
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
	Elf64_Half shnum;
	Elf64_Xword i;
	Elf64_Xword nsyms;
	Elf64_Sym *sym;
	struct buffer b;

	ehdr = &pelf->ehdr;

	shdr = NULL;
	for (shnum = 0; shnum < ehdr->e_shnum; shnum++) {
		if (pelf->shdr[shnum].sh_type != SHT_SYMTAB)
			continue;

		if (shdr != NULL) {
			ERROR("Multiple symbol sections found. %u and %u\n",
			      (unsigned int)(shdr - pelf->shdr), shnum);
			return -1;
		}

		shdr = &pelf->shdr[shnum];
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
		DEBUG("The stage file is not in ELF format!\n");
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

void elf_init_eheader(Elf64_Ehdr *ehdr, int machine, int nbits, int endian)
{
	memset(ehdr, 0, sizeof(*ehdr));
	ehdr->e_ident[EI_MAG0] = ELFMAG0;
	ehdr->e_ident[EI_MAG1] = ELFMAG1;
	ehdr->e_ident[EI_MAG2] = ELFMAG2;
	ehdr->e_ident[EI_MAG3] = ELFMAG3;
	ehdr->e_ident[EI_CLASS] = nbits;
	ehdr->e_ident[EI_DATA] = endian;
	ehdr->e_ident[EI_VERSION] = EV_CURRENT;
	ehdr->e_type = ET_EXEC;
	ehdr->e_machine = machine;
	ehdr->e_version = EV_CURRENT;
	if (nbits == ELFCLASS64) {
		ehdr->e_ehsize = sizeof(Elf64_Ehdr);
		ehdr->e_phentsize = sizeof(Elf64_Phdr);
		ehdr->e_shentsize = sizeof(Elf64_Shdr);
	} else {
		ehdr->e_ehsize = sizeof(Elf32_Ehdr);
		ehdr->e_phentsize = sizeof(Elf32_Phdr);
		ehdr->e_shentsize = sizeof(Elf32_Shdr);
	}
}

/* Arbitrary maximum number of sections. */
#define MAX_SECTIONS 16
struct elf_writer_section {
	Elf64_Shdr shdr;
	struct buffer content;
	const char *name;
};

struct elf_writer_string_table {
	size_t next_offset;
	size_t max_size;
	char *buffer;
};

struct elf_writer_sym_table {
	size_t max_entries;
	size_t num_entries;
	Elf64_Sym *syms;
};

#define MAX_REL_NAME 32
struct elf_writer_rel {
	size_t num_entries;
	size_t max_entries;
	Elf64_Rel *rels;
	struct elf_writer_section *sec;
	char name[MAX_REL_NAME];
};

struct elf_writer
{
	Elf64_Ehdr ehdr;
	struct xdr *xdr;
	size_t num_secs;
	struct elf_writer_section sections[MAX_SECTIONS];
	struct elf_writer_rel rel_sections[MAX_SECTIONS];
	Elf64_Phdr *phdrs;
	struct elf_writer_section *shstrtab_sec;
	struct elf_writer_section *strtab_sec;
	struct elf_writer_section *symtab_sec;
	struct elf_writer_string_table strtab;
	struct elf_writer_sym_table symtab;
	int bit64;
};

static size_t section_index(struct elf_writer *ew,
					struct elf_writer_section *sec)
{
	return sec - &ew->sections[0];
}

static struct elf_writer_section *last_section(struct elf_writer *ew)
{
	return &ew->sections[ew->num_secs - 1];
}

static void strtab_init(struct elf_writer *ew, size_t size)
{
	struct buffer b;
	Elf64_Shdr shdr;

	/* Start adding strings after the initial NUL entry. */
	ew->strtab.next_offset = 1;
	ew->strtab.max_size = size;
	ew->strtab.buffer = calloc(1, ew->strtab.max_size);

	buffer_init(&b, NULL, ew->strtab.buffer, ew->strtab.max_size);
	memset(&shdr, 0, sizeof(shdr));
	shdr.sh_type = SHT_STRTAB;
	shdr.sh_addralign = 1;
	shdr.sh_size = ew->strtab.max_size;
	elf_writer_add_section(ew, &shdr, &b, ".strtab");
	ew->strtab_sec = last_section(ew);
}

static void symtab_init(struct elf_writer *ew, size_t max_entries)
{
	struct buffer b;
	Elf64_Shdr shdr;

	memset(&shdr, 0, sizeof(shdr));
	shdr.sh_type = SHT_SYMTAB;

	if (ew->bit64) {
		shdr.sh_entsize = sizeof(Elf64_Sym);
		shdr.sh_addralign = sizeof(Elf64_Addr);
	} else {
		shdr.sh_entsize = sizeof(Elf32_Sym);
		shdr.sh_addralign = sizeof(Elf32_Addr);
	}

	shdr.sh_size = shdr.sh_entsize * max_entries;

	ew->symtab.syms = calloc(max_entries, sizeof(Elf64_Sym));
	ew->symtab.num_entries = 1;
	ew->symtab.max_entries = max_entries;

	buffer_init(&b, NULL, ew->symtab.syms, shdr.sh_size);

	elf_writer_add_section(ew, &shdr, &b, ".symtab");
	ew->symtab_sec = last_section(ew);
}

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
	ew->shstrtab_sec = last_section(ew);
	ew->ehdr.e_shstrndx = section_index(ew, ew->shstrtab_sec);

	/* Add a small string table and symbol table. */
	strtab_init(ew, 4096);
	symtab_init(ew, 100);

	return ew;
}

/*
 * Clean up any internal state represented by ew. Aftewards the elf_writer
 * is invalid.
 * It is safe to call elf_writer_destroy with ew as NULL. It returns without
 * performing any action.
 */
void elf_writer_destroy(struct elf_writer *ew)
{
	int i;
	if (ew == NULL)
		return;
	if (ew->phdrs != NULL)
		free(ew->phdrs);
	free(ew->strtab.buffer);
	free(ew->symtab.syms);
	for (i = 0; i < MAX_SECTIONS; i++)
		free(ew->rel_sections[i].rels);
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
	if (bit64) {
		xdr->put64(m, shdr->sh_flags);
		xdr->put64(m, shdr->sh_addr);
		xdr->put64(m, shdr->sh_offset);
		xdr->put64(m, shdr->sh_size);
		xdr->put32(m, shdr->sh_link);
		xdr->put32(m, shdr->sh_info);
		xdr->put64(m, shdr->sh_addralign);
		xdr->put64(m, shdr->sh_entsize);
	} else {
		xdr->put32(m, shdr->sh_flags);
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

static int section_consecutive(struct elf_writer *ew, Elf64_Half secidx)
{
	Elf64_Half i;
	struct elf_writer_section *prev_alloc = NULL;

	if (secidx == 0)
		return 0;

	for (i = 0; i < secidx; i++) {
		if (ew->sections[i].shdr.sh_flags & SHF_ALLOC)
			prev_alloc = &ew->sections[i];
	}

	if (prev_alloc == NULL)
		return 0;

	if (prev_alloc->shdr.sh_addr + prev_alloc->shdr.sh_size ==
	    ew->sections[secidx].shdr.sh_addr)
		return 1;

	return 0;
}

static void write_phdrs(struct elf_writer *ew, struct buffer *phdrs)
{
	Elf64_Half i;
	Elf64_Phdr phdr;
	size_t num_written = 0;
	size_t num_needs_write = 0;

	for (i = 0; i < ew->num_secs; i++) {
		struct elf_writer_section *sec = &ew->sections[i];

		if (!(sec->shdr.sh_flags & SHF_ALLOC))
			continue;

		if (!section_consecutive(ew, i)) {
			/* Write out previously set phdr. */
			if (num_needs_write != num_written) {
				phdr_write(ew, phdrs, &phdr);
				num_written++;
			}
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
			num_needs_write++;

		} else {
			/* Accumulate file size and memsize. The assumption
			 * is that each section is either NOBITS or full
			 * (sh_size == file size). This is standard in that
			 * an ELF section doesn't have a file size component. */
			if (sec->shdr.sh_flags & SHF_EXECINSTR)
				phdr.p_flags |= PF_X | PF_R;
			if (sec->shdr.sh_flags & SHF_WRITE)
				phdr.p_flags |= PF_W;
			phdr.p_filesz += buffer_size(&sec->content);
			phdr.p_memsz += sec->shdr.sh_size;
		}
	}

	/* Write out the last phdr. */
	if (num_needs_write != num_written) {
		phdr_write(ew, phdrs, &phdr);
		num_written++;
	}
	assert(num_written == ew->ehdr.e_phnum);
}

static void fixup_symbol_table(struct elf_writer *ew)
{
	struct elf_writer_section *sec = ew->symtab_sec;

	/* If there is only the NULL section, mark section as inactive. */
	if (ew->symtab.num_entries == 1) {
		sec->shdr.sh_type = SHT_NULL;
		sec->shdr.sh_size = 0;
	} else {
		size_t i;
		struct buffer wr;

		buffer_clone(&wr, &sec->content);
		/* To appease xdr. */
		buffer_set_size(&wr, 0);
		for (i = 0; i < ew->symtab.num_entries; i++) {
			/* Create local copy as were over-writing backing
			 * store of the symbol. */
			Elf64_Sym sym = ew->symtab.syms[i];
			if (ew->bit64) {
				ew->xdr->put32(&wr, sym.st_name);
				ew->xdr->put8(&wr, sym.st_info);
				ew->xdr->put8(&wr, sym.st_other);
				ew->xdr->put16(&wr, sym.st_shndx);
				ew->xdr->put64(&wr, sym.st_value);
				ew->xdr->put64(&wr, sym.st_size);
			} else {
				ew->xdr->put32(&wr, sym.st_name);
				ew->xdr->put32(&wr, sym.st_value);
				ew->xdr->put32(&wr, sym.st_size);
				ew->xdr->put8(&wr, sym.st_info);
				ew->xdr->put8(&wr, sym.st_other);
				ew->xdr->put16(&wr, sym.st_shndx);
			}
		}

		/* Update section size. */
		sec->shdr.sh_size = sec->shdr.sh_entsize;
		sec->shdr.sh_size *= ew->symtab.num_entries;

		/* Fix up sh_link to point to string table. */
		sec->shdr.sh_link = section_index(ew, ew->strtab_sec);
		/* sh_info is supposed to be 1 greater than symbol table
		 * index of last local binding. Just use max symbols. */
		sec->shdr.sh_info = ew->symtab.num_entries;
	}

	buffer_set_size(&sec->content, sec->shdr.sh_size);
}

static void fixup_relocations(struct elf_writer *ew)
{
	int i;
	Elf64_Xword type;

	switch (ew->ehdr.e_machine) {
	case EM_386:
		type = R_386_32;
		break;
	case EM_X86_64:
		type =  R_AMD64_64;
		break;
	case EM_ARM:
		type = R_ARM_ABS32;
		break;
	case EM_AARCH64:
		type = R_AARCH64_ABS64;
		break;
	case EM_MIPS:
		type = R_MIPS_32;
		break;
	case EM_RISCV:
		type = R_RISCV_32;
		break;
	case EM_PPC64:
		type = R_PPC64_ADDR32;
		break;
	default:
		ERROR("Unable to handle relocations for e_machine %x\n",
			ew->ehdr.e_machine);
		return;
	}

	for (i = 0; i < MAX_SECTIONS; i++) {
		struct elf_writer_rel *rel_sec = &ew->rel_sections[i];
		struct elf_writer_section *sec = rel_sec->sec;
		struct buffer writer;
		size_t j;

		if (sec == NULL)
			continue;

		/* Update section header size as well as content size. */
		buffer_init(&sec->content, sec->content.name, rel_sec->rels,
				rel_sec->num_entries * sec->shdr.sh_entsize);
		sec->shdr.sh_size = buffer_size(&sec->content);
		buffer_clone(&writer, &sec->content);
		/* To make xdr happy. */
		buffer_set_size(&writer, 0);

		for (j = 0; j < ew->rel_sections[i].num_entries; j++) {
			/* Make copy as we're overwriting backing store. */
			Elf64_Rel rel = rel_sec->rels[j];
			rel.r_info = ELF64_R_INFO(ELF64_R_SYM(rel.r_info),
						  ELF64_R_TYPE(type));

			if (ew->bit64) {
				ew->xdr->put64(&writer, rel.r_offset);
				ew->xdr->put64(&writer, rel.r_info);
			} else {
				Elf32_Rel rel32;
				rel32.r_offset = rel.r_offset;
				rel32.r_info =
					ELF32_R_INFO(ELF64_R_SYM(rel.r_info),
						     ELF64_R_TYPE(rel.r_info));
				ew->xdr->put32(&writer, rel32.r_offset);
				ew->xdr->put32(&writer, rel32.r_info);
			}
		}
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

	/* Perform any necessary work for special sections. */
	fixup_symbol_table(ew);
	fixup_relocations(ew);

	/* Determine size of sections to be written. */
	program_size = 0;
	/* Start with 1 byte for first byte of section header string table. */
	shstrlen = 1;
	for (i = 0; i < ew->num_secs; i++) {
		struct elf_writer_section *sec = &ew->sections[i];

		if (sec->shdr.sh_flags & SHF_ALLOC) {
			if (!section_consecutive(ew, i))
				ew->ehdr.e_phnum++;
		}

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
	metadata_size += (Elf64_Xword)ew->ehdr.e_shnum * ew->ehdr.e_shentsize;
	metadata_size += (Elf64_Xword)ew->ehdr.e_phnum * ew->ehdr.e_phentsize;
	shstroffset = metadata_size;
	/* Align up section header string size and metadata size to 4KiB */
	metadata_size = ALIGN_UP(metadata_size + shstrlen, 4096);

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
			   (Elf64_Off)ew->ehdr.e_shnum * ew->ehdr.e_shentsize;

	buffer_splice(&metadata, out, 0, metadata_size);
	buffer_splice(&phdrs, out, ew->ehdr.e_phoff,
		      (uint32_t)ew->ehdr.e_phnum * ew->ehdr.e_phentsize);
	buffer_splice(&data, out, metadata_size, program_size);
	/* Set up the section header string table contents. */
	strtab = &ew->shstrtab_sec->content;
	buffer_splice(strtab, out, shstroffset, shstrlen);
	ew->shstrtab_sec->shdr.sh_size = shstrlen;

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
		struct elf_writer_section *sec = &ew->sections[i];

		/* Update section offsets. Be sure to not update SHN_UNDEF. */
		if (sec == ew->shstrtab_sec)
			sec->shdr.sh_offset = shstroffset;
		else if (i != SHN_UNDEF)
			sec->shdr.sh_offset = buffer_size(&data) +
			                      metadata_size;

		shdr_write(ew, i, &metadata);

		/* Add section name to string table. */
		if (sec->name != NULL)
			bputs(strtab, sec->name, strlen(sec->name) + 1);

		/* Output section data for all sections but SHN_UNDEF and
		 * section header string table. */
		if (i != SHN_UNDEF && sec != ew->shstrtab_sec)
			bputs(&data, buffer_get(&sec->content),
			      buffer_size(&sec->content));
	}

	write_phdrs(ew, &phdrs);

	return 0;
}

/* Add a string to the string table returning index on success, < 0 on error. */
static int elf_writer_add_string(struct elf_writer *ew, const char *new)
{
	size_t current_offset;
	size_t new_len;

	for (current_offset = 0; current_offset < ew->strtab.next_offset; ) {
		const char *str = ew->strtab.buffer + current_offset;
		size_t len = strlen(str) + 1;

		if (!strcmp(str, new))
			return current_offset;
		current_offset += len;
	}

	new_len = strlen(new) + 1;

	if (current_offset + new_len > ew->strtab.max_size) {
		ERROR("No space for string in .strtab.\n");
		return -1;
	}

	memcpy(ew->strtab.buffer + current_offset, new, new_len);
	ew->strtab.next_offset = current_offset + new_len;

	return current_offset;
}

static int elf_writer_section_index(struct elf_writer *ew, const char *name)
{
	size_t i;

	for (i = 0; i < ew->num_secs; i++) {
		if (ew->sections[i].name == NULL)
			continue;
		if (!strcmp(ew->sections[i].name, name))
			return i;
	}

	ERROR("ELF Section not found: %s\n", name);

	return -1;
}

int elf_writer_add_symbol(struct elf_writer *ew, const char *name,
				const char *section_name,
				Elf64_Addr value, Elf64_Word size,
				int binding, int type)
{
	int i;
	Elf64_Sym sym = {
		.st_value = value,
		.st_size = size,
		.st_info = ELF64_ST_INFO(binding, type),
	};

	if (ew->symtab.max_entries == ew->symtab.num_entries) {
		ERROR("No more symbol entries left.\n");
		return -1;
	}

	i = elf_writer_add_string(ew, name);
	if (i < 0)
		return -1;
	sym.st_name = i;

	i = elf_writer_section_index(ew, section_name);
	if (i < 0)
		return -1;
	sym.st_shndx = i;

	ew->symtab.syms[ew->symtab.num_entries++] = sym;

	return 0;
}

static int elf_sym_index(struct elf_writer *ew, const char *sym)
{
	int j;
	size_t i;
	Elf64_Word st_name;

	/* Determine index of symbol in the string table. */
	j = elf_writer_add_string(ew, sym);
	if (j < 0)
		return -1;

	st_name = j;

	for (i = 0; i < ew->symtab.num_entries; i++)
		if (ew->symtab.syms[i].st_name == st_name)
			return i;

	return -1;
}

static struct elf_writer_rel *rel_section(struct elf_writer *ew,
						const Elf64_Rel *r)
{
	Elf64_Sym *sym;
	struct elf_writer_rel *rel;
	Elf64_Shdr shdr;
	struct buffer b;

	sym = &ew->symtab.syms[ELF64_R_SYM(r->r_info)];

	/* Determine if section has been initialized yet. */
	rel = &ew->rel_sections[sym->st_shndx];
	if (rel->sec != NULL)
		return rel;

	memset(&shdr, 0, sizeof(shdr));
	shdr.sh_type = SHT_REL;
	shdr.sh_link = section_index(ew, ew->symtab_sec);
	shdr.sh_info = sym->st_shndx;

	if (ew->bit64) {
		shdr.sh_addralign = sizeof(Elf64_Addr);
		shdr.sh_entsize = sizeof(Elf64_Rel);
	} else {
		shdr.sh_addralign = sizeof(Elf32_Addr);
		shdr.sh_entsize = sizeof(Elf32_Rel);
	}

	if ((strlen(".rel") + strlen(ew->sections[sym->st_shndx].name) + 1) >
	    MAX_REL_NAME) {
		ERROR("Rel Section name won't fit\n");
		return NULL;
	}

	strcat(rel->name, ".rel");
	strcat(rel->name, ew->sections[sym->st_shndx].name);
	buffer_init(&b, rel->name, NULL, 0);

	elf_writer_add_section(ew, &shdr, &b, rel->name);
	rel->sec = last_section(ew);

	return rel;
}

static int add_rel(struct elf_writer_rel *rel_sec, const Elf64_Rel *rel)
{
	if (rel_sec->num_entries == rel_sec->max_entries) {
		size_t num = rel_sec->max_entries * 2;
		Elf64_Rel *old_rels;

		if (num == 0)
			num = 128;

		old_rels = rel_sec->rels;
		rel_sec->rels = calloc(num, sizeof(Elf64_Rel));

		memcpy(rel_sec->rels, old_rels,
			rel_sec->num_entries * sizeof(Elf64_Rel));
		free(old_rels);

		rel_sec->max_entries = num;
	}

	rel_sec->rels[rel_sec->num_entries] = *rel;
	rel_sec->num_entries++;

	return 0;
}

int elf_writer_add_rel(struct elf_writer *ew, const char *sym, Elf64_Addr addr)
{
	Elf64_Rel rel;
	Elf64_Xword sym_info;
	int sym_index;
	struct elf_writer_rel *rel_sec;

	sym_index = elf_sym_index(ew, sym);

	if (sym_index < 0) {
		ERROR("Unable to locate symbol: %s\n", sym);
		return -1;
	}

	sym_info = sym_index;

	/* The relocation type will get fixed prior to serialization. */
	rel.r_offset = addr;
	rel.r_info = ELF64_R_INFO(sym_info, 0);

	rel_sec = rel_section(ew, &rel);

	if (rel_sec == NULL)
		return -1;

	return add_rel(rel_sec, &rel);
}

int elf_program_file_size(const struct buffer *input, size_t *file_size)
{
	Elf64_Ehdr ehdr;
	Elf64_Phdr *phdr;
	int i;
	size_t loadable_file_size = 0;

	if (elf_headers(input, &ehdr, &phdr, NULL))
		return -1;

	for (i = 0; i < ehdr.e_phnum; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;
		loadable_file_size += phdr[i].p_filesz;
	}

	*file_size = loadable_file_size;

	free(phdr);

	return 0;
}
