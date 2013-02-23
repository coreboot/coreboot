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

#define TIMEOUT_MESSAGE "Press ESC for the menu (%2d)...\r"
#define TIMEOUT_KEY     '\033'

void run_payload(struct payload *p)
{
	int ret, i;

	/* For chooser entries, just run the payload. */
	if (p->pentry.type == BPT_TYPE_CHOOSER) {
		self_load_and_run(p, &ret);
		return;
	}

	/* For chained entries, run all the sub-chain items. */
	for (i = 0; i < bayoucfg.n_entries; i++) {
		struct payload *s = &(bayoucfg.entries[i]);

		if (s->pentry.parent == p->pentry.index)
			self_load_and_run(s, &ret);
	}
}

char *payload_get_name(struct payload *p)
{
	if (p->pentry.type == BPT_TYPE_CHAIN)
		return (char *)p->pentry.title;
	else if (p->pentry.type == BPT_TYPE_CHOOSER) {
		if (p->pentry.title[0] != 0)
			return (char *)p->pentry.title;
		return p->params[BAYOU_PARAM_DESC];
	}

	return NULL;
}

struct payload *payload_get_default(void)
{
	int i;

	for (i = 0; i < bayoucfg.n_entries; i++) {
		struct payload *s = &(bayoucfg.entries[i]);

		if (s->pentry.parent == 0 && s->pentry.flags & BPT_FLAG_DEFAULT)
			return s;
	}

	return NULL;
}

void run_payload_timeout(struct payload *p, int timeout)
{
	int t, ch, tval;

	for (t = timeout; t >= 0; t--) {
		printf(TIMEOUT_MESSAGE, t);

		tval = 1000;
		ch = getchar_timeout(&tval);

		if (ch == TIMEOUT_KEY)
			return;
	}

	run_payload(p);
}

void payload_parse_params(struct payload *pload, u8 *params, int len)
{
	char *ptr = (char *)params;
	int i = 0;

	if (ptr == NULL)
		return;

	while (ptr < ((char *)params + len)) {

		if (!strncmp(ptr, "name=", 5)) {
			pload->params[BAYOU_PARAM_NAME] = ptr + 5;
		} else if (!strncmp(ptr, "desc=", 5)) {
			pload->params[BAYOU_PARAM_DESC] = ptr + 5;
		} else if (!strncmp(ptr, "listname=", 9)) {
			pload->params[BAYOU_PARAM_LIST] = ptr + 9;
		}

		ptr += strnlen(ptr, len - i);

		if (ptr < ((char *)params + len) && *ptr == 0)
			ptr++;
	}
}
