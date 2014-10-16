/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <string.h>

#include <vendorcode/google/chromeos/cros_vpd.h>

void lb_table_add_macs_from_vpd(struct lb_header *header)
{
	/*
	 * In case there is one or more MAC addresses stored in the VPD, the
	 * key is "ethernet_mac{0..9}", up to 10 values.
	 */
	static const char mac_addr_key_base[] = "ethernet_mac0";
	char mac_addr_key[sizeof(mac_addr_key_base)];
	char mac_addr_str[13]; /* 12 symbols and the trailing zero. */
	int count;
	struct lb_macs *macs = NULL;
	const int index_of_index = sizeof(mac_addr_key) - 2;

	/*
	 * MAC addresses are stored in the VPD as strings of hex numbers,
	 * which need to be converted into binary for storing in the coreboot
	 * table.
	 */
	strcpy(mac_addr_key, mac_addr_key_base);
	count = 0;
	do {
		int i;

		if (!cros_vpd_gets(mac_addr_key, mac_addr_str,
				   sizeof(mac_addr_str)))
			break; /* No more MAC addresses in VPD */

		if (!macs) {
			macs = (struct lb_macs *)lb_new_record(header);
			macs->tag = LB_TAG_MAC_ADDRS;
		}

		/* MAC address in symbolic form is in mac_addr_str. */
		for (i = 0; i < sizeof(macs->mac_addrs[0].mac_addr); i++) {
			int j;
			uint8_t n = 0;

			for (j = 0; j < 2; j++) {
				char c = mac_addr_str[i * 2 + j];

				if (isxdigit(c)) {
					if (isdigit(c))
						c -= '0';
					else
						c = tolower(c) - 'a' + 10;
				} else {
					printk(BIOS_ERR,
					       "%s: non hexadecimal symbol "
					       "%#2.2x in the VPD field %s\n",
					       __func__, (uint8_t)c,
					       mac_addr_key);
					c = 0;
				}
				n <<= 4;
				n |= c;
			}
			macs->mac_addrs[count].mac_addr[i] = n;
		}
		count++;
		mac_addr_key[index_of_index] = '0' + count;
	} while (count < 10);

	if (!count)
		return; /* No MAC addresses in the VPD. */

	macs->count = count;
	macs->size = sizeof(*macs) + count * sizeof(struct mac_address);
}
