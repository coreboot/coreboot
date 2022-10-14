/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "cbfs.h"
#include "linux.h"

/* trampoline */
extern unsigned char trampoline[];
extern unsigned int trampoline_len;

/*
 * Current max number of segments include:
 *
 * 1. parameters
 * 2. kernel
 * 3. trampoline
 * 4. optional cmdline
 * 5. optional initrd
 * 6. terminating entry segment
 */
#define MAX_NUM_SEGMENTS 6

struct bzpayload {
	/* Input variables. */
	int num_segments;
	struct cbfs_payload_segment segs[MAX_NUM_SEGMENTS];
	struct buffer parameters;
	struct buffer kernel;
	struct buffer trampoline;
	struct buffer cmdline;
	struct buffer initrd;
	/* Output variables. */
	enum cbfs_compression algo;
	comp_func_ptr compress;
	struct buffer output;
	size_t offset;
	struct cbfs_payload_segment *out_seg;
};

static int bzp_init(struct bzpayload *bzp, enum cbfs_compression algo)
{
	memset(bzp, 0, sizeof(*bzp));

	/*
	 * Need at least the terminating entry segment.
	 */
	bzp->num_segments = 1;

	bzp->algo = algo;
	bzp->compress = compression_function(algo);
	if (bzp->compress == NULL) {
		ERROR("Invalid compression algorithm specified.\n");
		return -1;
	}

	return 0;
}

static int bzp_add_initrd(struct bzpayload *bzp, const char *fname)
{
	if (fname == NULL)
		return 0;

	if (buffer_from_file(&bzp->initrd, fname)) {
		ERROR("could not open initrd.\n");
		return -1;
	}

	bzp->num_segments++;

	return 0;
}

static void bzp_add_segment(struct bzpayload *bzp, struct buffer *b, void *data,
                            size_t size)
{
	buffer_init(b, NULL, data, size);
	bzp->num_segments++;
}

static int bzp_add_trampoline(struct bzpayload *bzp)
{
	bzp_add_segment(bzp, &bzp->trampoline, trampoline,
			trampoline_len);
	return 0;
}

static int bzp_add_cmdline(struct bzpayload *bzp, char *cmdline)
{
	if (cmdline == NULL)
		return 0;

	bzp_add_segment(bzp, &bzp->cmdline, cmdline, strlen(cmdline) + 1);

	return 0;
}

static int bzp_add_params(struct bzpayload *bzp, struct linux_params *params)
{
	bzp_add_segment(bzp, &bzp->parameters, params, sizeof(*params));

	return 0;
}

static int bzp_add_kernel(struct bzpayload *bzp, const struct buffer *in,
                          size_t setup_size)
{
	char *input = buffer_get(in);
	size_t kern_sz = buffer_size(in) - setup_size;

	bzp_add_segment(bzp, &bzp->kernel, &input[setup_size], kern_sz);

	return 0;
}

static int bzp_init_output(struct bzpayload *bzp, const char *name)
{
	size_t sz = 0;

	sz += buffer_size(&bzp->parameters);
	sz += buffer_size(&bzp->kernel);
	sz += buffer_size(&bzp->trampoline);
	sz += buffer_size(&bzp->cmdline);
	sz += buffer_size(&bzp->initrd);

	bzp->offset = bzp->num_segments * sizeof(struct cbfs_payload_segment);
	sz += bzp->offset;

	if (buffer_create(&bzp->output, sz, name) != 0)
		return -1;

	bzp->out_seg = &bzp->segs[0];

	return 0;
}

