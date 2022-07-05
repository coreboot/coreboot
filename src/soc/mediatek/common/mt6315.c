/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <soc/mt6315.h>
#include <soc/pmif.h>
#include <soc/pmif_spmi.h>

static struct pmif *pmif_arb = NULL;

static void mt6315_read(u32 slvid, u32 reg, u32 *data)
{
	pmif_arb->read(pmif_arb, slvid, reg, data);
}

static void mt6315_write(u32 slvid, u32 reg, u32 data)
{
	pmif_arb->write(pmif_arb, slvid, reg, data);
}

void mt6315_write_field(u32 slvid, u32 reg, u32 val, u32 mask, u32 shift)
{
	pmif_arb->write_field(pmif_arb, slvid, reg, val, mask, shift);
}

static void mt6315_wdt_enable(u32 slvid)
{
	mt6315_write(slvid, 0x3A9, 0x63);
	mt6315_write(slvid, 0x3A8, 0x15);
	mt6315_write(slvid, 0x127, 0x2);
	mt6315_write(slvid, 0x127, 0x1);
	mt6315_write(slvid, 0x127, 0x8);
	udelay(50);
	mt6315_write(slvid, 0x128, 0x8);
	mt6315_write(slvid, 0x3A8, 0);
	mt6315_write(slvid, 0x3A9, 0);
}

void mt6315_buck_set_voltage(u32 slvid, u32 buck_id, u32 buck_uv)
{
	unsigned int vol_reg, vol_val;

	if (!pmif_arb)
		die("ERROR: pmif_arb not initialized");

	switch (buck_id) {
	case MT6315_BUCK_1:
		vol_reg = MT6315_BUCK_TOP_ELR0;
		break;
	case MT6315_BUCK_3:
		vol_reg = MT6315_BUCK_TOP_ELR3;
		break;
	default:
		die("ERROR: Unknown buck_id %u", buck_id);
		return;
	};

	vol_val = buck_uv / 6250;
	mt6315_write(slvid, vol_reg, vol_val);
}

u32 mt6315_buck_get_voltage(u32 slvid, u32 buck_id)
{
	u32 vol_reg, vol;

	if (!pmif_arb)
		die("ERROR: pmif_arb not initialized");

	switch (buck_id) {
	case MT6315_BUCK_1:
		vol_reg = MT6315_BUCK_VBUCK1_DBG0;
		break;
	case MT6315_BUCK_3:
		vol_reg = MT6315_BUCK_VBUCK1_DBG3;
		break;
	default:
		die("ERROR: Unknown buck_id %u", buck_id);
		return 0;
	};

	mt6315_read(slvid, vol_reg, &vol);
	return vol * 6250;
}

static void init_pmif_arb(void)
{
	if (!pmif_arb) {
		pmif_arb = get_pmif_controller(PMIF_SPMI, 0);
		if (!pmif_arb)
			die("ERROR: No spmi device");
	}

	if (pmif_arb->is_pmif_init_done(pmif_arb))
		die("ERROR - Failed to initialize pmif spmi");
}

void mt6315_init(void)
{
	size_t i;

	init_pmif_arb();

	for (i = 0; i < spmi_dev_cnt; i++)
		mt6315_wdt_enable(spmi_dev[i].slvid);

	mt6315_init_setting();
}
