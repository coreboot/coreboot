#ifndef CPU_X86_PAE_H
#define CPU_X86_PAE_H

#include <stdint.h>

/* Enable paging with cr3 value for page directory pointer table as well as PAE
   option in cr4. */
void paging_enable_pae_cr3(uintptr_t cr3);
/* Enable paging as well as PAE option in cr4. */
void paging_enable_pae(void);
/* Disable paging as well as PAE option in cr4. */
void paging_disable_pae(void);

/* Set/Clear NXE bit in IA32_EFER MSR */
void paging_set_nxe(int enable);

#define PAT_UC		0
#define PAT_WC		1
#define PAT_WT		4
#define PAT_WP		5
#define PAT_WB		6
#define PAT_UC_MINUS	7
#define PAT_ENCODE(type, idx) (((uint64_t)PAT_ ## type) << 8*(idx))

/* Set PAT MSR */
void paging_set_pat(uint64_t pat);
/* Set coreboot default PAT value. */
void paging_set_default_pat(void);

/* Load page directory pointer table and page tables from cbfs identified by
 * the provided the names then enable paging. Return 0 on success, < 0 on
 * failure. */
int paging_enable_for_car(const char *pdpt_name, const char *pt_name);

/* Identity map the region indicated by 'base' and 'size'. Both 'base' and
 * 'size' need to be 4KiB or 2 MiB aligned. 'pat' should be one of the
 * PAT defines above. 0 is returned on success, < 0 on failure. */
int paging_identity_map_addr(uintptr_t base, size_t size, int pat);

#define MAPPING_ERROR ((void *)0xffffffffUL)
void *map_2M_page(unsigned long page);

#endif /* CPU_X86_PAE_H  */
