/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <arch/ioapic.h>
#include <assert.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/gpmr.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <lib.h>
#include "lpc_def.h"
#include <soc/irq.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>

uint16_t lpc_enable_fixed_io_ranges(uint16_t io_enables)
{
	uint16_t reg_io_enables;

	reg_io_enables = pci_read_config16(PCH_DEV_LPC, LPC_IO_ENABLES);
	io_enables |= reg_io_enables;
	pci_write_config16(PCH_DEV_LPC, LPC_IO_ENABLES, io_enables);
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_LPC_MIRROR_TO_GPMR))
		gpmr_write32(GPMR_LPCIOE, io_enables);

	return io_enables;
}

uint16_t lpc_get_fixed_io_decode(void)
{
	return pci_read_config16(PCH_DEV_LPC, LPC_IO_DECODE);
}

uint16_t lpc_set_fixed_io_ranges(uint16_t io_ranges, uint16_t mask)
{
	uint16_t reg_io_ranges;

	reg_io_ranges = lpc_get_fixed_io_decode() & ~mask;
	io_ranges |= reg_io_ranges & mask;
	pci_write_config16(PCH_DEV_LPC, LPC_IO_DECODE, io_ranges);
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_LPC_MIRROR_TO_GPMR))
		gpmr_write32(GPMR_LPCIOD, io_ranges);

	return io_ranges;
}

/*
 * Find the first unused IO window.
 * Returns -1 if not found, 0 for reg 0x84, 1 for reg 0x88 ...
 */
static int find_unused_pmio_window(void)
{
	int i;
	uint32_t lgir;

	for (i = 0; i < LPC_NUM_GENERIC_IO_RANGES; i++) {
		lgir = pci_read_config32(PCH_DEV_LPC, LPC_GENERIC_IO_RANGE(i));

		if (!(lgir & LPC_LGIR_EN))
			return i;
	}

	return -1;
}

void lpc_open_pmio_window(uint16_t base, uint16_t size)
{
	int i, lgir_reg_num;
	uint32_t lgir_reg_offset, lgir, window_size, alignment;
	resource_t bridged_size, bridge_base;

	printk(BIOS_SPEW, "LPC: Trying to open IO window from %x size %x\n",
				base, size);

	bridged_size = 0;
	bridge_base = base;

	while (bridged_size < size) {
		/* Each IO range register can only open a 256-byte window. */
		window_size = MIN(size, LPC_LGIR_MAX_WINDOW_SIZE);

		if (window_size <= 0)
			return;

		/* Window size must be a power of two for the AMASK to work. */
		alignment = 1UL << (log2_ceil(window_size));
		window_size = ALIGN_UP(window_size, alignment);

		/* Address[15:2] in LGIR[15:12] and Mask[7:2] in LGIR[23:18]. */
		lgir = (bridge_base & LPC_LGIR_ADDR_MASK) | LPC_LGIR_EN;
		lgir |= ((window_size - 1) << 16) & LPC_LGIR_AMASK_MASK;

		/* Skip programming if same range already programmed. */
		for (i = 0; i < LPC_NUM_GENERIC_IO_RANGES; i++) {
			if (lgir == pci_read_config32(PCH_DEV_LPC,
						LPC_GENERIC_IO_RANGE(i)))
				return;
		}

		lgir_reg_num = find_unused_pmio_window();
		if (lgir_reg_num < 0) {
			printk(BIOS_ERR,
				"LPC: Cannot open IO window: %llx size %llx\n",
				bridge_base, size - bridged_size);
			printk(BIOS_ERR, "No more IO windows\n");
			return;
		}
		lgir_reg_offset = LPC_GENERIC_IO_RANGE(lgir_reg_num);

		pci_write_config32(PCH_DEV_LPC, lgir_reg_offset, lgir);
		if (CONFIG(SOC_INTEL_COMMON_BLOCK_LPC_MIRROR_TO_GPMR))
			gpmr_write32(GPMR_LPCLGIR1 + lgir_reg_num * 4, lgir);

		printk(BIOS_DEBUG,
		       "LPC: Opened IO window LGIR%d: base %llx size %x\n",
		       lgir_reg_num, bridge_base, window_size);

		bridged_size += window_size;
		bridge_base += window_size;
	}
}

