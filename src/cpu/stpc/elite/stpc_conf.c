/*
 * Functions to Read/write the special configuration registers on the STPC
 * by Steve M. Gehlbach
 *
 * Modified by Peter Fox:
 * Header changed to elite version
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

#include <cpu/stpc/elite/stpc.h>
#include <types.h>
#include <cpu/p5/io.h>

u8 stpc_conf_readb( u8 port) {
	outb(port, STPC_CONF_REG_INDEX);
	return inb(STPC_CONF_REG_DATA);
}

void stpc_conf_writeb( u8 data, u8 port) {
	outb(port, STPC_CONF_REG_INDEX);
	outb(data, STPC_CONF_REG_DATA);
}
