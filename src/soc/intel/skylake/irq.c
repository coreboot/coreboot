/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/itss.h>
#include <soc/ramstage.h>
#include <soc/interrupt.h>
#include <soc/irq.h>
#include <string.h>

static const SI_PCH_DEVICE_INTERRUPT_CONFIG devintconfig[] = {
	/*
	 * cAVS(Audio, Voice, Speech), INTA is default, programmed in
	 * PciCfgSpace 3Dh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_HDA, int_A, cAVS_INTA_IRQ),
	/*
	 * SMBus Controller, no default value, programmed in
	 * PciCfgSpace 3Dh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_SMBUS, int_A, SMBUS_INTA_IRQ),
	/* GbE Controller, INTA is default, programmed in PciCfgSpace 3Dh */
	DEVICE_INT_CONFIG(PCH_DEVFN_GBE, int_A, GbE_INTA_IRQ),
	/* TraceHub, INTA is default, RO register */
	DEVICE_INT_CONFIG(PCH_DEVFN_TRACEHUB, int_A, TRACE_HUB_INTA_IRQ),
	/*
	 * SerialIo: UART #0, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[7]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_UART0, int_A, LPSS_UART0_IRQ),
	/*
	 * SerialIo: UART #1, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[8]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_UART1, int_B, LPSS_UART1_IRQ),
	/*
	 * SerialIo: SPI #0, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[10]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_GSPI0, int_C, LPSS_SPI0_IRQ),
	/*
	 * SerialIo: SPI #1, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[11]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_GSPI1, int_D, LPSS_SPI1_IRQ),
	/* SCS: eMMC (SKL PCH-LP Only) */
	DEVICE_INT_CONFIG(PCH_DEVFN_EMMC, int_B, eMMC_IRQ),
	/* SCS: SDIO (SKL PCH-LP Only) */
	DEVICE_INT_CONFIG(PCH_DEVFN_SDIO, int_C, SDIO_IRQ),
	/* SCS: SDCard (SKL PCH-LP Only) */
	DEVICE_INT_CONFIG(PCH_DEVFN_SDCARD, int_D, SD_IRQ),
	/* PCI Express Port, INT is default,
	 * programmed in PciCfgSpace + FCh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE9, int_A, PCIE_9_IRQ),
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE10, int_B, PCIE_10_IRQ),
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE11, int_C, PCIE_11_IRQ),
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE12, int_D, PCIE_12_IRQ),
	/*
	 * PCI Express Port 1, INT is default,
	 * programmed in PciCfgSpace + FCh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE1, int_A, PCIE_1_IRQ),
	/*
	 * PCI Express Port 2, INT is default,
	 * programmed in PciCfgSpace + FCh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE2, int_B, PCIE_2_IRQ),
	/*
	 * PCI Express Port 3, INT is default,
	 * programmed in PciCfgSpace + FCh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE3, int_C, PCIE_3_IRQ),
	/*
	 * PCI Express Port 4, INT is default,
	 * programmed in PciCfgSpace + FCh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE4, int_D, PCIE_4_IRQ),
	/*
	 * PCI Express Port 5, INT is default,
	 * programmed in PciCfgSpace + FCh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE5, int_A, PCIE_5_IRQ),
	/*
	 * PCI Express Port 6, INT is default,
	 * programmed in PciCfgSpace + FCh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE6, int_B, PCIE_6_IRQ),
	/*
	 * PCI Express Port 7, INT is default,
	 * programmed in PciCfgSpace + FCh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE7, int_C, PCIE_7_IRQ),
	/*
	 * PCI Express Port 8, INT is default,
	 * programmed in PciCfgSpace + FCh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_PCIE8, int_D, PCIE_8_IRQ),
	/*
	 * SerialIo UART Controller #2, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[9]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_UART2, int_A, LPSS_UART2_IRQ),
	/*
	 * SerialIo UART Controller #5, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[6]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_I2C5, int_B, LPSS_I2C5_IRQ),
	/*
	 * SerialIo UART Controller #4, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[5]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_I2C4, int_C, LPSS_I2C4_IRQ),
	/*
	 * SATA Controller, INTA is default,
	 * programmed in PciCfgSpace + 3Dh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_SATA, int_A, SATA_IRQ),
	/* CSME: HECI #1 */
	DEVICE_INT_CONFIG(PCH_DEVFN_CSE, int_A, HECI_1_IRQ),
	/* CSME: HECI #2 */
	DEVICE_INT_CONFIG(PCH_DEVFN_CSE_2, int_B, HECI_2_IRQ),
	/* CSME: IDE-Redirection (IDE-R) */
	DEVICE_INT_CONFIG(PCH_DEVFN_CSE_IDER, int_C, IDER_IRQ),
	/* CSME: Keyboard and Text (KT) Redirection */
	DEVICE_INT_CONFIG(PCH_DEVFN_CSE_KT, int_D, KT_IRQ),
	/* CSME: HECI #3 */
	DEVICE_INT_CONFIG(PCH_DEVFN_CSE_3, int_A, HECI_3_IRQ),
	/*
	 * SerialIo I2C Controller #0, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[1]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_I2C0, int_A, LPSS_I2C0_IRQ),
	/*
	 * SerialIo I2C Controller #1, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[2]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_I2C1, int_B, LPSS_I2C1_IRQ),
	/*
	 * SerialIo I2C Controller #2, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[3]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_I2C2, int_C, LPSS_I2C2_IRQ),
	/*
	 * SerialIo I2C Controller #3, INTA is default,
	 * programmed in PCR[SERIALIO] + PCICFGCTRL[4]
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_I2C3, int_D, LPSS_I2C3_IRQ),
	/*
	 * USB 3.0 xHCI Controller, no default value,
	 * programmed in PciCfgSpace 3Dh
	 */
	DEVICE_INT_CONFIG(PCH_DEVFN_XHCI, int_A, XHCI_IRQ),
	/* USB Device Controller (OTG) */
	DEVICE_INT_CONFIG(PCH_DEVFN_USBOTG, int_B, OTG_IRQ),
	/* Thermal Subsystem */
	DEVICE_INT_CONFIG(PCH_DEVFN_THERMAL, int_C, THERMAL_IRQ),
	/* Camera IO Host Controller */
	DEVICE_INT_CONFIG(PCH_DEVFN_CIO, int_A, CIO_INTA_IRQ),
	/* Integrated Sensor Hub */
	DEVICE_INT_CONFIG(PCH_DEVFN_ISH, int_A, ISH_IRQ)
};

