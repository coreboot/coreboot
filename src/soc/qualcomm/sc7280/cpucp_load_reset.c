/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/mmu.h>
#include <soc/cpucp.h>
#include <soc/clock.h>
#include <device/mmio.h>
#include <timer.h>

void cpucp_prepare(void)
{
	/* allow NS access to EPSS memory */
	setbits32(&epss_top->access_override, 0x1);

	/* Enable subsystem clock. Required for CPUCP PDMEM access */
	setbits32(&epss_fast->epss_muc_clk_ctrl, 0x1);
}

void cpucp_fw_load_reset(void)
{
	struct prog cpucp_fw_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/cpucp");

	if (!wait_ms(300, (read32(&epss_fast->epss_muc_clk_ctrl) & 0x1) == 0x1))
		printk(BIOS_ERR, "%s: cannot get CPUCP PDMEM access.\n", __func__);

	if (!selfload(&cpucp_fw_prog))
		die("SOC image: CPUCP load failed");

	printk(BIOS_DEBUG, "SOC:CPUCP image loaded successfully.\n");
}
