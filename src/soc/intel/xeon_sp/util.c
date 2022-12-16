/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/sort.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/cpulib.h>
#include <soc/pci_devs.h>
#include <soc/msr.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <timer.h>

uint8_t get_stack_busno(const uint8_t stack)
{
	if (stack >= MAX_IIO_STACK) {
		printk(BIOS_ERR, "%s: Stack %u does not exist!\n", __func__, stack);
		return 0;
	}
	const pci_devfn_t dev = PCI_DEV(UBOX_DECS_BUS, UBOX_DECS_DEV, UBOX_DECS_FUNC);
	const uint16_t offset = stack / 4 ? UBOX_DECS_CPUBUSNO1_CSR : UBOX_DECS_CPUBUSNO_CSR;
	return pci_io_read_config32(dev, offset) >> (8 * (stack % 4)) & 0xff;
}

void unlock_pam_regions(void)
{
	uint32_t pam0123_unlock_dram = 0x33333330;
	uint32_t pam456_unlock_dram = 0x00333333;
	uint32_t bus1 = get_stack_busno(1);

	pci_io_write_config32(PCI_DEV(bus1, SAD_ALL_DEV, SAD_ALL_FUNC),
		SAD_ALL_PAM0123_CSR, pam0123_unlock_dram);
	pci_io_write_config32(PCI_DEV(bus1, SAD_ALL_DEV, SAD_ALL_FUNC),
		SAD_ALL_PAM456_CSR, pam456_unlock_dram);

	uint32_t reg1 = pci_io_read_config32(PCI_DEV(bus1, SAD_ALL_DEV,
		SAD_ALL_FUNC), SAD_ALL_PAM0123_CSR);
	uint32_t reg2 = pci_io_read_config32(PCI_DEV(bus1, SAD_ALL_DEV,
		SAD_ALL_FUNC), SAD_ALL_PAM456_CSR);
	printk(BIOS_DEBUG, "%s:%s pam0123_csr: 0x%x, pam456_csr: 0x%x\n",
		__FILE__, __func__, reg1, reg2);
}

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

void get_iiostack_info(struct iiostack_resource *info)
{
	const IIO_UDS *hob = get_iio_uds();

	// copy IIO Stack info from FSP HOB
	info->no_of_stacks = 0;
	for (int s = 0; s < hob->PlatformData.numofIIO; ++s) {
		for (int x = 0; x < MAX_IIO_STACK; ++x) {
			const STACK_RES *ri = &hob->PlatformData.IIO_resource[s].StackRes[x];
			if (!is_iio_stack_res(ri))
				continue;
			assert(info->no_of_stacks < (CONFIG_MAX_SOCKET * MAX_IIO_STACK));
			memcpy(&info->res[info->no_of_stacks++], ri, sizeof(STACK_RES));
		}
	}
}

unsigned int soc_get_num_cpus(void)
{
	/* The FSP IIO UDS HOB has field numCpus, it is actually socket count */
	return get_iio_uds()->SystemStatus.numCpus;
}

#if ENV_RAMSTAGE /* Setting devtree variables is only allowed in ramstage. */
static void get_core_thread_bits(uint32_t *core_bits, uint32_t *thread_bits)
{
	register int ecx;
	struct cpuid_result cpuid_regs;

	/* get max index of CPUID */
	cpuid_regs = cpuid(0);
	assert(cpuid_regs.eax >= 0xb); /* cpuid_regs.eax is max input value for cpuid */

	*thread_bits = *core_bits = 0;
	ecx = 0;
	while (1) {
		cpuid_regs = cpuid_ext(0xb, ecx);
		if (ecx == 0) {
			*thread_bits = (cpuid_regs.eax & 0x1f);
		} else {
			*core_bits = (cpuid_regs.eax & 0x1f) - *thread_bits;
			break;
		}
		ecx++;
	}
}

static void get_cpu_info_from_apicid(uint32_t apicid, uint32_t core_bits, uint32_t thread_bits,
	uint8_t *package, uint8_t *core, uint8_t *thread)
{
	if (package)
		*package = (apicid >> (thread_bits + core_bits));
	if (core)
		*core = (uint32_t)((apicid >> thread_bits) & ~((~0) << core_bits));
	if (thread)
		*thread = (uint32_t)(apicid & ~((~0) << thread_bits));
}

