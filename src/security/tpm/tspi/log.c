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

static struct tcpa_table *tcpa_log_init(void)
{
	MAYBE_STATIC struct tcpa_table *tclt = NULL;

	if (!cbmem_possibly_online())
		return NULL;

	if (tclt != NULL)
		return tclt;

	tclt = (struct tcpa_table *) cbmem_entry_find(CBMEM_ID_TCPA_LOG);
	if (tclt)
		return tclt;

	tclt = cbmem_add(CBMEM_ID_TCPA_LOG,
			 sizeof(struct tcpa_table) +
			 MAX_TCPA_LOG_ENTRIES *
			 sizeof(struct tcpa_entry));

	if (!tclt) {
		printk(BIOS_ERR, "ERROR: Could not create TCPA log table\n");
		return NULL;
	}

	tclt->max_entries = MAX_TCPA_LOG_ENTRIES;
	tclt->num_entries = 0;

	printk(BIOS_DEBUG, "TCPA log created at %p\n", tclt);

	return tclt;
}

void tcpa_log_add_table_entry(const char *name, const uint32_t pcr,
			      const uint8_t *digest, const size_t digest_length)
{
	struct tcpa_table *tclt;
	struct tcpa_entry *tce;

	tclt = tcpa_log_init();
	if (tclt->num_entries == tclt->max_entries) {
		printk(BIOS_WARNING, "ERROR: TCPA log table is full\n");
		return;
	}

	tce = &tclt->entries[tclt->num_entries++];

	strncpy(tce->name, name, TCPA_PCR_HASH_NAME - 1);
	tce->pcr = pcr;

	if (digest_length > TCPA_DIGEST_MAX_LENGTH) {
		printk(BIOS_WARNING, "ERROR: PCR digest too long for TCPA log entry\n");
		return;
	}
	memcpy(tce->digest, digest, digest_length);
	tce->digest_length = digest_length;
}