void lpc_open_mmio_window(uintptr_t base, size_t size)
{
	uint32_t lgmr;

	lgmr = pci_read_config32(PCH_DEV_LPC, LPC_GENERIC_MEM_RANGE);

	if (lgmr & LPC_LGMR_EN) {
		printk(BIOS_ERR,
		       "LPC: Cannot open window to resource %lx size %zx\n",
		       base, size);
		printk(BIOS_ERR, "LPC: MMIO window already in use\n");
		return;
	}

	if (size > LPC_LGMR_WINDOW_SIZE) {
		printk(BIOS_WARNING,
		       "LPC:  Resource %lx size %zx larger than window(%x)\n",
		       base, size, LPC_LGMR_WINDOW_SIZE);
	}

	lgmr = (base & LPC_LGMR_ADDR_MASK) | LPC_LGMR_EN;

	pci_write_config32(PCH_DEV_LPC, LPC_GENERIC_MEM_RANGE, lgmr);
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_LPC_MIRROR_TO_GPMR))
		gpmr_write32(GPMR_LPCGMR, lgmr);
}

/*
 * Set LPC BIOS Control register based on input bit field.
 */
static void lpc_set_bios_control_reg(uint8_t bios_cntl_bit)
{
	pci_devfn_t dev = PCH_DEV_LPC;
	uint8_t bc_cntl;

	assert(IS_POWER_OF_2(bios_cntl_bit));
	bc_cntl = pci_read_config8(dev, LPC_BIOS_CNTL);
	bc_cntl |= bios_cntl_bit;
	pci_write_config8(dev, LPC_BIOS_CNTL, bc_cntl);

	/*
	* Ensure an additional read back after performing lock down
	*/
	pci_read_config8(PCH_DEV_LPC, LPC_BIOS_CNTL);
}

/*
* Set LPC BIOS Control BILD bit.
*/
void lpc_set_bios_interface_lock_down(void)
{
	lpc_set_bios_control_reg(LPC_BC_BILD);
}

/*
* Set LPC BIOS Control LE bit.
*/
void lpc_set_lock_enable(void)
{
	lpc_set_bios_control_reg(LPC_BC_LE);
}

/*
* Set LPC BIOS Control EISS bit.
*/
void lpc_set_eiss(void)
{
	lpc_set_bios_control_reg(LPC_BC_EISS);
}

static void lpc_configure_write_protect(bool status)
{
	const pci_devfn_t dev = PCH_DEV_LPC;
	uint8_t bios_cntl;

	bios_cntl = pci_read_config8(dev, LPC_BIOS_CNTL);
	if (status)
		bios_cntl &= ~LPC_BC_WPD;
	else
		bios_cntl |= LPC_BC_WPD;
	pci_write_config8(dev, LPC_BIOS_CNTL, bios_cntl);
}

/* Enable LPC Write Protect. */
void lpc_enable_wp(void)
{
	lpc_configure_write_protect(true);
}

/* Disable LPC Write Protect. */
void lpc_disable_wp(void)
{
	lpc_configure_write_protect(false);
}

/*
* Set LPC Serial IRQ mode.
*/
void lpc_set_serirq_mode(enum serirq_mode mode)
{
	pci_devfn_t dev = PCH_DEV_LPC;
	uint8_t scnt;

	scnt = pci_read_config8(dev, LPC_SERIRQ_CTL);
	scnt &= ~(LPC_SCNT_EN | LPC_SCNT_MODE);

	switch (mode) {
	case SERIRQ_QUIET:
		scnt |= LPC_SCNT_EN;
		break;
	case SERIRQ_CONTINUOUS:
		scnt |= LPC_SCNT_EN | LPC_SCNT_MODE;
		break;
	case SERIRQ_OFF:
	default:
		break;
	}

	pci_write_config8(dev, LPC_SERIRQ_CTL, scnt);
}

