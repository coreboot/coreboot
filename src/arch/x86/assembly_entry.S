/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

/* This file assembles the start of the romstage program by the order of the
 * includes. Thus, it's extremely important that one pays very careful
 * attention to the order of the includes. */

#include <arch/x86/prologue.inc>
#include <cpu/x86/32bit/entry32.inc>
#include <cpu/x86/fpu_enable.inc>
#if IS_ENABLED(CONFIG_SSE)
#include <cpu/x86/sse_enable.inc>
#endif

/*
 * The assembly.inc is generated based on the requirements of the mainboard.
 * For example, for ROMCC boards the MAINBOARDDIR/romstage.c would be
 * processed by ROMCC and added. In non-ROMCC boards the chipsets'
 * cache-as-ram setup files would be here.
 */
#include <generated/assembly.inc>
