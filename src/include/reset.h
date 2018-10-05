#ifndef RESET_H
#define RESET_H

/*
 * Generic board reset function. Call from common code that
 * wants to trigger a reset.
 */
__noreturn void board_reset(void);
/*
 * SoC or board specific implementation of the board reset.
 *
 * Implementations shall meet the following criteria:
 *
 *   o For vboot support, the TPM MUST be reset.
 *
 *   o All SoC/chipset blocks SHOULD be reset except for those
 *     that are intentionally meant to survive reset (e.g. tomb-
 *     stone registers and that sort of stuff).
 *
 *   o All external SoC pins SHOULD return to power-on reset values.
 *
 *   o The CPU MUST resume execution from power-on reset vector
 *     (same as cold boot).
 *
 *   o Other board components (e.g. PCI, SDIO and stuff) SHOULD
 *     be reset.
 *
 *   o USB SHOULD be power-cycled.
 *
 *   o Board components that are intended to be fully independent
 *     from SoC (e.g. EC and EC-attached devices, the Cr50 on
 *     Chromebooks) SHOULD NOT be reset.
 *
 * General recommendations:
 *
 *   o DRAM SHOULD NOT lose power if possible.
 *
 *   o Reset time SHOULD be minimized
 */
void do_board_reset(void);

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
