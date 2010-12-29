#ifndef NORTHBRIDGE_AMD_GX2_H
#define NORTHBRIDGE_AMD_GX2_H

#include <cpu/amd/gx2def.h>

/* northbridge.c */
unsigned int gx2_scan_root_bus(device_t root, unsigned int max);
int sizeram(void);
void do_vsmbios(void);
void graphics_init(void);

/* northbridgeinit.c */
void northbridge_init_early(void);
uint32_t get_systop(void);

#endif /* NORTHBRIDGE_AMD_GX2_H */
