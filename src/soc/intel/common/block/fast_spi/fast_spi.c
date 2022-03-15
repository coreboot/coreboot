/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <device/mmio.h>
#include <assert.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <commonlib/helpers.h>
#include <cpu/x86/mtrr.h>
#include <fast_spi_def.h>
#include <intelblocks/dmi.h>
#include <intelblocks/fast_spi.h>
#include <lib.h>
#include <soc/pci_devs.h>
#include <spi_flash.h>
#include <spi-generic.h>

/*
 * Get the FAST_SPIBAR.
 */
void *fast_spi_get_bar(void)
{
	const pci_devfn_t dev = PCH_DEV_SPI;
	uintptr_t bar;

	bar = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	assert(bar != 0);
	/*
	 * Bits 31-12 are the base address as per EDS for SPI,
	 * Don't care about 0-11 bit
	 */
	return (void *)(bar & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK);
}

/*
 * Disable the BIOS write protect and Enable Prefetching and Caching.
 */
void fast_spi_init(void)
{
	const pci_devfn_t dev = PCH_DEV_SPI;
	uint8_t bios_cntl;

	bios_cntl = pci_read_config8(dev, SPI_BIOS_CONTROL);

	/* Disable the BIOS write protect so write commands are allowed. */
	bios_cntl &= ~SPI_BIOS_CONTROL_EISS;
	bios_cntl |= SPI_BIOS_CONTROL_WPD;
	/* Enable Prefetching and caching. */
	bios_cntl |= SPI_BIOS_CONTROL_PREFETCH_ENABLE;
	bios_cntl &= ~SPI_BIOS_CONTROL_CACHE_DISABLE;

	pci_write_config8(dev, SPI_BIOS_CONTROL, bios_cntl);
}

/*
 * Set FAST_SPIBAR BIOS Control register based on input bit field.
 */
static void fast_spi_set_bios_control_reg(uint32_t bios_cntl_bit)
{
	const pci_devfn_t dev = PCH_DEV_SPI;
	uint32_t bc_cntl;

	assert((bios_cntl_bit & (bios_cntl_bit - 1)) == 0);
	bc_cntl = pci_read_config32(dev, SPI_BIOS_CONTROL);
	bc_cntl |= bios_cntl_bit;
	pci_write_config32(dev, SPI_BIOS_CONTROL, bc_cntl);
}

/*
 * Ensure an additional read back after performing lock down
 */
static void fast_spi_read_post_write(uint8_t reg)
{
	pci_read_config8(PCH_DEV_SPI, reg);
}

/*
 * Set FAST_SPIBAR BIOS Control BILD bit.
 */
void fast_spi_set_bios_interface_lock_down(void)
{
	fast_spi_set_bios_control_reg(SPI_BIOS_CONTROL_BILD);

	fast_spi_read_post_write(SPI_BIOS_CONTROL);
}

/*
 * Set FAST_SPIBAR BIOS Control LE bit.
 */
void fast_spi_set_lock_enable(void)
{
	fast_spi_set_bios_control_reg(SPI_BIOS_CONTROL_LOCK_ENABLE);

	fast_spi_read_post_write(SPI_BIOS_CONTROL);
}

/*
 * Set FAST_SPIBAR BIOS Control EXT BIOS LE bit.
 */
void fast_spi_set_ext_bios_lock_enable(void)
{
	if (!CONFIG(FAST_SPI_SUPPORTS_EXT_BIOS_WINDOW))
		return;

	fast_spi_set_bios_control_reg(SPI_BIOS_CONTROL_EXT_BIOS_LOCK_ENABLE);

	fast_spi_read_post_write(SPI_BIOS_CONTROL);
}

/*
 * Set FAST_SPIBAR BIOS Control EISS bit.
 */
void fast_spi_set_eiss(void)
{
	fast_spi_set_bios_control_reg(SPI_BIOS_CONTROL_EISS);

	fast_spi_read_post_write(SPI_BIOS_CONTROL);
}

/*
 * Set FAST_SPI opcode menu.
 */
