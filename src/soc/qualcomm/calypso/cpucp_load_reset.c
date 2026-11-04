/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <arch/mmu.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/cpucp.h>
#include <device/mmio.h>
#include <security/vboot/vboot_common.h>
#include <soc/addressmap.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <symbols.h>

static void pdp_fw_load(void)
{
	const char *pdp_name = (CONFIG(VBOOT) && !vboot_recovery_mode_enabled())
			 ? CONFIG_CBFS_PREFIX "/pdp_rw"
			 : CONFIG_CBFS_PREFIX "/pdp_ro";

	struct prog pdp_fw_prog = PROG_INIT(PROG_PAYLOAD, pdp_name);

	if (!selfload(&pdp_fw_prog))
		die("SOC image: 'pdp' load failed");
	printk(BIOS_DEBUG, "SOC image: 'pdp' loaded successfully.\n");

	struct prog pdp_cdb_fw_prog = PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/pdp_cdb");

	if (!selfload(&pdp_cdb_fw_prog))
		die("SOC image: 'pdp_cdb' load failed");
	printk(BIOS_DEBUG, "SOC image: 'pdp_cdb' loaded successfully.\n");
}

static void cpucp_fw_load(void)
{
	/* map to cached region to force address to be 4 byte aligned */
	mmu_config_range((void *)_cpucp, REGION_SIZE(cpucp), CACHED_RAM);

	struct prog cpucp_dtbs_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/cpucp_dtbs");

	if (!selfload(&cpucp_dtbs_prog))
		die("SOC image: 'cpucp dtbs' load failed");
	printk(BIOS_DEBUG, "SOC image: 'cpucp dtbs' image loaded successfully.\n");

	const char *cpucp_name = (CONFIG(VBOOT) && !vboot_recovery_mode_enabled())
			 ? CONFIG_CBFS_PREFIX "/cpucp_rw"
			 : CONFIG_CBFS_PREFIX "/cpucp_ro";

	struct prog cpucp_fw_prog = PROG_INIT(PROG_PAYLOAD, cpucp_name);

	if (!selfload(&cpucp_fw_prog))
		die("SOC image: 'cpucp' load failed");
	printk(BIOS_DEBUG, "SOC image: 'cpucp' image loaded successfully.\n");

	/* flush cached region */
	dcache_clean_by_mva(_cpucp, REGION_SIZE(cpucp));
	/* remap back to device memory */
	mmu_config_range((void *)_cpucp, REGION_SIZE(cpucp), DEV_MEM);
}


void cpucp_pdp_fw_load_reset(void)
{
	cpucp_fw_load();
	pdp_fw_load();

	/* Debug scripts may have set Wakeup Mask. Clear it */
	write32((void *) HWIO_APSS_CPUCP_CPUCP_LPM_SEQ_WAIT_EVT_CTRL_MASK_ADDR, 0x0);

	/* Zero out SEQ_SLEEP_REQ to have a clean state */
	write32((void *) HWIO_APSS_CPUCP_LPMBFSM_CPUCP_SW_SLEEP_REQ, 0x0);

	/* Trigger CPUCP boot */
	write32((void *) APSS_GLUE_BOOT_REG_BASE_SW_TRIGGER, 0x1);
}
