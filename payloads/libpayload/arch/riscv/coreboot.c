/* SPDX-License-Identifier: GPL-2.0-only */

#include <libpayload-config.h>
#include <libpayload.h>
#include <coreboot_tables.h>

/*
 * This pointer is no longer set in head.S by default like in other arch, payloads should have
 * their own ways to init it from any source before using it.
 * e.g. If run after OpenSBI, this could be retrieved from fdt node.
 *      If run directly after Ramstage, could be from arg0.
 */
void *cb_header_ptr;

/* == Architecture specific == */

int cb_parse_arch_specific(struct cb_record *rec, struct sysinfo_t *info)
{
	switch (rec->tag) {
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
