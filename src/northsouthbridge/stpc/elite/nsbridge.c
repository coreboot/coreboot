/*
 * STPC basic initialization
 * by
 * Steve M. Gehlbach
 *
 * Most stpc work has to be done earlier
 * so not much is in this file
 *
 * Modified by Peter Fox for the STPC Elite:
 * Header changed to elite version
 * Removed graphics stuff (No memory used for graphics)
 * Added GPL header
 *
 * This software comes with ABSOLUTELY NO WARRANTY; This is free software, and
 * you are welcome to redistribute it under certain conditions; you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2 of the
 * License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA. "
 *
 */
#include <mem.h>
#include <cpu/p5/io.h>
#include <printk.h>

int display_cpuid ( void ) {
	return 0;
}

int mtrr_check ( void ) {
	return 0;
}

struct mem_range *sizeram(void)
{
	static struct mem_range mem[3];
	int mem_size;

// get ram size from settings in stpc SDRAM controller registers
// the last bank register = top memory in MB less one
	mem_size = ( stpc_conf_readb(0x33) + 1 ) * 1024;
	printk_info("stpc memory size= %d MB\n",mem_size/1024);

	if (mem_size < 0 || mem_size > 128*1024) mem_size = 64*1024;

	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 1024;
	mem[1].sizek = mem_size;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	if (mem[1].sizek == 0) {
		mem[1].sizek = 64*1024;
	}
	mem[1].sizek -= mem[1].basek;
	return &mem[0];
}

int nvram_on ( void ) {
	return 0;
}

