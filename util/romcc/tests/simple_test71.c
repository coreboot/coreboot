static void foo(void)
{
	__builtin_outl(22, 0x5678);
}
static void main(void)
{
	int i;

#if 1
	foo();
#endif
#if 1
	foo();
#endif
	for(i = 0; i < 10; i++) {
#if 1
		foo();
#endif
#if 0
		foo();
#endif
		__builtin_outl(i, 0x1234);
	}

}
