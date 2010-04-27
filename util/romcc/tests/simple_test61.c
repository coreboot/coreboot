static void spd_set_nbxcfg(void)
{
	/*
	 * Effects:	Uses serial presence detect to set the
	 *              ECC support flags in the NBXCFG register
	 * FIXME:	Check for illegal/unsupported ram configurations and abort
	 */
	unsigned device;

	for(device = 0x50; device <= 0x53; device += 1) {
		int byte;

		byte = 0; /* Disable ECC */
		/* 0 == None, 1 == Parity, 2 == ECC */
		if (byte != 2) continue;

		/* set the device I'm talking too */
		__builtin_outb(device, 0x1004);

		/* poll for transaction completion */
		byte = __builtin_inb(0x10);
		while(byte == 0) {
			byte = __builtin_inb(0x10);
		}
	}
}
