/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <delay.h>
#include "chip.h"
#include "iobp.h"
#include "pch.h"

#if CONFIG(INTEL_LYNXPOINT_LP)
#define SATA_PORT_MASK	0x0f
#else
#define SATA_PORT_MASK	0x3f
#endif

static inline u32 sir_read(struct device *dev, int idx)
{
	pci_write_config32(dev, SATA_SIRI, idx);
	return pci_read_config32(dev, SATA_SIRD);
}

static inline void sir_write(struct device *dev, int idx, u32 value)
{
	pci_write_config32(dev, SATA_SIRI, idx);
	pci_write_config32(dev, SATA_SIRD, value);
}

static inline void sir_unset_and_set_mask(struct device *dev, int idx, u32 unset, u32 set)
{
	pci_write_config32(dev, SATA_SIRI, idx);

	const u32 value = pci_read_config32(dev, SATA_SIRD) & ~unset;
	pci_write_config32(dev, SATA_SIRD, value | set);
}

static void sata_init(struct device *dev)
{
	u32 reg32;

	u32 *abar;

	/* Get the chip configuration */
	struct southbridge_intel_lynxpoint_config *config = dev->chip_info;

	printk(BIOS_DEBUG, "SATA: Initializing...\n");

	if (config == NULL) {
		printk(BIOS_ERR, "SATA: ERROR: Device not in devicetree.cb!\n");
		return;
	}

	/* SATA configuration */

	/* Enable memory space decoding for ABAR */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	printk(BIOS_DEBUG, "SATA: Controller in AHCI mode.\n");

	/* Set Interrupt Line */
	/* Interrupt Pin is set by D31IP.PIP */
	pci_write_config8(dev, PCI_INTERRUPT_LINE, 0x0a);

	pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE);
	pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE);

	/* for AHCI, Port Enable is managed in memory mapped space */
	pci_update_config16(dev, 0x92, ~SATA_PORT_MASK, 0x8000 | config->sata_port_map);
	udelay(2);

	/* Setup register 98h */
	reg32 = pci_read_config32(dev, 0x98);
	reg32 |= 1 << 19;    /* BWG step 6 */
	reg32 |= 1 << 22;    /* BWG step 5 */
	reg32 &= ~(0x3f << 7);
	reg32 |= 0x04 << 7;  /* BWG step 7 */
	reg32 |= 1 << 20;    /* BWG step 8 */
	reg32 &= ~(0x03 << 5);
	reg32 |= 1 << 5;     /* BWG step 9 */
	reg32 |= 1 << 18;    /* BWG step 10 */
	reg32 |= 1 << 29;    /* BWG step 11 */
	if (pch_is_lp()) {
		reg32 &= ~((1 << 31) | (1 << 30));
		reg32 |= 1 << 23;
		reg32 |= 1 << 24; /* Disable listen mode (hotplug) */
	}
	pci_write_config32(dev, 0x98, reg32);

	/* Setup register 9Ch: Disable alternate ID and BWG step 12 */
	pci_write_config16(dev, 0x9c, 1 << 5);

	/* SATA Initialization register */
	reg32 = 0x183;
	reg32 |= (config->sata_port_map ^ SATA_PORT_MASK) << 24;
	reg32 |= (config->sata_devslp_mux & 1) << 15;
	pci_write_config32(dev, 0x94, reg32);

	/* Initialize AHCI memory-mapped space */
	abar = (u32 *)pci_read_config32(dev, PCI_BASE_ADDRESS_5);
	printk(BIOS_DEBUG, "ABAR: %p\n", abar);
	/* CAP (HBA Capabilities) : enable power management */
	reg32 = read32(abar + 0x00);
	reg32 |= 0x0c006000;  // set PSC+SSC+SALP+SSS
	reg32 &= ~0x00020060; // clear SXS+EMS+PMS
	if (pch_is_lp())
		reg32 |= (1 << 18);   // SAM: SATA AHCI MODE ONLY
	write32(abar + 0x00, reg32);
	/* PI (Ports implemented) */
	write32(abar + 0x03, config->sata_port_map);
	(void)read32(abar + 0x03); /* Read back 1 */
	(void)read32(abar + 0x03); /* Read back 2 */
	/* CAP2 (HBA Capabilities Extended)*/
	reg32 = read32(abar + 0x09);
	/* Enable DEVSLP */
	if (pch_is_lp()) {
		if (config->sata_devslp_disable)
			reg32 &= ~(1 << 3);
		else
			reg32 |= (1 << 5)|(1 << 4)|(1 << 3)|(1 << 2);
	} else {
		reg32 &= ~0x00000002;
	}
	write32(abar + 0x09, reg32);

	/* Set Gen3 Transmitter settings if needed */
	if (config->sata_port0_gen3_tx)
		pch_iobp_update(SATA_IOBP_SP0G3IR, 0,
				config->sata_port0_gen3_tx);

	if (config->sata_port1_gen3_tx)
		pch_iobp_update(SATA_IOBP_SP1G3IR, 0,
				config->sata_port1_gen3_tx);

	/* Set Gen3 DTLE DATA / EDGE registers if needed */
	if (config->sata_port0_gen3_dtle) {
		pch_iobp_update(SATA_IOBP_SP0DTLE_DATA,
				~(SATA_DTLE_MASK << SATA_DTLE_DATA_SHIFT),
				(config->sata_port0_gen3_dtle & SATA_DTLE_MASK)
				<< SATA_DTLE_DATA_SHIFT);

		pch_iobp_update(SATA_IOBP_SP0DTLE_EDGE,
				~(SATA_DTLE_MASK << SATA_DTLE_EDGE_SHIFT),
				(config->sata_port0_gen3_dtle & SATA_DTLE_MASK)
				<< SATA_DTLE_EDGE_SHIFT);
	}

	if (config->sata_port1_gen3_dtle) {
		pch_iobp_update(SATA_IOBP_SP1DTLE_DATA,
				~(SATA_DTLE_MASK << SATA_DTLE_DATA_SHIFT),
				(config->sata_port1_gen3_dtle & SATA_DTLE_MASK)
				<< SATA_DTLE_DATA_SHIFT);

		pch_iobp_update(SATA_IOBP_SP1DTLE_EDGE,
				~(SATA_DTLE_MASK << SATA_DTLE_EDGE_SHIFT),
				(config->sata_port1_gen3_dtle & SATA_DTLE_MASK)
				<< SATA_DTLE_EDGE_SHIFT);
	}

	/* Additional Programming Requirements */
	/* Power Optimizer */

	/* Step 1 */
	if (pch_is_lp())
		sir_write(dev, 0x64, 0x883c9003);
	else
		sir_write(dev, 0x64, 0x883c9001);

	/* Step 2: SIR 68h[15:0] = 880Ah */
	sir_unset_and_set_mask(dev, 0x68, 0xffff, 0x880a);

	/* Step 3: SIR 60h[3] = 1 */
	sir_unset_and_set_mask(dev, 0x60, 0, 1 << 3);

	/* Step 4: SIR 60h[0] = 1 */
	sir_unset_and_set_mask(dev, 0x60, 0, 1 << 0);

	/* Step 5: SIR 60h[1] = 1 */
	sir_unset_and_set_mask(dev, 0x60, 0, 1 << 1);

	/* Clock Gating */
	sir_write(dev, 0x70, 0x3f00bf1f);
	if (pch_is_lp()) {
		sir_write(dev, 0x54, 0xcf000f0f);
		sir_write(dev, 0x58, 0x00190000);
		RCBA32_AND_OR(0x333c, 0xffcfffff, 0x00c00000);
	}

	reg32 = pci_read_config32(dev, 0x300);
	reg32 |= (1 << 17) | (1 << 16);
	reg32 |= (1 << 31) | (1 << 30) | (1 << 29);
	pci_write_config32(dev, 0x300, reg32);
}

