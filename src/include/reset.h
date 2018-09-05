#ifndef RESET_H
#define RESET_H

#include <compiler.h>

/* Generic reset functions. Call from code that wants to trigger a reset. */

/* Super-hard reset specific to some Intel SoCs. */
__noreturn void global_reset(void);
/* Full board reset. Resets SoC and most/all board components (e.g. DRAM). */
__noreturn void hard_reset(void);
/* Board reset. Resets SoC some board components (e.g. TPM but not DRAM). */
__noreturn void soft_reset(void);

/* Reset implementations. Implement these in SoC or mainboard code. Implement
   at least hard_reset() if possible, others fall back to it if necessary. */
void do_global_reset(void);
void do_hard_reset(void);
void do_soft_reset(void);

enum reset_type {	/* listed in order of softness */
	GLOBAL_RESET,
	HARD_RESET,
	SOFT_RESET,
};

/* Callback that an SoC may override to perform special actions before reset.
   Take into account that softer resets may fall back to harder resets if not
   implemented... this will *not* trigger another callback! */
void soc_reset_prepare(enum reset_type reset_type);

#endif
