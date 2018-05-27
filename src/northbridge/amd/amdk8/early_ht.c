/*
 * This file is part of the coreboot project.
 *
 * 2005.11 yhlu add let the real sb to use small unitid
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

// only for sb ht chain
static void enumerate_ht_chain(void)
{
#if CONFIG_HT_CHAIN_UNITID_BASE != 0
/* CONFIG_HT_CHAIN_UNITID_BASE could be 0 (only one ht device in the ht chain), if so, don't need to go through the chain  */

	/* Assumption the HT chain that is bus 0 has the HT I/O Hub on it.
	 * On most boards this just happens.  If a CPU has multiple
	 * non Coherent links the appropriate bus registers for the
	 * links needs to be programed to point at bus 0.
	 */
	unsigned next_unitid, last_unitid;
	pci_devfn_t dev;
#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
	//let't record the device of last ht device, So we can set the Unitid to CONFIG_HT_CHAIN_END_UNITID_BASE
	unsigned real_last_unitid = 0;
	uint8_t real_last_pos = 0;
	int ht_dev_num = 0; // except host_bridge
	uint8_t end_used = 0;
#endif

	dev = PCI_DEV(0,0,0);
	next_unitid = CONFIG_HT_CHAIN_UNITID_BASE;
	do {
		uint32_t id;
		uint8_t hdr_type, pos;
		last_unitid = next_unitid;

		id = pci_read_config32(dev, PCI_VENDOR_ID);
		/* If the chain is enumerated quit */
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000))
		{
			break;
		}

		hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
		pos = 0;
		hdr_type &= 0x7f;

		if ((hdr_type == PCI_HEADER_TYPE_NORMAL) ||
			(hdr_type == PCI_HEADER_TYPE_BRIDGE))
		{
			pos = pci_read_config8(dev, PCI_CAPABILITY_LIST);
		}
		while (pos != 0) {
			uint8_t cap;
			cap = pci_read_config8(dev, pos + PCI_CAP_LIST_ID);
			if (cap == PCI_CAP_ID_HT) {
				uint16_t flags;
				/* Read and write and reread flags so the link
				 * direction bit is valid.
				 */
				flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
				pci_write_config16(dev, pos + PCI_CAP_FLAGS, flags);
				flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
				if ((flags >> 13) == 0) {
					unsigned count;
					unsigned ctrl, ctrl_off;
					pci_devfn_t devx;

#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
					if (next_unitid >= 0x18) { // don't get mask out by k8, at this time BSP, RT is not enabled, it will response from 0x18,0--0x1f.
						if (!end_used) {
							next_unitid = CONFIG_HT_CHAIN_END_UNITID_BASE;
							end_used = 1;
						} else {
							goto out;
						}
					}
					real_last_unitid = next_unitid;
					real_last_pos = pos;
					ht_dev_num++;
#endif

					flags &= ~0x1f;
					flags |= next_unitid & 0x1f;
					count = (flags >> 5) & 0x1f;

					devx = PCI_DEV(0, next_unitid, 0);
					pci_write_config16(dev, pos + PCI_CAP_FLAGS, flags);

					next_unitid += count;

					flags = pci_read_config16(devx, pos + PCI_CAP_FLAGS);
					/* Test for end of chain */
					ctrl_off = ((flags >> 10) & 1)?
						PCI_HT_CAP_SLAVE_CTRL0 : PCI_HT_CAP_SLAVE_CTRL1; // another end

					do {
						ctrl = pci_read_config16(devx, pos + ctrl_off);
						/* Is this the end of the hypertransport chain? */
						if (ctrl & (1 << 6)) {
							goto out;
						}

						if (ctrl & ((1 << 4) | (1 << 8))) {
						       /*
							* Either the link has failed, or we have
							* a CRC error.
							* Sometimes this can happen due to link
							* retrain, so lets knock it down and see
							* if its transient
							*/
							ctrl |= ((1 << 4) | (1 <<8)); // Link fail + Crc
							pci_write_config16(devx, pos + ctrl_off, ctrl);
							ctrl = pci_read_config16(devx, pos + ctrl_off);
							if (ctrl & ((1 << 4) | (1 << 8))) {
								// can not clear the error
								break;
							}
						}
					} while ((ctrl & (1 << 5)) == 0);

					break;
				}
			}
			pos = pci_read_config8(dev, pos + PCI_CAP_LIST_NEXT);
		}
	} while (last_unitid != next_unitid);

out:
	;

#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
	if ((ht_dev_num > 1) && (real_last_unitid != CONFIG_HT_CHAIN_END_UNITID_BASE) && !end_used) {
		uint16_t flags;
		dev = PCI_DEV(0,real_last_unitid, 0);
		flags = pci_read_config16(dev, real_last_pos + PCI_CAP_FLAGS);
	        flags &= ~0x1f;
	        flags |= CONFIG_HT_CHAIN_END_UNITID_BASE & 0x1f;
		pci_write_config16(dev, real_last_pos + PCI_CAP_FLAGS, flags);
	}
#endif

#endif

}
