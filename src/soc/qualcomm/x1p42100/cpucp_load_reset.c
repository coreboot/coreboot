/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <program_loading.h>
#include <soc/cpucp.h>
#include <device/mmio.h>
#include <soc/addressmap.h>

void cpucp_fw_load_reset(void)
{
	struct prog cpucp_dtbs_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/cpucp_dtbs");

	if (!selfload(&cpucp_dtbs_prog))
		die("SOC image: CPUCP DTBS load failed");

	printk(BIOS_DEBUG, "SOC image: CPUCP DTBS image loaded successfully.\n");

	struct prog cpucp_fw_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/cpucp");

	if (!selfload(&cpucp_fw_prog))
		die("SOC image: CPUCP load failed");

	printk(BIOS_DEBUG, "SOC image: CPUCP image loaded successfully.\n");

	write32((void *) HWIO_APSS_CPUCP_CPUCP_LPM_SEQ_WAIT_EVT_CTRL_MASK_ADDR, 0x0);
	setbits32((void *) HWIO_APSS_CPUCP_CPUCP_SW_WAKEUP_REQ_ADDR, 0x1);
}
