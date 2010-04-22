#ifndef NORTHBRIDGE_AMD_GX2_H
#define NORTHBRIDGE_AMD_GX2_H

#if !defined(__ROMCC__)  && !defined(ASSEMBLY)
#if defined(__PRE_RAM__)
#else
unsigned int gx2_scan_root_bus(device_t root, unsigned int max);
int sizeram(void);
void gx2_chipsetinit (struct northbridge_amd_gx2_config *nb);
void graphics_init(void);
void northbridgeinit(void);
#endif
#endif

#endif /* NORTHBRIDGE_AMD_GX2_H */
