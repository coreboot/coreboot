static void order_dimms(void)
{
        unsigned long tom;

        tom = 0;
        for(;;) {

                unsigned csbase, csmask;
                unsigned size;
		unsigned index;
                csbase = 0;

                for(index = 0; index < 1; index++) {
			csbase = __builtin_inl(0x40);
                }
                if (csbase == 0) {
                        break;
                }


                size = csbase;

                csbase = (tom << 21);

                tom += size;


                csmask = size;
                csmask |= 0xfe00;


		__builtin_outl(csbase, 0xCFC);

		__builtin_outl(0xc260, 0xCF8);
		__builtin_outl(csmask, 0xCFC);
        }

	tom &=  ~0xff000000;

	__builtin_outl(tom, 0x1234);
}
