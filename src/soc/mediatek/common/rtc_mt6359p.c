/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <halt.h>
#include <soc/clkbuf.h>
#include <soc/mt6359p.h>
#include <soc/pmif.h>
#include <soc/rtc.h>
#include <soc/rtc_common.h>
#include <timer.h>

static struct pmif *pmif_arb = NULL;

void rtc_read(u16 addr, u16 *rdata)
{
	u32 data;

	if (!pmif_arb)
		pmif_arb = get_pmif_controller(PMIF_SPI, 0);
	pmif_arb->read(pmif_arb, 0, (u32)addr, &data);

	*rdata = (u16)data;
}

void rtc_write(u16 addr, u16 wdata)
{
	if (!pmif_arb)
		pmif_arb = get_pmif_controller(PMIF_SPI, 0);
	pmif_arb->write(pmif_arb, 0, (unsigned int)addr, (unsigned int)wdata);
}

static void rtc_write_field(u16 reg, u16 val, u16 mask, u16 shift)
{
	u16 old, new;

	rtc_read(reg, &old);
	new = old & ~(mask << shift);
	new |= (val << shift);
	rtc_write(reg, new);
}

/* initialize rtc setting of using dcxo clock */
static bool rtc_enable_dcxo(void)
{
	if (!rtc_writeif_unlock()) {
		rtc_info("rtc_writeif_unlock() failed\n");
		return false;
	}

	u16 bbpu, con, osc32con, sec;
	rtc_read(RTC_BBPU, &bbpu);
	rtc_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();
	rtc_read(RTC_OSC32CON, &osc32con);
	osc32con &= ~(RTC_EMBCK_SRC_SEL | RTC_EMBCK_SEL_MODE_MASK);
	osc32con |= (OSC32CON_ANALOG_SETTING | RTC_REG_XOSC32_ENB);

	if (!rtc_xosc_write(osc32con)) {
		rtc_info("rtc_xosc_write() failed\n");
		return false;
	}

	rtc_read(RTC_CON, &con);
	rtc_read(RTC_OSC32CON, &osc32con);
	rtc_read(RTC_AL_SEC, &sec);
	rtc_info("con=%#x, osc32con=%#x, sec=%#x\n", con, osc32con, sec);
	return true;
}

/* initialize rtc related gpio */
bool rtc_gpio_init(void)
{
	u16 con;

	/* GPI mode and pull down */
	rtc_read(RTC_CON, &con);
	con &= (RTC_CON_LPSTA_RAW | RTC_CON_LPRST | RTC_CON_EOSC32_LPEN
		| RTC_CON_XOSC32_LPEN);
	con |= (RTC_CON_GPEN | RTC_CON_GOE);
	con &= ~(RTC_CON_F32KOB);
	con &= ~RTC_CON_GPU;
	rtc_write(RTC_CON, con);

	return rtc_write_trigger();
}

