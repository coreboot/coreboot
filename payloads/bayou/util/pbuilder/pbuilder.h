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

#ifndef PBUILDER_H_
#define PBUILDER_H_

#include <stdint.h>
#include <stdio.h>

#define warn(fmt, args...) fprintf(stderr, fmt, ##args)
#define die(fmt, args...) \
  do { fprintf(stderr, fmt, ##args); exit(-1); } \
  while(0)

struct pentry {
	uint8_t index;
	uint8_t parent;
	uint8_t type;
	uint8_t flags;
	uint8_t title[64];
	char *file;
	char *larname;
};

struct config {
	int timeout;
	int n_entries;
	struct pentry **entries;
};

struct bpt_config {
	uint32_t id;
	uint8_t timeout;
	uint8_t entries;
	uint8_t padding[10];
};

struct bpt_pentry {
	uint8_t index;
	uint8_t parent;
	uint8_t type;
	uint8_t flags;
	uint8_t title[64];
	uint8_t nlen;
};

#define BPT_ID            0x30545042
#define BPT_TYPE_CHOOSER  0x01
#define BPT_TYPE_CHAIN    0x02
#define BPT_TYPE_SUBCHAIN 0x03

#define BPT_FLAG_DEFAULT  0x01
#define BPT_FLAG_NOSHOW   0x02

int pbuilder_show_lar(const char *input);
int create_lar_from_config(const char *input, const char *output);
void parseconfig(FILE * stream, struct config *config);

#endif
