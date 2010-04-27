
static void spd_enable_refresh(void)
{
	/*
	 * Effects:	Uses serial presence detect to set the
	 *              refresh rate in the DRAMC register.
	 *		see spd_set_dramc for the other values.
	 * FIXME:	Check for illegal/unsupported ram configurations and abort
	 */
	static const unsigned char refresh_rates[] = {
		0x01, /* Normal        15.625 us -> 15.6 us */
		0x05, /* Reduced(.25X) 3.9 us    -> 7.8 us */
		0x05, /* Reduced(.5X)  7.8 us    -> 7.8 us */
		0x02, /* Extended(2x)  31.3 us   -> 31.2 us */
		0x03, /* Extended(4x)  62.5 us   -> 62.4 us */
		0x04, /* Extended(8x)  125 us    -> 124.8 us */
	};
	/* Find the first dimm and assume the rest are the same */
	int byte;
	unsigned device;
	unsigned refresh_rate;
	byte = -1;
	device = 0x50;
	while ((byte < 0) && (device <= 0x57)) {
		byte = __builtin_inl(device);
		device += 1;
	}
	if (byte < 0) {
		/* We couldn't find anything we must have no memory */
		while(1);
	}
	byte &= 0x7f;
	/* Default refresh rate be conservative */
	refresh_rate = 5;
	/* see if the ram refresh is a supported one */
	if (byte < 6) {
		refresh_rate = refresh_rates[byte];
	}
	byte = __builtin_inb(0x57);
	byte &= 0xf8;
	byte |= refresh_rate;
	__builtin_outb(byte, 0x57);
}
