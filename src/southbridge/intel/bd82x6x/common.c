/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <stdint.h>

#include "pch.h"

int pch_silicon_revision(void)
{
	static int pch_revision_id = 0;

	if (!pch_revision_id)
		pch_revision_id = pci_read_config8(PCH_LPC_DEV, PCI_REVISION_ID);

	return pch_revision_id;
}

int pch_silicon_type(void)
{
	static int pch_type = 0;

	if (!pch_type)
		pch_type = pci_read_config8(PCH_LPC_DEV, PCI_DEVICE_ID + 1);

	return pch_type;
}

bool pch_is_mobile(void)
{
	const u16 devids[] = {
		PCI_DID_INTEL_6_SERIES_MOBILE_SFF, PCI_DID_INTEL_6_SERIES_MOBILE,
		PCI_DID_INTEL_6_SERIES_UM67, PCI_DID_INTEL_6_SERIES_HM65,
		PCI_DID_INTEL_6_SERIES_HM67, PCI_DID_INTEL_6_SERIES_QS67,
		PCI_DID_INTEL_6_SERIES_QM67,
		PCI_DID_INTEL_7_SERIES_MOBILE, PCI_DID_INTEL_7_SERIES_MOBILE_SFF,
		PCI_DID_INTEL_7_SERIES_QM77, PCI_DID_INTEL_7_SERIES_QS77,
		PCI_DID_INTEL_7_SERIES_HM77, PCI_DID_INTEL_7_SERIES_UM77,
		PCI_DID_INTEL_7_SERIES_HM76, PCI_DID_INTEL_7_SERIES_HM75,
		PCI_DID_INTEL_7_SERIES_HM70, PCI_DID_INTEL_7_SERIES_NM70
	};
	u16 devid = pci_s_read_config16(PCH_LPC_DEV, PCI_DEVICE_ID);

	for (size_t i = 0; i < ARRAY_SIZE(devids); i++)
		if (devid == devids[i])
			return true;
	return false;
}

int pch_silicon_supported(int type, int rev)
{
	int cur_type = pch_silicon_type();
	int cur_rev = pch_silicon_revision();

	switch (type) {
	case PCH_TYPE_CPT:
		/* CougarPoint minimum revision */
		if (cur_type == PCH_TYPE_CPT && cur_rev >= rev)
			return 1;
		/* PantherPoint any revision */
		if (cur_type == PCH_TYPE_PPT)
			return 1;
		break;

	case PCH_TYPE_PPT:
		/* PantherPoint minimum revision */
		if (cur_type == PCH_TYPE_PPT && cur_rev >= rev)
			return 1;
		break;
	}

	return 0;
}

#define IOBP_RETRY 1000
static inline int iobp_poll(void)
{
	unsigned int try = IOBP_RETRY;
	u32 data;

	while (try--) {
		data = RCBA32(IOBPS);
		if ((data & 1) == 0)
			return 1;
		udelay(10);
	}

	printk(BIOS_ERR, "IOBP timeout\n");
	return 0;
}

void pch_iobp_update(u32 address, u32 andvalue, u32 orvalue)
{
	u32 data;

	/* Set the address */
	RCBA32(IOBPIRI) = address;

	/* READ OPCODE */
	if (pch_silicon_supported(PCH_TYPE_CPT, PCH_STEP_B0))
		RCBA32(IOBPS) = IOBPS_RW_BX;
	else
		RCBA32(IOBPS) = IOBPS_READ_AX;
	if (!iobp_poll())
		return;

	/* Read IOBP data */
	data = RCBA32(IOBPD);
	if (!iobp_poll())
		return;

	/* Check for successful transaction */
	if ((RCBA32(IOBPS) & 0x6) != 0) {
		printk(BIOS_ERR, "IOBP read 0x%08x failed\n", address);
		return;
	}

	/* Update the data */
	data &= andvalue;
	data |= orvalue;

	/* WRITE OPCODE */
	if (pch_silicon_supported(PCH_TYPE_CPT, PCH_STEP_B0))
		RCBA32(IOBPS) = IOBPS_RW_BX;
	else
		RCBA32(IOBPS) = IOBPS_WRITE_AX;
	if (!iobp_poll())
		return;

	/* Write IOBP data */
	RCBA32(IOBPD) = data;
	if (!iobp_poll())
		return;
}
