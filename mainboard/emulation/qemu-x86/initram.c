/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Stefan Reinauer <stepan@coresystems.de>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define _MAINOBJECT

#include <console.h>

/* printktest1() is here to increase the likelihood of main() not ending up at
 * the beginning of the file, so we can check whether the entry point at main()
 * was honored.
 */
int printktest1(void)
{
	printk(BIOS_INFO, "printktest1: If the immediately preceding line does"
		" not say \"Nothing to do.\", then execution did not start at"
		" main()\n");

	return 0;
}

int main(void)
{
	printk(BIOS_INFO, "RAM init code started.\n");
	printk(BIOS_INFO, "Nothing to do.\n");
	printktest1();

	return 0;
}
