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

struct bayoucfg bayoucfg;

static int add_payload(struct LAR *lar, struct larent *larent)
{
	struct payload *payload;
	int plen;
	u8 *params = NULL;
	u8 *fptr;

	if (bayoucfg.n_entries == BAYOU_MAX_ENTRIES)
		return -1;

	payload = &bayoucfg.entries[bayoucfg.n_entries];

	if (strncmp((char *)larent->name, "payload/", 8))
		return -1;

	if (larstat(lar, (const char *)larent->name, &payload->stat))
		return -1;

	/* Make sure the LAR entry is valid. */
	if (!lfverify(lar, (const char *)larent->name))
		return -1;

	/* Get a pointer to the start of the file. */
	fptr = larfptr(lar, (const char *)larent->name);

	if (fptr == NULL)
		return -1;

	if (!verify_self(fptr))
		return -1;

	payload->pentry.index = bayoucfg.n_entries;
	payload->pentry.parent = 0;
	payload->pentry.type = BPT_TYPE_CHOOSER;
	payload->pentry.flags = 0;

	plen = self_get_params(fptr, &params);
	payload_parse_params(payload, params, plen);

	payload->fptr = fptr;

	bayoucfg.n_entries++;

	return 0;
}

static int lar_walk_files(struct LAR *lar,
			  int (*cb) (struct LAR *, struct larent *))
{
	struct larent *larent;
	int ret = 0;

	rewindlar(lar);

	while ((larent = readlar(lar)) != NULL) {
		if ((ret = cb(lar, larent)))
			break;
	}

	return ret;
}

/**
 * If reading the bayou_payload_table fails for some reason, then construct
 * a dummy table. All valid payloads in the lar are added as chooser items.
 */
static void build_dummy_table(struct LAR *lar)
{
	bayoucfg.timeout = 0xFF;
	bayoucfg.n_entries = 0;

	lar_walk_files(lar, add_payload);
}

int get_configuration(struct LAR *lar)
{
	struct larstat stat;
	struct bpt_config *bptcfg;
	u8 *fptr, *ptr;
	int i;

	/*
	 * If bayou_payload_table doesn't exist, then dummy up
	 * a table from the LAR contents.
	 */
	if (larstat(lar, "bayou_payload_table", &stat) ||
	    !lfverify(lar, "bayou_payload_table"))
		build_dummy_table(lar);

	/* Open up the BPT and get the creamy goodness within. */

	fptr = larfptr(lar, "bayou_payload_table");

	if (fptr == NULL)
		build_dummy_table(lar);

	bptcfg = (struct bpt_config *)fptr;
	bayoucfg.timeout = bptcfg->timeout;

	bayoucfg.n_entries = bptcfg->entries;

	if (bayoucfg.n_entries > BAYOU_MAX_ENTRIES) {
		printf("W:  Limiting the number of entries to %d\n",
		       BAYOU_MAX_ENTRIES);
		bayoucfg.n_entries = BAYOU_MAX_ENTRIES;
	}

	ptr = fptr + sizeof(struct bpt_config);

	for (i = 0; i < bayoucfg.n_entries; i++) {
		struct bpt_pentry *entry = (struct bpt_pentry *)ptr;
		struct payload *p = &(bayoucfg.entries[i]);
		int plen;
		u8 *params = NULL;

		memcpy(&p->pentry, entry, sizeof(struct bpt_pentry));

		if (entry->type != BPT_TYPE_CHAIN) {
			char *lname = (char *)ptr + sizeof(struct bpt_pentry);

			if (larstat(lar, (const char *)lname, &p->stat))
				build_dummy_table(lar);

			if (!lfverify(lar, (const char *)lname))
				build_dummy_table(lar);

			fptr = larfptr(lar, (const char *)lname);

			if (verify_self(fptr))
				p->fptr = fptr;
			else
				build_dummy_table(lar);

			plen = self_get_params(fptr, &params);
			payload_parse_params(p, params, plen);
		}

		ptr += sizeof(struct bpt_pentry) + entry->nlen;
	}

	return 0;
}