void fast_spi_set_opcode_menu(void)
{
	void *spibar = fast_spi_get_bar();

	write16(spibar + SPIBAR_PREOP, SPI_OPPREFIX);
	write16(spibar + SPIBAR_OPTYPE, SPI_OPTYPE);
	write32(spibar + SPIBAR_OPMENU_LOWER, SPI_OPMENU_LOWER);
	write32(spibar + SPIBAR_OPMENU_UPPER, SPI_OPMENU_UPPER);
}

/*
 * Lock FAST_SPIBAR.
 * Use 16bit write to avoid touching two upper bytes what may cause the write
 * cycle to fail in case a prior transaction has not completed.
 * While WRSDIS is lockable with FLOCKDN, writing both in the same
 * cycle is guaranteed to work by design.
 *
 * Avoid read->modify->write not to clear RW1C bits unintentionally.
 */
void fast_spi_lock_bar(void)
{
	void *spibar = fast_spi_get_bar();
	uint16_t hsfs = SPIBAR_HSFSTS_FLOCKDN | SPIBAR_HSFSTS_PRR34_LOCKDN;

	if (CONFIG(FAST_SPI_DISABLE_WRITE_STATUS))
		hsfs |= SPIBAR_HSFSTS_WRSDIS;

	write16(spibar + SPIBAR_HSFSTS_CTL, hsfs);
}

/*
 * Set FAST_SPIBAR + DLOCK (0x0C) register bits to discrete lock the
 * FAST_SPI Protected Range (PR) registers.
 */
void fast_spi_pr_dlock(void)
{
	void *spibar = fast_spi_get_bar();
	uint32_t dlock;

	dlock = read32(spibar + SPIBAR_DLOCK);
	dlock |= (SPIBAR_DLOCK_PR0LOCKDN | SPIBAR_DLOCK_PR1LOCKDN
			| SPIBAR_DLOCK_PR2LOCKDN | SPIBAR_DLOCK_PR3LOCKDN
			| SPIBAR_DLOCK_PR4LOCKDN);

	write32(spibar + SPIBAR_DLOCK, dlock);
}

/*
 * Set FAST_SPIBAR Soft Reset Data Register value.
 */
void fast_spi_set_strap_msg_data(uint32_t soft_reset_data)
{
	void *spibar = fast_spi_get_bar();
	uint32_t ssl, ssms;

	/* Set Strap Lock Disable */
	ssl = read32(spibar + SPIBAR_RESET_LOCK);
	ssl &= ~SPIBAR_RESET_LOCK_ENABLE;
	write32(spibar + SPIBAR_RESET_LOCK, ssl);

	/* Write Soft Reset Data register at SPIBAR0 offset 0xF8[0:15] */
	write32(spibar + SPIBAR_RESET_DATA, soft_reset_data);

	/* Set Strap Mux Select  set to '1' */
	ssms = read32(spibar + SPIBAR_RESET_CTRL);
	ssms |= SPIBAR_RESET_CTRL_SSMC;
	write32(spibar + SPIBAR_RESET_CTRL, ssms);

	/* Set Strap Lock Enable */
	ssl = read32(spibar + SPIBAR_RESET_LOCK);
	ssl |= SPIBAR_RESET_LOCK_ENABLE;
	write32(spibar + SPIBAR_RESET_LOCK, ssl);
}

/*
 * Returns bios_start and fills in size of the BIOS region.
 */
size_t fast_spi_get_bios_region(size_t *bios_size)
{
	size_t bios_start, bios_end;
	/*
	 * BIOS_BFPREG provides info about BIOS Flash Primary Region
	 * Base and Limit.
	 * Base and Limit fields are in units of 4KiB.
	 */
	uint32_t val = read32(fast_spi_get_bar() + SPIBAR_BFPREG);

	bios_start = (val & SPIBAR_BFPREG_PRB_MASK) * 4 * KiB;
	bios_end = (((val & SPIBAR_BFPREG_PRL_MASK) >>
		     SPIBAR_BFPREG_PRL_SHIFT) + 1) * 4 * KiB;
	*bios_size = bios_end - bios_start;
	return bios_start;
}

static bool fast_spi_ext_bios_cache_range(uintptr_t *base, size_t *size)
{
	uint32_t alignment;
	if (!CONFIG(FAST_SPI_SUPPORTS_EXT_BIOS_WINDOW))
		return false;

	fast_spi_get_ext_bios_window(base, size);

	/* Enable extended bios only if Size of Bios region is greater than 16MiB */
	if (*size == 0 || *base == 0)
		return false;

	/* Round to power of two */
	alignment = 1UL << (log2_ceil(*size));
	*size = ALIGN_UP(*size, alignment);
	*base = ALIGN_DOWN(*base, *size);

	return true;
}

