typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;


struct iter {
	uint32_t i;
};

struct point {
	uint32_t i, j;
};

static void outb(uint8_t value, uint16_t port)
{
        __builtin_outb(value, port);
}


static struct point mkpoint(void)
{
	struct point p;
	p.i = 1000;
	p.j = 2000;
	return p;
}

static void main(void)
{
	struct iter iter;
	for(iter.i = 0; iter.i < 32; iter.i++) {
		outb(iter.i, 0x80);
	}
	struct point p;
	p = mkpoint();
	outb(p.i, 0x80);
	outb(p.j, 0x80);
}

