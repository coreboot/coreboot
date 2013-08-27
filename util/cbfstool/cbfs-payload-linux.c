/*
 * cbfs-payload-linux
 *
 * Copyright (C) 2013 Patrick Georgi <patrick@georgi-clan.de>
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
#include "linux.h"

/* TODO:
 *   handle special arguments
 *     mem= argument - only affects loading decisions (kernel + initrd), not e820 -> build time
 *     vga= argument (FILO ignores this)
 *   add support for more parameters to trampoline:
 *     alt_mem_k, ext_mem_k (not strictly necessary since e820 takes precedence)
 *     framebuffer/console values
 *
 *  larger work:
 *     is compress() safe to use in a size constrained buffer? ie. do(es) the
 *     compression algorithm(s) stop once the compression result reaches input
 *     size (ie. incompressible data)?
 */
int parse_bzImage_to_payload(const struct buffer *input,
			     struct buffer *output, const char *initrd_name,
			     char *cmdline, comp_algo algo)
{
	int cur_len = 0;
	int num_segments = 3; /* parameter block, real kernel, and trampoline */

	comp_func_ptr compress = compression_function(algo);
	if (!compress)
		return -1;

	unsigned int initrd_base = 64*1024*1024;
	unsigned int initrd_size = 0;
	void *initrd_data = NULL;
	if (initrd_name != NULL) {
		/* TODO: load initrd, set initrd_size */
		num_segments++;
		FILE *initrd_file = fopen(initrd_name, "rb");
		if (!initrd_file) {
			ERROR("could not open initrd.\n");
			return -1;
		}
		fseek(initrd_file, 0, SEEK_END);
		initrd_size = ftell(initrd_file);
		fseek(initrd_file, 0, SEEK_SET);
		initrd_data = malloc(initrd_size);
		if (!initrd_data) {
			ERROR("could not allocate memory for initrd.\n");
			return -1;
		}
		if (fread(initrd_data, initrd_size, 1, initrd_file) != 1) {
			ERROR("could not load initrd.\n");
			return -1;
		}
		fclose(initrd_file);
	}

	unsigned int cmdline_size = 0;
	if (cmdline != NULL) {
		num_segments++;
		cmdline_size = strlen(cmdline) + 1;
	}

	struct linux_header *hdr = (struct linux_header *)input->data;
	unsigned int setup_size = 4 * 512;
	if (hdr->setup_sects != 0) {
		setup_size = (hdr->setup_sects + 1) * 512;
	}

	/* Setup parameter block. Imitate FILO. */
	struct linux_params params;
	params.mount_root_rdonly = hdr->root_flags;
	params.orig_root_dev = hdr->root_dev;
	/* Sensible video defaults. Might be overridden on runtime by coreboot tables. */
	params.orig_video_mode = 3;
	params.orig_video_cols = 80;
	params.orig_video_lines = 25;
	params.orig_video_isVGA = 1;
	params.orig_video_points = 16;

	params.loader_type = 0xff; /* Unregistered Linux loader */

	if (cmdline != NULL) {
		if (hdr->protocol_version < 0x202) {
			params.cl_magic = CL_MAGIC_VALUE;
			params.cl_offset = COMMAND_LINE_LOC - LINUX_PARAM_LOC;
		} else {
			params.cmd_line_ptr = COMMAND_LINE_LOC;
		}
	}

	unsigned long kernel_base = 0x100000;
	if ((hdr->protocol_version >= 0x200) && (!hdr->loadflags & 1)) {
		kernel_base = 0x1000; /* zImage kernel */
	}
	/* kernel prefers an address, so listen */
	if ((hdr->protocol_version >= 0x20a) && (!(hdr->pref_address >> 32))) {
		kernel_base = hdr->pref_address;
	}
	if (hdr->protocol_version >= 0x205) {
		params.relocatable_kernel = hdr->relocatable_kernel;
		params.kernel_alignment = hdr->kernel_alignment;
		if (hdr->relocatable_kernel != 0) {
			/* 16 MB should be way outside coreboot's playground,
			 * so if possible (relocatable kernel) use that to
			 * avoid a trampoline copy. */
			kernel_base = ALIGN(16*1024*1024, params.kernel_alignment);
		}
	}

	/* We have a trampoline and use that, but it can simply use
	 * this information for its jump to real Linux. */
	params.kernel_start = kernel_base;

	void *kernel_data = input->data + setup_size;
	unsigned int kernel_size = input->size - setup_size;

	if (initrd_data != NULL) {
		/* TODO: this is a bit of a hack. Linux recommends to store
		 * initrd near to end-of-mem, but that's hard to do on build
		 * time. It definitely fails to read the image if it's too
		 * close to the kernel, so give it some room.
		 */
		initrd_base = ALIGN(kernel_base + kernel_size, 16*1024*1024);

		params.initrd_start = initrd_base;
		params.initrd_size = initrd_size;
	}

	struct cbfs_payload_segment *segs;
	unsigned long doffset = (num_segments + 1) * sizeof(*segs);

	/* Allocate a block of memory to store the data in */
	int isize = sizeof(params) + kernel_size + cmdline_size + initrd_size;
	if (buffer_create(output, doffset + isize, input->name) != 0)
		return -1;
	memset(output->data, 0, output->size);

	segs = (struct cbfs_payload_segment *)output->data;

	/* parameter block */
	segs[0].type = PAYLOAD_SEGMENT_DATA;
	segs[0].load_addr = htonll(LINUX_PARAM_LOC);
	segs[0].mem_len = htonl(sizeof(params));
	segs[0].offset = htonl(doffset);

	compress((void*)&params, sizeof(params), output->data + doffset, &cur_len);
	segs[0].compression = htonl(algo);
	segs[0].len = htonl(cur_len);

	doffset += cur_len;

	/* code block */
	segs[1].type = PAYLOAD_SEGMENT_CODE;
	segs[1].load_addr = htonll(kernel_base);
	segs[1].mem_len = htonl(kernel_size);
	segs[1].offset = htonl(doffset);

	compress(kernel_data, kernel_size, output->data + doffset, &cur_len);
	segs[1].compression = htonl(algo);
	segs[1].len = htonl(cur_len);

	doffset += cur_len;

	/* trampoline */
	extern void *trampoline_start;
	extern long trampoline_size;

	unsigned int entrypoint = 0x40000; /* TODO: any better place? */

	segs[2].type = PAYLOAD_SEGMENT_CODE;
	segs[2].load_addr = htonll(entrypoint);
	segs[2].mem_len = htonl(trampoline_size);
	segs[2].offset = htonl(doffset);

	compress(trampoline_start, trampoline_size, output->data + doffset, &cur_len);
	segs[2].compression = htonl(algo);
	segs[2].len = htonl(cur_len);

	doffset += cur_len;

	if (cmdline_size > 0) {
		/* command line block */
		segs[3].type = PAYLOAD_SEGMENT_DATA;
		segs[3].load_addr = htonll(COMMAND_LINE_LOC);
		segs[3].mem_len = htonl(cmdline_size);
		segs[3].offset = htonl(doffset);

		compress(cmdline, cmdline_size, output->data + doffset, &cur_len);
		segs[3].compression = htonl(algo);
		segs[3].len = htonl(cur_len);

		doffset += cur_len;
	}

	if (initrd_size > 0) {
		/* setup block */
		segs[num_segments-1].type = PAYLOAD_SEGMENT_DATA;
		segs[num_segments-1].load_addr = htonll(initrd_base);
		segs[num_segments-1].mem_len = htonl(initrd_size);
		segs[num_segments-1].offset = htonl(doffset);

		compress(initrd_data, initrd_size, output->data + doffset, &cur_len);
		segs[num_segments-1].compression = htonl(algo);
		segs[num_segments-1].len = htonl(cur_len);

		doffset += cur_len;
	}

	/* prepare entry point segment */
	segs[num_segments].type = PAYLOAD_SEGMENT_ENTRY;
	segs[num_segments].load_addr = htonll(entrypoint);
	output->size = doffset;

	return 0;
}


