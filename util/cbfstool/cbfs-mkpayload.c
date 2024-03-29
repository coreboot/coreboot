/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commonlib/endian.h>

#include "elfparsing.h"
#include "common.h"
#include "cbfs.h"
#include "fv.h"
#include "coff.h"
#include "fdt.h"

/* serialize the seg array into the buffer.
 * The buffer is assumed to be large enough.
 */
void xdr_segs(struct buffer *output,
	struct cbfs_payload_segment *segs, int nseg)
{
	struct buffer outheader;
	int i;

	outheader.data = output->data;
	outheader.size = 0;

	for(i = 0; i < nseg; i++){
		xdr_be.put32(&outheader, segs[i].type);
		xdr_be.put32(&outheader, segs[i].compression);
		xdr_be.put32(&outheader, segs[i].offset);
		xdr_be.put64(&outheader, segs[i].load_addr);
		xdr_be.put32(&outheader, segs[i].len);
		xdr_be.put32(&outheader, segs[i].mem_len);
	}
}

void xdr_get_seg(struct cbfs_payload_segment *out,
		struct cbfs_payload_segment *in)
{
	struct buffer inheader;

	inheader.data = (void *)in;
	inheader.size = sizeof(*in);

	out->type = xdr_be.get32(&inheader);
	out->compression = xdr_be.get32(&inheader);
	out->offset = xdr_be.get32(&inheader);
	out->load_addr = xdr_be.get64(&inheader);
	out->len = xdr_be.get32(&inheader);
	out->mem_len = xdr_be.get32(&inheader);
}

int parse_elf_to_payload(const struct buffer *input, struct buffer *output,
			 enum cbfs_compression algo)
{
	Elf64_Phdr *phdr;
	Elf64_Ehdr ehdr;
	char *header;
	int headers;
	int segments = 1;
	int isize = 0, osize = 0;
	int doffset = 0;
	struct cbfs_payload_segment *segs = NULL;
	int i;
	int ret = 0;

	comp_func_ptr compress = compression_function(algo);
	if (!compress)
		return -1;

	if (elf_headers(input, &ehdr, &phdr, NULL) < 0)
		return -1;

	DEBUG("start: parse_elf_to_payload\n");
	headers = ehdr.e_phnum;
	header = input->data;

	/* Count the number of segment headers - we only care about PT_LOAD
	   headers, because that's what we're actually going to load */
	for (i = 0; i < headers; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		/* Empty segments are never interesting */
		if (phdr[i].p_memsz == 0)
			continue;

		isize += phdr[i].p_filesz;

		segments++;
	}
	/* Allocate and initialize the segment header array */
	segs = calloc(segments, sizeof(*segs));
	if (segs == NULL) {
		ret = -1;
		goto out;
	}
	/* Allocate a block of memory to store the data in */
	if (buffer_create(output, (segments * sizeof(*segs)) + isize,
			  input->name) != 0) {
		ret = -1;
		goto out;
	}
	memset(output->data, 0, output->size);

	doffset = (segments * sizeof(*segs));

	/* set up for output marshaling. This is a bit
	 * tricky as we are marshaling the headers at the front,
	 * and the data starting after the headers. We need to convert
	 * the headers to the right format but the data
	 * passes through unchanged. Unlike most XDR code,
	 * we are doing these two concurrently. The doffset is
	 * used to compute the address for the raw data, and the
	 * outheader is used to marshal the headers. To make it simpler
	 * for The Reader, we set up the headers in a separate array,
	 * then marshal them all at once to the output.
	 */
	segments = 0;

	for (i = 0; i < headers; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;
		if (phdr[i].p_memsz == 0)
			continue;
		if (phdr[i].p_filesz == 0) {
			segs[segments].type = PAYLOAD_SEGMENT_BSS;
			segs[segments].load_addr = phdr[i].p_paddr;
			segs[segments].mem_len = phdr[i].p_memsz;
			segs[segments].offset = doffset;

			segments++;
			continue;
		}

		if (phdr[i].p_flags & PF_X)
			segs[segments].type = PAYLOAD_SEGMENT_CODE;
		else
			segs[segments].type = PAYLOAD_SEGMENT_DATA;
		segs[segments].load_addr = phdr[i].p_paddr;
		segs[segments].mem_len = phdr[i].p_memsz;
		segs[segments].offset = doffset;

		/* If the compression failed or made the section is larger,
		   use the original stuff */

		int len;
		if (compress((char *)&header[phdr[i].p_offset],
			     phdr[i].p_filesz, output->data + doffset, &len) ||
		    (unsigned int)len > phdr[i].p_filesz) {
			WARN("Compression failed or would make the data bigger "
			     "- disabled.\n");
			segs[segments].compression = 0;
			segs[segments].len = phdr[i].p_filesz;
			memcpy(output->data + doffset,
			       &header[phdr[i].p_offset], phdr[i].p_filesz);
		} else {
			segs[segments].compression = algo;
			segs[segments].len = len;
		}

		doffset += segs[segments].len;
		osize += segs[segments].len;

		segments++;
	}

