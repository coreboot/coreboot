

struct result {
	int a, b, c, d;
};

struct big_arg {
	int a, b;
};
static struct result main(int a, int b, struct big_arg d)
{
	struct result result;
	result.a = 1;
	result.b = 1;
	result.c = b + 1;
	result.d = a + 1;

}

