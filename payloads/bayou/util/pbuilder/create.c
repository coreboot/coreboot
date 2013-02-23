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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include "liblar.h"
#include "pbuilder.h"

void do_lzma_compress(char *in, int in_len, char *out, int *out_len);

int add_bpt_to_lar(struct LAR *lar, struct config *config)
{
	char *buffer;
	int ret, i, len = sizeof(struct bpt_config);
	struct bpt_config *cfg;
	struct LARAttr attr;
	char *ptr;

	for (i = 0; i < config->n_entries; i++) {
		len += sizeof(struct bpt_pentry);

		if (config->entries[i]->type != BPT_TYPE_CHAIN)
			len += ((strlen(config->entries[i]->larname)
				+ 15) & ~0x0F);
	}

	buffer = calloc(len, 1);

	if (buffer == NULL)
		return -1;

	cfg = (struct bpt_config *)buffer;

	cfg->id = BPT_ID;
	cfg->timeout = config->timeout;
	cfg->entries = config->n_entries;

	ptr = buffer + sizeof(struct bpt_config);

	for (i = 0; i < config->n_entries; i++) {
		int nlen = 0;
		struct bpt_pentry *pentry = (struct bpt_pentry *)ptr;

		pentry->index = config->entries[i]->index;
		pentry->parent = config->entries[i]->parent;
		pentry->type = config->entries[i]->type;
		pentry->flags = config->entries[i]->flags;

		strncpy((char *)pentry->title,
			(char *)config->entries[i]->title,
			sizeof(pentry->title));

		if (config->entries[i]->type != BPT_TYPE_CHAIN) {
			nlen = strlen(config->entries[i]->larname);
			nlen = (nlen + 15) & ~0x0F;

			strcpy((char *)(ptr + sizeof(struct bpt_pentry)),
			       config->entries[i]->larname);

			pentry->nlen = nlen;
		}

		ptr += sizeof(struct bpt_pentry);

		if (config->entries[i]->type != BPT_TYPE_CHAIN)
			ptr += nlen;
	}

	LAR_SetAttrs(&attr, "bayou_payload_table", ALGO_NONE);

	ret = LAR_AppendBuffer(lar, (unsigned char *)buffer, len, &attr);
	free(buffer);
	return ret;
}

struct lfile {
	char *file;
	char *larname;
};

int n_lfiles;

int create_lar_from_config(const char *input, const char *output)
{
	struct config config;
	FILE *stream;
	struct LAR *lar;
	struct LARAttr attr;
	int i, j, ret = -1;
	struct lfile *lfiles;

	stream = fopen(input, "r");

	if (stream == NULL) {
		warn("E:  Couldn't open %s for reading\n", input);
		return -1;
	}

	memset(&config, 0, sizeof(config));

	parseconfig(stream, &config);
	fclose(stream);

	lar = LAR_Create(output);

	if (lar == NULL) {
		warn("E:  Couldn't create a new lar file\n");
		return -1;
	}

	LAR_SetCompressionFuncs(lar, ALGO_LZMA, do_lzma_compress, NULL);

	lfiles = calloc(sizeof(struct lfile), config.n_entries);

	if (lfiles == NULL) {
		warn("E: Couldn't allocate memory: %m\n");
		return -1;
	}

	for (i = 0; i < config.n_entries; i++) {
		/* Master chain entries don't have files associated with them. */
		if (config.entries[i]->type == BPT_TYPE_CHAIN)
			continue;

		if (access(config.entries[i]->file, R_OK)) {
			warn("E:  Could not find file %s\n",
			     config.entries[i]->file);

			goto err;
		}

		if (config.entries[i]->larname == NULL) {
			config.entries[i]->larname =
			    strdup(basename(config.entries[i]->file));

			if (config.entries[i]->larname == NULL) {
				warn("E: Could not allocate memory for the default name\n");
				goto err;
			}
		}

		/*
		 * Add the file to the list of files to add to the LAR - skip
		 * any duplicates, but be on the lookout for the same LAR name
		 * attached to a different file.
		 */
		for (j = 0; j < n_lfiles; j++) {
			if (!strcmp(lfiles[j].larname,
				    config.entries[i]->larname)) {
				if (strcmp(lfiles[j].file,
					   config.entries[i]->file)) {
					warn("E:  LAR name '%s' has already been used\n", config.entries[i]->larname);
					goto err;
				}
				break;
			}
		}

		if (j == n_lfiles) {
			lfiles[n_lfiles].file = config.entries[i]->file;
			lfiles[n_lfiles++].larname = config.entries[i]->larname;
		}
	}

	/* Add all the files to the LAR. */
	for (i = 0; i < n_lfiles; i++) {
		LAR_SetAttrs(&attr, lfiles[i].larname, ALGO_LZMA);

		if (LAR_AppendFile(lar, lfiles[i].file, &attr)) {
			warn("E: Could not add %s to the LAR\n",
			     lfiles[i].file);
			goto err;
		}
	}

	ret = add_bpt_to_lar(lar, &config);

err:
	LAR_Close(lar);
	return ret;
}
