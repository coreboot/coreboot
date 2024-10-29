/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/sort.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/cfg.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/p2sb.h>
#include <intelpch/lockdown.h>
#include <soc/chip_common.h>
#include <soc/pch_pci_devs.h>
#include <soc/pci_devs.h>
#include <soc/msr.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <timer.h>

msr_t read_msr_ppin(void)
{
	msr_t ppin = {0};
	msr_t msr;

	/* If MSR_PLATFORM_INFO PPIN_CAP is 0, PPIN capability is not supported */
	msr = rdmsr(MSR_PLATFORM_INFO);
	if ((msr.lo & MSR_PPIN_CAP) == 0) {
		printk(BIOS_ERR, "MSR_PPIN_CAP is 0, PPIN is not supported\n");
		return ppin;
	}

	/* Access to MSR_PPIN is permitted only if MSR_PPIN_CTL LOCK is 0 and ENABLE is 1 */
	msr = rdmsr(MSR_PPIN_CTL);
	if (msr.lo & MSR_PPIN_CTL_LOCK) {
		printk(BIOS_ERR, "MSR_PPIN_CTL_LOCK is 1, PPIN access is not allowed\n");
		return ppin;
	}

	if ((msr.lo & MSR_PPIN_CTL_ENABLE) == 0) {
		/* Set MSR_PPIN_CTL ENABLE to 1 */
		msr.lo |= MSR_PPIN_CTL_ENABLE;
		wrmsr(MSR_PPIN_CTL, msr);
	}
	ppin = rdmsr(MSR_PPIN);
	return ppin;
}

static unsigned int get_threads_per_package(void)
{
	unsigned int core_count, thread_count;
	cpu_read_topology(&core_count, &thread_count);
	return thread_count;
}

int get_platform_thread_count(void)
{
	return soc_get_num_cpus() * get_threads_per_package();
}

const IIO_UDS *get_iio_uds(void)
{
	size_t hob_size;
	static const IIO_UDS *hob;
	const uint8_t fsp_hob_iio_universal_data_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;

	if (hob)
		return hob;

	hob = fsp_find_extension_hob_by_guid(fsp_hob_iio_universal_data_guid, &hob_size);
	assert(hob && hob_size != 0);
	return hob;
}

/*
 * Returns true if the CPU in the specified socket was found
 * during QPI init, false otherwise.
 */
bool soc_cpu_is_enabled(const size_t idx)
{
	const IIO_UDS *hob = get_iio_uds();
	assert(idx < CONFIG_MAX_SOCKET);

	return hob->PlatformData.IIO_resource[idx].Valid;
}

unsigned int soc_get_num_cpus(void)
{
	return get_iio_uds()->SystemStatus.numCpus;
}

unsigned int smbios_soc_get_max_sockets(void)
{
	return soc_get_num_cpus();
}

union p2sb_bdf soc_get_hpet_bdf(void)
{
	if (CONFIG(SOC_INTEL_COMMON_IBL_BASE)) {
		union p2sb_bdf bdf = {
			.bus = HPET_BUS_NUM,
			.dev = HPET_DEV_NUM,
			.fn = HPET0_FUNC_NUM
		};
		return bdf;
	}
	return p2sb_get_hpet_bdf();
}

union p2sb_bdf soc_get_ioapic_bdf(void)
{
	if (CONFIG(SOC_INTEL_COMMON_IBL_BASE)) {
		union p2sb_bdf bdf = {
			.bus = PCH_IOAPIC_BUS_NUMBER,
			.dev = PCH_IOAPIC_DEV_NUM,
			.fn = PCH_IOAPIC_FUNC_NUM
		};
		return bdf;
	}
	return p2sb_get_ioapic_bdf();
}

#if ENV_RAMSTAGE /* Setting devtree variables is only allowed in ramstage. */

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
	struct device *pcu0 = dev_find_device_on_socket(socket, PCI_VID_INTEL, PCU_CR0_DEVID);
	struct device *pcu1 = dev_find_device_on_socket(socket, PCI_VID_INTEL, PCU_CR1_DEVID);
	uint32_t data;
	bool timedout;

	if (!pcu0 || !pcu1)
		die("Failed to locate PCU PCI device\n");

	/* read PCU config */
	timedout = write_bios_mailbox_cmd(pcu1, BIOS_CMD_READ_PCU_MISC_CFG, 0);
	if (timedout) {
		/* 2nd try */
		timedout = write_bios_mailbox_cmd(pcu1, BIOS_CMD_READ_PCU_MISC_CFG, 0);
		if (timedout)
			die("BIOS PCU Misc Config Read timed out.\n");

		/* Since the 1st try failed, we need to make sure PCU is in stable state */
		data = pci_read_config32(pcu1, PCU_CR1_BIOS_MB_DATA_REG);
		printk(BIOS_SPEW, "%s - pci_read_config32 reg: 0x%x, data: 0x%x\n",
			__func__, PCU_CR1_BIOS_MB_DATA_REG, data);
		timedout = write_bios_mailbox_cmd(pcu1, BIOS_CMD_WRITE_PCU_MISC_CFG, data);
		if (timedout)
			die("BIOS PCU Misc Config Write timed out.\n");
	}

	/* update RST_CPL3, PCODE_INIT_DONE3 */
	timedout = set_bios_reset_cpl_for_package(pcu1, RST_CPL3_MASK,
		PCODE_INIT_DONE3_MASK, RST_CPL3_MASK);
	if (timedout)
		die("BIOS RESET CPL3 timed out.\n");

	/* Set PMAX_LOCK - must be set before RESET CPL4 */
	data = pci_read_config32(pcu0, PCU_CR0_PMAX);
	data |= PMAX_LOCK;
	pci_write_config32(pcu0, PCU_CR0_PMAX, data);

	/* update RST_CPL4, PCODE_INIT_DONE4 */
	timedout = set_bios_reset_cpl_for_package(pcu1, RST_CPL4_MASK,
		PCODE_INIT_DONE4_MASK, RST_CPL4_MASK);
	if (timedout)
		die("BIOS RESET CPL4 timed out.\n");

	/* set CSR_DESIRED_CORES_CFG2 lock bit */
	data = pci_read_config32(pcu1, PCU_CR1_DESIRED_CORES_CFG2_REG);
	data |= PCU_CR1_DESIRED_CORES_CFG2_REG_LOCK_MASK;
	printk(BIOS_SPEW, "%s - pci_write_config32 PCU_CR1_DESIRED_CORES_CFG2_REG 0x%x, data: 0x%x\n",
		__func__, PCU_CR1_DESIRED_CORES_CFG2_REG, data);
	pci_write_config32(pcu1, PCU_CR1_DESIRED_CORES_CFG2_REG, data);
}

void set_bios_init_completion(void)
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
#endif
