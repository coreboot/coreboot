#ifndef __LINUXBIOS_P6_L2_CACHE_H
#define __LINUXBIOS_P6_L2_CACHE_H

#define EBL_CR_POWERON	0x2A

#define BBL_CR_D0	0x88
#define BBL_CR_D1	0x89
#define BBL_CR_D2	0x8A
#define BBL_CR_D3	0x8B

#define BBL_CR_ADDR	0x116
#define BBL_CR_DECC	0x118
#define BBL_CR_CTL	0x119
#define BBL_CR_TRIG	0x11A
#define BBL_CR_BUSY	0x11B
#define BBL_CR_CTL3	0x11E

extern int p6_configure_l2_cache(void);

#endif /* __LINUXBIOS_P6_L2_CACHE_H */
