/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SUPERIO_SCH555x_H
#define SUPERIO_SCH555x_H

#include <types.h>

// Global registers
#define SCH555x_DEVICE_ID		0x20
#define SCH555x_DEVICE_REV		0x21
#define SCH555x_DEVICE_MODE		0x24

// Logical device numbers
#define SCH555x_LDN_EMI			0x00
#define SCH555x_LDN_8042		0x01
#define SCH555x_LDN_UART1		0x07
#define SCH555x_LDN_UART2		0x08
#define SCH555x_LDN_RUNTIME		0x0a
#define SCH555x_LDN_FDC			0x0b
#define SCH555x_LDN_LPCI		0x0c
#define SCH555x_LDN_PP			0x11
#define SCH555x_LDN_GLOBAL		0x3f

// LPC interface registers
#define SCH555x_LPCI_IRQ(i)		(0x40 + (i))
// DMA channel register is 2 bytes, we care about the second byte
#define SCH555x_LPCI_DMA(i)		(0x50 + (i) * 2 + 1)
// BAR offset (inside LPCI) for each LDN
#define SCH555x_LPCI_LPCI_BAR		0x60
#define SCH555x_LPCI_EMI_BAR		0x64
#define SCH555x_LPCI_UART1_BAR		0x68
#define SCH555x_LPCI_UART2_BAR		0x6c
#define SCH555x_LPCI_RUNTIME_BAR	0x70
#define SCH555x_LPCI_8042_BAR		0x78
#define SCH555x_LPCI_FDC_BAR		0x7c
#define SCH555x_LPCI_PP_BAR		0x80

// Runtime registers (in I/O space)
#define SCH555x_RUNTIME_PME_STS		0x00
#define SCH555x_RUNTIME_PME_EN		0x01
#define SCH555x_RUNTIME_PME_EN1		0x05
#define SCH555x_RUNTIME_LED		0x25
// NOTE: not in the SCH5627P datasheet but Dell's firmware writes to it
#define SCH555x_RUNTIME_UNK1		0x35

// Needed in the bootblock, thus we map them at a fixed address
#define SCH555x_EMI_IOBASE		0xa00
#define SCH555x_RUNTIME_IOBASE		0xa40

/*
 * EMI access
 */

uint8_t sch555x_emi_read8(uint16_t addr);
uint16_t sch555x_emi_read16(uint16_t addr);
uint32_t sch555x_emi_read32(uint16_t addr);
void sch555x_emi_write8(uint16_t addr, uint8_t val);
void sch555x_emi_write16(uint16_t addr, uint16_t val);
void sch555x_emi_write32(uint16_t addr, uint32_t val);

/*
 * Bootblock entry points
 */

void sch555x_early_init(pnp_devfn_t global_dev);
void sch555x_enable_serial(pnp_devfn_t uart_dev, uint16_t serial_iobase);

#endif
