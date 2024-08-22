/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <cpu/x86/smm.h>
#include <soc/soc_util.h>
#include <soc/pci_devs.h>
#include <soc/util.h>
#include <security/intel/txt/txt_platform.h>

void smm_region(uintptr_t *start, size_t *size)
{
	uintptr_t tseg_base = pci_read_config32(VTD_DEV(0), VTD_TSEG_BASE_CSR);
	uintptr_t tseg_limit = pci_read_config32(VTD_DEV(0), VTD_TSEG_LIMIT_CSR);

	tseg_base = ALIGN_DOWN(tseg_base, 1 * MiB);
	tseg_limit = ALIGN_DOWN(tseg_limit, 1 * MiB);
	/* Only the upper [31:20] bits of an address are checked against
	 * VTD_TSEG_LIMIT_CSR[31:20] which must be below or equal, so this
	 * effectively means +1MiB for the limit.
	 */
	tseg_limit += 1 * MiB;

	*start = tseg_base;
	*size = tseg_limit - tseg_base;
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	const uintptr_t top_of_ram = cbmem_top();
	uintptr_t cbmem_base;
	size_t cbmem_size;

	/* Try account for the CBMEM region currently used and for future use */
	if (cbmem_get_region((void **)&cbmem_base, &cbmem_size))
		die("Could not find cbmem region");
	printk(BIOS_DEBUG, "top_of_ram = 0x%lx\n", top_of_ram);
	printk(BIOS_DEBUG, "cbmem base_ptr: 0x%lx, size: 0x%zx\n", cbmem_base, cbmem_size);
	/* Assume 4MiB will be enough for future cbmem objects (FSP-S, ramstage, ...) */
	cbmem_base -= 4 * MiB;
	cbmem_base = ALIGN_DOWN(cbmem_base, 4 * MiB);

	/* Align the top to make sure we don't use too many MTRR's */
	cbmem_size = ALIGN_UP(top_of_ram - cbmem_base, 4 * MiB);

	postcar_frame_add_mtrr(pcf, cbmem_base, cbmem_size, MTRR_TYPE_WRBACK);
	/* Cache the TSEG region */
	if (CONFIG(TSEG_STAGE_CACHE))
		postcar_enable_tseg_cache(pcf);
}

#if !defined(__SIMPLE_DEVICE__)
union dpr_register txt_get_chipset_dpr(void)
{
	union dpr_register dpr;
	struct device *dev = VTD_DEV(0);

	dpr.raw = 0;

	if (!dev) {
		printk(BIOS_ERR, "BUS 0: Unable to find VTD PCI dev");
		return dpr;
	}

	dpr.raw = pci_read_config32(dev, VTD_LTDPR);

	dev = NULL;
	/* Look for VTD devices on all sockets */
	while ((dev = dev_find_device(PCI_VID_INTEL, MMAP_VTD_STACK_CFG_REG_DEVID, dev))) {
		/* Compare the LTDPR register on all iio stacks */
		union dpr_register test_dpr = { .raw = pci_read_config32(dev, VTD_LTDPR) };
		if (dpr.raw != test_dpr.raw) {
			printk(BIOS_ERR, "LTDPR not the same on all IIO's");
			dpr.raw = 0;
			return dpr;
		}
	}
	return dpr;
}
#endif
