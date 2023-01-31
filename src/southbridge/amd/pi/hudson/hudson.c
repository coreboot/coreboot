/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <acpi/acpi.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <types.h>

#include "hudson.h"
#include "smbus.h"
#include "smi.h"
#include "fchec.h"

int acpi_get_sleep_type(void)
{
	u16 tmp = inw(ACPI_PM1_CNT_BLK);
	tmp = ((tmp & (7 << 10)) >> 10);
	return (int)tmp;
}

void hudson_enable(struct device *dev)
{
	printk(BIOS_DEBUG, "%s()\n", __func__);
	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(0x14, 7): /* SD */
		if (dev->enabled == 0) {
			u32 sd_device_id = pci_read_config16(dev, PCI_DEVICE_ID);
			/* turn off the SDHC controller in the PM reg */
			u8 reg8;
			if (sd_device_id == PCI_DID_AMD_HUDSON_SD) {
				reg8 = pm_read8(PM_HUD_SD_FLASH_CTRL);
				reg8 &= ~BIT(0);
				pm_write8(PM_HUD_SD_FLASH_CTRL, reg8);
			} else if (sd_device_id == PCI_DID_AMD_YANGTZE_SD) {
				reg8 = pm_read8(PM_YANG_SD_FLASH_CTRL);
				reg8 &= ~BIT(0);
				pm_write8(PM_YANG_SD_FLASH_CTRL, reg8);
			}
			/* remove device 0:14.7 from PCI space */
			reg8 = pm_read8(PM_MANUAL_RESET);
			reg8 &= ~BIT(6);
			pm_write8(PM_MANUAL_RESET, reg8);
		}
		break;
	default:
		break;
	}
}

static void hudson_init_acpi_ports(void)
{
	/* We use some of these ports in SMM regardless of whether or not
	 * ACPI tables are generated. Enable these ports indiscriminately.
	 */

	pm_write16(PM_EVT_BLK, ACPI_PM_EVT_BLK);
	pm_write16(PM1_CNT_BLK, ACPI_PM1_CNT_BLK);
	pm_write16(PM_TMR_BLK, ACPI_PM_TMR_BLK);
	pm_write16(PM_GPE0_BLK, ACPI_GPE0_BLK);
	/* CpuControl is in \_PR.CP00, 6 bytes */
	pm_write16(PM_CPU_CTRL, ACPI_CPU_CONTROL);

	if (CONFIG(HAVE_SMI_HANDLER)) {
		pm_write16(PM_ACPI_SMI_CMD, ACPI_SMI_CTL_PORT);
		hudson_enable_acpi_cmd_smi();
	} else {
		pm_write16(PM_ACPI_SMI_CMD, 0);
	}

	/* AcpiDecodeEnable, When set, SB uses the contents of the PM registers
	 * at index 60-6B to decode ACPI I/O address. AcpiSmiEn & SmiCmdEn
	 */
	pm_write8(PM_ACPI_CONF, BIT(0) | BIT(1) | BIT(4) | BIT(2));
}

static void hudson_init(void *chip_info)
{
	hudson_init_acpi_ports();
}

static void hudson_final(void *chip_info)
{
	if (CONFIG(HUDSON_IMC_FWM)) {
		agesawrapper_fchecfancontrolservice();
		enable_imc_thermal_zone();
	}
}

struct chip_operations southbridge_amd_pi_hudson_ops = {
	CHIP_NAME("ATI HUDSON")
	.enable_dev = hudson_enable,
	.init = hudson_init,
	.final = hudson_final
};
