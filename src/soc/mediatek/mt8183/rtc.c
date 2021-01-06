/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <halt.h>
#include <soc/rtc.h>
#include <soc/rtc_common.h>
#include <soc/mt6358.h>
#include <soc/pmic_wrap.h>
#include <timer.h>

#define RTC_GPIO_USER_MASK	  ((1 << 13) - (1 << 8))

/* initialize rtc setting of using dcxo clock */
static bool rtc_enable_dcxo(void)
{
	u16 bbpu, con, osc32con, sec;

	rtc_read(RTC_BBPU, &bbpu);
	rtc_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	mdelay(1);
	if (!rtc_writeif_unlock()) {
		rtc_info("rtc_writeif_unlock() failed\n");
		return false;
	}

	rtc_read(RTC_OSC32CON, &osc32con);
	osc32con &= ~(RTC_EMBCK_SRC_SEL | RTC_EMBCK_SEL_MODE_MASK
		      | RTC_GPS_CKOUT_EN);
	osc32con |= RTC_XOSC32_ENB | RTC_REG_XOSC32_ENB
		    | RTC_EMB_K_EOSC32_MODE | RTC_EMBCK_SEL_OPTION;
	if (!rtc_xosc_write(osc32con)) {
		rtc_info("rtc_xosc_write() failed\n");
		return false;
	}

	rtc_read(RTC_CON, &con);
	rtc_read(RTC_OSC32CON, &osc32con);
	rtc_read(RTC_AL_SEC, &sec);
	rtc_info("con=0x%x, osc32con=0x%x, sec=0x%x\n", con, osc32con, sec);

	return true;
}

/* initialize rtc related gpio */
bool rtc_gpio_init(void)
{
	u16 con;

	/* RTC_32K1V8 clock change from 128k div 4 source
	 * to RTC 32k source
	 */
	pwrap_write_field(PMIC_RG_TOP_CKSEL_CON0_SET, 0x1, 0x1, 3);

	/* Export 32K clock RTC_32K1V8_1 */
	pwrap_write_field(PMIC_RG_TOP_CKPDN_CON1_CLR, 0x1, 0x1, 1);

	/* Export 32K clock RTC_32K2V8 */
	rtc_read(RTC_CON, &con);
	con &= (RTC_CON_LPSTA_RAW | RTC_CON_LPRST | RTC_CON_EOSC32_LPEN
		| RTC_CON_XOSC32_LPEN);
	con |= (RTC_CON_GPEN | RTC_CON_GOE);
	con &= ~(RTC_CON_F32KOB);
	rtc_write(RTC_CON, con);

	return rtc_write_trigger();
}

u16 rtc_get_frequency_meter(u16 val, u16 measure_src, u16 window_size)
{
	u16 bbpu, osc32con;
	u16 fqmtr_busy, fqmtr_data, fqmtr_rst, fqmtr_tcksel;
	struct stopwatch sw;

	if (val) {
		rtc_read(RTC_BBPU, &bbpu);
		rtc_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
		rtc_write_trigger();
		rtc_read(RTC_OSC32CON, &osc32con);
		rtc_xosc_write((osc32con & ~RTC_XOSCCALI_MASK) |
				(val & RTC_XOSCCALI_MASK));
	}

	/* enable FQMTR clock */
	pwrap_write_field(PMIC_RG_TOP_CKPDN_CON0_CLR, 1, 1,
			  PMIC_RG_FQMTR_32K_CK_PDN_SHIFT);
	pwrap_write_field(PMIC_RG_TOP_CKPDN_CON0_CLR, 1, 1,
			  PMIC_RG_FQMTR_CK_PDN_SHIFT);

	/* FQMTR reset */
	pwrap_write_field(PMIC_RG_FQMTR_RST, 1, 1, PMIC_FQMTR_RST_SHIFT);
	do {
		rtc_read(PMIC_RG_FQMTR_DATA, &fqmtr_data);
		rtc_read(PMIC_RG_FQMTR_CON0, &fqmtr_busy);
	} while (fqmtr_data && (fqmtr_busy & PMIC_FQMTR_CON0_BUSY));
	rtc_read(PMIC_RG_FQMTR_RST, &fqmtr_rst);
	/* FQMTR normal */
	pwrap_write_field(PMIC_RG_FQMTR_RST, 0, 1, PMIC_FQMTR_RST_SHIFT);

	/* set frequency meter window value (0=1X32K(fixed clock)) */
	rtc_write(PMIC_RG_FQMTR_WINSET, window_size);
	/* enable 26M and set test clock source */
	rtc_write(PMIC_RG_FQMTR_CON0, PMIC_FQMTR_CON0_DCXO26M_EN | measure_src);
	/* enable 26M -> delay 100us -> enable FQMTR */
	udelay(100);
	rtc_read(PMIC_RG_FQMTR_CON0, &fqmtr_tcksel);
	/* enable FQMTR */
	rtc_write(PMIC_RG_FQMTR_CON0, fqmtr_tcksel | PMIC_FQMTR_CON0_FQMTR_EN);
	udelay(100);

	stopwatch_init_usecs_expire(&sw, FQMTR_TIMEOUT_US);
	/* FQMTR read until ready */
	do {
		rtc_read(PMIC_RG_FQMTR_CON0, &fqmtr_busy);
		if (stopwatch_expired(&sw)) {
			rtc_info("get frequency time out !!\n");
			return false;
		}
	} while (fqmtr_busy & PMIC_FQMTR_CON0_BUSY);

	/* read data should be closed to 26M/32k = 794 */
	rtc_read(PMIC_RG_FQMTR_DATA, &fqmtr_data);

	rtc_read(PMIC_RG_FQMTR_CON0, &fqmtr_tcksel);
	/* disable FQMTR */
	rtc_write(PMIC_RG_FQMTR_CON0, fqmtr_tcksel & ~PMIC_FQMTR_CON0_FQMTR_EN);
	/* disable FQMTR -> delay 100us -> disable 26M */
	udelay(100);
	/* disable 26M */
	rtc_read(PMIC_RG_FQMTR_CON0, &fqmtr_tcksel);
	rtc_write(PMIC_RG_FQMTR_CON0,
		  fqmtr_tcksel & ~PMIC_FQMTR_CON0_DCXO26M_EN);
	rtc_info("input=0x%x, output=%d\n", val, fqmtr_data);

	/* disable FQMTR clock */
	pwrap_write_field(PMIC_RG_TOP_CKPDN_CON0_SET, 1, 1,
			  PMIC_RG_FQMTR_32K_CK_PDN_SHIFT);
	pwrap_write_field(PMIC_RG_TOP_CKPDN_CON0_SET, 1, 1,
			  PMIC_RG_FQMTR_CK_PDN_SHIFT);

	return fqmtr_data;
}

