#ifndef CACHE_RAM_H
#define CACHE_RAM_H

#ifndef CACHE_RAM_BASE
#define CACHE_RAM_BASE 0
#endif

#if !defined(ASSEMBLY)
#define CACHE_RAM_SEG_BASE (((unsigned long)CACHE_RAM_BASE) - ((unsigned long)_RAMBASE))
#else
#define CACHE_RAM_SEG_BASE (CACHE_RAM_BASE - _RAMBASE)
#endif



#if !defined(ASSEMBLY)

#define RAM_ADDR( addr) ((void *)(((unsigned char *)(addr)) - CACHE_RAM_SEG_BASE))
#define RAM(type, addr) (*((volatile type *)RAM_ADDR(addr)))

void cache_ram_start(void);
#endif

#endif /* CACHE_RAM_H */
