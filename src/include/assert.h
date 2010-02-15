/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Digital Design Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * assert.h: Debugging macros
 */

#ifndef __ASSERT_H_DEFINED
#define __ASSERT_H_DEFINED

// ROMCC doesn't support __FILE__ or __LINE__  :^{

#if CONFIG_DEBUG
#ifdef __PRE_RAM__
#define ASSERT(x)	{ if (!(x)) die("ASSERT failure!\r\n"); }
#else
#define ASSERT(x)	{ 				\
						if (!(x)) 	\
						{			\
							printk_emerg("ASSERT failure: file '%s', line %d\n", __FILE__, __LINE__); \
							die(""); \
						}			\
					}
#endif		// __ROMCC__
#else		// !CONFIG_DEBUG
#define ASSERT(x)	{ }
#endif

#ifdef __PRE_RAM__
#define BUG()		{ 	die("BUG encountered: system halted\r\n");  }
#else
#define BUG()		{ 				\
						printk_emerg("BUG: file '%s', line %d\n", __FILE__, __LINE__); \
						die(""); \
					}
#endif
					
#endif 	// __ASSERT_H_DEFINED
