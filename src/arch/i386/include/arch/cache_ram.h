#ifndef CACHE_RAM_H
#define CACHE_RAM_H

#ifndef CACHE_RAM_BASE
#define CACHE_RAM_BASE 0
#endif

#define CACHE_RAM_SEG_BASE (CACHE_RAM_BASE - _RAMBASE)

#define RAM(type, addr) (*((type *)((unsigned char*)((addr) - CACHE_RAM_SEG_BASE))))
#define RAM_ADDR( addr) ((void *)((addr) - CACHE_RAM_BASE))

#endif /* CACHE_RAM_H */
