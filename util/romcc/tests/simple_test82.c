

struct result {
	int a, b, c, d;
};

static struct result main(int a, int b, int c, int  d)
{
	struct result result;
	result.a = d;
	result.b = c;
	result.c = b;
	result.d = a;

	return result;
}

