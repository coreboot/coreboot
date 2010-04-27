static void main(void)
{
	unsigned int dch, dcl;
/* HERE I AM  async_lat */
	unsigned async_lat;
	int dimms;
	dimms = 1;
	async_lat = 0;
	dch = 0x1234;
	dcl = __builtin_inl(0x5678);
	if (!(dcl & (1 << 8))) {
		if (dimms == 4) {
			async_lat = 9;
		}
		else {
			async_lat = 8;
		}
	}
	else {
		async_lat = 6;
	}
	dch |= async_lat;
	__builtin_outl(dch, 0x9abc);
}
