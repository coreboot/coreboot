#ifndef _SDRAM_H
#define _SDRAM_H

/*
 * Provided for all PPC boards to do SDRAM initialization. This
 * happens prior to entry into hardwaremain().
 */
extern void memory_init(void);
#endif /* _SDRAM_H */
