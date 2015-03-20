/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 * Copyright (C) 2014 Imagination Technologies
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef PROGRAM_LOADING_H
#define PROGRAM_LOADING_H

#include <stdint.h>
#include <stddef.h>

enum {
	/* Last segment of program. Can be used to take different actions for
	 * cache maintenance of a program load. */
	SEG_FINAL = 1 << 0,
};

/* Called for each segment of a program loaded. The SEG_FINAL flag will be
 * set on the last segment loaded. */
void arch_segment_loaded(uintptr_t start, size_t size, int flags);

struct buffer_area {
	void *data;
	size_t size;
};

enum prog_type {
	PROG_ROMSTAGE,
	PROG_RAMSTAGE,
	PROG_PAYLOAD,
};

/* Representation of a program. */
struct prog {
	enum prog_type type;
	const char *name;
	/* The area can mean different things depending on what type the
	 * program is. e.g. a payload prog uses this field for the backing
	 * store of the payload_segments and data. After loading the segments
	 * area is updated to reflect the bounce buffer used. */
	struct buffer_area area;
	/* Entry to program with optional argument. It's up to the architecture
	 * to decide if argument is passed. */
	void (*entry)(void *);
	void *arg;
};

static inline size_t prog_size(const struct prog *prog)
{
	return prog->area.size;
}

static inline void *prog_start(const struct prog *prog)
{
	return prog->area.data;
}

static inline void *prog_entry(const struct prog *prog)
{
	return prog->entry;
}

static inline void *prog_entry_arg(const struct prog *prog)
{
	return prog->arg;
}

static inline void prog_set_area(struct prog *prog, void *start, size_t size)
{
	prog->area.data = start;
	prog->area.size = size;
}

static inline void prog_set_entry(struct prog *prog, void *e, void *arg)
{
	prog->entry = e;
	prog->arg = arg;
}

/* Run the program described by prog. */
void prog_run(struct prog *prog);
/* Per architecture implementation running a program. */
void arch_prog_run(struct prog *prog);
/* Platform (SoC/chipset) specific overrides for running a program. This is
 * called prior to calling the arch_prog_run. Thus, if there is anything
 * special that needs to be done by the platform similar to the architecture
 * code it needs to that as well. */
void platform_prog_run(struct prog *prog);

struct prog_loader_ops {
	const char *name;
	/* Returns < 0 on error or 0 on success. This function needs to do
	 * different things depending on the prog type. See definition
	 * of struct prog above. */
	int (*prepare)(struct prog *prog);
};

/************************
 *   ROMSTAGE LOADING   *
 ************************/

/* Run romstage from bootblock. */
void run_romstage(void);

/************************
 *   RAMSTAGE LOADING   *
 ************************/

/* Run ramstage from romstage. */
void run_ramstage(void);

struct romstage_handoff;
#if IS_ENABLED(CONFIG_RELOCATABLE_RAMSTAGE)
/* Cache the loaded ramstage described by prog. */
void cache_loaded_ramstage(struct romstage_handoff *, struct prog *p);
/* Load ramstage from cache filling in struct prog. */
void load_cached_ramstage(struct romstage_handoff *h, struct prog *p);
#else
static inline void cache_loaded_ramstage(struct romstage_handoff *h,
						struct prog *p) {}
static inline void load_cached_ramstage(struct romstage_handoff *h,
					struct prog *p) {}
#endif

/***********************
 *   PAYLOAD LOADING   *
 ***********************/

/* Load payload into memory in preparation to run. */
void payload_load(void);

/* Run the loaded payload. */
void payload_run(void);

/* Mirror the payload to be loaded. */
void mirror_payload(struct prog *payload);

/* Defined in src/lib/selfboot.c */
void *selfload(struct prog *payload);

#endif /* PROGRAM_LOADING_H */
