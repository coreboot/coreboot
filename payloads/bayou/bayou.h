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

#ifndef BAYOU_H_
#define BAYOU_H_

#include <libpayload.h>

#define BAYOU_MAX_ENTRIES 10

struct bpt_config {
	u32 id;
	u8 timeout;
	u8 entries;
	u8 padding[10];
};

struct bpt_pentry {
	u8 index;
	u8 parent;
	u8 type;
	u8 flags;
	u8 title[64];
	u8 nlen;
};

#define BPT_ID            0x30545042
#define BPT_TYPE_CHOOSER  0x01
#define BPT_TYPE_CHAIN    0x02
#define BPT_TYPE_SUBCHAIN 0x03

#define BPT_FLAG_DEFAULT  0x01
#define BPT_FLAG_NOSHOW   0x02

enum bayou_params {
	BAYOU_PARAM_NAME = 0,
	BAYOU_PARAM_LIST,
	BAYOU_PARAM_DESC,
	BAYOU_PARAMS_COUNT
};

struct payload {
	struct bpt_pentry pentry;
	struct larstat stat;
	u8 *fptr;
	char *params[BAYOU_PARAMS_COUNT];
};

struct bayoucfg {
	u8 timeout;
	int n_entries;
	struct payload entries[BAYOU_MAX_ENTRIES];
};

extern struct bayoucfg bayoucfg;

int verify_self(u8 *ptr);
int self_get_params(u8 *fptr, u8 **params);
int self_load_and_run(struct payload *p, int *ret);

void menu(void);

void run_payload(struct payload *p);
char *payload_get_name(struct payload *p);
struct payload *payload_get_default(void);
void run_payload_timeout(struct payload *p, int timeout);
void payload_parse_params(struct payload *pload, u8 *params, int len);

int get_configuration(struct LAR *lar);

#endif
