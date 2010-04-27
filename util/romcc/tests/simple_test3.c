static void spd_set_drb(void)
{
	/*
	 * Effects:	Uses serial presence detect to set the
	 *              DRB registers which holds the ending memory address assigned
	 *              to each DIMM.
	 */
	unsigned end_of_memory;
	unsigned device;

	end_of_memory = 0; /* in multiples of 8MiB */
	device = 0x50;
	while (device <= 0x53) {
		unsigned side1_bits, side2_bits;
		int byte, byte2;

		side1_bits = side2_bits = -1;

		/* rows */
		byte = -1;
		if (1) {
			/* now I have the ram size in bits as a power of two (less 1) */
			/* Make it mulitples of 8MB */
			side1_bits -= 25;
		}

		/* Compute the end address for the DRB register */
		/* Only process dimms < 2GB (2^8 * 8MB) */
		if (1) {
			end_of_memory += side1_bits;
		}
		__builtin_outl(end_of_memory, 0x1234);

		if (1) {
			end_of_memory += side2_bits;
		}
	}
}