void xeonsp_init_cpu_config(void)
{
	struct device *dev;
	int apic_ids[CONFIG_MAX_CPUS] = {0}, apic_ids_by_thread[CONFIG_MAX_CPUS] = {0};
	int  num_apics = 0;
	uint32_t core_bits, thread_bits;
	unsigned int core_count, thread_count;
	unsigned int num_sockets;

	/*
	 * sort APIC ids in ascending order to identify apicid ranges for
	 * each numa domain
	 */
	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type != DEVICE_PATH_APIC) ||
			(dev->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!dev->enabled)
			continue;
		if (num_apics >= ARRAY_SIZE(apic_ids))
			break;
	  apic_ids[num_apics++] = dev->path.apic.apic_id;
	}
	if (num_apics > 1)
		bubblesort(apic_ids, num_apics, NUM_ASCENDING);

	num_sockets = soc_get_num_cpus();
	cpu_read_topology(&core_count, &thread_count);
	assert(num_apics == (num_sockets * thread_count));

	/* sort them by thread i.e., all cores with thread 0 and then thread 1 */
	int index = 0;
	for (int id = 0; id < num_apics; ++id) {
		int apic_id = apic_ids[id];
		if (apic_id & 0x1) { /* 2nd thread */
			apic_ids_by_thread[index + (num_apics/2) - 1] = apic_id;
		} else { /* 1st thread */
			apic_ids_by_thread[index++] = apic_id;
		}
	}

	/* update apic_id, node_id in sorted order */
	num_apics = 0;
	get_core_thread_bits(&core_bits, &thread_bits);
	for (dev = all_devices; dev; dev = dev->next) {
		uint8_t package;

		if ((dev->path.type != DEVICE_PATH_APIC) ||
			(dev->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!dev->enabled)
			continue;
		if (num_apics >= ARRAY_SIZE(apic_ids))
			break;
		dev->path.apic.apic_id = apic_ids_by_thread[num_apics];
		get_cpu_info_from_apicid(dev->path.apic.apic_id, core_bits, thread_bits,
			&package, NULL, NULL);
		dev->path.apic.node_id = package;
		printk(BIOS_DEBUG, "CPU %d apic_id: 0x%x (%d), node_id: 0x%x\n",
			num_apics, dev->path.apic.apic_id,
			dev->path.apic.apic_id, dev->path.apic.node_id);

		++num_apics;
	}
}

/* return true if command timed out else false */
static bool wait_for_bios_cmd_cpl(pci_devfn_t dev, uint32_t reg, uint32_t mask,
	uint32_t target)
{
	const uint32_t max_delay = 5000; /* 5 seconds max */
	const uint32_t step_delay = 50; /* 50 us */
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, max_delay);
	while ((pci_s_read_config32(dev, reg) & mask) != target) {
		udelay(step_delay);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "%s timed out for dev: %x, reg: 0x%x, "
				"mask: 0x%x, target: 0x%x\n", __func__, dev, reg, mask, target);
			return true; /* timedout */
		}
	}
	return false; /* successful */
}

/* return true if command timed out else false */
static bool write_bios_mailbox_cmd(pci_devfn_t dev, uint32_t command, uint32_t data)
{
	/* verify bios is not in busy state */
	if (wait_for_bios_cmd_cpl(dev, PCU_CR1_BIOS_MB_INTERFACE_REG, BIOS_MB_RUN_BUSY_MASK, 0))
		return true; /* timed out */

	/* write data to data register */
	printk(BIOS_SPEW, "%s - pci_s_write_config32 reg: 0x%x, data: 0x%x\n", __func__,
		PCU_CR1_BIOS_MB_DATA_REG, data);
	pci_s_write_config32(dev, PCU_CR1_BIOS_MB_DATA_REG, data);

	/* write the command */
	printk(BIOS_SPEW, "%s - pci_s_write_config32 reg: 0x%x, data: 0x%lx\n", __func__,
		PCU_CR1_BIOS_MB_INTERFACE_REG, command | BIOS_MB_RUN_BUSY_MASK);
	pci_s_write_config32(dev, PCU_CR1_BIOS_MB_INTERFACE_REG,
		command | BIOS_MB_RUN_BUSY_MASK);

	/* wait for completion or time out*/
	return wait_for_bios_cmd_cpl(dev, PCU_CR1_BIOS_MB_INTERFACE_REG,
		BIOS_MB_RUN_BUSY_MASK, 0);
}

