/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"

static uint32_t get_mem_multiplier(const struct sysinfo *ctrl)
{
	const uint32_t mult = NS2MHZ_DIV256 / (ctrl->tCK * ctrl->base_freq);

	if (ctrl->base_freq == 100)
		return clamp_u32(7, mult, 12);

	if (ctrl->base_freq == 133)
		return clamp_u32(3, mult, 10);

	die("Unsupported base frequency\n");
}

static void normalize_tck(struct sysinfo *ctrl, const bool pll_ref100)
{
	/** TODO: Haswell supports up to DDR3-2600 **/
	if (ctrl->tCK <= TCK_1200MHZ) {
		ctrl->tCK = TCK_1200MHZ;
		ctrl->base_freq = 133;
		ctrl->mem_clock_mhz = 1200;

	} else if (ctrl->tCK <= TCK_1100MHZ) {
		ctrl->tCK = TCK_1100MHZ;
		ctrl->base_freq = 100;
		ctrl->mem_clock_mhz = 1100;

	} else if (ctrl->tCK <= TCK_1066MHZ) {
		ctrl->tCK = TCK_1066MHZ;
		ctrl->base_freq = 133;
		ctrl->mem_clock_mhz = 1066;

	} else if (ctrl->tCK <= TCK_1000MHZ) {
		ctrl->tCK = TCK_1000MHZ;
		ctrl->base_freq = 100;
		ctrl->mem_clock_mhz = 1000;

	} else if (ctrl->tCK <= TCK_933MHZ) {
		ctrl->tCK = TCK_933MHZ;
		ctrl->base_freq = 133;
		ctrl->mem_clock_mhz = 933;

	} else if (ctrl->tCK <= TCK_900MHZ) {
		ctrl->tCK = TCK_900MHZ;
		ctrl->base_freq = 100;
		ctrl->mem_clock_mhz = 900;

	} else if (ctrl->tCK <= TCK_800MHZ) {
		ctrl->tCK = TCK_800MHZ;
		ctrl->base_freq = 133;
		ctrl->mem_clock_mhz = 800;

	} else if (ctrl->tCK <= TCK_700MHZ) {
		ctrl->tCK = TCK_700MHZ;
		ctrl->base_freq = 100;
		ctrl->mem_clock_mhz = 700;

	} else if (ctrl->tCK <= TCK_666MHZ) {
		ctrl->tCK = TCK_666MHZ;
		ctrl->base_freq = 133;
		ctrl->mem_clock_mhz = 666;

	} else if (ctrl->tCK <= TCK_533MHZ) {
		ctrl->tCK = TCK_533MHZ;
		ctrl->base_freq = 133;
		ctrl->mem_clock_mhz = 533;

	} else if (ctrl->tCK <= TCK_400MHZ) {
		ctrl->tCK = TCK_400MHZ;
		ctrl->base_freq = 133;
		ctrl->mem_clock_mhz = 400;

	} else {
		ctrl->tCK = 0;
		ctrl->base_freq = 1;
		ctrl->mem_clock_mhz = 0;
		return;
	}
	if (!pll_ref100 && ctrl->base_freq == 100) {
		/* Skip unsupported frequency */
		ctrl->tCK++;
		normalize_tck(ctrl, pll_ref100);
	}
}

#define MIN_CAS	4
#define MAX_CAS	24

static uint8_t find_compatible_cas(struct sysinfo *ctrl)
{
	printk(RAM_DEBUG, "With tCK %u, try CAS: ", ctrl->tCK);
	const uint8_t cas_lower = MAX(MIN_CAS, DIV_ROUND_UP(ctrl->tAA, ctrl->tCK));
	const uint8_t cas_upper = MIN(MAX_CAS, 19); /* JEDEC MR0 limit */

	if (!(ctrl->cas_supported >> (cas_lower - MIN_CAS))) {
		printk(RAM_DEBUG, "DIMMs do not support CAS >= %u\n", cas_lower);
		ctrl->tCK++;
		return 0;
	}
	for (uint8_t cas = cas_lower; cas <= cas_upper; cas++) {
		printk(RAM_DEBUG, "%u ", cas);
		if (ctrl->cas_supported & BIT(cas - MIN_CAS)) {
			printk(RAM_DEBUG, "OK\n");
			return cas;
		}
	}
	return 0;
}

static enum raminit_status find_cas_tck(struct sysinfo *ctrl)
{
	/** TODO: Honor all possible PLL_REF100_CFG values **/
	uint8_t pll_ref100 = (pci_read_config32(HOST_BRIDGE, CAPID0_B) >> 21) & 0x7;
	printk(RAM_DEBUG, "PLL_REF100_CFG value: 0x%x\n", pll_ref100);
	printk(RAM_DEBUG, "100MHz reference clock support: %s\n", pll_ref100 ? "yes" : "no");

