/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <intelblocks/pcr.h>
#include <intelblocks/rtc.h>
#include <intelblocks/p2sb.h>
#include <soc/bootblock.h>
#include <soc/pch.h>
#include <soc/pmc.h>
#include <console/console.h>

void override_hpet_ioapic_bdf(void)
{
	union p2sb_bdf ioapic_bdf = {
		.bus = PCH_IOAPIC_BUS_NUMBER,
		.dev = PCH_IOAPIC_DEV_NUM,
		.fn = PCH_IOAPIC_FUNC_NUM,
	};
	union p2sb_bdf hpet_bdf = {
		.bus = HPET_BUS_NUM,
		.dev = HPET_DEV_NUM,
		.fn = HPET0_FUNC_NUM,
	};

	p2sb_set_ioapic_bdf(ioapic_bdf);
	p2sb_set_hpet_bdf(hpet_bdf);
}