void lpc_io_setup_comm_a_b(void)
{
	/* ComA Range 3F8h-3FFh [2:0] */
	uint16_t com_ranges = LPC_IOD_COMA_RANGE;
	uint16_t com_enable = LPC_IOE_COMA_EN;
	uint16_t com_mask   = LPC_IOD_COMA_RANGE_MASK;

	/* ComB Range 2F8h-2FFh [6:4] */
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_LPC_COMB_ENABLE)) {
		com_ranges |= LPC_IOD_COMB_RANGE;
		com_enable |= LPC_IOE_COMB_EN;
		com_mask   |= LPC_IOD_COMB_RANGE_MASK;
	}

	/* Setup I/O Decode Range Register for LPC */
	lpc_set_fixed_io_ranges(com_ranges, com_mask);
	/* Enable ComA and ComB Port */
	lpc_enable_fixed_io_ranges(com_enable);
}

static void lpc_set_gen_decode_range(
	uint32_t gen_io_dec[LPC_NUM_GENERIC_IO_RANGES])
{
	size_t i;

	/* Set in PCI generic decode range registers */
	for (i = 0; i < LPC_NUM_GENERIC_IO_RANGES; i++) {
		pci_write_config32(PCH_DEV_LPC, LPC_GENERIC_IO_RANGE(i), gen_io_dec[i]);
		if (CONFIG(SOC_INTEL_COMMON_BLOCK_LPC_MIRROR_TO_GPMR))
			gpmr_write32(GPMR_LPCLGIR1 + i * 4, gen_io_dec[i]);
	}
}

void pch_enable_lpc(void)
{
	uint32_t gen_io_dec[LPC_NUM_GENERIC_IO_RANGES];

	soc_get_gen_io_dec_range(gen_io_dec);
	lpc_set_gen_decode_range(gen_io_dec);
	if (ENV_PAYLOAD_LOADER)
		pch_pirq_init();
}

void lpc_enable_pci_clk_cntl(void)
{
	pci_write_config8(PCH_DEV_LPC, LPC_PCCTL, LPC_PCCTL_CLKRUN_EN);
}

void lpc_disable_clkrun(void)
{
	const uint8_t pcctl = pci_read_config8(PCH_DEV_LPC, LPC_PCCTL);
	pci_write_config8(PCH_DEV_LPC, LPC_PCCTL, pcctl & ~LPC_PCCTL_CLKRUN_EN);
}

/* PCH I/O APIC redirection entries */
#define PCH_REDIR_ETR 120

/* Enable PCH IOAPIC */
void pch_enable_ioapic(void)
{
	/* affirm full set of redirection table entries ("write once") */
	ioapic_set_max_vectors(VIO_APIC_VADDR, PCH_REDIR_ETR);

	setup_ioapic((void *)IO_APIC_ADDR, 0x02);
}

static const uint8_t pch_interrupt_routing[PIRQ_COUNT] = {
	[0] = PCH_IRQ11,	/* PIRQ_A */
	[1] = PCH_IRQ10,	/* PIRQ_B */
	[2] = PCH_IRQ11,	/* PIRQ_C */
	[3] = PCH_IRQ11,	/* PIRQ_D */
	[4] = PCH_IRQ11,	/* PIRQ_E */
	[5] = PCH_IRQ11,	/* PIRQ_F */
	[6] = PCH_IRQ11,	/* PIRQ_G */
	[7] = PCH_IRQ11,	/* PIRQ_H */
};

const uint8_t *lpc_get_pic_pirq_routing(size_t *num)
{
	*num = ARRAY_SIZE(pch_interrupt_routing);
	return pch_interrupt_routing;
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
void pch_pirq_init(void)
{
	const struct device *irq_dev;
	itss_irq_init(pch_interrupt_routing);

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		uint8_t int_pin = 0, int_line = 0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(PCI_BDF(irq_dev), PCI_INTERRUPT_PIN);

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

		pci_write_config8(PCI_BDF(irq_dev), PCI_INTERRUPT_LINE, int_line);
	}
}

#define PPI_PORT_B 0x61
#define  SERR_DIS  (1 << 2)
#define CMOS_NMI   0x70
#define  NMI_DIS   (1 << 7)

/* LPC MISC programming */
void pch_misc_init(void)
{
	uint8_t reg8;

	/* Setup NMI on errors, disable SERR */
	reg8 = (inb(PPI_PORT_B)) & 0xf0;
	outb((reg8 | SERR_DIS), PPI_PORT_B);

	/* Disable NMI sources */
	outb(NMI_DIS, CMOS_NMI);
}
