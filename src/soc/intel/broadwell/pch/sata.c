/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <delay.h>
#include <soc/rcba.h>
#include <soc/sata.h>
#include <soc/intel/broadwell/pch/chip.h>
#include <southbridge/intel/lynxpoint/iobp.h>

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

static void sata_init(struct device *dev)
{
	const struct soc_intel_broadwell_pch_config *config = config_of(dev);
	u32 reg32;
	u8 *abar;
	u16 reg16;
	int port;

	printk(BIOS_DEBUG, "SATA: Initializing controller in AHCI mode.\n");

	/* Enable memory space decoding for ABAR */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	/* Set Interrupt Line */
	/* Interrupt Pin is set by D31IP.PIP */
	pci_write_config8(dev, PCI_INTERRUPT_LINE, 0x0a);

	/* Set timings */
	pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE);
	pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE);

	/* for AHCI, Port Enable is managed in memory mapped space */
	reg16 = pci_read_config16(dev, 0x92);
	reg16 &= ~0xf;
	reg16 |= 0x8000 | config->sata_port_map;
	pci_write_config16(dev, 0x92, reg16);
	udelay(2);

	/* Setup register 98h */
	reg32 = pci_read_config32(dev, 0x98);
	reg32 &= ~((1 << 31) | (1 << 30));
	reg32 |= 1 << 23;
	reg32 |= 1 << 24; /* Enable MPHY Dynamic Power Gating */
	pci_write_config32(dev, 0x98, reg32);

	/* Setup register 9Ch */
	reg16 = (1 << 5);   /* BWG step 12 */
	pci_write_config16(dev, 0x9c, reg16);

	/* SATA Initialization register */
	reg32 = 0x183;
	reg32 |= (config->sata_port_map ^ 0xf) << 24;
	reg32 |= (config->sata_devslp_mux & 1) << 15;
	pci_write_config32(dev, 0x94, reg32);

	/* Initialize AHCI memory-mapped space */
	abar = (u8 *)(pci_read_config32(dev, PCI_BASE_ADDRESS_5));
	printk(BIOS_DEBUG, "ABAR: %p\n", abar);

	/* CAP (HBA Capabilities) : enable power management */
	reg32 = read32(abar + 0x00);
	reg32 |= 0x0c006000;  /* set PSC+SSC+SALP+SSS */
	reg32 &= ~0x00020060; /* clear SXS+EMS+PMS */
	reg32 |= (1 << 18);   /* SAM: SATA AHCI MODE ONLY */
	write32(abar + 0x00, reg32);

	/* PI (Ports implemented) */
	write32(abar + 0x0c, config->sata_port_map);
	(void) read32(abar + 0x0c); /* Read back 1 */
	(void) read32(abar + 0x0c); /* Read back 2 */

	/* CAP2 (HBA Capabilities Extended)*/
	if (config->sata_devslp_disable) {
		reg32 = read32(abar + 0x24);
		reg32 &= ~(1 << 3);
		write32(abar + 0x24, reg32);
	} else {
		/* Enable DEVSLP */
		reg32 = read32(abar + 0x24);
		reg32 |= (1 << 5)|(1 << 4)|(1 << 3)|(1 << 2);
		write32(abar + 0x24, reg32);

		for (port = 0; port < 4; port++) {
			if (!(config->sata_port_map & (1 << port)))
				continue;
			reg32 = read32(abar + 0x144 + (0x80 * port));
			reg32 |= (1 << 1);	/* DEVSLP DSP */
			write32(abar + 0x144 + (0x80 * port), reg32);
		}
	}

	/*
	 * Static Power Gating for unused ports
	 */
	reg32 = RCBA32(0x3a84);
	/* Port 3 and 2 disabled */
	if ((config->sata_port_map & ((1 << 3)|(1 << 2))) == 0)
		reg32 |= (1 << 24) | (1 << 26);
	/* Port 1 and 0 disabled */
	if ((config->sata_port_map & ((1 << 1)|(1 << 0))) == 0)
		reg32 |= (1 << 20) | (1 << 18);
	RCBA32(0x3a84) = reg32;

	/* Set Gen3 Transmitter settings if needed */
	if (config->sata_port0_gen3_tx)
		pch_iobp_update(SATA_IOBP_SP0_SECRT88,
				~(SATA_SECRT88_VADJ_MASK <<
				  SATA_SECRT88_VADJ_SHIFT),
				(config->sata_port0_gen3_tx &
				 SATA_SECRT88_VADJ_MASK)
				<< SATA_SECRT88_VADJ_SHIFT);

	if (config->sata_port1_gen3_tx)
		pch_iobp_update(SATA_IOBP_SP1_SECRT88,
				~(SATA_SECRT88_VADJ_MASK <<
				  SATA_SECRT88_VADJ_SHIFT),
				(config->sata_port1_gen3_tx &
				 SATA_SECRT88_VADJ_MASK)
				<< SATA_SECRT88_VADJ_SHIFT);

	if (config->sata_port2_gen3_tx)
		pch_iobp_update(SATA_IOBP_SP2_SECRT88,
				~(SATA_SECRT88_VADJ_MASK <<
				  SATA_SECRT88_VADJ_SHIFT),
				(config->sata_port2_gen3_tx &
				 SATA_SECRT88_VADJ_MASK)
				<< SATA_SECRT88_VADJ_SHIFT);

	if (config->sata_port3_gen3_tx)
		pch_iobp_update(SATA_IOBP_SP3_SECRT88,
				~(SATA_SECRT88_VADJ_MASK <<
				  SATA_SECRT88_VADJ_SHIFT),
				(config->sata_port3_gen3_tx &
				 SATA_SECRT88_VADJ_MASK)
				<< SATA_SECRT88_VADJ_SHIFT);

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

	if (config->sata_port2_gen3_dtle) {
		pch_iobp_update(SATA_IOBP_SP2DTLE_DATA,
				~(SATA_DTLE_MASK << SATA_DTLE_DATA_SHIFT),
				(config->sata_port2_gen3_dtle & SATA_DTLE_MASK)
				<< SATA_DTLE_DATA_SHIFT);

		pch_iobp_update(SATA_IOBP_SP2DTLE_EDGE,
				~(SATA_DTLE_MASK << SATA_DTLE_EDGE_SHIFT),
				(config->sata_port2_gen3_dtle & SATA_DTLE_MASK)
				<< SATA_DTLE_EDGE_SHIFT);
	}
	if (config->sata_port3_gen3_dtle) {
		pch_iobp_update(SATA_IOBP_SP3DTLE_DATA,
				~(SATA_DTLE_MASK << SATA_DTLE_DATA_SHIFT),
				(config->sata_port3_gen3_dtle & SATA_DTLE_MASK)
				<< SATA_DTLE_DATA_SHIFT);

		pch_iobp_update(SATA_IOBP_SP3DTLE_EDGE,
				~(SATA_DTLE_MASK << SATA_DTLE_EDGE_SHIFT),
				(config->sata_port3_gen3_dtle & SATA_DTLE_MASK)
				<< SATA_DTLE_EDGE_SHIFT);
	}

	/*
	 * Additional Programming Requirements for Power Optimizer
	 */

	/* Step 1 */
	sir_write(dev, 0x64, 0x883c9003);

	/* Step 2: SIR 68h[15:0] = 880Ah */
	reg32 = sir_read(dev, 0x68);
	reg32 &= 0xffff0000;
	reg32 |= 0x880a;
	sir_write(dev, 0x68, reg32);

	/* Step 3: SIR 60h[3] = 1 */
	reg32 = sir_read(dev, 0x60);
	reg32 |= (1 << 3);
	sir_write(dev, 0x60, reg32);

	/* Step 4: SIR 60h[0] = 1 */
	reg32 = sir_read(dev, 0x60);
	reg32 |= (1 << 0);
	sir_write(dev, 0x60, reg32);

	/* Step 5: SIR 60h[1] = 1 */
	reg32 = sir_read(dev, 0x60);
	reg32 |= (1 << 1);
	sir_write(dev, 0x60, reg32);

	/* Clock Gating */
	sir_write(dev, 0x70, 0x3f00bf1f);
	sir_write(dev, 0x54, 0xcf000f0f);
	sir_write(dev, 0x58, 0x00190000);
	RCBA32_AND_OR(0x333c, 0xffcfffff, 0x00c00000);

	reg32 = pci_read_config32(dev, 0x300);
	reg32 |= (1 << 17) | (1 << 16) | (1 << 19);
	reg32 |= (1 << 31) | (1 << 30) | (1 << 29);
	pci_write_config32(dev, 0x300, reg32);

	reg32 = pci_read_config32(dev, 0x98);
	reg32 |= 1 << 29;
	pci_write_config32(dev, 0x98, reg32);

	/* Register Lock */
	reg32 = pci_read_config32(dev, 0x9c);
	reg32 |= (1 << 31);
	pci_write_config32(dev, 0x9c, reg32);
}

/*
 * Set SATA controller mode early so the resource allocator can
 * properly assign IO/Memory resources for the controller.
 */
static void sata_enable(struct device *dev)
{
	/* Get the chip configuration */
	const struct soc_intel_broadwell_pch_config *config = config_of(dev);
	u16 map = 0x0060;

	map |= (config->sata_port_map ^ 0xf) << 8;

	pci_write_config16(dev, 0x90, map);
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
	0x9c03, 0x9c05, 0x9c07, 0x9c0f,                 /* LynxPoint-LP */
	0x9c83,	0x9c85, 0x282a, 0x9c87, 0x282a, 0x9c8f, /* WildcatPoint */
	0
};

static const struct pci_driver pch_sata __pci_driver = {
	.ops	 = &sata_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
