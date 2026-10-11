/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_AMPERE_ALTRA_ADDRESSMAP_H
#define SOC_AMPERE_ALTRA_ADDRESSMAP_H

/*
 * Ampere Altra (AC01) address map, see "Altra ATF Interface Specification"
 * (DRAM layout) and edk2-platforms AmpereAltraPkg (Platform/Ac01.h,
 * ArmPlatformLib/PlatformMemoryMap.h).
 */

/*
 * 0x8000_0000 - 0x8fff_ffff belongs to TF-A (secure memory and buffers
 * shared with the secure world), except for the MM communication buffer at
 * 0x8830_0000, which coreboot uses itself.
 */
#define AC01_NS_DRAM_START		0x90000000

/* TF-A hands BL33 a HOB list with the platform information at 0x9000_0000 */
#define AC01_TFA_HOB_BASE		0x90000000
#define AC01_TFA_HOB_SIZE		(512 * KiB)

/* SoC peripherals of socket 0 (UART, SMpro/PMpro doorbells, eFuse shadow, ...) */
#define AC01_DEVICE_MEMORY_S0_BASE	0x100000000000
#define AC01_DEVICE_MEMORY_S0_SIZE	0x102000000

#define AC01_UART0_BASE			0x100002600000
#define AC01_UART1_BASE			0x100002620000

/* Input only GPIOs 24-31 of socket 0, their levels are in the register at offset 0 */
#define AC01_GPI_BASE			0x1000026d0000

/*
 * SMpro/PMpro doorbells: 8 per processor, 4 KiB apart. Their interrupts are
 * SPI 40-47 (SMpro) and 56-63 (PMpro).
 */
#define AC01_SMPRO_DB_BASE		0x100000540000
#define AC01_PMPRO_DB_BASE		0x100001540000
#define AC01_DB_STRIDE			0x1000
#define AC01_SMPRO_DB0_IRQ		40
#define AC01_PMPRO_DB0_IRQ		56
#define AC01_SMPRO_EFUSE_SHADOW0	0x10000054a000

/* GICv3 of socket 0, with one ITS per PCIe root complex */
#define AC01_GICD_BASE			0x100100000000
#define AC01_GICR_BASE			0x100100140000
#define AC01_GICR_SIZE			0x1000000
#define AC01_GIC_ITS_BASE(rc)		(AC01_GICD_BASE + 0x40000 + (rc) * 0x20000)

/* Memory mapped generic timer (CNTCTLBase, CNTBaseN) and generic watchdog */
#define AC01_GT_CTL_BASE		0x100002700000
#define AC01_GT_FRAME_BASE(n)		(0x100002710000 + (n) * 0x10000)
#define AC01_GWDT_CONTROL_BASE		0x1000027C0000
#define AC01_GWDT_REFRESH_BASE		0x1000027D0000

/*
 * PCIe root complexes of socket 0: CSR base of each root complex. The CSR
 * block (512 MiB) contains the host bridge, controller and SerDes registers
 * and, at offset 0x1000_0000, the ECAM window (256 buses).
 */
#define AC01_PCIE_CSR_BASE_LIST		0x33FFE0000000, 0x37FFE0000000, 0x3BFFE0000000, \
					0x3FFFE0000000, 0x23FFE0000000, 0x27FFE0000000, \
					0x2BFFE0000000, 0x2FFFE0000000
#define AC01_PCIE_CSR_SIZE		0x20000000
#define AC01_PCIE_ECAM_OFFSET		0x10000000
#define AC01_PCIE_ECAM_SIZE		0x10000000

/* 64-bit MMIO windows (identity mapped) */
#define AC01_PCIE_MMIO_BASE_LIST	0x300000000000, 0x340000000000, 0x380000000000, \
					0x3C0000000000, 0x200000000000, 0x240000000000, \
					0x280000000000, 0x2C0000000000
#define AC01_PCIE_MMIO_SIZE		0x3FFE0000000

/* 32-bit MMIO windows in a single socket system (identity mapped) */
#define AC01_PCIE_MMIO32_BASE_1P_LIST	0x40000000, 0x50000000, 0x60000000, 0x70000000, \
					0x08000000, 0x10000000, 0x20000000, 0x30000000
#define AC01_PCIE_MMIO32_SIZE_1P_LIST	0x10000000, 0x10000000, 0x10000000, 0x10000000, \
					0x08000000, 0x10000000, 0x10000000, 0x10000000

/* The top of the 32-bit MMIO window is reserved for the I/O space (unused) */
#define AC01_PCIE_IO_SIZE		0x2000

#endif
