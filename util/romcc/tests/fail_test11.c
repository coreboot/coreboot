

struct big_arg {
	int x, y;
};
struct result {
	struct big_arg a;
	int c, d;
};

static struct result main(int a, int b, int c, int  d)
{
	struct result result;
	result.a.x = d + 1;
	result.a.y = c + 1;
	result.c = b + 1;
	result.d = a + 1;

	return result;
}