u16 rtc_get_frequency_meter(u16 val, u16 measure_src, u16 window_size)
{
	u16 bbpu, osc32con;
	u16 fqmtr_busy, fqmtr_data, fqmtr_tcksel;
	struct stopwatch sw;

	if (val) {
		rtc_read(RTC_BBPU, &bbpu);
		rtc_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
		rtc_write_trigger();
		rtc_read(RTC_OSC32CON, &osc32con);
		rtc_xosc_write((osc32con & ~RTC_XOSCCALI_MASK) |
				(val & RTC_XOSCCALI_MASK));
	}

	/* RG_BANK_FQMTR_RST=1, reset FQMTR*/
	rtc_write_field(PMIC_RG_BANK_FQMTR_RST, 1, 1,
			  PMIC_RG_BANK_FQMTR_RST_SHIFT);
	udelay(20);
	/* RG_BANK_FQMTR_RST=0, release FQMTR*/
	rtc_write_field(PMIC_RG_BANK_FQMTR_RST, 0, 1,
			  PMIC_RG_BANK_FQMTR_RST_SHIFT);

	/* enable FQMTR clock */
	rtc_write_field(PMIC_RG_TOP_CKPDN_CON0_CLR, 1, 1,
			  PMIC_RG_FQMTR_CK_PDN_SHIFT);
	rtc_write_field(PMIC_RG_TOP_CKPDN_CON0_CLR, 1, 1,
			  PMIC_RG_FQMTR_32K_CK_PDN_SHIFT);


	rtc_write_field(PMIC_RG_FQMTR_CON0, 1, 1,
			  PMIC_RG_FQMTR_DCXO26M_EN_SHIFT);

	/* set frequency meter window value (0=1X32K(fixed clock)) */
	rtc_write(PMIC_RG_FQMTR_WINSET, window_size);
	/* enable 26M and set test clock source */
	rtc_write(PMIC_RG_FQMTR_CON0, PMIC_FQMTR_CON0_DCXO26M_EN | measure_src);
	/* enable 26M -> delay 100us -> enable FQMTR */
	mdelay(1);
	rtc_read(PMIC_RG_FQMTR_CON0, &fqmtr_tcksel);
	/* enable FQMTR */
	rtc_write(PMIC_RG_FQMTR_CON0, fqmtr_tcksel | PMIC_FQMTR_CON0_FQMTR_EN);
	mdelay(1);
	stopwatch_init_usecs_expire(&sw, FQMTR_TIMEOUT_US);
	/* FQMTR read until ready */
	do {
		rtc_read(PMIC_RG_FQMTR_CON0, &fqmtr_busy);
		if (stopwatch_expired(&sw)) {
			rtc_info("get frequency time out!\n");
			return false;
		}
	} while (fqmtr_busy & PMIC_FQMTR_CON0_BUSY);

	/* read data should be closed to 26M/32k = 794 */
	rtc_read(PMIC_RG_FQMTR_DATA, &fqmtr_data);

	/* disable FQMTR */
	rtc_read(PMIC_RG_FQMTR_CON0, &fqmtr_tcksel);
	rtc_write(PMIC_RG_FQMTR_CON0, fqmtr_tcksel & ~PMIC_FQMTR_CON0_FQMTR_EN);
	/* disable FQMTR -> delay 100us -> disable 26M */
	mdelay(1);
	/* disable 26M */
	rtc_read(PMIC_RG_FQMTR_CON0, &fqmtr_tcksel);
	rtc_write(PMIC_RG_FQMTR_CON0,
		  fqmtr_tcksel & ~PMIC_FQMTR_CON0_DCXO26M_EN);
	rtc_info("input=%d, output=%d\n", val, fqmtr_data);

	/* disable FQMTR clock */
	rtc_write_field(PMIC_RG_TOP_CKPDN_CON0_SET, 1, 1,
			  PMIC_RG_FQMTR_CK_PDN_SHIFT);
	rtc_write_field(PMIC_RG_TOP_CKPDN_CON0_SET, 1, 1,
			  PMIC_RG_FQMTR_32K_CK_PDN_SHIFT);

	return fqmtr_data;
}

/* low power detect setting */
static bool rtc_lpd_init(void)
{
	u16 con, sec;

	/* enable both XOSC & EOSC LPD */
	rtc_read(RTC_AL_SEC, &sec);
	sec &= ~RTC_LPD_OPT_F32K_CK_ALIVE;
	rtc_write(RTC_AL_SEC, sec);

	if (!rtc_write_trigger())
		return false;

	/* init XOSC32 to detect 32k clock stop */
	rtc_read(RTC_CON, &con);
	con |= RTC_CON_XOSC32_LPEN;

	if (!rtc_lpen(con))
		return false;

	/* init EOSC32 to detect rtc low power */
	rtc_read(RTC_CON, &con);
	con |= RTC_CON_EOSC32_LPEN;

	if (!rtc_lpen(con))
		return false;

	rtc_read(RTC_CON, &con);
	rtc_info("check RTC_CON_LPSTA_RAW after LP init: %#x\n", con);

	return true;
}

static bool rtc_hw_init(void)
{
	u16 bbpu;

	rtc_read(RTC_BBPU, &bbpu);
	bbpu |= RTC_BBPU_KEY | RTC_BBPU_RESET_ALARM | RTC_BBPU_RESET_SPAR;
	rtc_write(RTC_BBPU, bbpu & (~RTC_BBPU_SPAR_SW));
	rtc_write_trigger();
	udelay(500);

	rtc_read(RTC_BBPU, &bbpu);
	rtc_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();
	rtc_read(RTC_BBPU, &bbpu);

	if (bbpu & RTC_BBPU_RESET_ALARM || bbpu & RTC_BBPU_RESET_SPAR) {
		rtc_info("timeout\n");
		return false;
	}
	return true;
}