	segs[segments].type = PAYLOAD_SEGMENT_ENTRY;
	segs[segments++].load_addr = ehdr.e_entry;

	output->size = (segments * sizeof(*segs)) + osize;
	xdr_segs(output, segs, segments);

out:
	if (segs) free(segs);
	if (phdr) free(phdr);
	return ret;
}

int parse_flat_binary_to_payload(const struct buffer *input,
				 struct buffer *output,
				 uint64_t loadaddress,
				 uint64_t entrypoint,
				 enum cbfs_compression algo)
{
	comp_func_ptr compress;
	struct cbfs_payload_segment segs[2] = { {0} };
	int doffset, len = 0;

	compress = compression_function(algo);
	if (!compress)
		return -1;

	DEBUG("start: parse_flat_binary_to_payload\n");
	if (buffer_create(output, (sizeof(segs) + input->size),
			  input->name) != 0)
		return -1;
	memset(output->data, 0, output->size);

	doffset = (2 * sizeof(*segs));

	/* Prepare code segment */
	segs[0].type = PAYLOAD_SEGMENT_CODE;
	segs[0].load_addr = loadaddress;
	segs[0].mem_len = input->size;
	segs[0].offset = doffset;

	if (!compress(input->data, input->size, output->data + doffset, &len) &&
	    (unsigned int)len < input->size) {
		segs[0].compression = algo;
		segs[0].len = len;
	} else {
		WARN("Compression failed or would make the data bigger "
		     "- disabled.\n");
		segs[0].compression = 0;
		segs[0].len = input->size;
		memcpy(output->data + doffset, input->data, input->size);
	}

	/* prepare entry point segment */
	segs[1].type = PAYLOAD_SEGMENT_ENTRY;
	segs[1].load_addr = entrypoint;
	output->size = doffset + segs[0].len;
	xdr_segs(output, segs, 2);
	return 0;
}

int parse_fv_to_payload(const struct buffer *input, struct buffer *output,
			enum cbfs_compression algo)
{
	comp_func_ptr compress;
	struct cbfs_payload_segment segs[2] = { {0} };
	int doffset, len = 0;
	firmware_volume_header_t *fv;
	firmware_volume_ext_header_t *fvh_ext;
	ffs_file_header_t *fh;
	common_section_header_t *cs;
	dos_header_t *dh;
	coff_header_t *ch;
	int dh_offset;

	uint32_t loadaddress = 0;
	uint32_t entrypoint = 0;

	compress = compression_function(algo);
	if (!compress)
		return -1;

	DEBUG("start: parse_fv_to_payload\n");

	fv = (firmware_volume_header_t *)input->data;
	if (fv->signature != FV_SIGNATURE) {
		INFO("Not a UEFI firmware volume.\n");
		return -1;
	}

	fh = (ffs_file_header_t *)(input->data + fv->header_length);
	if (fv->ext_header_offs != 0) {
		fvh_ext = (firmware_volume_ext_header_t *)((uintptr_t)fv + fv->ext_header_offs);
		fh = (ffs_file_header_t *)((uintptr_t)fvh_ext + fvh_ext->ext_header_size);
		fh = (ffs_file_header_t *)(((uintptr_t)fh + 7) & ~7);
	}

	while (fh->file_type == FILETYPE_PAD) {
		unsigned long offset = (fh->size[2] << 16) | (fh->size[1] << 8) | fh->size[0];
		DEBUG("skipping %lu bytes of FV padding\n", offset);
		fh = (ffs_file_header_t *)(((uintptr_t)fh) + offset);
	}
	if (fh->file_type != FILETYPE_SEC) {
		ERROR("Not a usable UEFI firmware volume.\n");
		INFO("First file in first FV not a SEC core.\n");
		return -1;
	}