static void fast_spi_cache_ext_bios_window(void)
{
	size_t ext_bios_size;
	uintptr_t ext_bios_base;
	const int type = MTRR_TYPE_WRPROT;

	if (!fast_spi_ext_bios_cache_range(&ext_bios_base, &ext_bios_size))
		return;

	if (ENV_PAYLOAD_LOADER) {
		mtrr_use_temp_range(ext_bios_base, ext_bios_size, type);
	} else {
		int mtrr = get_free_var_mtrr();
		if (mtrr == -1)
			return;
		set_var_mtrr(mtrr, ext_bios_base, ext_bios_size, type);
	}
}

void fast_spi_cache_ext_bios_postcar(struct postcar_frame *pcf)
{
	size_t ext_bios_size;
	uintptr_t ext_bios_base;
	const int type = MTRR_TYPE_WRPROT;

	if (!fast_spi_ext_bios_cache_range(&ext_bios_base, &ext_bios_size))
		return;

	postcar_frame_add_mtrr(pcf, ext_bios_base, ext_bios_size, type);
}

void fast_spi_cache_bios_region(void)
{
	size_t bios_size;
	uint32_t alignment;
	const int type = MTRR_TYPE_WRPROT;
	uintptr_t base;

	/* Only the IFD BIOS region is memory mapped (at top of 4G) */
	fast_spi_get_bios_region(&bios_size);

	/* LOCAL APIC default address is 0xFEE0000, bios_size over 16MB will
	 * cause memory type conflict when setting memory type to write
	 * protection, so limit the cached BIOS region to be no more than 16MB.
	 * */
	bios_size = MIN(bios_size, 16 * MiB);
	if (bios_size <= 0)
		return;

	/* Round to power of two */
	alignment = 1UL << (log2_ceil(bios_size));
	bios_size = ALIGN_UP(bios_size, alignment);
	base = 4ULL*GiB - bios_size;

	if (ENV_PAYLOAD_LOADER) {
		mtrr_use_temp_range(base, bios_size, type);
	} else {
		int mtrr = get_free_var_mtrr();

		if (mtrr == -1)
			return;

		set_var_mtrr(mtrr, base, bios_size, type);
	}

	/* Check if caching is needed for extended bios region if supported */
	fast_spi_cache_ext_bios_window();
}

/*
 * Enable extended BIOS support
 * Checks BIOS region in the flashmap, if its more than 16Mib, enables extended BIOS
 * region support.
 */
static void fast_spi_enable_ext_bios(void)
{
	const pci_devfn_t dev = PCH_DEV_SPI;
	if (!CONFIG(FAST_SPI_SUPPORTS_EXT_BIOS_WINDOW))
		return;

#if CONFIG(FAST_SPI_SUPPORTS_EXT_BIOS_WINDOW)
	/*
	 * Ensure that the base for the extended window in host space is a multiple of 32 MiB
	 * and size is fixed at 32 MiB. Controller assumes that the extended window has a fixed
	 * size of 32 MiB even if the actual BIOS region is smaller. The mapping of the BIOS
	 * region happens at the top of the extended window in this case.
	 */
	_Static_assert(ALIGN_UP(CONFIG_EXT_BIOS_WIN_BASE, 32 * MiB) == CONFIG_EXT_BIOS_WIN_BASE,
		       "Extended BIOS window base must be a multiple of 32 * MiB!");
	_Static_assert(CONFIG_EXT_BIOS_WIN_SIZE == (32 * MiB),
		       "Only 32MiB windows are supported for extended BIOS!");
#endif

	/* Configure Source decode for Extended BIOS Region */
	if (dmi_enable_gpmr(CONFIG_EXT_BIOS_WIN_BASE, CONFIG_EXT_BIOS_WIN_SIZE,
				soc_get_spi_psf_destination_id()) == CB_ERR)
		return;

	/* Program EXT_BIOS_BAR1 with obtained ext_bios_base */
	pci_write_config32(dev, SPI_CFG_BAR1,
			   CONFIG_EXT_BIOS_WIN_BASE | PCI_BASE_ADDRESS_SPACE_MEMORY);

	/*
	 * Since the top 16MiB of the BIOS region is always decoded by the standard window
	 * below the 4G boundary, we need to map the rest of the BIOS region that lies
	 * below the top 16MiB in the extended window. Thus, EXT_BIOS_LIMIT will be set to
	 * 16MiB. This determines the maximum address in the SPI flash space that is mapped
	 * to the top of the extended window in the host address space. EXT_BIOS_LIMIT is
	 * basically the offset from the end of the BIOS region that will be mapped to the top
	 * of the extended window.
	 * This enables the decoding as follows:
		-Standard decode window: (bios_region_top - 16MiB) to bios_region_top
		-Extended decode window:
			(bios_region_top - 16MiB - MIN(extended_window_size, bios_size - 16MiB))
			to (bios_region_top - 16MiB).
	 */
	pci_or_config32(dev, SPI_BIOS_CONTROL, SPI_BIOS_CONTROL_EXT_BIOS_LIMIT(16 * MiB));

	/* Program EXT_BIOS EN */
	pci_or_config32(dev, SPI_BIOS_CONTROL, SPI_BIOS_CONTROL_EXT_BIOS_ENABLE);
}

