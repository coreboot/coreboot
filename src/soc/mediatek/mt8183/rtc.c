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

#include <soc/rtc_common.h>
#include <soc/rtc.h>
#include <soc/mt6358.h>
#include <soc/pmic_wrap.h>

#define RTC_GPIO_USER_MASK	  ((1 << 13) - (1 << 8))

/* initialize rtc setting of using dcxo clock */
static void rtc_enable_dcxo(void)
{
	u16 bbpu, con, osc32con, sec;

	pwrap_read(RTC_BBPU, &bbpu);
	pwrap_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	mdelay(1);
	if (!rtc_writeif_unlock()) /* Unlock for reload */
		printk(BIOS_INFO,
			"[RTC] %s rtc_writeif_unlock() fail\n", __func__);

	pwrap_read(RTC_OSC32CON, &osc32con);
	rtc_xosc_write((osc32con & ~RTC_EMBCK_SRC_SEL)
			| RTC_XOSC32_ENB | RTC_REG_XOSC32_ENB);
	pwrap_read(RTC_BBPU, &bbpu);
	pwrap_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	pwrap_read(RTC_CON, &con);
	pwrap_read(RTC_OSC32CON, &osc32con);
	pwrap_read(RTC_AL_SEC, &sec);
	printk(BIOS_INFO, "[RTC] %s con = 0x%x, osc32con = 0x%x, sec = 0x%x\n",
		__func__, con, osc32con, sec);
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
	pwrap_read(RTC_CON, &con);
	con &= (RTC_CON_LPSTA_RAW | RTC_CON_LPRST | RTC_CON_EOSC32_LPEN);
	con |= (RTC_CON_GPEN | RTC_CON_GOE);
	con &= ~(RTC_CON_F32KOB);
	pwrap_write(RTC_CON, con);
	return rtc_write_trigger();
}

/* set xosc mode */
void rtc_osc_init(void)
{
	/* enable 32K export */
	rtc_gpio_init();
}

/* low power detect setting */
static int rtc_lpd_init(void)
{
	u16 con;

	con = pwrap_read(RTC_CON, &con) | RTC_CON_XOSC32_LPEN;
	con &= ~RTC_CON_LPRST;
	pwrap_write(RTC_CON, con);
	if (!rtc_write_trigger())
		return 0;

	con |= RTC_CON_LPRST;
	pwrap_write(RTC_CON, con);
	if (!rtc_write_trigger())
		return 0;

	con &= ~RTC_CON_LPRST;
	pwrap_write(RTC_CON, con);
	if (!rtc_write_trigger())
		return 0;

	con = pwrap_read(RTC_CON, &con) | RTC_CON_EOSC32_LPEN;
	con &= ~RTC_CON_LPRST;
	pwrap_write(RTC_CON, con);
	if (!rtc_write_trigger())
		return 0;

	con |= RTC_CON_LPRST;
	pwrap_write(RTC_CON, con);
	if (!rtc_write_trigger())
		return 0;

	con &= ~RTC_CON_LPRST;
	pwrap_write(RTC_CON, con);
	if (!rtc_write_trigger())
		return 0;

	return 1;
}

static bool rtc_hw_init(void)
{
	u16 bbpu;

	pwrap_read(RTC_BBPU, &bbpu);
	pwrap_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_INIT);
	rtc_write_trigger();

	udelay(500);

	pwrap_read(RTC_BBPU, &bbpu);
	pwrap_write(RTC_BBPU, bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	pwrap_read(RTC_BBPU, &bbpu);
	if (bbpu & RTC_BBPU_INIT) {
		printk(BIOS_INFO, "[RTC] %s:%d timeout\n", __func__, __LINE__);
		return false;
	}

	return true;
}

/* rtc init check */
int rtc_init(u8 recover)
{
	printk(BIOS_INFO, "[RTC] %s recovery: %d\n", __func__, recover);

	if (!rtc_writeif_unlock())
		return 0;

	if (!rtc_gpio_init())
		return 0;

	/* using dcxo 32K clock */
	rtc_enable_dcxo();

	if (recover)
		mdelay(20);

	/* write powerkeys */
	pwrap_write(RTC_POWERKEY1, RTC_POWERKEY1_KEY);
	pwrap_write(RTC_POWERKEY2, RTC_POWERKEY2_KEY);
	if (!rtc_write_trigger())
		return 0;

	if (!rtc_reg_init())
		return 0;
	if (!rtc_lpd_init())
		return 0;
	if (!rtc_hw_init())
		return 0;

	return 1;
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
	pwrap_write(RTC_BBPU, bbpu);
	ret = rtc_write_trigger();
	printk(BIOS_INFO, "[RTC] %s rtc_write_trigger=%d\n", __func__, ret);

	pwrap_read(RTC_BBPU, &bbpu);
	printk(BIOS_INFO, "[RTC] %s done BBPU=%#x\n", __func__, bbpu);
}

void poweroff(void)
{
	u16 bbpu;

	if (!rtc_writeif_unlock())
		printk(BIOS_INFO,
			"[RTC] %s rtc_writeif_unlock() fail\n", __func__);
	/* pull PWRBB low */
	bbpu = RTC_BBPU_KEY | RTC_BBPU_RELOAD | RTC_BBPU_PWREN;
	pwrap_write(RTC_BBPU, bbpu);

	pmic_set_power_hold(false);
	halt();
}

static void dcxo_init(void)
{
	/* Buffer setting */
	pwrap_write(PMIC_RG_DCXO_CW15, 0xA2AA);
	pwrap_write(PMIC_RG_DCXO_CW13, 0x98E9);
	pwrap_write(PMIC_RG_DCXO_CW16, 0x9855);

	/* 26M enable control */
	/* Enable clock buffer XO_SOC, XO_CEL */
	pwrap_write(PMIC_RG_DCXO_CW00, 0x4805);
	pwrap_write(PMIC_RG_DCXO_CW11, 0x8000);

	/* Load thermal coefficient */
	pwrap_write(PMIC_RG_TOP_TMA_KEY, 0x9CA7);
	pwrap_write(PMIC_RG_DCXO_CW21, 0x12A7);
	pwrap_write(PMIC_RG_DCXO_ELR0, 0xD004);
	pwrap_write(PMIC_RG_TOP_TMA_KEY, 0x0000);

	/* Adjust OSC FPM setting */
	pwrap_write(PMIC_RG_DCXO_CW07, 0x8FFE);

	/* Re-Calibrate OSC current */
	pwrap_write(PMIC_RG_DCXO_CW09, 0x008F);
	udelay(100);
	pwrap_write(PMIC_RG_DCXO_CW09, 0x408F);
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
