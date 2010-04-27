typedef __builtin_div_t div_t;
typedef __builtin_ldiv_t ldiv_t;
typedef __builtin_udiv_t udiv_t;
typedef __builtin_uldiv_t uldiv_t;

static div_t div(int numer, int denom)
{
	return __builtin_div(numer, denom);
}
static ldiv_t ldiv(long numer, long denom)
{
	return __builtin_ldiv(numer, denom);
}
static udiv_t udiv(unsigned numer, unsigned denom)
{
	return __builtin_udiv(numer, denom);
}
static uldiv_t uldiv(unsigned long numer, unsigned long denom)
{
	return __builtin_uldiv(numer, denom);
}

static void main(void)
{
	volatile long *sval = (volatile long *)0x1234;
	volatile unsigned long *uval = (volatile unsigned long *)0x5678;
	long int a, b, c;
	unsigned long e, f, g;
	ldiv_t lresult;
	uldiv_t ulresult;

	a = sval[0];
	b = sval[1];
	lresult = ldiv(a,b);
	sval[2] = lresult.quot;
	sval[3] = lresult.rem;

	e = uval[0];
	f = uval[1];
	ulresult = uldiv(e, f);
	uval[2] = ulresult.quot;
	uval[3] = ulresult.rem;
}
