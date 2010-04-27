#define SMBUS_MEM_DEVICE_START 0x50
#define SMBUS_MEM_DEVICE_END 0x53
#define SMBUS_MEM_DEVICE_INC 1

static void spd_set_drb(void)
{
	/*
	 * Effects:	Uses serial presence detect to set the
	 *              DRB registers which holds the ending memory address assigned
	 *              to each DIMM.
	 */
	unsigned end_of_memory;
	unsigned char device;

	end_of_memory = 0; /* in multiples of 8MiB */
	device = SMBUS_MEM_DEVICE_START;
	while (device <= SMBUS_MEM_DEVICE_END) {
		unsigned side1_bits, side2_bits;
		int byte, byte2;

		side1_bits = side2_bits = -1;

		/* Compute the end address for the DRB register */
		/* Only process dimms < 2GB (2^8 * 8MB) */
		if (side1_bits < 8) {
			end_of_memory += (1 << side1_bits);
		}
		device += SMBUS_MEM_DEVICE_INC;
	}
}

