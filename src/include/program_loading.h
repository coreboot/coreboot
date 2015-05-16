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
 * Foundation, Inc.
 */
#ifndef PROGRAM_LOADING_H
#define PROGRAM_LOADING_H

#include <stdint.h>
#include <stddef.h>
#include <region.h>

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
	PROG_VERSTAGE,
	PROG_ROMSTAGE,
	PROG_RAMSTAGE,
	PROG_REFCODE,
	PROG_PAYLOAD,
	PROG_BL31,
};

/* Representation of a program. */
struct prog {
	enum prog_type type;
	const char *name;
	/* Source of program content to load. */
	struct region_device rdev;
	/* The area can mean different things depending on what type the
	 * program is. A stage after being loaded reflects the memory occupied
	 * by the program, Since payloads are multi-segment one can't express
	 * the memory layout with one range. Instead this field is updated
	 * to reflect the bounce buffer used. */
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

/* Locate the identified program to run. Return 0 on success. < 0 on error. */
int prog_locate(struct prog *prog);
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
	/* Determine if the loader is the active one. If so returns 1 else 0
	 * or < 0 on error. */
	int (*is_loader_active)(struct prog *prog);
	/* Returns < 0 on error or 0 on success. This function locates
	 * the rdev representing the file data associated with the passed in
	 * prog. */
	int (*locate)(struct prog *prog);
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

/* Called when the stage cache couldn't load ramstage on resume. */
void ramstage_cache_invalid(void);

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
