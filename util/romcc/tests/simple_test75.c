static void goto_test(void)
{
	int i;

	i = 0;
	goto bottom;
	{
	top:
		i = i + 1;
	}
 bottom:
	if (i < 10) {
		goto top;
	}
	;
}

static void main(void)
{
	goto_test();
}
