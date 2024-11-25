/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootstate.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/pci_devs.h>
#include <soc/pcu.h>
#include <soc/ramstage.h>
#include <soc/util.h>
#include <timer.h>

/* return true if command timed out else false */
static bool wait_for_bios_cmd_cpl(struct device *pcu1, uint32_t reg, uint32_t mask,
				  uint32_t target)
{
	const uint32_t max_delay = 5000; /* 5 seconds max */
	const uint32_t step_delay = 50; /* 50 us */
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, max_delay);
	while ((pci_read_config32(pcu1, reg) & mask) != target) {
		udelay(step_delay);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "%s timed out for dev: %s, reg: 0x%x, "
			       "mask: 0x%x, target: 0x%x\n",
			       __func__, dev_path(pcu1), reg, mask, target);
			return true; /* timedout */
		}
	}
	return false; /* successful */
}

/* return true if command timed out else false */
static bool write_bios_mailbox_cmd(struct device *pcu1, uint32_t command, uint32_t data)
{
	/* verify bios is not in busy state */
	if (wait_for_bios_cmd_cpl(pcu1, PCU_CR1_BIOS_MB_INTERFACE_REG, BIOS_MB_RUN_BUSY_MASK, 0))
		return true; /* timed out */

	/* write data to data register */
	printk(BIOS_SPEW, "%s - pci_write_config32 reg: 0x%x, data: 0x%x\n", __func__,
	       PCU_CR1_BIOS_MB_DATA_REG, data);

	pci_write_config32(pcu1, PCU_CR1_BIOS_MB_DATA_REG, data);

	/* write the command */
	printk(BIOS_SPEW, "%s - pci_write_config32 reg: 0x%x, data: 0x%lx\n", __func__,
	       PCU_CR1_BIOS_MB_INTERFACE_REG, command | BIOS_MB_RUN_BUSY_MASK);

	pci_write_config32(pcu1, PCU_CR1_BIOS_MB_INTERFACE_REG,
			   command | BIOS_MB_RUN_BUSY_MASK);

	/* wait for completion or time out*/
	return wait_for_bios_cmd_cpl(pcu1, PCU_CR1_BIOS_MB_INTERFACE_REG,
				     BIOS_MB_RUN_BUSY_MASK, 0);
}

/* return true if command timed out else false */
static bool set_bios_reset_cpl_for_package(struct device *pcu1,
					   uint32_t rst_cpl_mask,
					   uint32_t pcode_init_mask,
					   uint32_t val)
{
	/* update BIOS RESET completion bit */
	pci_update_config32(pcu1, PCU_CR1_BIOS_RESET_CPL_REG, ~rst_cpl_mask, val);

	/* wait for PCU ack */
	return wait_for_bios_cmd_cpl(pcu1, PCU_CR1_BIOS_RESET_CPL_REG,
				     pcode_init_mask, pcode_init_mask);
}

static void set_bios_init_completion_for_package(uint32_t socket)
{
	struct device *pcu1 = dev_find_device_on_socket(socket, PCI_VID_INTEL, PCU_CR1_DEVID);
	bool timedout;

	if (!pcu1)
		die("Failed to locate PCU PCI device\n");

	/* update RST_CPL3, PCODE_INIT_DONE3 */
	timedout = set_bios_reset_cpl_for_package(pcu1, RST_CPL3_MASK,
		PCODE_INIT_DONE3_MASK, RST_CPL3_MASK);
	if (timedout)
		die("BIOS RESET CPL3 timed out.\n");

	/* update RST_CPL4, PCODE_INIT_DONE4 */
	timedout = set_bios_reset_cpl_for_package(pcu1, RST_CPL4_MASK,
		PCODE_INIT_DONE4_MASK, RST_CPL4_MASK);
	if (timedout)
		die("BIOS RESET CPL4 timed out.\n");
}

static void set_bios_init_completion(void *unused)
{
	uint32_t sbsp_socket_id = 0;

	/*
	 * According to the BIOS Writer's Guide, the SBSP must be the last socket
	 * to receive the BIOS init completion message. So, we send it to all non-SBSP
	 * sockets first.
	 */
	for (uint32_t socket = 0; socket < CONFIG_MAX_SOCKET; ++socket) {
		if (!soc_cpu_is_enabled(socket))
			continue;
		if (socket == sbsp_socket_id)
			continue;
		set_bios_init_completion_for_package(socket);
	}

	/* And finally, take care of the SBSP */
	set_bios_init_completion_for_package(sbsp_socket_id);
}

/* Set BIOS INIT completion after locking registers */
BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY, set_bios_init_completion, NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, set_bios_init_completion, NULL);

static void pcu1_init(struct device *dev)
{
	u32 data;
	bool timedout;

	printk(BIOS_INFO, "%s: init registers\n", dev_path(dev));

	/* Make sure to issue BIOS_CMD_WRITE_PCU_MISC_CFG at least once */
	timedout = write_bios_mailbox_cmd(dev, BIOS_CMD_READ_PCU_MISC_CFG, 0);
	if (timedout) {
		/* 2nd try */
		timedout = write_bios_mailbox_cmd(dev, BIOS_CMD_READ_PCU_MISC_CFG, 0);
		if (timedout)
			die("BIOS PCU Misc Config Read timed out.\n");
	}

	data = pci_read_config32(dev, PCU_CR1_BIOS_MB_DATA_REG);
	printk(BIOS_SPEW, "%s - pci_read_config32 reg: 0x%x, data: 0x%x\n",
			__func__, PCU_CR1_BIOS_MB_DATA_REG, data);

	timedout = write_bios_mailbox_cmd(dev, BIOS_CMD_WRITE_PCU_MISC_CFG, data);
	if (timedout)
		die("BIOS PCU Misc Config Write timed out.\n");

	if (CONFIG(SOC_INTEL_SKYLAKE_SP)) {
		/* clear bits 27:31 - FSP sets this with 0x7 which needs to be cleared */
		pci_and_config32(dev, PCU_CR1_SAPMCTL, 0xfffffff);
	}
}

static void pcu1_final(struct device *dev)
{
	printk(BIOS_INFO, "%s: locking registers\n", dev_path(dev));
	if (CONFIG(SOC_INTEL_SKYLAKE_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP)) {
		pci_or_config32(dev, PCU_CR1_SAPMCTL, SAPMCTL_LOCK_MASK);
	}
	pci_or_config32(dev, PCU_CR1_DESIRED_CORES_CFG2_REG,
			PCU_CR1_DESIRED_CORES_CFG2_REG_LOCK_MASK);
}

static struct device_operations pcu1_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pcu1_init,
	.final            = pcu1_final,
};

static const struct pci_driver pcu1_driver __pci_driver = {
	.ops = &pcu1_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCU_CR1_DEVID,
};
