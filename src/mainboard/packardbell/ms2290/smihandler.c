/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <soc/nvs.h>
#include <southbridge/intel/common/pmutil.h>
#include <northbridge/intel/ironlake/ironlake.h>
#include <ec/acpi/ec.h>

void mainboard_smi_gpi(u32 gpi_sts)
{
}

int mainboard_smi_apmc(u8 data)
{
	u8 tmp;
	switch (data) {
	case APM_CNT_ACPI_ENABLE:
		tmp = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xbb);
		tmp &= ~0x03;
		tmp |= 0x02;
		pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xbb, tmp);
		break;
	case APM_CNT_ACPI_DISABLE:
		tmp = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xbb);
		tmp &= ~0x03;
		tmp |= 0x01;
		pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xbb, tmp);
		break;
	default:
		break;
	}
	return 0;
}