/* rtc init check */
int rtc_init(int recover)
{
	int ret;
	u16 year;

	rtc_info("recovery: %d\n", recover);

	/* write powerkeys to enable rtc functions */
	if (!rtc_powerkey_init()) {
		ret = -RTC_STATUS_POWERKEY_INIT_FAIL;
		goto err;
	}

	/* write interface unlock need to be set after powerkey match */
	if (!rtc_writeif_unlock()) {
		ret = -RTC_STATUS_WRITEIF_UNLOCK_FAIL;
		goto err;
	}

	rtc_osc_init();

	/* In recovery mode, we need 20ms delay for register setting. */
	if (recover)
		mdelay(20);

	if (!rtc_gpio_init()) {
		ret = -RTC_STATUS_GPIO_INIT_FAIL;
		goto err;
	}

	if (!rtc_hw_init()) {
		ret = -RTC_STATUS_HW_INIT_FAIL;
		goto err;
	}

	if (!rtc_reg_init()) {
		ret = -RTC_STATUS_REG_INIT_FAIL;
		goto err;
	}

	/* solution1 for EOSC cali*/
	rtc_read(RTC_AL_YEA, &year);
	rtc_write(RTC_AL_YEA, (year | RTC_K_EOSC_RSV_0) & (~RTC_K_EOSC_RSV_1)
		& (~RTC_K_EOSC_RSV_2));
	rtc_write_trigger();

	if (!rtc_lpd_init()) {
		ret = -RTC_STATUS_LPD_INIT_FAIL;
		goto err;
	}

	/*
	 * After lpd init, powerkeys need to be written again to enable
	 * low power detect function.
	 */
	if (!rtc_powerkey_init()) {
		ret = -RTC_STATUS_POWERKEY_INIT_FAIL;
		goto err;
	}
	return RTC_STATUS_OK;

err:
	rtc_info("init failed: ret=%d\n", ret);
	return ret;
}

/* enable rtc bbpu */
void rtc_bbpu_power_on(void)
{
	u16 bbpu;
	int ret;

	/* pull powerhold high, control by pmic */
	rtc_write_field(PMIC_PWRHOLD, 1, 0x1, 0);
	bbpu = RTC_BBPU_KEY | RTC_BBPU_ENABLE_ALARM;
	rtc_write(RTC_BBPU, bbpu);
	ret = rtc_write_trigger();
	rtc_info("rtc_write_trigger=%d\n", ret);
	rtc_read(RTC_BBPU, &bbpu);
	rtc_info("done BBPU=%#x\n", bbpu);
}

void poweroff(void)
{
	u16 bbpu;

	if (!rtc_writeif_unlock())
		rtc_info("rtc_writeif_unlock() failed\n");
	/* pull PWRBB low */
	bbpu = RTC_BBPU_KEY | RTC_BBPU_ENABLE_ALARM;
	rtc_write(RTC_BBPU, bbpu);
	rtc_write_field(PMIC_PWRHOLD, 0, 0x1, 0);
	halt();
}

/* the rtc boot flow entry */
void rtc_boot(void)
{
	u16 tmp;

	/* dcxo 32k init settings */
	rtc_write_field(PMIC_RG_DCXO_CW02, 0xF, 0xF, 0);
	rtc_read(PMIC_RG_SCK_TOP_CON0, &tmp);
	rtc_info("PMIC_RG_SCK_TOP_CON0,%#x:%#x\n", PMIC_RG_SCK_TOP_CON0, tmp);
	rtc_write_field(PMIC_RG_SCK_TOP_CON0, 0x1, 0x1, 0);
	rtc_read(PMIC_RG_SCK_TOP_CON0, &tmp);
	rtc_info("PMIC_RG_SCK_TOP_CON0,%#x:%#x\n", PMIC_RG_SCK_TOP_CON0, tmp);
	/* use dcxo 32K clock */
	if (!rtc_enable_dcxo())
		rtc_info("rtc_enable_dcxo() failed\n");
	rtc_boot_common();
	rtc_bbpu_power_on();
}
