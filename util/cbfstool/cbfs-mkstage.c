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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "cbfs.h"
#include "elf.h"

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
 * Hence, this new approach.
 * To cover word size issues, we *always* transform the two structs we care about,
 * the file header and program header, into a native struct in the 64 bit format:
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

/* Get the ident array, so we can figure out
 * endian-ness, word size, and in future other useful
 * parameters
 */
static void
elf_eident(struct buffer *input, Elf64_Ehdr *ehdr)
{
	memmove(ehdr->e_ident, input->data, sizeof(ehdr->e_ident));
	input->data += sizeof(ehdr->e_ident);
	input->size -= sizeof(ehdr->e_ident);
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

void
elf_phdr(struct buffer *pinput, Elf64_Phdr *phdr, int entsize, struct xdr *xdr, int bit64)
{
	/*
	 * The entsize need not be sizeof(*phdr).
	 * Hence, it is easier to keep a copy of the input,
	 * as the xdr functions may not advance the input
	 * pointer the full entsize; rather than get tricky
	 * we just advance it below.
	 */
	struct buffer input = *pinput;
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
	pinput->size -= entsize;
	pinput->data += entsize;
}

/* Get the headers from the buffer.
 * Return -1 in the event of an error.
 */
int elf_headers(const struct buffer *pinput, Elf64_Ehdr *ehdr, Elf64_Phdr **pphdr)
{
	int i;
	struct xdr *xdr = &xdr_le;
	int bit64 = 0;
	struct buffer input = *(struct buffer *)pinput;
	struct buffer phdr_buf;
	Elf64_Phdr *phdr;

	if (!iself((unsigned char *)pinput->data)) {
		ERROR("The stage file is not in ELF format!\n");
		return -1;
	}

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

	if (pinput->size < ehdr->e_phoff){
		ERROR("The program header offset is greater than "
		      "the remaining file size."
		      "%ld bytes left, program header offset is %ld \n",
		      pinput->size, ehdr->e_phoff);
		return -1;
	}
	/* cons up an input buffer for the headers.
	 * Note that the program headers can be anywhere,
	 * per the ELF spec, You'd be surprised how many ELF
	 * readers miss this little detail.
	 */
	phdr_buf.data = &pinput->data[ehdr->e_phoff];
	phdr_buf.size = ehdr->e_phentsize * ehdr->e_phnum;
	if (phdr_buf.size > (pinput->size - ehdr->e_phoff)){
		ERROR("The file is not large enough for the program headers."
		      "%ld bytes left, %ld bytes of headers\n",
		      pinput->size - ehdr->e_phoff, phdr_buf.size);
		return -1;
	}
	/* gather up all the phdrs.
	 * We do them all at once because there is more
	 * than one loop over all the phdrs.
	 */
	phdr = calloc(sizeof(*phdr), ehdr->e_phnum);
	for (i = 0; i < ehdr->e_phnum; i++)
		elf_phdr(&phdr_buf, &phdr[i], ehdr->e_phentsize, xdr, bit64);
	*pphdr = phdr;
	return 0;
}
/* returns size of result, or -1 if error.
 * Note that, with the new code, this function
 * works for all elf files, not just the restricted set.
 */
int parse_elf_to_stage(const struct buffer *input, struct buffer *output,
		       comp_algo algo, uint32_t *location)
{
	Elf64_Phdr *phdr;
	Elf64_Ehdr ehdr;
	char *buffer;
	struct buffer outheader;

	int headers;
	int i, outlen;
	uint32_t data_start, data_end, mem_end;

	comp_func_ptr compress = compression_function(algo);
	if (!compress)
		return -1;

	DEBUG("start: parse_elf_to_stage(location=0x%x)\n", *location);

	if (elf_headers(input, &ehdr, &phdr) < 0)
		return -1;

	headers = ehdr.e_phnum;

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
		ERROR("data ends (%08lx) before it starts(%08lx). Make sure the "
		      "ELF file is correct and resides in ROM space.\n",
		      (unsigned long)data_end, (unsigned long)data_start);
		exit(1);
	}

	/* allocate an intermediate buffer for the data */
	buffer = calloc(data_end - data_start, 1);

	if (buffer == NULL) {
		ERROR("Unable to allocate memory: %m\n");
		return -1;
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

		memcpy(buffer + (l_start - data_start),
		       &input->data[phdr[i].p_offset + l_offset],
		       phdr[i].p_filesz - l_offset);
	}

	/* Now make the output buffer */
	if (buffer_create(output, sizeof(struct cbfs_stage) + data_end - data_start,
			  input->name) != 0) {
		ERROR("Unable to allocate memory: %m\n");
		free(buffer);
		return -1;
	}
	memset(output->data, 0, output->size);

	/* Compress the data, at which point we'll know information
	 * to fill out the header. This seems backward but it works because
	 * - the output header is a known size (not always true in many xdr's)
	 * - we do need to know the compressed output size first
	 */
	compress(buffer, data_end - data_start, (output->data + sizeof(struct cbfs_stage)),
		 &outlen);
	free(buffer);

	/* Set up for output marshaling. */
	outheader.data = output->data;
	outheader.size = 0;
	/* N.B. The original plan was that SELF data was B.E.
	 * but: this is all L.E.
	 * Maybe we should just change the spec.
	 */
	xdr_le.put32(&outheader, algo);
	xdr_le.put64(&outheader, ehdr.e_entry);
	xdr_le.put64(&outheader, data_start);
	xdr_le.put32(&outheader, outlen);
	xdr_le.put32(&outheader, mem_end - data_start);

	if (*location)
		*location -= sizeof(struct cbfs_stage);
	output->size = sizeof(struct cbfs_stage) + outlen;
	return 0;
}
