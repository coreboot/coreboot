/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD_BLOCK_LPC_H__
#define __AMD_BLOCK_LPC_H__

#include <types.h>

/* PCI registers for D14F3 */
#define LPC_PCI_CONTROL			0x40
#define   LEGACY_DMA_EN			BIT(2)
#define   VW_ROM_SHARING_EN		BIT(3)
#define   EXT_ROM_SHARING_EN		BIT(4)

#define LPC_IO_PORT_DECODE_ENABLE	0x44
#define   DECODE_ENABLE_PARALLEL_PORT0	BIT(0)
#define   DECODE_ENABLE_PARALLEL_PORT1	BIT(1)
#define   DECODE_ENABLE_PARALLEL_PORT2	BIT(2)
#define   DECODE_ENABLE_PARALLEL_PORT3	BIT(3)
#define   DECODE_ENABLE_PARALLEL_PORT4	BIT(4)
#define   DECODE_ENABLE_PARALLEL_PORT5	BIT(5)
#define   DECODE_ENABLE_SERIAL_PORT0	BIT(6)
#define   DECODE_ENABLE_SERIAL_PORT1	BIT(7)
#define   DECODE_ENABLE_SERIAL_PORT2	BIT(8)
#define   DECODE_ENABLE_SERIAL_PORT3	BIT(9)
#define   DECODE_ENABLE_SERIAL_PORT4	BIT(10)
#define   DECODE_ENABLE_SERIAL_PORT5	BIT(11)
#define   DECODE_ENABLE_SERIAL_PORT6	BIT(12)
#define   DECODE_ENABLE_SERIAL_PORT7	BIT(13)
#define   DECODE_ENABLE_AUDIO_PORT0	BIT(14)
#define   DECODE_ENABLE_AUDIO_PORT1	BIT(15)
#define   DECODE_ENABLE_AUDIO_PORT2	BIT(16)
#define   DECODE_ENABLE_AUDIO_PORT3	BIT(17)
#define   DECODE_ENABLE_MIDI_PORT0	BIT(18)
#define   DECODE_ENABLE_MIDI_PORT1	BIT(19)
#define   DECODE_ENABLE_MIDI_PORT2	BIT(20)
#define   DECODE_ENABLE_MIDI_PORT3	BIT(21)
#define   DECODE_ENABLE_MSS_PORT0	BIT(22)
#define   DECODE_ENABLE_MSS_PORT1	BIT(23)
#define   DECODE_ENABLE_MSS_PORT2	BIT(24)
#define   DECODE_ENABLE_MSS_PORT3	BIT(25)
#define   DECODE_ENABLE_FDC_PORT0	BIT(26)
#define   DECODE_ENABLE_FDC_PORT1	BIT(27)
#define   DECODE_ENABLE_GAME_PORT	BIT(28)
#define   DECODE_ENABLE_KBC_PORT	BIT(29)
#define   DECODE_ENABLE_ACPIUC_PORT	BIT(30)
#define   DECODE_ENABLE_ADLIB_PORT	BIT(31)

#define LPC_IO_OR_MEM_DECODE_ENABLE	0x48
#define   LPC_WIDEIO2_ENABLE		BIT(25)
#define   LPC_WIDEIO1_ENABLE		BIT(24)
#define   DECODE_IO_PORT_ENABLE6	BIT(23)
#define   DECODE_IO_PORT_ENABLE5	BIT(22)
#define   DECODE_IO_PORT_ENABLE4	BIT(21)
#define   DECODE_MEM_PORT_ENABLE1	BIT(20)
#define   DECODE_IO_PORT_ENABLE3	BIT(19)
#define   DECODE_IO_PORT_ENABLE2	BIT(18)
#define   DECODE_IO_PORT_ENABLE1	BIT(17)
#define   DECODE_IO_PORT_ENABLE0	BIT(16)
#define   LPC_SYNC_TIMEOUT_COUNT_MASK	(0xff << 8)
#define   LPC_SYNC_TIMEOUT_COUNT_ENABLE	BIT(7)
#define   LPC_DECODE_RTC_IO_ENABLE	BIT(6)
#define   DECODE_MEM_PORT_ENABLE0	BIT(5)
#define   LPC_WIDEIO0_ENABLE		BIT(2)
#define   DECODE_ALTERNATE_SIO_ENABLE	BIT(1)
#define   DECODE_SIO_ENABLE		BIT(0)
#define     LPC_SELECT_SIO_4E4F		1
#define     LPC_SELECT_SIO_2E2F		0
#define   WIDEIO_RANGE_ERROR		-1

/* Assuming word access to higher word (register 0x4a) */
#define LPC_IO_OR_MEM_DEC_EN_HIGH	0x4a
#define   LPC_WIDEIO2_ENABLE_H		BIT(9)
#define   LPC_WIDEIO1_ENABLE_H		BIT(8)
#define   DECODE_IO_PORT_ENABLE6_H	BIT(7)
#define   DECODE_IO_PORT_ENABLE5_H	BIT(6)
#define   DECODE_IO_PORT_ENABLE4_H	BIT(5)
#define   DECODE_IO_PORT_ENABLE3_H	BIT(3)
#define   DECODE_IO_PORT_ENABLE2_H	BIT(2)
#define   DECODE_IO_PORT_ENABLE1_H	BIT(1)
#define   DECODE_IO_PORT_ENABLE0_H	BIT(0)

