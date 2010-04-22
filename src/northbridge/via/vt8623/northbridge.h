#ifndef NORTHBRIDGE_VIA_VT8623_H
#define NORTHBRIDGE_VIA_VT8623_H

unsigned int vt8623_scan_root_bus(device_t root, unsigned int max);
extern void (*vga_enable_console)(void) __attribute__((regparm(0)));
void write_protect_vgabios(void);

#endif /* NORTHBRIDGE_VIA_VT8623_H */
