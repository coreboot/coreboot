static void outl(unsigned int value, unsigned short port)
{
        __builtin_outl(value, port);
}

static unsigned char inl(unsigned short port)
{
        return __builtin_inl(port);
}


static void setup_coherent_ht_domain(void)
{
        static const unsigned int register_values[] = {
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x40) & 0xFF)), 0xfff0f0f0, 0x00010101,

        };
	unsigned long reg;
	reg = inl(0xFC);
	reg &= register_values[1];
	reg |= register_values[2] & ~register_values[1];
	outl(register_values[0], 0xF8);
	outl(reg, 0xFC);
}