/*
 * Program temporary BAR for SPI in case any of the stages before ramstage need
 * to access FAST_SPI MMIO regs. Ramstage will assign a new BAR during PCI
 * enumeration.
 */
void fast_spi_early_init(uintptr_t spi_base_address)
{
	const pci_devfn_t dev = PCH_DEV_SPI;
	uint16_t pcireg;

	/* Assign Resources to SPI Controller */
	/* Clear BIT 1-2 SPI Command Register */
	pcireg = pci_read_config16(dev, PCI_COMMAND);
	pcireg &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pci_write_config16(dev, PCI_COMMAND, pcireg);

	/* Program Temporary BAR for SPI */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0,
		spi_base_address | PCI_BASE_ADDRESS_SPACE_MEMORY);

	/*
	 * Enable extended bios support. Since it configures memory BAR, this is done before
	 * enabling MMIO space.
	 */
	fast_spi_enable_ext_bios();

	/* Enable Bus Master and MMIO Space */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	/* Initialize SPI to allow BIOS to write/erase on flash. */
	fast_spi_init();
}

/* Clear SPI Synchronous SMI status bit and return its value. */
bool fast_spi_clear_sync_smi_status(void)
{
	const uint32_t bios_cntl = pci_read_config32(PCH_DEV_SPI, SPI_BIOS_CONTROL);
	const bool smi_asserted = bios_cntl & SPI_BIOS_CONTROL_SYNC_SS;
	/*
	 * Do not unconditionally write 1 to clear SYNC_SS. Hardware could set
	 * SYNC_SS here (after we read but before we write SPI_BIOS_CONTROL),
	 * and the event would be lost when unconditionally clearing SYNC_SS.
	 */
	pci_write_config32(PCH_DEV_SPI, SPI_BIOS_CONTROL, bios_cntl);
	return smi_asserted;
}

/* Read SPI Write Protect disable status. */
bool fast_spi_wpd_status(void)
{
	return pci_read_config16(PCH_DEV_SPI, SPI_BIOS_CONTROL) &
		SPI_BIOS_CONTROL_WPD;
}

/* Enable SPI Write Protect. */
void fast_spi_enable_wp(void)
{
	const pci_devfn_t dev = PCH_DEV_SPI;
	uint8_t bios_cntl;

	bios_cntl = pci_read_config8(dev, SPI_BIOS_CONTROL);
	bios_cntl &= ~SPI_BIOS_CONTROL_WPD;
	pci_write_config8(dev, SPI_BIOS_CONTROL, bios_cntl);
}

/* Disable SPI Write Protect. */
void fast_spi_disable_wp(void)
{
	const pci_devfn_t dev = PCH_DEV_SPI;
	uint8_t bios_cntl;

	bios_cntl = pci_read_config8(dev, SPI_BIOS_CONTROL);
	bios_cntl |= SPI_BIOS_CONTROL_WPD;
	pci_write_config8(dev, SPI_BIOS_CONTROL, bios_cntl);
}
