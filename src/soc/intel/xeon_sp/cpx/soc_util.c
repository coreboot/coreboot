/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/sort.h>
#include <device/device.h>
#include <intelblocks/cpulib.h>
#include <soc/cpu.h>
#include <soc/soc_util.h>
#include <stdlib.h>
#include <string.h>

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

const IIO_UDS *get_iio_uds(void)
{
	size_t hob_size;
	const IIO_UDS *hob;
	const uint8_t fsp_hob_iio_universal_data_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;

	hob = fsp_find_extension_hob_by_guid(fsp_hob_iio_universal_data_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);
	return hob;
}

unsigned int xeon_sp_get_cpu_count(void)
{
	return get_iio_uds()->SystemStatus.numCpus;
}

void xeonsp_init_cpu_config(void)
{
	struct device *dev;
	int apic_ids[CONFIG_MAX_CPUS] = {0}, apic_ids_by_thread[CONFIG_MAX_CPUS] = {0};
	int  num_apics = 0;
	uint32_t core_bits, thread_bits;
	unsigned int core_count, thread_count;
	unsigned int num_cpus;

	/*
	 * sort APIC ids in asending order to identify apicid ranges for
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

	/* Here num_cpus is the number of processors */
	/* The FSP HOB parameter has it named as num_cpus */
	num_cpus = xeon_sp_get_cpu_count();
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
