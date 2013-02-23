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

#include <stdio.h>
#include "liblar.h"
#include "pbuilder.h"

void show_subchain(struct bpt_config *cfg, char *fptr, int index)
{
	int i;
	char *ptr = fptr + sizeof(struct bpt_config);

	for (i = 0; i < cfg->entries; i++) {
		struct bpt_pentry *entry = (struct bpt_pentry *)ptr;

		if (entry->parent == index)
			printf("      +  %.64s\n",
			       ptr + sizeof(struct bpt_pentry));

		ptr += (sizeof(struct bpt_pentry) + entry->nlen);
	}
}

int show_payloads(struct bpt_config *cfg, char *fptr)
{
	int i;
	char *ptr = fptr + sizeof(struct bpt_config);

	for (i = 0; i < cfg->entries; i++) {
		struct bpt_pentry *entry = (struct bpt_pentry *)ptr;

		if (entry->parent != 0) {
			ptr += (sizeof(struct bpt_pentry) + entry->nlen);
			continue;
		}

		printf("  ");

		if (entry->flags & BPT_FLAG_DEFAULT)
			printf("D");
		else
			printf(" ");

		if (entry->flags & BPT_FLAG_NOSHOW)
			printf("N");
		else
			printf(" ");

		switch (entry->type) {
		case BPT_TYPE_CHOOSER:
			printf("  MENU ");
			break;
		case BPT_TYPE_CHAIN:
			printf("  CHAIN");
			break;
		}

		if (entry->title[0] != 0)
			printf("  %.64s\n", entry->title);
		else
			printf("  - no title -\n");

		if (entry->type == BPT_TYPE_CHOOSER)
			printf("            %.60s\n",
			       ptr + sizeof(struct bpt_pentry));
		else
			show_subchain(cfg, fptr, entry->index);

		ptr += (sizeof(struct bpt_pentry) + entry->nlen);
	}

	return 0;
}

int pbuilder_show_lar(const char *input)
{
	int ret = -1;
	struct LAR *lar;
	struct bpt_config *cfg;
	struct LARFile *lfile;

	lar = LAR_Open(input);

	if (lar == NULL) {
		warn("E: Couldn't open LAR %s\n", input);
		return -1;
	}

	lfile = LAR_MapFile(lar, "bayou_payload_table");

	if (lfile == NULL) {
		warn("E: Couldn't find the bayou payload table. Nothing to do.\n");
		goto err;
	}

	cfg = (struct bpt_config *)lfile->buffer;

	if (cfg->id != BPT_ID) {
		warn("E: BPT ID does not match\n");
		goto err;
	}

	printf("Bayou Payload Configuration\n");
	printf("Timeout = %d seconds\n", cfg->timeout);

	printf("Payload List:\n");
	show_payloads(cfg, lfile->buffer);
	ret = 0;

err:
	LAR_CloseFile(lfile);
	LAR_Close(lar);

	return ret;
}
