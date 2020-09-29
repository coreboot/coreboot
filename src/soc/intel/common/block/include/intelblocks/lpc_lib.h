/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_COMMON_BLOCK_LPC_LIB_H_
#define _SOC_COMMON_BLOCK_LPC_LIB_H_

#include <device/device.h>
#include <stddef.h>
#include <stdint.h>

/*
 * IO decode enable macros are in the format IO_<peripheral>_<IO port>.
 * For example, to open ports 0x60, 0x64 for the keyboard controller,
 * use IOE_KBC_60_64 macro. For IOE_ macros that do not specify a port range,
 * the port range is selectable via the IO decodes register.
 */
#define LPC_IOE_EC_4E_4F               (1 << 13)
#define LPC_IOE_SUPERIO_2E_2F          (1 << 12)
#define LPC_IOE_EC_62_66               (1 << 11)
#define LPC_IOE_KBC_60_64              (1 << 10)
#define LPC_IOE_HGE_208                (1 << 9)
#define LPC_IOE_LGE_200                (1 << 8)
#define LPC_IOE_FDD_EN                 (1 << 3)
#define LPC_IOE_LPT_EN                 (1 << 2)
#define LPC_IOE_COMB_EN                (1 << 1)
#define LPC_IOE_COMA_EN                (1 << 0)
#define LPC_NUM_GENERIC_IO_RANGES       4

#define PCR_DMI_LPCLGIR1        0x2730
#define PCR_DMI_LPCLGIR2        0x2734
#define PCR_DMI_LPCLGIR3        0x2738
#define PCR_DMI_LPCLGIR4        0x273c

#define PCR_DMI_LPCIOD          0x2770
#define PCR_DMI_LPCIOE          0x2774

/* LPC PCR configuration */
#define PCR_LPC_PRC            0x341c
#define PCR_LPC_CCE_EN         0xf
#define PCR_LPC_PCE_EN         (9 << 8)

/* Serial IRQ control. SERIRQ_QUIET is the default (0). */
enum serirq_mode {
	SERIRQ_QUIET,
	SERIRQ_CONTINUOUS,
	SERIRQ_OFF,
};

struct lpc_mmio_range {
	uintptr_t base;
	size_t size;
};

/*
 * Enable fixed IO ranges to LPC. IOE_* macros can be OR'ed together.
 * Output:I/O Enable Bits
 */
uint16_t lpc_enable_fixed_io_ranges(uint16_t io_enables);
/* Return the current decode settings */
uint16_t lpc_get_fixed_io_decode(void);
/* Set the current decode ranges */
uint16_t lpc_set_fixed_io_ranges(uint16_t io_ranges, uint16_t mask);
/* Open a generic IO window to the LPC bus. Four windows are available. */
void lpc_open_pmio_window(uint16_t base, uint16_t size);
/* Close all generic IO windows to the LPC bus. */
void lpc_close_pmio_windows(void);
/* Open a generic MMIO window to the LPC bus. One window is available. */
void lpc_open_mmio_window(uintptr_t base, size_t size);
/* Returns true if given window is decoded to LPC via a fixed range. */
bool lpc_fits_fixed_mmio_window(uintptr_t base, size_t size);
/* Init SoC Specific LPC features. Common definition will be weak and
each soc will need to define the init. */
void lpc_soc_init(struct device *dev);
/* Fill up LPC IO resource structure inside SoC directory */
void pch_lpc_soc_fill_io_resources(struct device *dev);
/* Init LPC GPIO pads */
void lpc_configure_pads(void);
/* Get SoC specific MMIO ranges */
const struct lpc_mmio_range *soc_get_fixed_mmio_ranges(void);
/* Set LPC BIOS Control BILD bit. */
void lpc_set_bios_interface_lock_down(void);
/* Set LPC BIOS Control LE bit. */
void lpc_set_lock_enable(void);
/* Set LPC BIOS Control EISS bit. */
void lpc_set_eiss(void);
/* Set LPC Serial IRQ mode. */
void lpc_set_serirq_mode(enum serirq_mode mode);
/* Enable CLKRUN_EN for power gating LPC. */
void lpc_enable_pci_clk_cntl(void);
/* LPC Clock Run is a feature to stop LPC clock unless a peripheral objects. */
void lpc_disable_clkrun(void);
/*
* Setup I/O Decode Range Register for LPC
* ComA Range 3F8h-3FFh [2:0]
* ComB Range 2F8h-2FFh [6:4]
* Enable ComA and ComB Port
*/
void lpc_io_setup_comm_a_b(void);
/* Enable PCH LPC by setting up generic decode range registers. */
void pch_enable_lpc(void);
/* Get SoC's generic IO decoder range register settings. */
void soc_get_gen_io_dec_range(const struct device *dev,
			uint32_t gen_io_dec[LPC_NUM_GENERIC_IO_RANGES]);
/* Mirror generic IO decoder range register settings into DMI PCR. */
void soc_setup_dmi_pcr_io_dec(uint32_t gen_io_dec[LPC_NUM_GENERIC_IO_RANGES]);
/* Add resource into LPC PCI device space */
void pch_lpc_add_new_resource(struct device *dev, uint8_t offset,
	uintptr_t base, size_t size, unsigned long flags);
/* Enable PCH IOAPIC */
void pch_enable_ioapic(void);
/* Retrieve and setup PCH LPC interrupt routing. */
void pch_pirq_init(void);

#endif /* _SOC_COMMON_BLOCK_LPC_LIB_H_ */
