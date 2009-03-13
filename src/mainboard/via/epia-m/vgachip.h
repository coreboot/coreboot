#ifndef _PC80_VGABIOS
#define _PC80_VGABIOS

extern struct chip_control pc80_vgabios_control;

struct pc80_vgabios_config {
	int nothing;
};

void vga_enable_console(void);
void do_vgabios(void);
void setup_realmode_idt(void);
void write_protect_vgabios(void);

#endif /* _PC80_VGABIOS */
