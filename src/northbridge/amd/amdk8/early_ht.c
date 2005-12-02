/*  
	2005.11 yhlu add let the real sb to use small unitid
*/
// only for sb ht chain
static void enumerate_ht_chain(void)
{
#if HT_CHAIN_UNITID_BASE != 0 
/* HT_CHAIN_UNITID_BASE could be 0 (only one ht device in the ht chain), if so, don't need to go through the chain  */ 

	/* Assumption the HT chain that is bus 0 has the HT I/O Hub on it.
	 * On most boards this just happens.  If a cpu has multiple
	 * non Coherent links the appropriate bus registers for the
	 * links needs to be programed to point at bus 0.
	 */
	unsigned next_unitid, last_unitid;
#if HT_CHAIN_END_UNITID_BASE < HT_CHAIN_UNITID_BASE
	//let't record the device of last ht device, So we can set the Unitid to HT_CHAIN_END_UNITID_BASE
	unsigned real_last_unitid;
	uint8_t real_last_pos;
	int ht_dev_num = 0; // except host_bridge
#endif

	next_unitid = HT_CHAIN_UNITID_BASE;
	do {
		uint32_t id;
		uint8_t hdr_type, pos;
		last_unitid = next_unitid;

		id = pci_read_config32(PCI_DEV(0,0,0), PCI_VENDOR_ID);
		/* If the chain is enumerated quit */
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000))
		{
			break;
		}

#if CK804_DEVN_BASE==0 
                //CK804 workaround: 
                // CK804 UnitID changes not use
                if(id == 0x005e10de) {
                        break;
                }
#endif

		hdr_type = pci_read_config8(PCI_DEV(0,0,0), PCI_HEADER_TYPE);
		pos = 0;
		hdr_type &= 0x7f;

		if ((hdr_type == PCI_HEADER_TYPE_NORMAL) ||
			(hdr_type == PCI_HEADER_TYPE_BRIDGE)) 
		{
			pos = pci_read_config8(PCI_DEV(0,0,0), PCI_CAPABILITY_LIST);
		}
		while(pos != 0) {
			uint8_t cap;
			cap = pci_read_config8(PCI_DEV(0,0,0), pos + PCI_CAP_LIST_ID);
			if (cap == PCI_CAP_ID_HT) {
				uint16_t flags;
				/* Read and write and reread flags so the link
				 * direction bit is valid.
				 */
				flags = pci_read_config16(PCI_DEV(0,0,0), pos + PCI_CAP_FLAGS);
				pci_write_config16(PCI_DEV(0,0,0), pos + PCI_CAP_FLAGS, flags);
				flags = pci_read_config16(PCI_DEV(0,0,0), pos + PCI_CAP_FLAGS);
				if ((flags >> 13) == 0) {
					unsigned count;
					unsigned ctrl, ctrl_off;

					flags &= ~0x1f;
					flags |= next_unitid & 0x1f;
					count = (flags >> 5) & 0x1f;
#if HT_CHAIN_END_UNITID_BASE < HT_CHAIN_UNITID_BASE
					real_last_unitid = next_unitid;
					real_last_pos = pos;
					ht_dev_num++ ;
#endif
					next_unitid += count;

					/* Test for end of chain */
					ctrl_off = ((flags >> 10) & 1)?
						PCI_HT_CAP_SLAVE_CTRL1 : PCI_HT_CAP_SLAVE_CTRL0;
					ctrl = pci_read_config16(PCI_DEV(0,0,0), pos + ctrl_off);
					/* Is this the end of the hypertransport chain.
					 * or has the link failed?
					 */
					if (ctrl & ((1 << 6)|(1 << 4))) {
						next_unitid = 0x20;
					}
					
					pci_write_config16(PCI_DEV(0, 0, 0), pos + PCI_CAP_FLAGS, flags);
					break;
				}
			}
			pos = pci_read_config8(PCI_DEV(0, 0, 0), pos + PCI_CAP_LIST_NEXT);
		}
	} while((last_unitid != next_unitid) && (next_unitid <= 0x1f));
#if HT_CHAIN_END_UNITID_BASE < HT_CHAIN_UNITID_BASE
	if(ht_dev_num>0) {
		uint16_t flags;
		flags = pci_read_config16(PCI_DEV(0,real_last_unitid,0), real_last_pos + PCI_CAP_FLAGS); 
                flags &= ~0x1f;
                flags |= HT_CHAIN_END_UNITID_BASE & 0x1f;
		pci_write_config16(PCI_DEV(0, real_last_unitid, 0), real_last_pos + PCI_CAP_FLAGS, flags);
	}
#endif

#endif

}

