#ifndef __HELPER_EXEC_H__
#define __HELPER_EXEC_H__

u32 getIntVect(int num);
int run_bios_int(int num);
void pushw(u16 val);

int port_rep_inb(u16 port, u32 base, int d_f, u32 count);
int port_rep_inw(u16 port, u32 base, int d_f, u32 count);
int port_rep_inl(u16 port, u32 base, int d_f, u32 count);
int port_rep_outb(u16 port, u32 base, int d_f, u32 count);
int port_rep_outw(u16 port, u32 base, int d_f, u32 count);
int port_rep_outl(u16 port, u32 base, int d_f, u32 count);

u8 x_inb(u16 port);
u16 x_inw(u16 port);
void x_outb(u16 port, u8 val);
void x_outw(u16 port, u16 val);
u32 x_inl(u16 port);
void x_outl(u16 port, u32 val);

u8 Mem_rb(int addr);
u16 Mem_rw(int addr);
u32 Mem_rl(int addr);

void Mem_wb(int addr, u8 val);
void Mem_ww(int addr, u16 val);
void Mem_wl(int addr, u32 val);
void getsecs(unsigned long *sec, unsigned long *usec);
u8 bios_checksum(u8 * start, int size);

#endif
