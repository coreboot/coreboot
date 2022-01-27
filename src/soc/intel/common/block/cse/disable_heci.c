/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/cse.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

#define CSME0_FBE	0xf
#define CSME0_BAR	0x0
#define CSME0_FID	0xb0

/* Disable HECI using PCR */
static void heci1_disable_using_pcr(void)
{
	soc_disable_heci1_using_pcr();
}

/* Disable HECI using PMC IPC communication */
static void heci1_disable_using_pmc(void)
{
	cse_disable_mei_devices();
}

/* Disable HECI using Sideband interface communication */
static void heci1_disable_using_sbi(void)
{
	struct pcr_sbi_msg msg = {
		.pid = PID_CSME0,
		.offset = 0,
		.opcode = PCR_WRITE,
		.is_posted = false,
		.fast_byte_enable = CSME0_FBE,
		.bar = CSME0_BAR,
		.fid = CSME0_FID
	};
	/* Bit 0: Set to make HECI#1 Function disable */
	uint32_t data32 = 1;
	uint8_t response;
	int status;

	/* unhide p2sb device */
	p2sb_unhide();

	/* Send SBI command to make HECI#1 function disable */
	status = pcr_execute_sideband_msg(PCH_DEV_P2SB, &msg, &data32, &response);
	if (status || response)
		printk(BIOS_ERR, "Fail to make CSME function disable\n");

	/* Ensure to Lock SBI interface after this command */
	p2sb_disable_sideband_access();

	/* hide p2sb device */
	p2sb_hide();
}

void heci1_disable(void)
{
	if (!CONFIG(DISABLE_HECI1_AT_PRE_BOOT))
		return;

	if (ENV_SMM && CONFIG(SOC_INTEL_COMMON_BLOCK_HECI1_DISABLE_USING_SBI)) {
		printk(BIOS_INFO, "Disabling Heci using SBI in SMM mode\n");
		return heci1_disable_using_sbi();
	} else if (!ENV_SMM && CONFIG(SOC_INTEL_COMMON_BLOCK_HECI1_DISABLE_USING_PMC_IPC)) {
		printk(BIOS_INFO, "Disabling Heci using PMC IPC\n");
		return heci1_disable_using_pmc();
	} else if (!ENV_SMM && CONFIG(SOC_INTEL_COMMON_BLOCK_HECI1_DISABLE_USING_PCR)) {
		printk(BIOS_INFO, "Disabling Heci using PCR\n");
		return heci1_disable_using_pcr();
	} else {
		printk(BIOS_ERR, "%s Error: Unable to make HECI1 function disable!\n",
				__func__);
	}
}
