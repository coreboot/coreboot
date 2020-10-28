/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/sort.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci.h>
#include <hob_iiouds.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <timer.h>


/*
 *     +-------------------------+  TOLM
 *     | System Management Mode  |
 *     |      code and data      |
 *     |         (TSEG)          |
 *     +-------------------------+  SMM base (aligned)
 *     |                         |
 *     | Chipset Reserved Memory |
 *     |                         |
 *     +-------------------------+  top_of_ram (aligned)
 *     |                         |
 *     |       CBMEM Root        |
 *     |                         |
 *     +-------------------------+
 *     |                         |
 *     |   FSP Reserved Memory   |
 *     |                         |
 *     +-------------------------+
 *     |                         |
 *     |  Various CBMEM Entries  |
 *     |                         |
 *     +-------------------------+  top_of_stack (8 byte aligned)
 *     |                         |
 *     |   stack (CBMEM Entry)   |
 *     |                         |
 *     +-------------------------+
 */

static uint32_t get_socket_stack_busno(uint32_t socket, uint32_t stack)
{
	size_t hob_size;
	const IIO_UDS *hob;
	const uint8_t fsp_hob_iio_universal_data_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;

	assert(socket < MAX_SOCKET && stack < MAX_IIO_STACK);

	hob = fsp_find_extension_hob_by_guid(fsp_hob_iio_universal_data_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);

	return hob->PlatformData.CpuQpiInfo[socket].StackBus[stack];
}

/* return 1 if command timed out else 0 */
static uint32_t wait_for_bios_cmd_cpl(pci_devfn_t dev, uint32_t reg, uint32_t mask,
	uint32_t target)
{
	uint32_t max_delay = 5000; /* 5 seconds max */
	uint32_t step_delay = 50; /* 50 us */
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, max_delay);
	while ((pci_mmio_read_config32(dev, reg) & mask) != target) {
		udelay(step_delay);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "%s timed out for dev: 0x%x, reg: 0x%x, "
				"mask: 0x%x, target: 0x%x\n", __func__, dev, reg, mask, target);
			return 1; /* timedout */
		}
	}
	return 0; /* successful */
}

/* return 1 if command timed out else 0 */
static int set_bios_reset_cpl_for_package(uint32_t socket, uint32_t rst_cpl_mask,
	uint32_t pcode_init_mask, uint32_t val)
{
	uint32_t bus = get_socket_stack_busno(socket, PCU_IIO_STACK);
	pci_devfn_t dev = PCI_DEV(bus, PCU_DEV, PCU_CR1_FUN);

	uint32_t reg = pci_mmio_read_config32(dev, PCU_CR1_BIOS_RESET_CPL_REG);
	reg &= (uint32_t) ~rst_cpl_mask;
	reg |= rst_cpl_mask;
	reg |= val;

	/* update BIOS RESET completion bit */
	pci_mmio_write_config32(dev, PCU_CR1_BIOS_RESET_CPL_REG, reg);

	/* wait for PCU ack */
	return wait_for_bios_cmd_cpl(dev, PCU_CR1_BIOS_RESET_CPL_REG, pcode_init_mask,
		pcode_init_mask);
}

/* return 1 if command timed out else 0 */
static uint32_t write_bios_mailbox_cmd(pci_devfn_t dev, uint32_t command, uint32_t data)
{
	/* verify bios is not in busy state */
	if (wait_for_bios_cmd_cpl(dev, PCU_CR1_BIOS_MB_INTERFACE_REG, BIOS_MB_RUN_BUSY_MASK, 0))
		return 1; /* timed out */

	/* write data to data register */
	printk(BIOS_SPEW, "%s - pci_mmio_write_config32 reg: 0x%x, data: 0x%x\n", __func__,
				 PCU_CR1_BIOS_MB_DATA_REG, data);
	pci_mmio_write_config32(dev, PCU_CR1_BIOS_MB_DATA_REG, data);

	/* write the command */
	printk(BIOS_SPEW, "%s - pci_mmio_write_config32 reg: 0x%x, data: 0x%x\n", __func__,
		PCU_CR1_BIOS_MB_INTERFACE_REG,
		(uint32_t) (command | BIOS_MB_RUN_BUSY_MASK));
	pci_mmio_write_config32(dev, PCU_CR1_BIOS_MB_INTERFACE_REG,
		(uint32_t) (command | BIOS_MB_RUN_BUSY_MASK));

	/* wait for completion or time out*/
	return wait_for_bios_cmd_cpl(dev, PCU_CR1_BIOS_MB_INTERFACE_REG,
		BIOS_MB_RUN_BUSY_MASK, 0);
}

