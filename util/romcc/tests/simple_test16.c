typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;


static void outb(uint8_t value, uint16_t port)
{
        __builtin_outb(value, port);
}


static void main(void)
{
	uint32_t i;
	for(i = 0; i < 32; i++) {
		outb(i, 0x80);
	}
}
