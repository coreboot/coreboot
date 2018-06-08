/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <compiler.h>
#include <console/console.h>
#include <halt.h>

#ifndef __ROMCC__
#define NORETURN __attribute__((noreturn))

/*
 * The method should be overwritten in mainboard directory to signal that a
 * fatal error had occurred. On boards that do share the same EC and where the
 * EC is capable of controlling LEDs or a buzzer the method can be overwritten
 * in EC directory instead.
 */
__weak void die_notify(void)
{
}

/* Report a fatal error */
void NORETURN die(const char *msg)
{
	printk(BIOS_EMERG, "%s", msg);
	die_notify();
	halt();
}
#endif
