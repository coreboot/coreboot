

struct result {
	int a, b, c, d;
};

static struct result main(int a, int b, int c, int  d)
{
	struct result result;
	result.a = d + 1;
	result.b = c + 1;
	result.c = b + 1;
	result.d = a + 1;

	return result;
}
