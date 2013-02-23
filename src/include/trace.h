/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Rudolf Marek <r.marek@assembler.cz>
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

#ifndef __TRACE_H
#define __TRACE_H


#ifdef __PRE_RAM__

#define DISABLE_TRACE
#define ENABLE_TRACE
#define DISABLE_TRACE_ON_FUNCTION

#else /* !__PRE_RAM__ */

#if CONFIG_TRACE && !defined(__SMM__)

void __cyg_profile_func_enter( void *, void * )
				 __attribute__ ((no_instrument_function));

void __cyg_profile_func_exit( void *, void * )
				__attribute__ ((no_instrument_function));

extern volatile int trace_dis;

#define DISABLE_TRACE  do { trace_dis = 1; } while (0);
#define ENABLE_TRACE    do { trace_dis = 0; } while (0);
#define DISABLE_TRACE_ON_FUNCTION  __attribute__ ((no_instrument_function));

#else /* !CONFIG_TRACE */

#define DISABLE_TRACE
#define ENABLE_TRACE
#define DISABLE_TRACE_ON_FUNCTION

#endif
#endif
#endif
