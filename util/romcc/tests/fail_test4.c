static void main(void)
{
	static const int foo = 2;
	switch(foo) {
	case 1:
		break;
	case 2:
		break;
	case 1:
		break;
	default:
		break;
	}
}
