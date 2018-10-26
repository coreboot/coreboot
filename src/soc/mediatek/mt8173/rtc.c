/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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
#include <soc/mt6391.h>
#include <soc/pmic_wrap.h>

#define RTC_GPIO_USER_MASK	  ((1 << 13) - (1 << 8))


/* initialize rtc related gpio */
static int rtc_gpio_init(void)
{
	u16 con;

	mt6391_gpio_set_pull(3, MT6391_GPIO_PULL_DISABLE,
				MT6391_GPIO_PULL_DOWN);  /* RTC_32K1V8 */

	/* Export 32K clock RTC_32K2V8 */
	pwrap_read(RTC_CON, &con);
	con &= (RTC_CON_LPSTA_RAW | RTC_CON_LPRST | RTC_CON_LPEN);
	con |= (RTC_CON_GPEN | RTC_CON_GOE);
	con &= ~(RTC_CON_F32KOB);
	pwrap_write(RTC_CON, con);
	return rtc_write_trigger();
}

/* set xosc mode */
void rtc_osc_init(void)
{
	u16 con;

	/* enable 32K export */
	rtc_gpio_init();

	pwrap_write(PMIC_RG_TOP_CKTST2, 0x0);
	pwrap_read(RTC_OSC32CON, &con);
	if ((con & 0x1f) != 0x0)	/* check XOSCCALI */
		rtc_xosc_write(0x3);
}

/* low power detect setting */
static int rtc_lpd_init(void)
{
	pwrap_write_field(RTC_CON, RTC_CON_LPEN, RTC_CON_LPRST, 0);
	if (!rtc_write_trigger())
		return 0;

	pwrap_write_field(RTC_CON, RTC_CON_LPRST, 0, 0);
	if (!rtc_write_trigger())
		return 0;

	pwrap_write_field(RTC_CON, 0, RTC_CON_LPRST, 0);
	if (!rtc_write_trigger())
		return 0;

	return 1;
}

/* rtc init check */
int rtc_init(u8 recover)
{
	printk(BIOS_INFO, "[RTC] %s recovery: %d\n", __func__, recover);

	if (!rtc_writeif_unlock())
		return 0;

	if (!rtc_gpio_init())
		return 0;

	/* Use SW to detect 32K mode instead of HW */
	if (recover)
		pwrap_write_field(PMIC_RG_CHRSTATUS, 0x4, 0x1, 9);

	rtc_xosc_write(0x3);

	if (recover)
		mdelay(1000);

	/* write powerkeys */
	pwrap_write(RTC_POWERKEY1, RTC_POWERKEY1_KEY);
	pwrap_write(RTC_POWERKEY2, RTC_POWERKEY2_KEY);
	if (!rtc_write_trigger())
		return 0;

	if (recover)
		pwrap_write_field(PMIC_RG_CHRSTATUS, 0, 0x4, 9);

	rtc_xosc_write(0);

	if (!rtc_reg_init())
		return 0;
	if (!rtc_lpd_init())
		return 0;

	return 1;
}

/* enable rtc bbpu */
static void rtc_bbpu_power_on(void)
{
	u16 bbpu;
	int ret;

	/* pull PWRBB high */
	bbpu = RTC_BBPU_KEY | RTC_BBPU_AUTO | RTC_BBPU_BBPU | RTC_BBPU_PWREN;
	pwrap_write(RTC_BBPU, bbpu);
	ret = rtc_write_trigger();
	printk(BIOS_INFO, "[RTC] %s rtc_write_trigger=%d\n", __func__, ret);

	/* enable DCXO to transform external 32KHz clock to 26MHz clock
	   directly sent to SoC */
	pwrap_write_field(PMIC_RG_DCXO_FORCE_MODE1, BIT(11), 0, 0);
	pwrap_write_field(PMIC_RG_DCXO_POR2_CON3,
		     BIT(8) | BIT(9) | BIT(10) | BIT(11), 0, 0);
	pwrap_write_field(PMIC_RG_DCXO_CON2,
		     BIT(1) | BIT(3) | BIT(5) | BIT(6), 0, 0);

	pwrap_read(RTC_BBPU, &bbpu);
	printk(BIOS_INFO, "[RTC] %s done BBPU=%#x\n", __func__, bbpu);

	/* detect hw clock done,close RG_RTC_75K_PDN for low power setting. */
	pwrap_write_field(PMIC_RG_TOP_CKPDN2, 0x1, 0, 14);
}

/* the rtc boot flow entry */
void rtc_boot(void)
{
	pwrap_write(PMIC_RG_TOP_CKPDN, 0);
	pwrap_write(PMIC_RG_TOP_CKPDN2, 0);

	rtc_boot_common();
	rtc_bbpu_power_on();
}
