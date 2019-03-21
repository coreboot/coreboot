/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <delay.h>
#include <halt.h>
#include <soc/rtc_common.h>
#include <soc/rtc.h>
#include <soc/mt6358.h>
#include <soc/pmic_wrap.h>

#define RTC_GPIO_USER_MASK	  ((1 << 13) - (1 << 8))

/* initialize rtc setting of using dcxo clock */
static int rtc_enable_dcxo(void)
{
	u16 bbpu, con, osc32con, sec;

	rtc_read(RTC_BBPU, &bbpu);
	rtc_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	mdelay(1);
	if (!rtc_writeif_unlock()) { /* Unlock for reload */
		rtc_info("rtc_writeif_unlock() fail\n");
		return 0;
	}

	rtc_read(RTC_OSC32CON, &osc32con);
	osc32con &= ~RTC_EMBCK_SRC_SEL;
	osc32con |= RTC_XOSC32_ENB | RTC_REG_XOSC32_ENB;
	if (!rtc_xosc_write(osc32con)) {
		rtc_info("rtc_xosc_write() fail\n");
		return 0;
	}
	rtc_read(RTC_BBPU, &bbpu);
	rtc_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	rtc_read(RTC_CON, &con);
	rtc_read(RTC_OSC32CON, &osc32con);
	rtc_read(RTC_AL_SEC, &sec);
	rtc_info("con=0x%x, osc32con=0x%x, sec=0x%x\n", con, osc32con, sec);

	return 1;
}

/* initialize rtc related gpio */
static int rtc_gpio_init(void)
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

/* set xosc mode */
void rtc_osc_init(void)
{
	/* enable 32K export */
	rtc_gpio_init();
}

/* enable lpd subroutine */
static int rtc_lpen(u16 con)
{
	con &= ~RTC_CON_LPRST;
	rtc_write(RTC_CON, con);
	if (!rtc_write_trigger())
		return 0;

	con |= RTC_CON_LPRST;
	rtc_write(RTC_CON, con);
	if (!rtc_write_trigger())
		return 0;

	con &= ~RTC_CON_LPRST;
	rtc_write(RTC_CON, con);
	if (!rtc_write_trigger())
		return 0;

	return 1;
}

/* low power detect setting */
static int rtc_lpd_init(void)
{
	u16 con, sec;

	/* set RTC_LPD_OPT */
	rtc_read(RTC_AL_SEC, &sec);
	sec |= RTC_LPD_OPT_F32K_CK_ALIVE;
	rtc_write(RTC_AL_SEC, sec);
	if (!rtc_write_trigger())
		return 0;

	/* init XOSC32 to detect 32k clock stop */
	rtc_read(RTC_CON, &con);
	con |= RTC_CON_XOSC32_LPEN;
	if (!rtc_lpen(con))
		return 0;

	/* init EOSC32 to detect rtc low power */
	rtc_read(RTC_CON, &con);
	con |= RTC_CON_EOSC32_LPEN;
	if (!rtc_lpen(con))
		return 0;

	rtc_read(RTC_CON, &con);
	con &= ~RTC_CON_XOSC32_LPEN;
	rtc_write(RTC_CON, con);

	/* set RTC_LPD_OPT */
	rtc_read(RTC_AL_SEC, &sec);
	sec &= ~RTC_LPD_OPT_MASK;
	sec |= RTC_LPD_OPT_EOSC_LPD;
	rtc_write(RTC_AL_SEC, sec);
	if (!rtc_write_trigger())
		return 0;

	return 1;
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

/* write powerkeys to enable rtc functions */
static int rtc_powerkey_init(void)
{
	rtc_write(RTC_POWERKEY1, RTC_POWERKEY1_KEY);
	rtc_write(RTC_POWERKEY2, RTC_POWERKEY2_KEY);
	return rtc_write_trigger();
}

/* rtc init check */
int rtc_init(u8 recover)
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

	/* using dcxo 32K clock */
	if (!rtc_enable_dcxo()) {
		ret = -RTC_STATUS_OSC_SETTING_FAIL;
		goto err;
	}

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

	/* After lpd init, powerkeys need to be written again to enable
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
		rtc_info("rtc_writeif_unlock() fail\n");
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

/* the rtc boot flow entry */
void rtc_boot(void)
{
	/* dcxo clock init settings */
	dcxo_init();

	/* dcxo 32k init settings */
	pwrap_write_field(PMIC_RG_DCXO_CW02, 0xF, 0xF, 0);
	pwrap_write_field(PMIC_RG_SCK_TOP_CON0, 0x1, 0x1, 0);

	rtc_boot_common();
	rtc_bbpu_power_on();
}