static void sata_enable(struct device *dev)
{
	/* Get the chip configuration */
	struct southbridge_intel_lynxpoint_config *config = dev->chip_info;

	if (!config)
		return;

	/*
	 * Set SATA controller mode early so the resource allocator can
	 * properly assign IO/Memory resources for the controller.
	 */
	pci_write_config16(dev, 0x90, 0x0060 | (config->sata_port_map ^ SATA_PORT_MASK) << 8);
}

static struct device_operations sata_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sata_init,
	.enable			= sata_enable,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_LPT_H_DESKTOP_SATA_IDE,
	PCI_DID_INTEL_LPT_H_DESKTOP_SATA_AHCI,
	PCI_DID_INTEL_LPT_H_DESKTOP_SATA_RAID_1,
	PCI_DID_INTEL_LPT_H_DESKTOP_SATA_RAID_PREM,
	PCI_DID_INTEL_LPT_H_DESKTOP_SATA_IDE_P45,
	PCI_DID_INTEL_LPT_H_DESKTOP_SATA_RAID_2,
	PCI_DID_INTEL_LPT_H_MOBILE_SATA_IDE,
	PCI_DID_INTEL_LPT_H_MOBILE_SATA_AHCI,
	PCI_DID_INTEL_LPT_H_MOBILE_SATA_RAID_1,
	PCI_DID_INTEL_LPT_H_MOBILE_SATA_RAID_PREM,
	PCI_DID_INTEL_LPT_H_MOBILE_SATA_IDE_P45,
	PCI_DID_INTEL_LPT_H_MOBILE_SATA_RAID_2,
	PCI_DID_INTEL_LPT_LP_SATA_AHCI,
	PCI_DID_INTEL_LPT_LP_SATA_RAID_1,
	PCI_DID_INTEL_LPT_LP_SATA_RAID_PREM,
	PCI_DID_INTEL_LPT_LP_SATA_RAID_2,
	0
};

static const struct pci_driver pch_sata __pci_driver = {
	.ops	 = &sata_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
