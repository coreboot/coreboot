static void enumerate_ht_chain(void)
{
	/* Assumption the HT chain that is bus 0 has the HT I/O Hub on it.
	 * On most boards this just happens.  If a cpu has multiple
	 * non Coherent links the appropriate bus registers for the
	 * links needs to be programed to point at bus 0.
	 */
	unsigned next_unitid, last_unitid;;
	next_unitid = 1;
	do {
		uint32_t id;
		uint8_t hdr_type, pos;
		last_unitid = next_unitid;

		id = pci_read_config32(PCI_DEV(0,0,0), PCI_VENDOR_ID);
		/* If the chain is enumerated quit */
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			break;
		}
		hdr_type = pci_read_config8(PCI_DEV(0,0,0), PCI_HEADER_TYPE);
		pos = 0;
		hdr_type &= 0x7f;

		if ((hdr_type == PCI_HEADER_TYPE_NORMAL) ||
			(hdr_type == PCI_HEADER_TYPE_BRIDGE)) {
			pos = pci_read_config8(PCI_DEV(0,0,0), PCI_CAPABILITY_LIST);
		}
		while(pos != 0) {
			uint8_t cap;
			cap = pci_read_config8(PCI_DEV(0,0,0), pos + PCI_CAP_LIST_ID);
			if (cap == PCI_CAP_ID_HT) {
				uint16_t flags;
				flags = pci_read_config16(PCI_DEV(0,0,0), pos + PCI_CAP_FLAGS);
				if ((flags >> 13) == 0) {
					unsigned count;
					flags &= ~0x1f;
					flags |= next_unitid & 0x1f;
					count = (flags >> 5) & 0x1f;
					pci_write_config16(PCI_DEV(0, 0, 0), pos + PCI_CAP_FLAGS, flags);
					next_unitid += count;
					break;
				}
			}
			pos = pci_read_config8(PCI_DEV(0, 0, 0), pos + PCI_CAP_LIST_NEXT);
		}
	} while((last_unitid != next_unitid) && (next_unitid <= 0x1f));
}