static void bzp_output_segment(struct bzpayload *bzp, struct buffer *b,
                               uint32_t type, uint64_t load_addr)
{
	struct buffer out;
	struct cbfs_payload_segment *seg;
	int len = 0;

	/* Don't process empty buffers. */
	if (b != NULL && buffer_size(b) == 0)
		return;

	seg = bzp->out_seg;
	seg->type = type;
	seg->load_addr = load_addr;
	bzp->out_seg++;

	/* No buffer associated with segment. */
	if (b == NULL)
		return;

	/* Use a temp buffer for easier management. */
	buffer_splice(&out, &bzp->output, bzp->offset, buffer_size(b));

	seg->mem_len = buffer_size(b);
	seg->offset = bzp->offset;
	bzp->compress(buffer_get(b), buffer_size(b), buffer_get(&out), &len);
	seg->compression = bzp->algo;
	seg->len = len;

	/* Update output offset. */
	bzp->offset += len;
}

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
			     char *cmdline, enum cbfs_compression algo)
{
	struct bzpayload bzp;
	unsigned int initrd_base = 64*1024*1024;
	struct linux_header *hdr = (struct linux_header *)input->data;
	unsigned int setup_size = 4 * 512;

	if (bzp_init(&bzp, algo) != 0)
		return -1;

	if (bzp_add_trampoline(&bzp) != 0)
		return -1;

	if (bzp_add_initrd(&bzp, initrd_name) != 0)
		return -1;

	if (bzp_add_cmdline(&bzp, cmdline) != 0)
		return -1;

	if (hdr->setup_sects != 0) {
		setup_size = (hdr->setup_sects + 1) * 512;
	} else {
		WARN("hdr->setup_sects is 0, which could cause boot problems.\n");
	}

	/* Setup parameter block. Imitate FILO. */
	struct linux_params params;

	memset(&params, 0, sizeof(struct linux_params));

	params.mount_root_rdonly = hdr->root_flags;
	params.orig_root_dev = hdr->root_dev;
	params.init_size = hdr->init_size;

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
	if ((hdr->protocol_version < 0x200) || !(hdr->loadflags & 1)) {
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
			kernel_base = ALIGN_UP(16*1024*1024, params.kernel_alignment);
			if (hdr->init_size == 0) {
				ERROR("init_size 0 for relocatable kernel\n");
				return -1;
			}
		}
	}

	/* We have a trampoline and use that, but it can simply use
	 * this information for its jump to real Linux. */
	params.kernel_start = kernel_base;

	if (bzp_add_kernel(&bzp, input, setup_size) != 0)
		return -1;

	if (buffer_size(&bzp.initrd) != 0) {
		/* TODO: this is a bit of a hack. Linux recommends to store
		 * initrd near to end-of-mem, but that's hard to do on build
		 * time. It definitely fails to read the image if it's too
		 * close to the kernel, so give it some room.
		 */
		initrd_base = kernel_base + buffer_size(&bzp.kernel);
		initrd_base = ALIGN_UP(initrd_base, 64*1024*1024);

		params.initrd_start = initrd_base;
		params.initrd_size = buffer_size(&bzp.initrd);
	}

	if (bzp_add_params(&bzp, &params) != 0)
		return -1;

	if (bzp_init_output(&bzp, input->name) != 0)
		return -1;

	/* parameter block */
	bzp_output_segment(&bzp, &bzp.parameters,
	                   PAYLOAD_SEGMENT_DATA, LINUX_PARAM_LOC);

	/* code block */
	bzp_output_segment(&bzp, &bzp.kernel,
	                   PAYLOAD_SEGMENT_CODE, kernel_base);

	/* trampoline */
	bzp_output_segment(&bzp, &bzp.trampoline,
	                   PAYLOAD_SEGMENT_CODE, TRAMPOLINE_ENTRY_LOC);

	/* cmdline */
	bzp_output_segment(&bzp, &bzp.cmdline,
	                   PAYLOAD_SEGMENT_DATA, COMMAND_LINE_LOC);

	/* initrd */
	bzp_output_segment(&bzp, &bzp.initrd,
	                   PAYLOAD_SEGMENT_DATA, initrd_base);

	/* Terminating entry segment. */
	bzp_output_segment(&bzp, NULL, PAYLOAD_SEGMENT_ENTRY, TRAMPOLINE_ENTRY_LOC);

	/* Set size of buffer taking into account potential compression. */
	buffer_set_size(&bzp.output, bzp.offset);
	/* Make passed-in output buffer be valid. */
	buffer_clone(output, &bzp.output);

	/* Serialize the segments with the correct encoding. */
	xdr_segs(output, bzp.segs, bzp.num_segments);
	return 0;
}
