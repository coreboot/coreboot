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
 */

/* StartTimer1
 *
 * Entry: none
 * Exit: Starts Timer 1 for port 61 use
 * Destroys: Al,
 */
static void StartTimer1(void)
{
	outb(0x56, 0x43);
	outb(0x12, 0x41);
}

void SystemPreInit(void)
{
	/* they want a jump ... */
#if !IS_ENABLED(CONFIG_CACHE_AS_RAM)
	__asm__ __volatile__("jmp .+2\ninvd\njmp .+2\n");
#endif
	StartTimer1();
}
