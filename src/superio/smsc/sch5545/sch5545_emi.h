/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SUPERIO_SCH_5545_EMI_H
#define SUPERIO_SCH_5545_EMI_H

#include <stdlib.h>
#include <types.h>

/* Embedded Memory Interface registers */
#define SCH5545_EMI_HOST_TO_EC_MAILBOX		0x0
#define SCH5545_EMI_EC_TO_HOST_MAILBOX		0x1
#define SCH5545_EMI_EC_ADDR			0x2
#define SCH5545_EMI_EC_DATA			0x4
#define SCH5545_EMI_INT_SOURCE			0x8
#define SCH5545_EMI_INT_MASK			0xa

#define EMI_EC_8BIT_ACCESS			0
#define EMI_EC_16BIT_ACCESS			1
#define EMI_EC_32BIT_ACCESS			2
#define EMI_EC_32BIT_AUTO_ACCESS		3

/**
 * Reads and returns the base address of EMI from the SuperIO.
 */
uint16_t sch5545_read_emi_bar(uint8_t sio_port);
/**
 * One must call this function at every stage before using any of the EMI
 * functions. The base address of EMI interface must not be zero.
 */
void sch5545_emi_init(uint8_t sio_port);
/**
 * Reads the EC to Host mailbox register and then writes the same content to
 * clear it.
 */
void sch5545_emi_ec2h_mailbox_clear(void);
/**
 * Writes the interrupt mask register with 0.
 */
void sch5545_emi_disable_interrupts(void);
/**
 * Writes the Host to EC mailbox 8bit register with mbox_message.
 */
void sch5545_emi_h2ec_mbox_write(uint8_t mbox_message);
/**
 * Reads and returns the Host to EC mailbox 8bit register.
 */
uint8_t sch5545_emi_h2ec_mbox_read(void);
/**
 * Writes the EC to Host mailbox 8bit register with mbox_message.
 */
void sch5545_emi_ec2h_mbox_write(uint8_t mbox_message);
/**
 * Reads and returns the EC to Host mailbox 8bit register.
 */
uint8_t sch5545_emi_ec2h_mbox_read(void);
/**
 * Sets the mask for all EC interrupts.
 */
void sch5545_emi_set_int_mask(uint16_t mask);
/**
 * Sets the EC interrupt mask for LSB in the Interrupt Mask register.
 */
void sch5545_emi_set_int_mask_low(uint8_t mask);
/**
 * Sets the EC interrupt mask for MSB in the Interrupt Mask register.
 */
void sch5545_emi_set_int_mask_high(uint8_t mask);
/**
 * Returns LSB of Interrupt mask register.
 */
uint8_t sch5545_emi_get_int_mask_low(void);
/**
 * Returns MSB of Interrupt mask register.
 */
uint8_t sch5545_emi_get_int_mask_high(void);
/**
 * Returns the content of interrupt mask register.
 */
uint16_t sch5545_emi_get_int_mask(void);
/**
 * Clears the interrupt status bits.
 */
void sch5545_emi_clear_int_src(void);
/**
 * Writes int_src bits to clear the desired interrupt source LSB.
 */
void sch5545_emi_set_int_src_low(uint8_t int_src);
/**
 * Writes int_src bits to clear the desired interrupt source MSB.
 */
void sch5545_emi_set_int_src_high(uint8_t int_src);
/**
 * Writes int_src bits to clear the desired interrupt source bits.
 */
void sch5545_emi_set_int_src(uint16_t int_src);
/**
 * Returns LSB of interrupt source register.
 */
uint8_t sch5545_emi_get_int_src_low(void);
/**
 * Returns MSB of interrupt source register.
 */
uint8_t sch5545_emi_get_int_src_high(void);
/**
 * Returns the content of interrupt source register.
 */
uint16_t sch5545_emi_get_int_src(void);
/**
 * Sets the EC address registers with given addr for indirect access to
 * Embedded Memory.
 */
void sch5545_emi_set_ec_addr(uint16_t addr);
/**
 * Return the current EC address used for indirect access to Embedded Memory.
 */
uint16_t sch5545_emi_read_ec_addr(void);
/**
 * Writes any byte of 4 bytes from the 32bit dword indicated by addr. The
 * function will automatically align to the matching 32bit dword.
 */
void sch5545_emi_ec_write8(uint16_t addr, uint8_t data);
/**
 * Writes any word of 2 words from the 32bit dword indicated by addr. The addr
 * must be aligned to 16bit access, because function programs the right access
 * mode rounding the address to be written to 16 bit boundary.
 */
void sch5545_emi_ec_write16(uint16_t addr, uint16_t data);
/**
 * Writes dword of data at the desired address indicated by addr. The addr must
 * be aligned to 32bit access, because function programs the right access mode
 * rounding the address to be written to 32 bit boundary.
 */
void sch5545_emi_ec_write32(uint16_t addr, uint32_t data);
/**
 * Writes an array of dwords at the desired address indicated by addr. The addr
 * must be aligned to 32bit access, because function programs the right access
 * mode rounding the address to be written to 32 bit boundary. The address is
 * autoincremented by each IO write operation automatically.
 */
void sch5545_emi_ec_write32_bulk(uint16_t addr, const uint32_t *buffer, size_t len);
/**
 * Reads any byte of 4 bytes from the 32bit dword indicated by addr. The
 * function will automatically align to the matching 32bit dword.
 */
uint8_t sch5545_emi_ec_read8(uint16_t addr);
/**
 * Reads any word of 2 words from the 32bit dword indicated by addr. The addr
 * must be aligned to 16bit access, because function programs the right access
 * mode rounding the address to be read to 16 bit boundary.
 */
uint16_t sch5545_emi_ec_read16(uint16_t addr);
/**
 * Reads dword of data at the desired address indicated by addr. The addr must
 * be aligned to 32bit access, because function programs the right access mode
 * rounding the address to be read to 32 bit boundary.
 */
uint32_t sch5545_emi_ec_read32(uint16_t addr);
/**
 * Reads a stream of dwords of size len to an array of dwords from the desired
 * address indicated by addr. The addr must be aligned to 32bit access, because
 * function programs the right access mode rounding the start address to be
 * read to 32 bit boundary. The address is autoincremented by each IO read
 * operation automatically.
 */
void sch5545_emi_ec_read32_bulk(uint16_t addr, uint32_t *buffer, size_t len);

#endif /* SUPERIO_SCH_5545_EMI_H */
