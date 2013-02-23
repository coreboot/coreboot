/*
 * This file is part of the bayou project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "bayou.h"
#include "self.h"

static int nop_decompress(void *dst, void *src, int len)
{
	memcpy(dst, src, len);
	return len;
}

#ifdef CONFIG_LZMA
extern int ulzma(u8 *, u8 *);

static int lzma_decompress(void *dst, void *src, int len)
{
	return ulzma((u8 *) src, (u8 *) dst);
}
#endif

#ifdef CONFIG_NRV2B
extern int unrv2b(u8 *, u8 *, unsigned long *);

static int nrv2b_decompress(void *dst, void *src, int len)
{
	unsigned long l = (u32) len;
	return unrv2b(src, dst, &l);
}
#endif

static int zeros_decompress(void *dst, void *src, int len)
{
	memset(dst, 0, len);
	return len;
}

int self_get_params(u8 *fptr, u8 **params)
{
	struct self_segment *seg = (struct self_segment *)fptr;

	while (seg->type != SELF_TYPE_ENTRY) {
		if (seg->type == 0)
			return -1;

		if (seg->type == SELF_TYPE_PARAMS) {
			*params = (u8 *) (fptr + seg->offset);
			return seg->len;
		}

		seg++;
	}

	*params = NULL;

	return 0;
}

int verify_self(u8 *ptr)
{
	struct self_segment *seg = (struct self_segment *)ptr;

	switch (seg->type) {
	case SELF_TYPE_CODE:
	case SELF_TYPE_DATA:
	case SELF_TYPE_BSS:
	case SELF_TYPE_PARAMS:
	case SELF_TYPE_ENTRY:
		return 1;
	}

	return 0;
}

int self_load_and_run(struct payload *p, int *ret)
{
	struct self_segment *seg = (struct self_segment *)p->fptr;
	int (*dcmp) (void *, void *, int);
	int dlen;

	switch (p->stat.compression) {
#ifdef CONFIG_LZMA
	case ALGO_LZMA:
		dcmp = lzma_decompress;
		break;
#endif
#ifdef CONFIG_NRV2B
	case ALGO_NRV2B:
		dcmp = nrv2b_decompress;
		break;
#endif
	case ALGO_ZEROES:
		dcmp = zeros_decompress;
		break;
	case ALGO_NONE:
		dcmp = nop_decompress;
	default:
		printf("E: Unsupported decompression type\n");
		return -1;
	}

	while (1) {
		u32 laddr = (u32) (seg->load_addr & 0xFFFFFFFF);

		switch (seg->type) {
		case SELF_TYPE_CODE:
		case SELF_TYPE_DATA:
			dlen = dcmp((void *)laddr,
				    (void *)p->fptr + seg->offset, seg->len);

			if (dlen < seg->mem_len) {
				memset((void *)(laddr + dlen), 0,
				       seg->mem_len - dlen);
			}
			break;

		case SELF_TYPE_BSS:
			memset((void *)laddr, 0, seg->len);
			break;
		case SELF_TYPE_ENTRY:
			*ret = exec(laddr, 0, NULL);
			return 0;
		default:
			break;
		}

		seg++;
	}

	return -1;
}
