/* SPDX-License-Identifier: GPL-2.0-only */
#include <soc/clock.h>
#include <device/mmio.h>
#include <soc/addressmap.h>

#define PLL_REF_CLK	50000000	/* 50 MHz */

union cavm_rst_boot {
	u64 u;
	struct {
		u64 rboot_pin:1;
		u64 rboot:1;
		u64 lboot:10;
		u64 lboot_ext23:6;
		u64 lboot_ext45:6;
		u64 reserved_24_29:6;
		u64 lboot_oci:3;
		u64 pnr_mul:6;
		u64 reserved_39_39:1;
		u64 c_mul:7;
		u64 reserved_47_54:8;
		u64 dis_scan:1;
		u64 dis_huk:1;
		u64 vrm_err:1;
		u64 jt_tstmode:1;
		u64 ckill_ppdis:1;
		u64 trusted_mode:1;
		u64 ejtagdis:1;
		u64 jtcsrdis:1;
		u64 chipkill:1;
	} s;
};

/**
 * Returns the reference clock speed in Hz
 */
u64 thunderx_get_ref_clock(void)
{
	return PLL_REF_CLK;
}

/**
 * Returns the I/O clock speed in Hz
 */
u64 thunderx_get_io_clock(void)
{
	union cavm_rst_boot rst_boot;

	rst_boot.u = read64p(RST_PF_BAR0);

	return ((u64)rst_boot.s.pnr_mul) * PLL_REF_CLK;
}

/**
 * Returns the core clock speed in Hz
 */
u64 thunderx_get_core_clock(void)
{
	union cavm_rst_boot rst_boot;

	rst_boot.u = read64p(RST_PF_BAR0);

	return ((u64)rst_boot.s.c_mul) * PLL_REF_CLK;
}
