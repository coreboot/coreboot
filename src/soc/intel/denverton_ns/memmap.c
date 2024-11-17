/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/romstage.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>

/* Returns base of requested region encoded in the system agent. */
static inline uintptr_t system_agent_region_base(size_t reg)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = SA_DEV_ROOT;
#else
	struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
#endif
	/* All regions concerned for have 1 MiB alignment. */
	return ALIGN_DOWN(pci_read_config32(dev, reg), 1 * MiB);
}

static inline uintptr_t smm_region_start(void)
{
	return system_agent_region_base(TSEGMB);
}

static inline size_t smm_region_size(void)
{
	return system_agent_region_base(TOLUD) - smm_region_start();
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = smm_region_start();
	*size = smm_region_size();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	/*
	 * We need to make sure ramstage will be run cached. At this point exact
	 * location of ramstage in cbmem is not known. Instruct postcar to cache
	 * 16 megs under cbmem top which is a safe bet to cover ramstage.
	 */
	const uintptr_t top_of_ram = cbmem_top();
	postcar_frame_add_mtrr(pcf, top_of_ram - 16 * MiB, 16 * MiB,
			       MTRR_TYPE_WRBACK);

	/* Cache the TSEG region */
	postcar_enable_tseg_cache(pcf);
}