void soc_irq_settings(FSP_SIL_UPD *params)
{

	uint32_t i, intdeventry;
	u8 irq_config[PCH_MAX_IRQ_CONFIG];
	const struct device *dev = pcidev_path_on_root(PCH_DEVFN_LPC);
	const struct soc_intel_skylake_config *config = config_of(dev);

	/* Get Device Int Count */
	intdeventry = ARRAY_SIZE(devintconfig);
	/* update irq table */
	memcpy((SI_PCH_DEVICE_INTERRUPT_CONFIG *)
		(params->DevIntConfigPtr), devintconfig, intdeventry *
		sizeof(SI_PCH_DEVICE_INTERRUPT_CONFIG));

	params->NumOfDevIntConfig = intdeventry;
	/* PxRC to IRQ programming */
	for (i = 0; i < PCH_MAX_IRQ_CONFIG; i++) {
		switch (i) {
		case PCH_PARC:
		case PCH_PCRC:
		case PCH_PDRC:
		case PCH_PERC:
		case PCH_PFRC:
		case PCH_PGRC:
		case PCH_PHRC:
			irq_config[i] = PCH_IRQ11;
			break;
		case PCH_PBRC:
			irq_config[i] = PCH_IRQ10;
			break;
		}
	}
	memcpy(params->PxRcConfig, irq_config, PCH_MAX_IRQ_CONFIG);
	/* GPIO IRQ Route  The valid values is 14 or 15 */
	if (config->GpioIrqSelect == 0)
		params->GpioIrqRoute = GPIO_IRQ14;
	else
		params->GpioIrqRoute = config->GpioIrqSelect;
	/* SCI IRQ Select  The valid values is 9, 10, 11 and 20 21, 22, 23 */
	if (config->SciIrqSelect == 0)
		params->SciIrqSelect = SCI_IRQ9;
	else
		params->SciIrqSelect = config->SciIrqSelect;
	/* TCO IRQ Select  The valid values is 9, 10, 11, 20 21, 22, 23 */
	if (config->TcoIrqSelect == 0)
		params->TcoIrqSelect = TCO_IRQ9;
	else
		params->TcoIrqSelect = config->TcoIrqSelect;
	/* TCO Irq enable/disable */
	params->TcoIrqEnable = config->TcoIrqEnable;
}

/*
 * PIRQ[n]_ROUT[3:0] - PIRQ Routing Control
 * 0x00 - 0000 = Reserved
 * 0x01 - 0001 = Reserved
 * 0x02 - 0010 = Reserved
 * 0x03 - 0011 = IRQ3
 * 0x04 - 0100 = IRQ4
 * 0x05 - 0101 = IRQ5
 * 0x06 - 0110 = IRQ6
 * 0x07 - 0111 = IRQ7
 * 0x08 - 1000 = Reserved
 * 0x09 - 1001 = IRQ9
 * 0x0A - 1010 = IRQ10
 * 0x0B - 1011 = IRQ11
 * 0x0C - 1100 = IRQ12
 * 0x0D - 1101 = Reserved
 * 0x0E - 1110 = IRQ14
 * 0x0F - 1111 = IRQ15
 * PIRQ[n]_ROUT[7] - PIRQ Routing Control
 * 0x80 - The PIRQ is not routed.
 */

void soc_pch_pirq_init(const struct device *dev)
{
	uint8_t pch_interrupt_routing[MAX_PXRC_CONFIG];
	struct device *irq_dev;

	pch_interrupt_routing[0] = PCH_IRQ11;
	pch_interrupt_routing[1] = PCH_IRQ10;
	pch_interrupt_routing[2] = PCH_IRQ11;
	pch_interrupt_routing[3] = PCH_IRQ11;
	pch_interrupt_routing[4] = PCH_IRQ11;
	pch_interrupt_routing[5] = PCH_IRQ11;
	pch_interrupt_routing[6] = PCH_IRQ11;
	pch_interrupt_routing[7] = PCH_IRQ11;

	itss_irq_init(pch_interrupt_routing);

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin = 0, int_line = 0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		switch (int_pin) {
		case 1: /* INTA# */
			int_line = PCH_IRQ11;
			break;
		case 2: /* INTB# */
			int_line = PCH_IRQ10;
			break;
		case 3: /* INTC# */
			int_line = PCH_IRQ11;
			break;
		case 4: /* INTD# */
			int_line = PCH_IRQ11;
			break;
		}

		if (!int_line)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
	}
}
