typedef unsigned char uint8_t;
static unsigned int generate_row(uint8_t row, uint8_t maxnodes)
{

	unsigned int ret=0x00010101;
	static const unsigned int rows_2p[2][2] = {
		{ 0x00050101, 0x00010404 },
		{ 0x00010404, 0x00050101 }
	};
	if(maxnodes>2) {
		maxnodes=2;
	}
	if (row < maxnodes) {
		ret=rows_2p[0][row];
	}
	return ret;
}

static void setup_node(void)
{
	unsigned char row;
	for(row=0; row< 2; row++) {
		__builtin_outl(generate_row(row, 2), 0x1234);
	}
}
