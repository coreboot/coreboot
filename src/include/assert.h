/*
 * $Header: /home/cvs/BIR/ca-cpu/freebios/src/include/assert.h,v 1.1 2005/07/11 16:03:54 smagnani Exp $
 *
 * assert.h: Debugging macros
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Log: assert.h,v $
 * Revision 1.1  2005/07/11 16:03:54  smagnani
 * Initial revision.
 *
 *
 */

#ifndef __ASSERT_H_DEFINED
#define __ASSERT_H_DEFINED

// ROMCC doesn't support __FILE__ or __LINE__  :^{

#if DEBUG
#ifdef __ROMCC__
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
#else		// !DEBUG
#define ASSERT(x)	{ }
#endif

#ifdef __ROMCC__
#define BUG()		{ 	die("BUG encountered: system halted\r\n");  }
#else
#define BUG()		{ 				\
						printk_emerg("BUG: file '%s', line %d\n", __FILE__, __LINE__); \
						die(""); \
					}
#endif
					
#endif 	// __ASSERT_H_DEFINED
