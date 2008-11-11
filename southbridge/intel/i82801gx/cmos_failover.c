/*
 * This file is part of the coreboot project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "i82801gx.h"

#define RTC_FAILED    (1 <<2)
#define GEN_PMCON_3     0xa4

static void check_cmos_failed(void)
{
	u8 byte;
	byte = pci_read_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_3);
	if (byte & RTC_FAILED) {
		// clear bit 1 and bit 2
		byte = cmos_read(RTC_BOOT_BYTE);
		byte &= 0x0c;
		byte |= MAX_REBOOT_CNT << 4;
		cmos_write(byte, RTC_BOOT_BYTE);
	}
}
