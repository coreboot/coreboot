/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <arch/hlt.h>
#include <console/console.h>

/* GCC and CAR versions */
#define ASSERT(x) {						\
	if (!(x)) {						\
		printk(BIOS_EMERG, "ASSERTION ERROR: file '%s'"	\
			", line %d\n", __FILE__, __LINE__);	\
		if (IS_ENABLED(CONFIG_FATAL_ASSERTS)) hlt();	\
	}							\
}
#define BUG() {							\
	printk(BIOS_EMERG, "ERROR: BUG ENCOUNTERED at file '%s'"\
		", line %d\n", __FILE__, __LINE__);		\
	if (IS_ENABLED(CONFIG_FATAL_ASSERTS)) hlt();		\
}

#define assert(statement)	ASSERT(statement)

#endif // __ASSERT_H__
