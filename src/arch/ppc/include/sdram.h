#ifndef _SDRAM_H
#define _SDRAM_H

/*
 * Provided for all PPC boards to to SDRAM initialization. This
 * happens prior to entry into hardwaremain().
 */
extern void sdram_init(void);
#endif /* _SDRAM_H */
