#define COUNT 26
static void main(void)
{
	unsigned char a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z;
	a = 1;
	b = 2;
	c = 3;
	d = 4;
	e = 5;
	f = 6;
	g = 7;
	h = 8;
	i = 9;
	j = 10;
	k = 11;
	l = 12;
	m = 13;
	n = 14;
	o = 15;
	p = 16;
	q = 17;
	r = 18;
	s = 19;
	t = 20;
	u = 21;
	v = 22;
	w = 23;
	x = 24;
	y = 25;
	z = 26;
#if COUNT >= 26
	__builtin_outb(z, 0xab);
#endif
#if COUNT >= 25
	__builtin_outb(y, 0xab);
#endif
#if COUNT >= 24
	__builtin_outb(x, 0xab);
#endif
#if COUNT >= 23
	__builtin_outb(w, 0xab);
#endif
#if COUNT >= 22
	__builtin_outb(v, 0xab);
#endif
#if COUNT >= 21
	__builtin_outb(u, 0xab);
#endif
#if COUNT >= 20
	__builtin_outb(t, 0xab);
#endif
#if COUNT >= 19
	__builtin_outb(s, 0xab);
#endif
#if COUNT >= 18
	__builtin_outb(r, 0xab);
#endif
#if COUNT >= 17
	__builtin_outb(q, 0xab);
#endif
#if COUNT >= 16
	__builtin_outb(p, 0xab);
#endif
#if COUNT >= 15
	__builtin_outb(o, 0xab);
#endif
#if COUNT >= 14
	__builtin_outb(n, 0xab);
#endif
#if COUNT >= 13
	__builtin_outb(m, 0xab);
#endif
#if COUNT >= 12
	__builtin_outb(l, 0xab);
#endif
#if COUNT >= 11
	__builtin_outb(k, 0xab);
#endif
#if COUNT >= 10
	__builtin_outb(j, 0xab);
#endif
#if COUNT >= 9
	__builtin_outb(i, 0xab);
#endif
#if COUNT >= 8
	__builtin_outb(h, 0xab);
#endif
#if COUNT >= 7
	__builtin_outb(g, 0xab);
#endif
#if COUNT >= 6
	__builtin_outb(f, 0xab);
#endif
#if COUNT >= 5
	__builtin_outb(e, 0xab);
#endif
#if COUNT >= 4
	__builtin_outb(d, 0xab);
#endif
#if COUNT >= 3
	__builtin_outb(c, 0xab);
#endif
#if COUNT >= 2
	__builtin_outb(b, 0xab);
#endif
#if COUNT >= 1
	__builtin_outb(a, 0xab);
#endif
}
