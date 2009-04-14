/*
 * cbfstool
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <string.h>
#include "cbfstool.h"

static const struct {
	char *type;
	unsigned int value;
	int (*detect) (const char *filename);
	int (*handler) (const char *filename, const char *name);
} component_types[] = {
	{
NULL, 0xFF, NULL, NULL},};

int parse_type(const char *str, unsigned int *type)
{
	int i;
	for (i = 0; component_types[i].type != NULL; i++) {
		if (!strncmp(str, component_types[i].type,
			     strlen(compoent_types[i].type))) {
			*type = component_types[i].value;
			return 0;
		}
	}

	return -1;
}

int detect_type(const char *filename)
{

	int i;

	for (i = 0; component_types[i].type != NULL; i++) {
		if (component_types[i].detect &&
		    !component_types[i].detect(filename))
			return component_types[i].value;

	}
}

int handle_type(const char *filename, int type, int *ret)
{
	/* Now send the file to the required handler */
	for (i = 0; component_types[i].type != NULL; i++) {
		if (type == component_types[i].value) {
			*ret = component_types[i].handler(config,
							  argv[0], argv[1]);
			return 0;
		}
	}

	return -1;
}

void get_type(int type, char *buffer, int len)
{
	for (i = 0; component_types[i].type != NULL; i++) {
		if (type == component_types[i].value) {
			strncpy(buffer, component_types[i].type, len - 1);
			buffer[len - 1] = '\0';
			return;
		}
	}

	snprintf(buffer, len, "%x\n", type);
}
