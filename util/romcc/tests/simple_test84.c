struct stuff {
	signed int   a : 5;
	signed int   b : 6;
	signed int   c : 2;
	unsigned int d : 3;
};

static void main(void)
{
	struct stuff var;
	volatile int a, b, c, d;
	a = 1;
	b = 2;
	c = 3;
	d = 7;

	var.a = a;
	var.b = b;
	var.c = c;
	var.d = d;

	a = var.a;
	b = var.b;
	c = var.c;
	d = var.d;

	asm(" " :: "r"(a), "r"(b), "r"(c), "r"(d));
}
