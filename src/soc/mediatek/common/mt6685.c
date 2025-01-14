/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/mt6685.h>
#include <soc/pmif.h>
#include <timer.h>

#define MT6685_TOP_RST_MISC		0x127
#define MT6685_TOP_RST_MISC_SET		0x128

static struct pmif *pmif_arb;
u32 mt6685_read_field(u32 reg, u32 mask, u32 shift)
{
	assert(pmif_arb);
	return pmif_arb->read_field(pmif_arb, SPMI_SLAVE_9, reg, mask, shift);
}

void mt6685_write_field(u32 reg, u32 val, u32 mask, u32 shift)
{
	assert(pmif_arb);
	pmif_arb->write_field(pmif_arb, SPMI_SLAVE_9, reg, val, mask, shift);
}

u8 mt6685_read8(u32 reg)
{
	u32 rdata = 0;

	assert(pmif_arb);
	pmif_arb->read(pmif_arb, SPMI_SLAVE_9, reg, &rdata);

	return (u8)rdata;
}

void mt6685_write8(u32 reg, u8 reg_val)
{
	assert(pmif_arb);
	pmif_arb->write(pmif_arb, SPMI_SLAVE_9, reg, reg_val);
}

u16 mt6685_read16(u32 reg)
{
	u16 rdata = 0;

	assert(pmif_arb);
	pmif_arb->read16(pmif_arb, SPMI_SLAVE_9, reg, &rdata);
	return rdata;
}

void mt6685_write16(u32 reg, u16 reg_val)
{
	assert(pmif_arb);
	pmif_arb->write16(pmif_arb, SPMI_SLAVE_9, reg, reg_val);
}

static void mt6685_wdt_set(void)
{
	mt6685_write_field(MT6685_TOP_RST_MISC_SET, 0x3, 0xFF, 0);
}

static const struct mt6685_key_setting key_protect_setting[] = {
	{0x39E, 0x7A},
	{0x39F, 0x99},
	{0x3A8, 0x15},
	{0x3A9, 0x63},
	{0x3AA, 0x30},
	{0x3AB, 0x63},
	{0x9A5, 0x29},
	{0x9A6, 0x47},
	{0xF98, 0x85},
	{0xF99, 0x66},
};
static void mt6685_unlock(bool unlock)
{
	for (int i = 0; i < ARRAY_SIZE(key_protect_setting); i++)
		mt6685_write16(key_protect_setting[i].addr,
			       unlock ? key_protect_setting[i].val : 0);
}

void mt6685_init_pmif_arb(void)
{
	if (pmif_arb)
		return;

	pmif_arb = get_pmif_controller(PMIF_SPMI, SPMI_MASTER_1);
	assert(pmif_arb);

	if (pmif_arb->is_pmif_init_done(pmif_arb))
		die("ERROR - Failed to initialize pmif spi");

	printk(BIOS_INFO, "[%s]CHIP ID = %#x\n", __func__, mt6685_read_field(0xb, 0xFF, 0));
}

void mt6685_init(void)
{
	mt6685_init_pmif_arb();
	mt6685_unlock(true);
	mt6685_wdt_set();
	mt6685_init_setting();
	mt6685_unlock(false);
	printk(BIOS_INFO, "[%s]TOP_RST_MISC = %#x\n", __func__,
	       mt6685_read_field(MT6685_TOP_RST_MISC, 0xFF, 0));
}
