/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Facebook Inc.
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

#include <string.h>
#include <cbmem.h>
#include <console/console.h>
#include <security/tpm/tspi.h>

void tcpa_log_init(void)
{
	const struct cbmem_entry *ce;
	struct tcpa_table *tclt;

	if (!cbmem_possibly_online())
		return;

	ce = cbmem_entry_find(CBMEM_ID_TCPA_LOG);
	if (ce)
		return;

	tclt = cbmem_add(CBMEM_ID_TCPA_LOG,
			 sizeof(struct tcpa_table) +
				 MAX_TCPA_LOG_ENTRIES *
					 sizeof(struct tcpa_entry));

	if (!tclt)
		return;

	tclt->max_entries = MAX_TCPA_LOG_ENTRIES;
	tclt->num_entries = 0;

	printk(BIOS_DEBUG, "TCPA log created at %p\n", tclt);
}

void tcpa_log_add_table_entry(const char *name, const uint32_t pcr,
			      const uint8_t *digest, const size_t digest_length)
{
	MAYBE_STATIC struct tcpa_table *tclt = NULL;
	struct tcpa_entry *tce;

	if (!cbmem_possibly_online())
		return;

	tclt = cbmem_find(CBMEM_ID_TCPA_LOG);
	if (!tclt) {
		printk(BIOS_ERR, "ERROR: No TCPA log table found\n");
		return;
	}

	if (tclt->num_entries == tclt->max_entries) {
		printk(BIOS_WARNING, "ERROR: TCPA log table is full\n");
		return;
	}

	tce = &tclt->entries[tclt->num_entries++];

	memcpy(tce->name, name, TCPA_PCR_HASH_NAME);
	tce->pcr = pcr;
	memcpy(tce->digest, digest, digest_length);
	tce->digest_length = digest_length;
}