void config_reset_cpl3_csrs(void)
{
	uint32_t data, plat_info, max_min_turbo_limit_ratio;

	for (uint32_t socket = 0; socket < MAX_SOCKET; ++socket) {
		uint32_t bus = get_socket_stack_busno(socket, PCU_IIO_STACK);

		/* configure PCU_CR0_FUN csrs */
		pci_devfn_t cr0_dev = PCI_DEV(bus, PCU_DEV, PCU_CR0_FUN);
		data = pci_mmio_read_config32(cr0_dev, PCU_CR0_P_STATE_LIMITS);
		data |= P_STATE_LIMITS_LOCK;
		pci_mmio_write_config32(cr0_dev, PCU_CR0_P_STATE_LIMITS, data);

		plat_info = pci_mmio_read_config32(cr0_dev, PCU_CR0_PLATFORM_INFO);
		dump_csr64("", cr0_dev, PCU_CR0_PLATFORM_INFO);
		max_min_turbo_limit_ratio =
			(plat_info & MAX_NON_TURBO_LIM_RATIO_MASK) >>
				MAX_NON_TURBO_LIM_RATIO_SHIFT;
		printk(BIOS_SPEW, "plat_info: 0x%x, max_min_turbo_limit_ratio: 0x%x\n",
			plat_info, max_min_turbo_limit_ratio);

		/* configure PCU_CR1_FUN csrs */
		pci_devfn_t cr1_dev = PCI_DEV(bus, PCU_DEV, PCU_CR1_FUN);

		data = pci_mmio_read_config32(cr1_dev, PCU_CR1_SAPMCTL);
		/* clear bits 27:31 - FSP sets this with 0x7 which needs to be cleared */
		data &= 0x0fffffff;
		data |= SAPMCTL_LOCK_MASK;
		pci_mmio_write_config32(cr1_dev, PCU_CR1_SAPMCTL, data);

		/* configure PCU_CR1_FUN csrs */
		pci_devfn_t cr2_dev = PCI_DEV(bus, PCU_DEV, PCU_CR2_FUN);

		data = PCIE_IN_PKGCSTATE_L1_MASK;
		pci_mmio_write_config32(cr2_dev, PCU_CR2_PKG_CST_ENTRY_CRITERIA_MASK, data);

		data = KTI_IN_PKGCSTATE_L1_MASK;
		pci_mmio_write_config32(cr2_dev, PCU_CR2_PKG_CST_ENTRY_CRITERIA_MASK2, data);

		data = PROCHOT_RATIO;
		printk(BIOS_SPEW, "PCU_CR2_PROCHOT_RESPONSE_RATIO_REG data: 0x%x\n", data);
		pci_mmio_write_config32(cr2_dev, PCU_CR2_PROCHOT_RESPONSE_RATIO_REG, data);
		dump_csr("", cr2_dev, PCU_CR2_PROCHOT_RESPONSE_RATIO_REG);

		data = pci_mmio_read_config32(cr2_dev, PCU_CR2_DYNAMIC_PERF_POWER_CTL);
		data |= UNOCRE_PLIMIT_OVERRIDE_SHIFT;
		pci_mmio_write_config32(cr2_dev, PCU_CR2_DYNAMIC_PERF_POWER_CTL, data);
	}
}

static void set_bios_init_completion_for_package(uint32_t socket)
{
	uint32_t data;
	uint32_t timedout;
	uint32_t bus = get_socket_stack_busno(socket, PCU_IIO_STACK);
	pci_devfn_t dev = PCI_DEV(bus, PCU_DEV, PCU_CR1_FUN);

	/* read pcu config */
	timedout = write_bios_mailbox_cmd(dev, BIOS_CMD_READ_PCU_MISC_CFG, 0);
	if (timedout) {
		/* 2nd try */
		timedout = write_bios_mailbox_cmd(dev, BIOS_CMD_READ_PCU_MISC_CFG, 0);
		if (timedout)
			die("BIOS PCU Misc Config Read timed out.\n");

		data = pci_mmio_read_config32(dev, PCU_CR1_BIOS_MB_DATA_REG);
		printk(BIOS_SPEW, "%s - pci_mmio_read_config32 reg: 0x%x, data: 0x%x\n",
			__func__, PCU_CR1_BIOS_MB_DATA_REG, data);

		/* write PCU config */
		timedout = write_bios_mailbox_cmd(dev, BIOS_CMD_WRITE_PCU_MISC_CFG, data);
		if (timedout)
			die("BIOS PCU Misc Config Write timed out.\n");
	}

	/* update RST_CPL3, PCODE_INIT_DONE3 */
	timedout = set_bios_reset_cpl_for_package(socket, RST_CPL3_MASK,
		PCODE_INIT_DONE3_MASK, RST_CPL3_MASK);
	if (timedout)
		die("BIOS RESET CPL3 timed out.\n");

	/* update RST_CPL4, PCODE_INIT_DONE4 */
	timedout = set_bios_reset_cpl_for_package(socket, RST_CPL4_MASK,
		PCODE_INIT_DONE4_MASK, RST_CPL4_MASK);
	if (timedout)
		die("BIOS RESET CPL4 timed out.\n");
	/* set CSR_DESIRED_CORES_CFG2 lock bit */
	data = pci_mmio_read_config32(dev, PCU_CR1_DESIRED_CORES_CFG2_REG);
	data |= PCU_CR1_DESIRED_CORES_CFG2_REG_LOCK_MASK;
	printk(BIOS_SPEW, "%s - pci_mmio_write_config32 PCU_CR1_DESIRED_CORES_CFG2_REG 0x%x, data: 0x%x\n",
		__func__, PCU_CR1_DESIRED_CORES_CFG2_REG, data);
	pci_mmio_write_config32(dev, PCU_CR1_DESIRED_CORES_CFG2_REG, data);
}

