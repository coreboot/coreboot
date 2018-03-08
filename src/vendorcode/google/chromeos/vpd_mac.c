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
 */

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <string.h>

#include <drivers/vpd/vpd.h>

/*
 * Decode string representation of the MAC address (a string of 12 hex
 * symbols) into binary. 'key_name' is the name of the VPD field, it's used if
 * it is necessary to report an input data format problem.
 */
static void decode_mac(struct mac_address *mac,
		       const char *mac_addr_str,
		       const char *key_name)
{
	int i;

	for (i = 0; i < sizeof(mac->mac_addr); i++) {
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
				printk(BIOS_ERR, "%s: non hexadecimal symbol "
				       "%#2.2x in the VPD field %s:%s\n",
				       __func__, (uint8_t)c, key_name,
				       mac_addr_str);
				c = 0;
			}
			n <<= 4;
			n |= c;
		}
		mac->mac_addr[i] = n;
	}
}

void lb_table_add_macs_from_vpd(struct lb_header *header)
{
	/*
	 * Mac addresses in the VPD can be stored in two groups, for ethernet
	 * and WiFi, with keys 'ethernet_macX and wifi_macX.
	 */
	const char *mac_addr_key_bases[] = {"ethernet_mac0", "wifi_mac0"};
	char mac_addr_key[20]; /* large enough for either key */
	char mac_addr_str[13]; /* 12 symbols and the trailing zero. */
	int i, count;
	struct lb_macs *macs = NULL;

	/* Make sure the copy is always zero terminated. */
	mac_addr_key[sizeof(mac_addr_key) - 1] = '\0';

	count = 0;
	for (i = 0; i < ARRAY_SIZE(mac_addr_key_bases); i++) {
		int index_of_index;

		strncpy(mac_addr_key, mac_addr_key_bases[i],
			sizeof(mac_addr_key) - 1);
		index_of_index = strlen(mac_addr_key) - 1;

		do {
			/*
			 * If there are no more MAC addresses of this template
			 * in the VPD - move on.
			 */
			if (!vpd_gets(mac_addr_key, mac_addr_str,
					   sizeof(mac_addr_str), VPD_ANY))
				break;

			if (!macs) {
				macs = (struct lb_macs *)lb_new_record(header);
				macs->tag = LB_TAG_MAC_ADDRS;
			}

			decode_mac(macs->mac_addrs + count,
				   mac_addr_str,
				   mac_addr_key);

			count++;
			mac_addr_key[index_of_index]++;
		} while (count < 10);
	}
	if (!count)
		return; /* No MAC addresses in the VPD. */

	macs->count = count;
	macs->size = sizeof(*macs) + count * sizeof(struct mac_address);
}
