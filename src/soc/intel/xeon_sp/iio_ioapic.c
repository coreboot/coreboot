/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/acpi.h>
#include <soc/chip_common.h>
#include <soc/util.h>
#include <stdint.h>

static uintptr_t xeonsp_ioapic_bases[CONFIG_MAX_SOCKET * MAX_IIO_STACK + 1];

size_t soc_get_ioapic_info(const uintptr_t *ioapic_bases[])
{
	int index = 0;
	const IIO_UDS *hob = get_iio_uds();

	*ioapic_bases = xeonsp_ioapic_bases;

	for (int socket = 0; socket < CONFIG_MAX_SOCKET; socket++) {
		if (!soc_cpu_is_enabled(socket))
			continue;
		for (int stack = 0; stack < MAX_IIO_STACK; ++stack) {
			const STACK_RES *ri =
				&hob->PlatformData.IIO_resource[socket].StackRes[stack];
			uint32_t ioapic_base = ri->IoApicBase;
			if (ioapic_base == 0 || ioapic_base == 0xFFFFFFFF)
				continue;
			xeonsp_ioapic_bases[index++] = ioapic_base;
			/*
			 * Stack 0 has non-PCH IOAPIC and PCH IOAPIC.
			 * The IIO IOAPIC is placed at 0x1000 from the reported base.
			 */
			if (socket == 0 && stack == 0) {
				ioapic_base += 0x1000;
				xeonsp_ioapic_bases[index++] = ioapic_base;
			}
		}
	}

	return index;
}
