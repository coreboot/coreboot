#ifndef CACHE_RAM_H
#define CACHE_RAM_H

#ifndef CACHE_RAM_BASE
#define CACHE_RAM_BASE 0
#endif

#if !defined(ASSEMBLY)
extern char _cache_ram_seg_base[];
#define CACHE_RAM_SEG_BASE ((unsigned long)_cache_ram_seg_base)
#else
#define CACHE_RAM_SEG_BASE (_cache_ram_seg_base)
#endif

#if !defined(ASSEMBLY)
extern char _rom_code_seg_base[];
#define ROM_CODE_SEG_BASE ((unsigned long)_rom_code_seg_base)
#else
#define ROM_CODE_SEG_BASE (_rom_code_seg_base)
#endif


#if !defined(ASSEMBLY)

#define RAM_ADDR( addr) ((void *)(((unsigned char *)(addr)) - CACHE_RAM_SEG_BASE))
#define RAM(type, addr) (*((volatile type *)RAM_ADDR(addr)))

void cache_ram_start(void);
#endif

#endif /* CACHE_RAM_H */