/* return true if command timed out else false */
static bool set_bios_reset_cpl_for_package(uint32_t socket, uint32_t rst_cpl_mask,
	uint32_t pcode_init_mask, uint32_t val)
{
	const uint32_t bus = get_socket_stack_busno(socket, PCU_IIO_STACK);
	const pci_devfn_t dev = PCI_DEV(bus, PCU_DEV, PCU_CR1_FUN);

	uint32_t reg = pci_s_read_config32(dev, PCU_CR1_BIOS_RESET_CPL_REG);
	reg &= (uint32_t)~rst_cpl_mask;
	reg |= val;

	/* update BIOS RESET completion bit */
	pci_s_write_config32(dev, PCU_CR1_BIOS_RESET_CPL_REG, reg);

	/* wait for PCU ack */
	return wait_for_bios_cmd_cpl(dev, PCU_CR1_BIOS_RESET_CPL_REG, pcode_init_mask,
		pcode_init_mask);
}

static void set_bios_init_completion_for_package(uint32_t socket)
{
	uint32_t data;
	bool timedout;
	const uint32_t bus = get_socket_stack_busno(socket, PCU_IIO_STACK);
	const pci_devfn_t dev = PCI_DEV(bus, PCU_DEV, PCU_CR1_FUN);

	/* read PCU config */
	timedout = write_bios_mailbox_cmd(dev, BIOS_CMD_READ_PCU_MISC_CFG, 0);
	if (timedout) {
		/* 2nd try */
		timedout = write_bios_mailbox_cmd(dev, BIOS_CMD_READ_PCU_MISC_CFG, 0);
		if (timedout)
			die("BIOS PCU Misc Config Read timed out.\n");

		/* Since the 1st try failed, we need to make sure PCU is in stable state */
		data = pci_s_read_config32(dev, PCU_CR1_BIOS_MB_DATA_REG);
		printk(BIOS_SPEW, "%s - pci_s_read_config32 reg: 0x%x, data: 0x%x\n",
			__func__, PCU_CR1_BIOS_MB_DATA_REG, data);
		timedout = write_bios_mailbox_cmd(dev, BIOS_CMD_WRITE_PCU_MISC_CFG, data);
		if (timedout)
			die("BIOS PCU Misc Config Write timed out.\n");
	}

	/* update RST_CPL3, PCODE_INIT_DONE3 */
	timedout = set_bios_reset_cpl_for_package(socket, RST_CPL3_MASK,
		PCODE_INIT_DONE3_MASK, RST_CPL3_MASK);
	if (timedout)
		die("BIOS RESET CPL3 timed out.\n");

	/* Set PMAX_LOCK - must be set before RESET CPL4 */
	pci_or_config32(PCU_DEV_CR0(bus), PCU_CR0_PMAX, PMAX_LOCK);

	/* update RST_CPL4, PCODE_INIT_DONE4 */
	timedout = set_bios_reset_cpl_for_package(socket, RST_CPL4_MASK,
		PCODE_INIT_DONE4_MASK, RST_CPL4_MASK);
	if (timedout)
		die("BIOS RESET CPL4 timed out.\n");

	/* set CSR_DESIRED_CORES_CFG2 lock bit */
	data = pci_s_read_config32(dev, PCU_CR1_DESIRED_CORES_CFG2_REG);
	data |= PCU_CR1_DESIRED_CORES_CFG2_REG_LOCK_MASK;
	printk(BIOS_SPEW, "%s - pci_s_write_config32 PCU_CR1_DESIRED_CORES_CFG2_REG 0x%x, data: 0x%x\n",
		__func__, PCU_CR1_DESIRED_CORES_CFG2_REG, data);
	pci_s_write_config32(dev, PCU_CR1_DESIRED_CORES_CFG2_REG, data);
}

void set_bios_init_completion(void)
{
	/* FIXME: This may need to be changed for multi-socket platforms */
	uint32_t sbsp_socket_id = 0;

	/*
	 * According to the BIOS Writer's Guide, the SBSP must be the last socket
	 * to receive the BIOS init completion message. So, we send it to all non-SBSP
	 * sockets first.
	 */
	for (uint32_t socket = 0; socket < soc_get_num_cpus(); ++socket) {
		if (socket == sbsp_socket_id)
			continue;
		set_bios_init_completion_for_package(socket);
	}

	/* And finally, take care of the SBSP */
	set_bios_init_completion_for_package(sbsp_socket_id);
}
#endif
