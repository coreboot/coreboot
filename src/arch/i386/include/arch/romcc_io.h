static void outb(unsigned char value, unsigned short port)
{
	__builtin_outb(value, port);
}

static void outw(unsigned short value, unsigned short port)
{
	__builtin_outw(value, port);
}

static void outl(unsigned int value, unsigned short port)
{
	__builtin_outl(value, port);
}


static unsigned char inb(unsigned short port)
{
	return __builtin_inb(port);
}


static unsigned char inw(unsigned short port)
{
	return __builtin_inw(port);
}

static unsigned char inl(unsigned short port)
{
	return __builtin_inl(port);
}

static void hlt(void)
{
	__builtin_hlt();
}

static unsigned int config_cmd(unsigned char bus, unsigned devfn, unsigned where)
{
	return 0x80000000 | (bus << 16) | (devfn << 8) | (where & ~3);
}

static unsigned char pcibios_read_config_byte(
	unsigned char bus, unsigned devfn, unsigned where)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	return inb(0xCFC + (where & 3));
}

static unsigned short pcibios_read_config_word(
	unsigned char bus, unsigned devfn, unsigned where)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	return inw(0xCFC + (where & 2));
}

static unsigned int pcibios_read_config_dword(
	unsigned char bus, unsigned devfn, unsigned where)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	return inl(0xCFC);
}


static void pcibios_write_config_byte(
	unsigned char bus, unsigned devfn, unsigned where, unsigned char value)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	outb(value, 0xCFC + (where & 3));
}

static void pcibios_write_config_word(
	unsigned char bus, unsigned devfn, unsigned where, unsigned short value)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	outw(value, 0xCFC + (where & 2));
}

static void pcibios_write_config_dword(
	unsigned char bus, unsigned devfn, unsigned where, unsigned int value)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	outl(value, 0xCFC);
}
