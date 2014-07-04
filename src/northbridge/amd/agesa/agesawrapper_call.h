/*
 * This file is part of the coreboot project.
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

#ifndef _AGESAWRAPPER_CALL_H_
#define _AGESAWRAPPER_CALL_H_

#include <stdint.h>
#include <console/console.h>
#include "AGESA.h"

static inline u32 do_agesawrapper(AGESA_STATUS (*func)(void), const char *name)
{
	AGESA_STATUS ret;
	printk(BIOS_DEBUG, "agesawrapper_%s() entry\n", name);
	ret = func();
	printk(BIOS_DEBUG, "agesawrapper_%s() AGESA_STATUS = %x\n", name, ret);
	return (u32)ret;
}

#define AGESAWRAPPER(func) do_agesawrapper(agesawrapper_ ## func, #func)

#endif
