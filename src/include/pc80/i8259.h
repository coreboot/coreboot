#ifndef PC80_I8259
#define PC80_I8259


#ifdef I8259
void setup_i8259(void);
#else
#define setup_i8259() do {} while(0)
#endif

#endif
