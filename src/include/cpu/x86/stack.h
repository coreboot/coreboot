/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 coresystems GmbH
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

#ifndef __CPU_X86_STACK_H
#define __CPU_X86_STACK_H

/* For now: use CONFIG_RAMBASE + 1MB - 64K (counting downwards) as stack. This
 * makes sure that we stay completely within the 1M-64K of memory that we
 * preserve for suspend/resume. This is basically HIGH_MEMORY_SAFE (see
 * cbmem.h)
 */

#define ROMSTAGE_STACK_OFFSET ( (1024 - 64) * 1024 )
#define ROMSTAGE_STACK	(CONFIG_RAMBASE + ROMSTAGE_STACK_OFFSET)

#endif
