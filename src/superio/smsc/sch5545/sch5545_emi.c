/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <arch/io.h>
#include <device/pnp.h>
#include <device/pnp_ops.h>
#include <superio/conf_mode.h>

#include "sch5545.h"
#include "sch5545_emi.h"

static uint16_t emi_bar;

#ifdef __SIMPLE_DEVICE__
static void sch5545_enter_conf_state(pnp_devfn_t dev)
{
	unsigned int port = dev >> 8;
	outb(0x55, port);
}

static void sch5545_exit_conf_state(pnp_devfn_t dev)
{
	unsigned int port = dev >> 8;
	outb(0xaa, port);
}
#endif

uint16_t sch5545_read_emi_bar(uint8_t sio_port)
{
	uint16_t bar;

#ifdef __SIMPLE_DEVICE__
	pnp_devfn_t lpcif = PNP_DEV(sio_port, SCH5545_LDN_LPC);
	sch5545_enter_conf_state(lpcif);
#else
	struct device *lpcif = dev_find_slot_pnp(sio_port, SCH5545_LDN_LPC);
	if (!lpcif)
		return 0;
	pnp_enter_conf_mode_55(lpcif);
#endif
	pnp_set_logical_device(lpcif);

	bar = pnp_read_config(lpcif, SCH5545_BAR_EM_IF + 2);
	bar |= pnp_read_config(lpcif, SCH5545_BAR_EM_IF + 3) << 8;

#ifdef __SIMPLE_DEVICE__
	sch5545_exit_conf_state(lpcif);
#else
	pnp_exit_conf_mode_aa(lpcif);
#endif
	return bar;
}

void sch5545_emi_init(uint8_t sio_port)
{
	emi_bar = sch5545_read_emi_bar(sio_port);
	assert(emi_bar != 0);
}

void sch5545_emi_ec2h_mailbox_clear(void)
{
	sch5545_emi_ec2h_mbox_write(sch5545_emi_ec2h_mbox_read());
}

void sch5545_emi_disable_interrupts(void)
{
	sch5545_emi_set_int_mask(0);
}

void sch5545_emi_h2ec_mbox_write(uint8_t mbox_message)
{
	outb(mbox_message, emi_bar + SCH5545_EMI_HOST_TO_EC_MAILBOX);
}

uint8_t sch5545_emi_h2ec_mbox_read(void)
{
	return inb(emi_bar + SCH5545_EMI_HOST_TO_EC_MAILBOX);
}

void sch5545_emi_ec2h_mbox_write(uint8_t mbox_message)
{
	outb(mbox_message, emi_bar + SCH5545_EMI_EC_TO_HOST_MAILBOX);
}

uint8_t sch5545_emi_ec2h_mbox_read(void)
{
	return inb(emi_bar + SCH5545_EMI_EC_TO_HOST_MAILBOX);
}

void sch5545_emi_set_int_mask(uint16_t mask)
{
	outw(mask, emi_bar + SCH5545_EMI_INT_MASK);
}

void sch5545_emi_set_int_mask_low(uint8_t mask)
{
	outb(mask, emi_bar + SCH5545_EMI_INT_MASK);
}

void sch5545_emi_set_int_mask_high(uint8_t mask)
{
	outb(mask, emi_bar + SCH5545_EMI_INT_MASK + 1);
}

uint8_t sch5545_emi_get_int_mask_low(void)
{
	return inb(emi_bar + SCH5545_EMI_INT_MASK);
}

uint8_t sch5545_emi_get_int_mask_high(void)
{
	return inb(emi_bar + SCH5545_EMI_INT_MASK + 1);
}

uint16_t sch5545_emi_get_int_mask(void)
{
	return inw(emi_bar + SCH5545_EMI_INT_MASK);
}

void sch5545_emi_set_int_src_low(uint8_t int_src)
{
	outb(int_src, emi_bar + SCH5545_EMI_INT_SOURCE);
}

void sch5545_emi_set_int_src_high(uint8_t int_src)
{
	outb(int_src, emi_bar + SCH5545_EMI_INT_SOURCE + 1);
}

uint8_t sch5545_emi_get_int_src_low(void)
{
	return inb(emi_bar + SCH5545_EMI_INT_SOURCE);
}

uint8_t sch5545_emi_get_int_src_high(void)
{
	return inb(emi_bar + SCH5545_EMI_INT_SOURCE + 1);
}
uint16_t sch5545_emi_get_int_src(void)
{
	return inw(emi_bar + SCH5545_EMI_INT_SOURCE);
}

void sch5545_emi_set_int_src(uint16_t int_src)
{
	outw(int_src, emi_bar + SCH5545_EMI_INT_SOURCE);
}

void sch5545_emi_set_ec_addr(uint16_t addr)
{
	outw(addr, emi_bar + SCH5545_EMI_EC_ADDR);
}

uint16_t sch5545_emi_read_ec_addr(void)
{
	return inw(emi_bar + SCH5545_EMI_EC_ADDR);
}

void sch5545_emi_ec_write8(uint16_t addr, uint8_t data)
{
	sch5545_emi_set_ec_addr((addr & 0xfffc) | EMI_EC_8BIT_ACCESS);
	outb(data, emi_bar + SCH5545_EMI_EC_DATA + (addr & 3));
}

void sch5545_emi_ec_write16(uint16_t addr, uint16_t data)
{
	sch5545_emi_set_ec_addr((addr & 0xfffc) | EMI_EC_16BIT_ACCESS);
	outw(data, emi_bar + SCH5545_EMI_EC_DATA + (addr & 2));
}

void sch5545_emi_ec_write32(uint16_t addr, uint32_t data)
{
	sch5545_emi_set_ec_addr((addr & 0xfffc) | EMI_EC_32BIT_ACCESS);
	outl(data, emi_bar + SCH5545_EMI_EC_DATA);
}

void sch5545_emi_ec_write32_bulk(uint16_t addr, const uint32_t *buffer, size_t len)
{
	sch5545_emi_set_ec_addr((addr & 0xfffc) | EMI_EC_32BIT_AUTO_ACCESS);

	while (len > 0) {
		outl(*(buffer++), emi_bar + SCH5545_EMI_EC_DATA);
		len--;
	}
}

uint8_t sch5545_emi_ec_read8(uint16_t addr)
{
	sch5545_emi_set_ec_addr((addr & 0xfffc) | EMI_EC_8BIT_ACCESS);
	return inb(emi_bar + SCH5545_EMI_EC_DATA + (addr & 3));
}

uint16_t sch5545_emi_ec_read16(uint16_t addr)
{
	sch5545_emi_set_ec_addr((addr & 0xfffc) | EMI_EC_16BIT_ACCESS);
	return inw(emi_bar + SCH5545_EMI_EC_DATA + (addr & 2));
}

uint32_t sch5545_emi_ec_read32(uint16_t addr)
{
	sch5545_emi_set_ec_addr((addr & 0xfffc) | EMI_EC_32BIT_ACCESS);
	return inb(emi_bar + SCH5545_EMI_EC_DATA);
}

void sch5545_emi_ec_read32_bulk(uint16_t addr, uint32_t *buffer, size_t len)
{
	sch5545_emi_set_ec_addr((addr & 0xfffc) | EMI_EC_32BIT_AUTO_ACCESS);

	while (len > 0) {
		*(buffer++) = inl(emi_bar + SCH5545_EMI_EC_DATA);
		len--;
	}
}
