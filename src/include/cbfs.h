/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
 * Copyright (C) 2013 The Chromium OS Authors. All rights reserved.
 *
 * This file is dual-licensed. You can choose between:
 *   - The GNU GPL, version 2, as published by the Free Software Foundation
 *   - The revised BSD license (without advertising clause)
 *
 * ---------------------------------------------------------------------------
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
 * ---------------------------------------------------------------------------
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ---------------------------------------------------------------------------
 */

#ifndef _CBFS_H_
#define _CBFS_H_

#include <cbfs_core.h>

int cbfs_execute_stage(struct cbfs_media *media, const char *name);
void *cbfs_load_optionrom(struct cbfs_media *media, uint16_t vendor,
			  uint16_t device, void * dest);
void *cbfs_load_payload(struct cbfs_media *media, const char *name);
void *cbfs_load_stage(struct cbfs_media *media, const char *name);

/* Simple buffer for streaming media. */
struct cbfs_simple_buffer {
	char *buffer;
	size_t allocated;
	size_t size;
	size_t last_allocate;
};

void *cbfs_simple_buffer_map(struct cbfs_simple_buffer *buffer,
			     struct cbfs_media *media,
			     size_t offset, size_t count);

void *cbfs_simple_buffer_unmap(struct cbfs_simple_buffer *buffer,
			       const void *address);

// Utility functions
int run_address(void *f);

/* Defined in src/lib/selfboot.c */
struct lb_memory;
void *selfload(struct lb_memory *mem, struct cbfs_payload *payload);
void selfboot(void *entry);

/* Defined in individual arch / board implementation. */
int init_default_cbfs_media(struct cbfs_media *media);

#if CONFIG_RELOCATABLE_RAMSTAGE && defined(__PRE_RAM__)
/* The cache_loaded_ramstage() and load_cached_ramstage() functions are defined
 * to be weak so that board and chipset code may override them. Their job is to
 * cache and load the ramstage for quick S3 resume. By default a copy of the
 * relocated ramstage is saved using the cbmem infrastructure. These
 * functions are only valid during romstage. */

struct romstage_handoff;
struct cbmem_entry;

/* The implementer of cache_loaded_ramstage() may use the romstage_handoff
 * structure to store information, but note that the handoff variable can be
 * NULL. The ramstage cbmem_entry represents the region occupied by the loaded
 * ramstage. */
void __attribute__((weak))
cache_loaded_ramstage(struct romstage_handoff *handoff,
                      const struct cbmem_entry *ramstage, void *entry_point);
/* Return NULL on error or entry point on success. The ramstage cbmem_entry is
 * the region where to load the cached contents to. */
void * __attribute__((weak))
load_cached_ramstage(struct romstage_handoff *handoff,
                     const struct cbmem_entry *ramstage);
#endif /* CONFIG_RELOCATABLE_RAMSTAGE */

#endif

