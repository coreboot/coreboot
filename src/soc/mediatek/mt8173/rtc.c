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

#include <assert.h>
#include <bcd.h>
#include <console/console.h>
#include <delay.h>
#include <rtc.h>
#include <timer.h>

#include <soc/mt6391.h>
#include <soc/pmic_wrap.h>
#include <soc/rtc.h>

#define RTC_GPIO_USER_MASK	  ((1 << 13) - (1 << 8))

/* ensure rtc write success */
static inline int rtc_busy_wait(void)
{
	struct stopwatch sw;
	u16 bbpu;

	stopwatch_init_usecs_expire(&sw, RTC_CBUSY_TIMEOUT_US);

	do {
		pwrap_read(RTC_BBPU, &bbpu);
		/* Time > 1sec, time out and set recovery mode enable.*/
		if (stopwatch_expired(&sw)) {
			printk(BIOS_INFO, "[RTC] BBPU CBUSY time out !!\n");
			return 0;
		}
	} while (bbpu & RTC_BBPU_CBUSY);

	return 1;
}

static int write_trigger(void)
{
	pwrap_write(RTC_WRTGR, 1);
	return rtc_busy_wait();
}

/* unlock rtc write interface */
static int writeif_unlock(void)
{
	pwrap_write(RTC_PROT, RTC_PROT_UNLOCK1);
	if (!write_trigger())
		return 0;
	pwrap_write(RTC_PROT, RTC_PROT_UNLOCK2);
	if (!write_trigger())
		return 0;

	return 1;
}

/* set rtc time */
int rtc_set(const struct rtc_time *time)
{
	return -1;
}

/* get rtc time */
int rtc_get(struct rtc_time *time)
{
	u16 value;

	pwrap_read(RTC_TC_SEC, &value);
	time->sec = value;
	pwrap_read(RTC_TC_MIN, &value);
	time->min = value;
	pwrap_read(RTC_TC_HOU, &value);
	time->hour = value;
	pwrap_read(RTC_TC_DOM, &value);
	time->mday = value;
	pwrap_read(RTC_TC_MTH, &value);
	time->mon = value;
	pwrap_read(RTC_TC_YEA, &value);
	time->year = (value + RTC_MIN_YEAR_OFFSET) % 100;

	return 0;
}

/* set rtc xosc setting */
static void rtc_xosc_write(u16 val)
{
	pwrap_write(RTC_OSC32CON, RTC_OSC32CON_UNLOCK1);
	udelay(200);
	pwrap_write(RTC_OSC32CON, RTC_OSC32CON_UNLOCK2);
	udelay(200);

	pwrap_write(RTC_OSC32CON, val);
	udelay(200);
	mt6391_write(RTC_BBPU, RTC_BBPU_KEY | RTC_BBPU_RELOAD, 0, 0);
	write_trigger();
}

/* initialize rtc related registers */
static int rtc_reg_init(void)
{
	u16 irqsta;

	pwrap_write(RTC_IRQ_EN, 0);
	pwrap_write(RTC_CII_EN, 0);
	pwrap_write(RTC_AL_MASK, 0);
	pwrap_write(RTC_AL_YEA, 1970 - RTC_MIN_YEAR);
	pwrap_write(RTC_AL_MTH, 1);
	pwrap_write(RTC_AL_DOM, 1);
	pwrap_write(RTC_AL_DOW, 4);
	pwrap_write(RTC_AL_HOU, 0);
	pwrap_write(RTC_AL_MIN, 0);
	pwrap_write(RTC_AL_SEC, 0);

	pwrap_write(RTC_DIFF, 0);
	pwrap_write(RTC_CALI, 0);
	if (!write_trigger())
		return 0;

	pwrap_read(RTC_IRQ_STA, &irqsta);  /* read clear */

	/* init time counters after resetting RTC_DIFF and RTC_CALI */
	pwrap_write(RTC_TC_YEA, RTC_DEFAULT_YEA - RTC_MIN_YEAR);
	pwrap_write(RTC_TC_MTH, RTC_DEFAULT_MTH);
	pwrap_write(RTC_TC_DOM, RTC_DEFAULT_DOM);
	pwrap_write(RTC_TC_DOW, RTC_DEFAULT_DOW);
	pwrap_write(RTC_TC_HOU, 0);
	pwrap_write(RTC_TC_MIN, 0);
	pwrap_write(RTC_TC_SEC, 0);

	return write_trigger();
}

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
	return write_trigger();
}