void set_bios_init_completion(void)
{
	uint32_t sbsp_socket_id = 0; /* TODO - this needs to be configurable */

	for (uint32_t socket = 0; socket < MAX_SOCKET; ++socket) {
		if (socket == sbsp_socket_id)
			continue;
		set_bios_init_completion_for_package(socket);
	}
	set_bios_init_completion_for_package(sbsp_socket_id);
}

void get_core_thread_bits(uint32_t *core_bits, uint32_t *thread_bits)
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

void get_cpu_info_from_apicid(uint32_t apicid, uint32_t core_bits, uint32_t thread_bits,
	uint8_t *package, uint8_t *core, uint8_t *thread)
{
	if (package != NULL)
		*package = (apicid >> (thread_bits + core_bits));
	if (core != NULL)
		*core = (uint32_t)((apicid >> thread_bits) & ~((~0) << core_bits));
	if (thread != NULL)
		*thread = (uint32_t)(apicid & ~((~0) << thread_bits));
}

unsigned int soc_get_num_cpus(void)
{
	size_t hob_size;
	const uint8_t fsp_hob_iio_universal_data_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;
	const IIO_UDS *hob;

	/* these fields are incorrect - need debugging */
	hob = fsp_find_extension_hob_by_guid(fsp_hob_iio_universal_data_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);
	return hob->SystemStatus.numCpus;
}

int get_threads_per_package(void)
{
	unsigned int core_count, thread_count;
	cpu_read_topology(&core_count, &thread_count);
	return thread_count;
}

int get_platform_thread_count(void)
{
	return soc_get_num_cpus() * get_threads_per_package();
}

uint8_t get_iiostack_info(struct iiostack_resource *info)
{
	size_t hob_size;
	const uint8_t fsp_hob_iio_universal_data_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;
	const IIO_UDS *hob;

	hob = fsp_find_extension_hob_by_guid(
		fsp_hob_iio_universal_data_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);

	// copy IIO Stack info from FSP HOB
	info->no_of_stacks = 0;
	for (int s = 0; s < hob->PlatformData.numofIIO; ++s) {
		for (int x = 0; x < MAX_IIO_STACK; ++x) {
			const STACK_RES *ri = &hob->PlatformData.IIO_resource[s].StackRes[x];
			// TODO: do we have situation with only bux 0 and one stack?
			if (ri->BusBase >= ri->BusLimit)
				continue;
			assert(info->no_of_stacks < (CONFIG_MAX_SOCKET * MAX_IIO_STACK));
			memcpy(&info->res[info->no_of_stacks++], ri, sizeof(STACK_RES));
		}
	}

	return hob->PlatformData.Pci64BitResourceAllocation;
}

#if ENV_RAMSTAGE
const struct SystemMemoryMapHob *get_system_memory_map(void)
{
	size_t hob_size;
	const uint8_t mem_hob_guid[16] = FSP_SYSTEM_MEMORYMAP_HOB_GUID;
	const struct SystemMemoryMapHob *memmap_addr;

	memmap_addr = fsp_find_extension_hob_by_guid(mem_hob_guid, &hob_size);
	assert(memmap_addr != NULL && hob_size != 0);

	return memmap_addr;
}

void xeonsp_init_cpu_config(void)
{
	struct device *dev;
	int apic_ids[CONFIG_MAX_CPUS] = {0}, apic_ids_by_thread[CONFIG_MAX_CPUS] = {0};
	int  num_apics = 0;
	uint32_t core_bits, thread_bits;
	unsigned int core_count, thread_count;
	unsigned int num_cpus;

	/* sort APIC ids in asending order to identify apicid ranges for
     each numa domain
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

	num_cpus = soc_get_num_cpus();
	cpu_read_topology(&core_count, &thread_count);
	assert(num_apics == (num_cpus * thread_count));

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

#endif
