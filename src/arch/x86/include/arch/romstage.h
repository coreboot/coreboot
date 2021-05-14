/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_ROMSTAGE_H__
#define __ARCH_ROMSTAGE_H__

#include <stddef.h>
#include <stdint.h>
#include <cpu/x86/mtrr.h>

void mainboard_romstage_entry(void);

/*
 * Support setting up a stack frame consisting of MTRR information
 * for use in bootstrapping the caching attributes after cache-as-ram
 * is torn down.
 */

struct postcar_frame {
	int skip_common_mtrr;
	struct var_mtrr_context *mtrr;
};

/*
 * Initialize postcar_frame object.
 * Returns 0 on success, < 0 on error.
 */
int postcar_frame_init(struct postcar_frame *pcf);

/*
 * Add variable MTRR covering the provided range with MTRR type.
 */
void postcar_frame_add_mtrr(struct postcar_frame *pcf,
				uintptr_t addr, size_t size, int type);

/*
 * Add variable MTRR covering the memory-mapped ROM with given MTRR type.
 */
void postcar_frame_add_romcache(struct postcar_frame *pcf, int type);

/*
 * fill_postcar_frame() is called after raminit completes and right before
 * calling run_postcar_phase(). Implementation should call postcar_frame_add_mtrr()
 * to tag memory ranges as cacheable to speed up execution of postcar and
 * early ramstage.
 */
void fill_postcar_frame(struct postcar_frame *pcf);

/*
 * prepare_and_run_postcar() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use.
 */
void prepare_and_run_postcar(struct postcar_frame *pcf);

/*
 * Load and run a program that takes control of execution that
 * tears down CAR and loads ramstage. The postcar_frame object
 * indicates how to set up the frame. If caching is enabled at
 * the time of the call it is up to the platform code to handle
 * coherency with dirty lines in the cache using some mechanism
 * such as platform_prog_run() because run_postcar_phase()
 * utilizes prog_run() internally.
 */
void run_postcar_phase(struct postcar_frame *pcf);

/*
 * Systems without a native coreboot cache-as-ram teardown may implement
 * this to use an alternate method.
 */
void late_car_teardown(void);

/*
 * Cache the TSEG region at the top of ram. This region is
 * not restricted to SMM mode until SMM has been relocated.
 * By setting the region to cacheable it provides faster access
 * when relocating the SMM handler as well as using the TSEG
 * region for other purposes.
 */
void postcar_enable_tseg_cache(struct postcar_frame *pcf);

#endif /* __ARCH_ROMSTAGE_H__ */
