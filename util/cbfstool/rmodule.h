/*
 * Copyright (C) 2014 Google, Inc.
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

#ifndef TOOL_RMODULE_H
#define TOOL_RMODULE_H

#include "elf.h"
#include "common.h"

/*
 * Parse an ELF file within the elfin buffer and fill in the elfout buffer
 * with a created rmodule in ELF format. Return 0 on success, < 0 on error.
 */
int rmodule_create(const struct buffer *elfin, struct buffer *elfout);

#endif /* TOOL_RMODULE_H */
