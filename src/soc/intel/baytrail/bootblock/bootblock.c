/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <soc/iosf.h>
#include <soc/iomap.h>
#include <soc/gpio.h>
#include <soc/lpc.h>
#include <soc/spi.h>
#include <soc/pm.h>

static void program_base_addresses(void)
{
	uint32_t reg;
	const uint32_t lpc_dev = PCI_DEV(0, LPC_DEV, LPC_FUNC);

	/* Memory Mapped IO registers. */
	reg = PMC_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, PBASE, reg);
	reg = IO_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, IOBASE, reg);
	reg = ILB_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, IBASE, reg);
	reg = SPI_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, SBASE, reg);
	reg = MPHY_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, MPBASE, reg);
	reg = PUNIT_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, PUBASE, reg);
	reg = RCBA_BASE_ADDRESS | 1;
	pci_write_config32(lpc_dev, RCBA, reg);

	/* IO Port Registers. */
	reg = ACPI_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, ABASE, reg);
	reg = GPIO_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, GBASE, reg);
}

static void tco_disable(void)
{
	uint32_t reg;

	reg = inl(ACPI_BASE_ADDRESS + TCO1_CNT);
	reg |= TCO_TMR_HALT;
	outl(reg, ACPI_BASE_ADDRESS + TCO1_CNT);
}

static void spi_init(void)
{
	void *scs = (void *)(SPI_BASE_ADDRESS + SCS);
	void *bcr = (void *)(SPI_BASE_ADDRESS + BCR);
	uint32_t reg;

	/* Disable generating SMI when setting WPD bit. */
	write32(scs, read32(scs) & ~SMIWPEN);
	/*
	 * Enable caching and prefetching in the SPI controller. Disable
	 * the SMM-only BIOS write and set WPD bit.
	 */
	reg = (read32(bcr) & ~SRC_MASK) | SRC_CACHE_PREFETCH | BCR_WPD;
	reg &= ~EISS;
	write32(bcr, reg);
}

static void byt_config_com1_and_enable(void)
{
	uint32_t reg;

	/* Enable the UART hardware for COM1. */
	reg = 1;
	pci_write_config32(PCI_DEV(0, LPC_DEV, 0), UART_CONT, reg);

	/* Set up the pads to select the UART function */
	score_select_func(UART_RXD_PAD, 1);
	score_select_func(UART_TXD_PAD, 1);
}

static void setup_mmconfig(void)
{
	uint32_t reg;

	/*
	 * Set up the MMCONF range. The register lives in the BUNIT. The IO variant of the
	 * config access needs to be used initially to properly configure as the IOSF access
	 * registers live in PCI config space.
	 */
	reg = 0;
	/* Clear the extended register. */
	pci_io_write_config32(IOSF_PCI_DEV, MCRX_REG, reg);
	reg = CONFIG_MMCONF_BASE_ADDRESS | 1;
	pci_io_write_config32(IOSF_PCI_DEV, MDR_REG, reg);
	reg = IOSF_OPCODE(IOSF_OP_WRITE_BUNIT) | IOSF_PORT(IOSF_PORT_BUNIT) |
	      IOSF_REG(BUNIT_MMCONF_REG) | IOSF_BYTE_EN;
	pci_io_write_config32(IOSF_PCI_DEV, MCR_REG, reg);
}

/* The distinction between nb/sb/cpu is not applicable here so
   just pick the one that is called first. */
void bootblock_early_northbridge_init(void)
{
	/* Allow memory-mapped PCI config access */
	setup_mmconfig();

	/* Early chipset initialization */
	program_base_addresses();
	tco_disable();

	if (CONFIG(ENABLE_BUILTIN_COM1))
		byt_config_com1_and_enable();

	spi_init();
}
