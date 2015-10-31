/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#include <libpayload-config.h>
#include <libpayload.h>
#include <coreboot_tables.h>

/* This pointer gets set in head.S and is passed in from coreboot. */
void *cb_header_ptr;

static void cb_parse_dma(void *ptr)
{
	struct lb_range *dma = (struct lb_range *)ptr;
	init_dma_memory(bus_to_virt(dma->range_start), dma->range_size);
}

/* Architecture specific */
int cb_parse_arch_specific(struct cb_record *rec, struct sysinfo_t *info)
{
	switch (rec->tag) {
	case CB_TAG_DMA:
		cb_parse_dma(rec);
		break;
	default:
		return 0;
	}
	return 1;

}

int get_coreboot_info(struct sysinfo_t *info)
{
	return cb_parse_header(cb_header_ptr, 1, info);
}

void *get_cb_header_ptr(void)
{
	return cb_header_ptr;
}