#define LPC_MEM_PORT1			0x4c
#define ROM_PROTECT_RANGE0		0x50
#define   ROM_BASE_MASK			0xfffff000		/* bits 31-12 */
#define   ROM_RANGE_WP			BIT(10)
#define   ROM_RANGE_RP			BIT(9)
#define   RANGE_UNIT			BIT(8)
#define   RANGE_ADDR_MASK		0x000000ff		/* Range defined by bits 7-0 */
#define ROM_PROTECT_RANGE_REG(n)	(ROM_PROTECT_RANGE0 + (4 * n))
#define MAX_ROM_PROTECT_RANGES		4
#define LPC_MEM_PORT0			0x60

/* Register 0x64 is 32-bit, composed by two 16-bit sub-registers.
   For ease of access, each sub-register is declared separately. */
#define LPC_WIDEIO_GENERIC_PORT		0x64
#define LPC_WIDEIO1_GENERIC_PORT	0x66
#define ROM_ADDRESS_RANGE1_START	0x68
#define ROM_ADDRESS_RANGE1_END		0x6a
#define ROM_ADDRESS_RANGE2_START	0x6c
#define ROM_ADDRESS_RANGE2_END		0x6e

#define LPC_ALT_WIDEIO_RANGE_ENABLE	0x74
#define   LPC_ALT_WIDEIO2_ENABLE	BIT(3)
#define   LPC_ALT_WIDEIO1_ENABLE	BIT(2)
#define   LPC_ALT_WIDEIO0_ENABLE	BIT(0)

#define LPC_MISC_CONTROL_BITS		0x78
#define   LPC_NOHOG			BIT(0)

#define LPC_TRUSTED_PLATFORM_MODULE	0x7c
#define   TPM_12_EN			BIT(0)
#define   TPM_LEGACY_EN			BIT(2)

#define LPC_WIDEIO2_GENERIC_PORT	0x90

#define SPIROM_BASE_ADDRESS_REGISTER	0xa0
#define SPI_BASE_ALIGNMENT		BIT(6)
#define   SPI_BASE_RESERVED		(BIT(4) | BIT(5))
#define   ROUTE_TPM_2_SPI		BIT(3)
#define   SPI_ABORT_ENABLE		BIT(2)
#define   SPI_ROM_ENABLE		BIT(1)
#define   SPI_ROM_ALT_ENABLE		BIT(0)
#define   SPI_PRESERVE_BITS		(BIT(0) | BIT(1) | BIT(2) | BIT(3))

/* LPC register 0xb8 is DWORD, here there are definitions for byte
   access. For example, bits 31-24 are accessed through byte access
   at register 0xbb. */
#define LPC_ROM_DMA_EC_HOST_CONTROL	0xb8
#define   SPI_FROM_HOST_PREFETCH_EN	BIT(24)
#define   SPI_FROM_USB_PREFETCH_EN	BIT(23)

#define LPC_HOST_CONTROL		0xbb
#define   PREFETCH_EN_SPI_FROM_HOST	BIT(0)
#define   T_START_ENH			BIT(3)

/* Clear all decoding to the LPC bus and erase any range registers associated
 * with the enable bits. */
void lpc_disable_decodes(void);
/* LPC is typically enabled very early, but this function is last opportunity */
void soc_late_lpc_bridge_enable(void);
void lpc_enable_port80(void);
void lpc_enable_pci_port80(void);
void lpc_enable_decode(uint32_t decodes);
/* addr = index/data to enable:  LPC_SELECT_SIO_2E2F or LPC_SELECT_SIO_4E4F */
void lpc_enable_sio_decode(const bool addr);
uintptr_t lpc_spibase(void);
void lpc_tpm_decode(void);
void lpc_tpm_decode_spi(void);
void lpc_enable_rom(void);
void lpc_enable_spi_prefetch(void);
void lpc_disable_spi_rom_sharing(void);

/**
 * @brief Find the size of a particular wide IO
 *
 * @param index = index of desired wide IO
 *
 * @return size of desired wide IO
 */
uint16_t lpc_wideio_size(int index);
/**
 * @brief Identify if any LPC wide IO is covering the IO range
 *
 * @param start = start of IO range
 * @param size = size of IO range
 *
 * @return Index of wide IO covering the range or error
 */
int lpc_find_wideio_range(uint16_t start, uint16_t size);
/**
 * @brief Program a LPC wide IO to support an IO range
 *
 * @param start = start of range to be routed through wide IO
 * @param size = size of range to be routed through wide IO
 *
 * @return Index of wide IO register used or error
 */
int lpc_set_wideio_range(uint16_t start, uint16_t size);

uintptr_t lpc_get_spibase(void);

/*
 * Perform early initialization for LPC:
 * 1. Enable LPC controller
 * 2. Disable any LPC decodes
 * 3. Set SPI Base which is the MMIO base for both SPI and eSPI controller (if supported by
 * platform).
 */
void lpc_early_init(void);

/*
 * Sets MMIO base address for SPI controller and eSPI controller (if supported by platform).
 *
 * eSPI base = SPI base + 0x10000
 */
void lpc_set_spibase(uint32_t base);

/* Enable SPI ROM (SPI_ROM_ENABLE, SPI_ROM_ALT_ENABLE) */
void lpc_enable_spi_rom(uint32_t enable);

#endif /* __AMD_BLOCK_LPC_H__ */
