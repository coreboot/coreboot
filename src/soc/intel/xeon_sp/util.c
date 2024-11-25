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
