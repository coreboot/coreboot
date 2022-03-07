/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/mmio.h>
#include <soc/adsp.h>
#include <soc/device_nvs.h>
#include <soc/pch.h>
#include <soc/rcba.h>
#include <soc/intel/broadwell/pch/chip.h>
#include <southbridge/intel/lynxpoint/iobp.h>

static void adsp_init(struct device *dev)
{
	const struct soc_intel_broadwell_pch_config *config = config_of(dev);
	struct resource *bar0, *bar1;
	u32 tmp32;

	/* Ensure memory and bus master are enabled */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	/* Find BAR0 and BAR1 */
	bar0 = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!bar0)
		return;
	bar1 = probe_resource(dev, PCI_BASE_ADDRESS_1);
	if (!bar1)
		return;

	/*
	 * Set LTR value in DSP shim LTR control register to 3ms
	 * SNOOP_REQ[13]=1b SNOOP_SCALE[12:10]=100b (1ms) SNOOP_VAL[9:0]=3h
	 */
	tmp32 = pch_is_wpt() ? ADSP_SHIM_BASE_WPT : ADSP_SHIM_BASE_LPT;
	write32(res2mmio(bar0, tmp32 + ADSP_SHIM_LTRC, 0),
		ADSP_SHIM_LTRC_VALUE);

	/* Program VDRTCTL2 D19:F0:A8[31:0] = 0x00000fff */
	pci_write_config32(dev, ADSP_PCI_VDRTCTL2, ADSP_VDRTCTL2_VALUE);

	/* Program ADSP IOBP VDLDAT1 to 0x040100 */
	pch_iobp_write(ADSP_IOBP_VDLDAT1, ADSP_VDLDAT1_VALUE);

	/* Set D3 Power Gating Enable in D19:F0:A0 based on PCH type */
	tmp32 = pci_read_config32(dev, ADSP_PCI_VDRTCTL0);
	if (pch_is_wpt()) {
		if (config->adsp_d3_pg_enable) {
			tmp32 &= ~ADSP_VDRTCTL0_D3PGD_WPT;
			if (config->adsp_sram_pg_enable)
				tmp32 &= ~ADSP_VDRTCTL0_D3SRAMPGD_WPT;
			else
				tmp32 |= ADSP_VDRTCTL0_D3SRAMPGD_WPT;
		} else {
			tmp32 |= ADSP_VDRTCTL0_D3PGD_WPT;
		}
	} else {
		if (config->adsp_d3_pg_enable) {
			tmp32 &= ~ADSP_VDRTCTL0_D3PGD_LPT;
			if (config->adsp_sram_pg_enable)
				tmp32 &= ~ADSP_VDRTCTL0_D3SRAMPGD_LPT;
			else
				tmp32 |= ADSP_VDRTCTL0_D3SRAMPGD_LPT;
		} else {
			tmp32 |= ADSP_VDRTCTL0_D3PGD_LPT;
		}
	}
	pci_write_config32(dev, ADSP_PCI_VDRTCTL0, tmp32);

	/* Set PSF Snoop to SA, RCBA+0x3350[10]=1b */
	RCBA32_OR(0x3350, (1 << 10));

	/* Set DSP IOBP PMCTL 0x1e0=0x3f */
	pch_iobp_write(ADSP_IOBP_PMCTL, ADSP_PMCTL_VALUE);

	if (config->sio_acpi_mode) {
		struct device_nvs *dev_nvs = acpi_get_device_nvs();

		/* Configure for ACPI mode */
		printk(BIOS_INFO, "ADSP: Enable ACPI Mode IRQ3\n");

		/* Save BAR0 and BAR1 to ACPI NVS */
		dev_nvs->bar0[SIO_NVS_ADSP] = (u32)bar0->base;
		dev_nvs->bar1[SIO_NVS_ADSP] = (u32)bar1->base;
		dev_nvs->enable[SIO_NVS_ADSP] = 1;

		/* Set PCI Config Disable Bit */
		pch_iobp_update(ADSP_IOBP_PCICFGCTL, ~0, ADSP_PCICFGCTL_PCICD);

		/* Set interrupt de-assert/assert opcode override to IRQ3 */
		pch_iobp_write(ADSP_IOBP_VDLDAT2, ADSP_IOBP_ACPI_IRQ3);

		/* Enable IRQ3 in RCBA */
		RCBA32_OR(ACPIIRQEN, ADSP_ACPI_IRQEN);

		/* Set ACPI Interrupt Enable Bit */
		pch_iobp_update(ADSP_IOBP_PCICFGCTL, ~ADSP_PCICFGCTL_SPCBAD,
				ADSP_PCICFGCTL_ACPIIE);

		/* Put ADSP in D3hot */
		tmp32 = read32(res2mmio(bar1, PCH_PCS, 0));
		tmp32 |= PCH_PCS_PS_D3HOT;
		write32(res2mmio(bar1, PCH_PCS, 0), tmp32);
	} else {
		printk(BIOS_INFO, "ADSP: Enable PCI Mode IRQ23\n");

		/* Configure for PCI mode */
		pci_write_config8(dev, PCI_INTERRUPT_LINE, ADSP_PCI_IRQ);

		/* Clear ACPI Interrupt Enable Bit */
		pch_iobp_update(ADSP_IOBP_PCICFGCTL,
			~(ADSP_PCICFGCTL_SPCBAD | ADSP_PCICFGCTL_ACPIIE), 0);
	}
}

static struct device_operations adsp_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= adsp_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	0x9c36, /* LynxPoint */
	0x9cb6, /* WildcatPoint */
	0
};

static const struct pci_driver pch_adsp __pci_driver = {
	.ops	 = &adsp_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
