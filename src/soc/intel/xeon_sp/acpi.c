/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <intelblocks/acpi.h>
#include <intelblocks/lpc_lib.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/intel/common/acpi.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <string.h>

acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	*entries = 0;
	return NULL;
}

static void print_madt_ioapic(int socket, int stack,
				     int ioapic_id, uint32_t ioapic_base, int gsi_base)
{
	printk(BIOS_DEBUG, "Adding MADT IOAPIC for socket: %d, stack: %d, ioapic_id: 0x%x, "
	       "ioapic_base: 0x%x, gsi_base: 0x%x\n",
	       socket, stack,  ioapic_id, ioapic_base, gsi_base);
	return;
}

const struct madt_ioapic_info *soc_get_ioapic_info(size_t *entries)
{
	int cur_index;
	const IIO_UDS *hob = get_iio_uds();

	/* With XEON-SP FSP, PCH IOAPIC is allocated with first 120 GSIs. */
#if (CONFIG(SOC_INTEL_COOPERLAKE_SP))
	const int gsi_bases[] = { 0, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0 };
#endif

#if (CONFIG(SOC_INTEL_SKYLAKE_SP))
	const int gsi_bases[] = { 0, 0x18, 0x20, 0x28, 0x30, 0x48, 0x50, 0x58, 0x60 };
#endif

	static struct madt_ioapic_info madt_tbl[ARRAY_SIZE(gsi_bases)];

	cur_index = 0;
	madt_tbl[cur_index].id  = PCH_IOAPIC_ID;
	madt_tbl[cur_index].addr = hob->PlatformData.IIO_resource[0].StackRes[0].IoApicBase;
	madt_tbl[cur_index].gsi_base = gsi_bases[cur_index];
	print_madt_ioapic(0, 0,  madt_tbl[cur_index].id,
			  madt_tbl[cur_index].addr, madt_tbl[cur_index].gsi_base);
	++cur_index;

	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		for (int stack = 0; stack < MAX_IIO_STACK; ++stack) {
			const STACK_RES *ri =
				&hob->PlatformData.IIO_resource[socket].StackRes[stack];
			if (!is_iio_stack_res(ri))
				continue;
			assert(cur_index < ARRAY_SIZE(gsi_bases));
			madt_tbl[cur_index].id = soc_get_iio_ioapicid(socket, stack);
			madt_tbl[cur_index].gsi_base = gsi_bases[cur_index];
			madt_tbl[cur_index].addr = ri->IoApicBase;

			/*
			 * Stack 0 has non-PCH IOAPIC and PCH IOAPIC.
			 * The IIO IOAPIC is placed at 0x1000 from the reported base.
			 */
			if (stack == 0 && socket == 0)
				madt_tbl[cur_index].addr += 0x1000;

			print_madt_ioapic(socket, stack, madt_tbl[cur_index].id,
					  madt_tbl[cur_index].addr,
					  madt_tbl[cur_index].gsi_base);
			++cur_index;
		}
	}

	*entries = cur_index;
	return madt_tbl;
}
