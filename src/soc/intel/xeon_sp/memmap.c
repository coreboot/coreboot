/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/pci_ops.h>
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
	/*
	 * We need to make sure ramstage will be run cached. At this
	 * point exact location of ramstage in cbmem is not known.
	 * Instruct postcar to cache 16 megs under cbmem top which is
	 * a safe bet to cover ramstage.
	 */
	uintptr_t top_of_ram = (uintptr_t)cbmem_top();

	printk(BIOS_DEBUG, "top_of_ram = 0x%lx\n", top_of_ram);
	top_of_ram -= 16 * MiB;
	postcar_frame_add_mtrr(pcf, top_of_ram, 16 * MiB, MTRR_TYPE_WRBACK);
	/* Cache the TSEG region */
	if (CONFIG(TSEG_STAGE_CACHE))
		postcar_enable_tseg_cache(pcf);
}

#if !defined(__SIMPLE_DEVICE__)
union dpr_register txt_get_chipset_dpr(void)
{
	const IIO_UDS *hob = get_iio_uds();
	union dpr_register dpr;
	struct device *dev = VTD_DEV(0);

	dpr.raw = 0;

	if (dev == NULL) {
		printk(BIOS_ERR, "BUS 0: Unable to find VTD PCI dev");
		return dpr;
	}

	dpr.raw = pci_read_config32(dev, VTD_LTDPR);

	/* Compare the LTDPR register on all iio stacks */
	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		for (int stack = 0; stack < MAX_IIO_STACK; ++stack) {
			const STACK_RES *ri =
				&hob->PlatformData.IIO_resource[socket].StackRes[stack];
			if (!is_iio_stack_res(ri))
				continue;
			uint8_t bus = ri->BusBase;
			dev = VTD_DEV(bus);

			if (dev == NULL) {
				printk(BIOS_ERR, "BUS %x: Unable to find VTD PCI dev\n", bus);
				dpr.raw = 0;
				return  dpr;
			}

			union dpr_register test_dpr = { .raw = pci_read_config32(dev, VTD_LTDPR) };
			if (dpr.raw != test_dpr.raw) {
				printk(BIOS_ERR, "LTDPR not the same on all IIO's");
				dpr.raw = 0;
				return dpr;
			}
		}
	}
	return dpr;
}
#endif
