#ifndef NORTHBRIDGE_VIA_VT8623_H
#define NORTHBRIDGE_VIA_VT8623_H

unsigned int vt8623_scan_root_bus(device_t root, unsigned int max);
extern void (*realmode_interrupt)(u32 intno, u32 eax, u32 ebx, u32 ecx, u32 edx, 
		u32 esi, u32 edi) __attribute__((regparm(0)));

#endif /* NORTHBRIDGE_VIA_VT8623_H */
