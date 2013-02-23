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

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <expat.h>
#include <ctype.h>
#include "pbuilder.h"

#define STATE_NONE                   0x00
#define STATE_BAYOUCONFIG            0x01
#define STATE_GLOBAL                 0x02
#define STATE_GLOBAL_TIMEOUT         0x03
#define STATE_PAYLOADS               0x04
#define STATE_PAYLOAD                0x05
#define STATE_PAYLOAD_TITLE          0x06
#define STATE_PAYLOAD_FILE           0x07
#define STATE_PAYLOAD_LARNAME        0x08
#define STATE_PAYLOAD_CHAIN          0x09
#define STATE_PAYLOAD_CHAIN_FILE     0x0A
#define STATE_PAYLOAD_CHAIN_LARNAME  0x0B

static struct userdata {
	struct config *config;
	struct pentry *payload;
	struct pentry *chain;
	int state;
} userdata;

static char buffer[8192];

static struct {
	char *element;
	int pstate;
	int state;
} states[] = {
	{"BayouConfig", STATE_NONE, STATE_BAYOUCONFIG},
	{"global", STATE_BAYOUCONFIG, STATE_GLOBAL},
	{"timeout", STATE_GLOBAL, STATE_GLOBAL_TIMEOUT},
	{"payloads", STATE_BAYOUCONFIG, STATE_PAYLOADS},
	{"payload", STATE_PAYLOADS, STATE_PAYLOAD},
	{"title", STATE_PAYLOAD, STATE_PAYLOAD_TITLE},
	{"lar", STATE_PAYLOAD, STATE_PAYLOAD_LARNAME},
	{"file", STATE_PAYLOAD, STATE_PAYLOAD_FILE},
	{"chain", STATE_PAYLOAD, STATE_PAYLOAD_CHAIN},
	{"file", STATE_PAYLOAD_CHAIN, STATE_PAYLOAD_CHAIN_FILE},
	{"lar", STATE_PAYLOAD_CHAIN, STATE_PAYLOAD_CHAIN_LARNAME},
	{NULL},
};

#ifndef __LINUX__
static char *strndup (const char *s, size_t n)
{
	size_t len = strlen (s);
	len = (len<n)?len:n;
	char *cpy = malloc (len + 1);
	if (cpy == NULL)
		return NULL;
	cpy[len] = '\0';
	memcpy (cpy, s, len);
	return cpy;
}
#endif

static struct pentry *newPayload(struct config *config)
{
	struct pentry **tmp, *ret;

	tmp = realloc(config->entries,
		      (config->n_entries + 1) * sizeof(struct pentry *));

	if (tmp == NULL)
		return NULL;

	config->entries = tmp;

	ret = config->entries[config->n_entries] = calloc(sizeof(*ret), 1);

	if (ret == NULL)
		return NULL;

	/* Yes, I want this to be ++config->n_entries (the index is 1 based). */
	ret->index = ++config->n_entries;

	return ret;
}

static void parseFlags(struct pentry *entry, const char *flags)
{
	char *p = (char *)flags;
	char *n;

	while (*p) {
		n = strchr(p, ',');

		if (n)
			*(n++) = 0;

		if (!strcmp(p, "default"))
			entry->flags |= BPT_FLAG_DEFAULT;
		else if (!strcmp(p, "nolist"))
			entry->flags |= BPT_FLAG_NOSHOW;
		else
			warn("W: Unknown payload flag %s\n", p);

		if (!n)
			break;

		for (p = n; *p && isspace(*p); p++) ;
	}
}

static struct pentry *addPayload(struct config *config, const char **attr)
{
	struct pentry *ret = newPayload(config);
	int i = 0;

	if (ret == NULL)
		die("E: Could not allocate memory for a new payload\n");

	while (attr[i] != NULL) {
		if (!strcmp(attr[i], "type")) {
			if (!strcmp(attr[i + 1], "chain"))
				ret->type = BPT_TYPE_CHAIN;
			else if (!strcmp(attr[i + 1], "chooser"))
				ret->type = BPT_TYPE_CHOOSER;
			else
				die("E: Invalid payload type %s\n",
				    attr[i + 1]);
		} else if (!strcmp(attr[i], "flags"))
			parseFlags(ret, attr[i + 1]);

		i += 2;
	}

	return ret;
}

static struct pentry *addChain(struct config *config, struct pentry *parent)
{
	struct pentry *ret = newPayload(config);

	if (ret == NULL)
		die("E: Could not allocate memory for a new payload\n");

	ret->parent = parent->index;
	ret->type = BPT_TYPE_SUBCHAIN;

	return ret;
}

static void start(void *data, const char *el, const char **attr)
{
	int i;
	struct userdata *d = (struct userdata *)data;

	for (i = 0; states[i].element != NULL; i++) {
		if (d->state != states[i].pstate)
			continue;

		if (!strcmp(el, states[i].element)) {
			d->state = states[i].state;
			break;
		}
	}

	if (states[i].element == NULL)
		die("E: Unknown element %s\n", el);

	if (d->state == STATE_PAYLOAD)
		d->payload = addPayload(d->config, attr);

	if (d->state == STATE_PAYLOAD_CHAIN)
		d->chain = addChain(d->config, d->payload);
}

static void data(void *data, const char *val, int len)
{
	struct userdata *d = (struct userdata *)data;
	int l;

	switch (d->state) {
	case STATE_GLOBAL_TIMEOUT:
		d->config->timeout = atoi(val);
		break;
	case STATE_PAYLOAD_TITLE:
		l = sizeof(d->payload->title) - 1 > len ?
		    len : sizeof(d->payload->title) - 1;

		strncpy((char *)d->payload->title, (char *)val, l);
		d->payload->title[l] = '\0';
		break;
	case STATE_PAYLOAD_FILE:
		d->payload->file = strndup(val, len);
		break;
	case STATE_PAYLOAD_LARNAME:
		d->payload->larname = strndup(val, len);
		break;
	case STATE_PAYLOAD_CHAIN_FILE:
		d->chain->file = strndup(val, len);
		break;
	case STATE_PAYLOAD_CHAIN_LARNAME:
		d->chain->larname = strndup(val, len);
		break;
	default:
		break;
	}
}

static void end(void *data, const char *el)
{
	struct userdata *d = (struct userdata *)data;
	int i;

	if (d->state == STATE_PAYLOAD_CHAIN)
		d->chain = NULL;

	if (d->state == STATE_PAYLOAD)
		d->payload = NULL;

	for (i = 0; states[i].element != NULL; i++) {
		if (d->state != states[i].state)
			continue;

		if (!strcmp(el, states[i].element)) {
			d->state = states[i].pstate;
			break;
		}
	}

	if (states[i].element == NULL)
		die("E:  Unable to find the reverse state for %s\n", el);
}

void parseconfig(FILE *stream, struct config *config)
{
	XML_Parser p = XML_ParserCreate(NULL);
	int eof = 0;

	if (p == NULL)
		die("E: Could not create the parser\n");

	XML_SetElementHandler(p, start, end);
	XML_SetCharacterDataHandler(p, data);

	userdata.config = config;

	XML_SetUserData(p, &userdata);

	while (!eof) {
		int len = fread(buffer, 1, 8192, stream);

		eof = feof(stream);

		if (ferror(stream))
			die("E: Error reading the stream\n");

		if (!XML_Parse(p, buffer, len, eof))
			die("E: Error parsing the XML data\n");
	}
}
