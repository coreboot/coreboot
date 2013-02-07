#if CONFIG_ARCH_X86
#include <arch/io.h>
#else
void outb(u8 val, u16 port);
void outw(u16 val, u16 port);
void outl(u32 val, u16 port);

u8 inb(u16 port);
u16 inw(u16 port);
u32 inl(u16 port);
#endif