/* low power detect setting */
static bool rtc_lpd_init(void)
{
	u16 con, sec;

	/* set RTC_LPD_OPT */
	rtc_read(RTC_AL_SEC, &sec);
	sec |= RTC_LPD_OPT_F32K_CK_ALIVE;
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
	con &= ~RTC_CON_XOSC32_LPEN;
	rtc_write(RTC_CON, con);

	/* set RTC_LPD_OPT */
	rtc_read(RTC_AL_SEC, &sec);
	sec &= ~RTC_LPD_OPT_MASK;
	sec |= RTC_LPD_OPT_EOSC_LPD;
	rtc_write(RTC_AL_SEC, sec);
	if (!rtc_write_trigger())
		return false;

	return true;
}

static bool rtc_hw_init(void)
{
	u16 bbpu;

	rtc_read(RTC_BBPU, &bbpu);
	rtc_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_INIT);
	rtc_write_trigger();

	udelay(500);

	rtc_read(RTC_BBPU, &bbpu);
	rtc_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	rtc_read(RTC_BBPU, &bbpu);
	if (bbpu & RTC_BBPU_INIT) {
		rtc_info("timeout\n");
		return false;
	}

	return true;
}

/* rtc init check */
int rtc_init(int recover)
{
	int ret;

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
	rtc_info("init fail: ret=%d\n", ret);
	return ret;
}

/* enable rtc bbpu */
void rtc_bbpu_power_on(void)
{
	u16 bbpu;
	int ret;

	/* pull powerhold high, control by pmic */
	pmic_set_power_hold(true);

	/* pull PWRBB high */
	bbpu = RTC_BBPU_KEY | RTC_BBPU_AUTO | RTC_BBPU_RELOAD | RTC_BBPU_PWREN;
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
	bbpu = RTC_BBPU_KEY | RTC_BBPU_RELOAD | RTC_BBPU_PWREN;
	rtc_write(RTC_BBPU, bbpu);

	pmic_set_power_hold(false);
	halt();
}

static void dcxo_init(void)
{
	/* Buffer setting */
	rtc_write(PMIC_RG_DCXO_CW15, 0xA2AA);
	rtc_write(PMIC_RG_DCXO_CW13, 0x98E9);
	rtc_write(PMIC_RG_DCXO_CW16, 0x9855);

	/* 26M enable control */
	/* Enable clock buffer XO_SOC, XO_CEL */
	rtc_write(PMIC_RG_DCXO_CW00, 0x4805);
	rtc_write(PMIC_RG_DCXO_CW11, 0x8000);

	/* Load thermal coefficient */
	rtc_write(PMIC_RG_TOP_TMA_KEY, 0x9CA7);
	rtc_write(PMIC_RG_DCXO_CW21, 0x12A7);
	rtc_write(PMIC_RG_DCXO_ELR0, 0xD004);
	rtc_write(PMIC_RG_TOP_TMA_KEY, 0x0000);

	/* Adjust OSC FPM setting */
	rtc_write(PMIC_RG_DCXO_CW07, 0x8FFE);

	/* Re-Calibrate OSC current */
	rtc_write(PMIC_RG_DCXO_CW09, 0x008F);
	udelay(100);
	rtc_write(PMIC_RG_DCXO_CW09, 0x408F);
	mdelay(5);
}

void mt6358_dcxo_disable_unused(void)
{
	/* Disable clock buffer XO_CEL */
	rtc_write(PMIC_RG_DCXO_CW00_CLR, 0x0800);
	/* Mask bblpm request and switch off bblpm mode */
	rtc_write(PMIC_RG_DCXO_CW23, 0x0052);
}

/* the rtc boot flow entry */
void rtc_boot(void)
{
	/* dcxo clock init settings */
	dcxo_init();

	/* dcxo 32k init settings */
	pwrap_write_field(PMIC_RG_DCXO_CW02, 0xF, 0xF, 0);
	pwrap_write_field(PMIC_RG_SCK_TOP_CON0, 0x1, 0x1, 0);

	/* use dcxo 32K clock */
	if (!rtc_enable_dcxo())
		rtc_info("rtc_enable_dcxo() failed\n");

	rtc_boot_common();
	rtc_bbpu_power_on();
}
