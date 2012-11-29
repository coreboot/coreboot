/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
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

/* GCC's libgcc handling is quite broken. While the libgcc functions
 * are always regparm(0) the code that calls them uses whatever the
 * compiler call specifies. Therefore we need a wrapper around those
 * functions. See gcc bug PR41055 for more information.
 */

#if CONFIG_ARCH_X86
#define WRAP_LIBGCC_CALL(type, name) \
	type __real_##name(type a, type b) __attribute__((regparm(0))); \
	type __wrap_##name(type a, type b); \
	type __wrap_##name(type a, type b) { return __real_##name(a, b); }
#elif CONFIG_ARCH_ARMV7
#define WRAP_LIBGCC_CALL(type, name) \
	type __real_##name(type a, type b); \
	type __wrap_##name(type a, type b); \
	type __wrap_##name(type a, type b) { return __real_##name(a, b); }
#else
#error Architecture unsupported.
#endif


WRAP_LIBGCC_CALL(long long, __divdi3)
WRAP_LIBGCC_CALL(unsigned long long, __udivdi3)
WRAP_LIBGCC_CALL(long long, __moddi3)
WRAP_LIBGCC_CALL(unsigned long long, __umoddi3)