/* set xosc mode */
static void rtc_osc_init(void)
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
	mt6391_write(RTC_CON, RTC_CON_LPEN, RTC_CON_LPRST, 0);
	if (!write_trigger())
		return 0;

	mt6391_write(RTC_CON, RTC_CON_LPRST, 0, 0);
	if (!write_trigger())
		return 0;

	mt6391_write(RTC_CON, 0, RTC_CON_LPRST, 0);
	if (!write_trigger())
		return 0;

	return 1;
}

/* rtc init check */
static int rtc_init(u8 recover)
{
	printk(BIOS_INFO, "[RTC] %s recovery: %d\n", __func__, recover);

	if (!writeif_unlock())
		return 0;

	if (!rtc_gpio_init())
		return 0;

	/* Use SW to detect 32K mode instead of HW */
	if (recover)
		mt6391_write(PMIC_RG_CHRSTATUS, 0x4, 0x1, 9);

	rtc_xosc_write(0x3);

	if (recover)
		mdelay(1000);

	/* write powerkeys */
	pwrap_write(RTC_POWERKEY1, RTC_POWERKEY1_KEY);
	pwrap_write(RTC_POWERKEY2, RTC_POWERKEY2_KEY);
	if (!write_trigger())
		return 0;

	if (recover)
		mt6391_write(PMIC_RG_CHRSTATUS, 0, 0x4, 9);

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
	ret = write_trigger();
	printk(BIOS_INFO, "[RTC] %s write_trigger=%d\n", __func__, ret);

	/* enable DCXO to transform external 32KHz clock to 26MHz clock
	   directly sent to SoC */
	mt6391_write(PMIC_RG_DCXO_FORCE_MODE1, BIT(11), 0, 0);
	mt6391_write(PMIC_RG_DCXO_POR2_CON3,
		     BIT(8) | BIT(9) | BIT(10) | BIT(11), 0, 0);
	mt6391_write(PMIC_RG_DCXO_CON2,
		     BIT(1) | BIT(3) | BIT(5) | BIT(6), 0, 0);

	pwrap_read(RTC_BBPU, &bbpu);
	printk(BIOS_INFO, "[RTC] %s done BBPU=%#x\n", __func__, bbpu);

	/* detect hw clock done,close RG_RTC_75K_PDN for low power setting. */
	mt6391_write(PMIC_RG_TOP_CKPDN2, 0x1, 0, 14);
}

static u8 rtc_check_state(void)
{
	u16 con;
	u16 pwrky1;
	u16 pwrky2;

	pwrap_read(RTC_CON, &con);
	pwrap_read(RTC_POWERKEY1, &pwrky1);
	pwrap_read(RTC_POWERKEY2, &pwrky2);

	if (con & RTC_CON_LPSTA_RAW)
		return RTC_STATE_INIT;

	if (!rtc_busy_wait())
		return RTC_STATE_RECOVER;

	if (!writeif_unlock())
		return RTC_STATE_RECOVER;

	if (pwrky1 != RTC_POWERKEY1_KEY || pwrky2 != RTC_POWERKEY2_KEY)
		return RTC_STATE_INIT;
	else
		return RTC_STATE_REBOOT;
}

/* the rtc boot flow entry */
void rtc_boot(void)
{
	u16 bbpu;
	u16 con;
	u16 irqsta;

	pwrap_write(PMIC_RG_TOP_CKPDN, 0);
	pwrap_write(PMIC_RG_TOP_CKPDN2, 0);

	switch (rtc_check_state()) {
	case RTC_STATE_REBOOT:
		mt6391_write(RTC_BBPU, RTC_BBPU_KEY | RTC_BBPU_RELOAD, 0, 0);
		write_trigger();
		rtc_osc_init();
		break;
	case RTC_STATE_RECOVER:
		rtc_init(1);
		break;
	case RTC_STATE_INIT:
	default:
		if (!rtc_init(0))
			rtc_init(1);
		break;
	}

	pwrap_read(RTC_IRQ_STA, &irqsta);  /* Read clear */
	pwrap_read(RTC_BBPU, &bbpu);
	pwrap_read(RTC_CON, &con);

	printk(BIOS_INFO, "[RTC] irqsta = %x", irqsta);
	printk(BIOS_INFO, " bbpu = %#x, con = %#x\n", bbpu, con);
	rtc_bbpu_power_on();
}