	cs = (common_section_header_t *)&fh[1];
	while (cs->section_type == SECTION_RAW) {
		unsigned long offset = (cs->size[2] << 16) | (cs->size[1] << 8) | cs->size[0];
		DEBUG("skipping %lu bytes of section padding\n", offset);
		cs = (common_section_header_t *)(((uintptr_t)cs) + offset);
	}
	if (cs->section_type != SECTION_PE32) {
		ERROR("Not a usable UEFI firmware volume.\n");
		INFO("Section type not PE32.\n");
		return -1;
	}

	dh = (dos_header_t *)&cs[1];
	if (dh->signature != DOS_MAGIC) {
		ERROR("Not a usable UEFI firmware volume.\n");
		INFO("DOS header signature wrong.\n");
		return -1;
	}

	dh_offset = (unsigned long)dh - (unsigned long)input->data;
	DEBUG("dos header offset = %x\n", dh_offset);

	ch = (coff_header_t *)(((uintptr_t)dh)+dh->e_lfanew);

	if (ch->machine == MACHINE_TYPE_X86) {
		pe_opt_header_32_t *ph;
		ph = (pe_opt_header_32_t *)&ch[1];
		if (ph->signature != PE_HDR_32_MAGIC) {
			WARN("PE header signature incorrect.\n");
			return -1;
		}
		DEBUG("image base %x\n", ph->image_addr);
		DEBUG("entry point %x\n", ph->entry_point);

		loadaddress = ph->image_addr - dh_offset;
		entrypoint = ph->image_addr + ph->entry_point;
	} else if (ch->machine == MACHINE_TYPE_X64 || ch->machine == MACHINE_TYPE_ARM64) {
		pe_opt_header_64_t *ph;
		ph = (pe_opt_header_64_t *)&ch[1];
		if (ph->signature != PE_HDR_64_MAGIC) {
			WARN("PE header signature incorrect.\n");
			return -1;
		}
		DEBUG("image base %lx\n", (unsigned long)ph->image_addr);
		DEBUG("entry point %x\n", ph->entry_point);

		loadaddress = ph->image_addr - dh_offset;
		entrypoint = ph->image_addr + ph->entry_point;
	} else {
		ERROR("Machine type not x86, x64, or arm64.\n");
		return -1;
	}

	if (buffer_create(output, (sizeof(segs) + input->size),
			  input->name) != 0)
		return -1;

	memset(output->data, 0, output->size);

	doffset = (sizeof(segs));

	/* Prepare code segment */
	segs[0].type = PAYLOAD_SEGMENT_CODE;
	segs[0].load_addr = loadaddress;
	segs[0].mem_len = input->size;
	segs[0].offset = doffset;

	if (!compress(input->data, input->size, output->data + doffset, &len) &&
	    (unsigned int)len < input->size) {
		segs[0].compression = algo;
		segs[0].len = len;
	} else {
		WARN("Compression failed or would make the data bigger "
		     "- disabled.\n");
		segs[0].compression = 0;
		segs[0].len = input->size;
		memcpy(output->data + doffset, input->data, input->size);
	}

	/* prepare entry point segment */
	segs[1].type = PAYLOAD_SEGMENT_ENTRY;
	segs[1].load_addr = entrypoint;
	output->size = doffset + segs[0].len;
	xdr_segs(output, segs, 2);
	return 0;

}

int parse_fit_to_payload(const struct buffer *input, struct buffer *output,
			 enum cbfs_compression algo)
{
	struct fdt_header *fdt_h;

	DEBUG("start: parse_fit_to_payload\n");

	fdt_h = buffer_get(input);
	if (read_be32(&fdt_h->magic) != FDT_HEADER_MAGIC) {
		INFO("Not a FIT payload.\n");
		return -1;
	}

	/**
	 * For developers:
	 * Compress the kernel binary you're sourcing in your its-script
	 * manually with LZ4 or LZMA and add 'compression = "lz4"' or "lzma" to
	 * the kernel@1 node in the its-script before assembling the image with
	 * mkimage.
	 */
	if (algo != CBFS_COMPRESS_NONE) {
		ERROR("FIT images don't support whole-image compression,"
		      " compress the kernel component instead!\n");
		return -1;
	}

	if (buffer_create(output, buffer_size(input), input->name) != 0)
		return -1;

	memcpy(buffer_get(output), buffer_get(input), buffer_size(input));

	DEBUG("done\n");

	return 0;
}