	uint8_t selected_cas;
	while (true) {
		/* Round tCK up so that it is a multiple of either 133 or 100 MHz */
		normalize_tck(ctrl, pll_ref100);
		if (!ctrl->tCK) {
			printk(BIOS_ERR, "Couldn't find compatible clock / CAS settings\n");
			return RAMINIT_STATUS_MPLL_INIT_FAILURE;
		}
		selected_cas = find_compatible_cas(ctrl);
		if (selected_cas)
			break;

		ctrl->tCK++;
	}
	printk(BIOS_DEBUG, "Found compatible clock / CAS settings\n");
	printk(BIOS_DEBUG, "Selected DRAM frequency: %u MHz\n", NS2MHZ_DIV256 / ctrl->tCK);
	printk(BIOS_DEBUG, "Selected CAS latency   : %uT\n", selected_cas);
	ctrl->multiplier = get_mem_multiplier(ctrl);
	return RAMINIT_STATUS_SUCCESS;
}

enum raminit_status initialise_mpll(struct sysinfo *ctrl)
{
	if (ctrl->tCK > TCK_400MHZ) {
		printk(BIOS_ERR, "tCK is too slow. Increasing to 400 MHz as last resort\n");
		ctrl->tCK = TCK_400MHZ;
	}
	while (true) {
		/*
		 * On fast and S3 flows, MPLL frequency is already decided and
		 * has to match training data. So, skip finding the frequency.
		 *
		 * Also skip this when the MPLL is already locked. This can be
		 * the case if retrying raminit for some reason e.g. fast flow
		 * failed or the margins post power training are below minima.
		 */
		/** FIXME: This terribleness ignores failure modes in fast / S3 flows **/
		if (!ctrl->qclkps) {
			const enum raminit_status status = find_cas_tck(ctrl);
			if (status)
				return status;
		}

		/*
		 * Unlike previous generations, Haswell's MPLL won't shut down if the
		 * requested frequency isn't supported. But we cannot reinitialize it.
		 * Another different thing: MPLL registers are 4-bit instead of 8-bit.
		 */

		/** FIXME: Obtain current clock frequency if we want to skip this **/
		//if (mchbar_read32(MC_BIOS_DATA) != 0)
		//	break;

		uint32_t mc_bios_req = ctrl->multiplier;
		if (ctrl->base_freq == 100) {
			/* Use 100 MHz reference clock */
			mc_bios_req |= BIT(4);
		}
		mc_bios_req |= BIT(31);
		printk(RAM_DEBUG, "MC_BIOS_REQ = 0x%08x\n", mc_bios_req);
		printk(BIOS_DEBUG, "MPLL busy... ");
		mchbar_write32(MC_BIOS_REQ, mc_bios_req);

		for (unsigned int i = 0; i <= 5000; i++) {
			if (!(mchbar_read32(MC_BIOS_REQ) & BIT(31))) {
				printk(BIOS_DEBUG, "done in %u us\n", i);
				break;
			}
			udelay(1);
		}
		if (mchbar_read32(MC_BIOS_REQ) & BIT(31))
			printk(BIOS_DEBUG, "did not lock\n");

		/* Verify locked frequency */
		const uint32_t mc_bios_data = mchbar_read32(MC_BIOS_DATA);
		printk(RAM_DEBUG, "MC_BIOS_DATA = 0x%08x\n", mc_bios_data);
		if ((mc_bios_data & 0xf) >= ctrl->multiplier)
			break;

		printk(BIOS_DEBUG, "Retrying at a lower frequency\n\n");
		ctrl->tCK++;
	}
	if (!ctrl->mem_clock_mhz) {
		printk(BIOS_ERR, "Could not program MPLL frequency\n");
		return RAMINIT_STATUS_MPLL_INIT_FAILURE;
	}
	printk(BIOS_DEBUG, "MPLL frequency is set to: %u MHz ", ctrl->mem_clock_mhz);
	ctrl->mem_clock_fs = 1000000000 / ctrl->mem_clock_mhz;
	printk(BIOS_DEBUG, "(period: %u femtoseconds)\n", ctrl->mem_clock_fs);
	ctrl->qclkps = ctrl->mem_clock_fs / 2000;
	printk(BIOS_DEBUG, "Quadrature clock period: %u picoseconds\n", ctrl->qclkps);
	return wait_for_first_rcomp();
}
