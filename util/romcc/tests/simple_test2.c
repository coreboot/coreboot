void outl(unsigned int value, unsigned short port)
{
	__builtin_outl(value, port);
}

#define PIIX4_DEVFN 0x90
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
	unsigned device;

	end_of_memory = 0; /* in multiples of 8MiB */
	device = SMBUS_MEM_DEVICE_START;
	while (device <= SMBUS_MEM_DEVICE_END) {
		unsigned side1_bits;

		side1_bits =  -1;

		/* Compute the end address for the DRB register */
		/* Only process dimms < 2GB (2^8 * 8MB) */
		if (side1_bits < 8) {
			end_of_memory += (1 << side1_bits);
		}
		outl(end_of_memory, 0x1234);
	}
}
