struct sub4 {
	unsigned a;
	unsigned b;
	unsigned c;
	unsigned d;
	unsigned e;
	unsigned f;
	unsigned g;
};
struct sub3 {
	unsigned a;
	unsigned b;
	unsigned c;
	unsigned d;
	unsigned e;
	unsigned f;
	struct sub4 s4;
};
struct sub2 {
	unsigned a;
	unsigned b;
	unsigned c;
	unsigned d;
	unsigned e;
	struct sub3 s3;
};
struct sub1 {
	unsigned a;
	unsigned b;
	struct sub2 s2;
};

struct stuff {
	signed int   a;
	signed int   b;
	signed int   c;
	unsigned int d;
	struct sub1  s1;
};


static void main(void)
{
	struct stuff *var;
	unsigned int *foo;

	var = (struct stuff *)(0x12345678);
	foo = &var->d;
	foo = &((*var).d);
	foo = &var->s1.s2.s3.s4.g;
}
