static void outb(unsigned char value, unsigned short port)
{
	__builtin_outb(value, port);
}

static void pnp_write_config(void)
{
	unsigned char port;
	unsigned char value;
	unsigned char reg;
	port = 0x2e;
	value = 0x03;
	reg = 0x07;
	outb(reg, port);
	outb(value, port +1);
	outb(value -1, port +2);
}

static void main(void)
{
	pnp_write_config();
}
