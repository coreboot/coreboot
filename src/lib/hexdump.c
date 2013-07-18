/*
 * Copyright 2013 Google Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <console/console.h>
#include <lib.h>

static int isprint(int c)
{
	return (c >= 32 && c <= 126);
}

void hexdump(unsigned long memory, int length)
{
        int i;
        uint8_t *m;
        int all_zero = 0;

        m = (uint8_t *)memory;

        for (i = 0; i < length; i += 16) {
                int j;

                all_zero++;
                for (j = 0; j < 16; j++) {
                        if(m[i+j] != 0) {
                                all_zero = 0;
                                break;
                        }
                }

                if (all_zero < 2) {
                        printk(BIOS_DEBUG, "%08lx:", memory + i);
                        for (j = 0; j < 16; j++)
                                printk(BIOS_DEBUG, " %02x", m[i+j]);
                        printk(BIOS_DEBUG, "  ");
                        for (j = 0; j < 16; j++)
                                printk(BIOS_DEBUG, "%c",
					isprint(m[i+j]) ? m[i+j] : '.');
                        printk(BIOS_DEBUG, "\n");
                } else if (all_zero == 2) {
                        printk(BIOS_DEBUG, "...\n");
                }
        }
}


