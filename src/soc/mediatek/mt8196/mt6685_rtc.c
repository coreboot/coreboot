/* SPDX-License-Identifier: GPL-2.0-only OR MIT */
/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 14.2.4
 */

#include <commonlib/bsd/bcd.h>
#include <console/console.h>
#include <delay.h>
#include <rtc.h>
#include <soc/mt6685.h>
#include <soc/rtc.h>
#include <stdbool.h>
#include <timer.h>

static u8 rtc_read_byte(u16 addr)
{
	return mt6685_read_field(addr, 0xFF, 0);
}

static void config_interface(u16 addr, u16 val, u16 mask, u16 shift)
{
	mt6685_write_field(addr, val, mask, shift);
}

void rtc_read(u16 addr, u16 *rdata)
{
	*rdata = mt6685_read16(addr);
}

void rtc_write(u16 addr, u16 wdata)
{
	mt6685_write16(addr, wdata);
}

static u16 rtc_get_prot_stat(void)
{
	u16 val;
	u16 state = 0;

	udelay(100);

	rtc_read(RTC_SPAR_MACRO, &val);

	state = (val >> RTC_SPAR_PROT_STAT_SHIFT) & RTC_SPAR_PROT_STAT_MASK;

	return state;
}

static bool mt6685_writeif_unlock(void)
{
	if (!retry(PROT_UNLOCK_RETRY_COUNT,
		   rtc_writeif_unlock() &&
		   rtc_get_prot_stat() == RTC_PROT_UNLOCK_SUCCESS)) {

		printk(BIOS_ERR, "%s: retry failed!!\n", __func__);

		return false;
	}

	return true;
}

static bool rtc_eosc_check_clock(const struct rtc_clk_freq *result)
{
	if ((result->fqm26m_ck >= 3 && result->fqm26m_ck <= 7) &&
	    (result->dcxo_f32k_ck > 2 && result->dcxo_f32k_ck < 9) &&
	    (result->fqm26m_target_ck > 300 && result->fqm26m_target_ck < 10400))
		return true;
	else
		return false;
}

u16 rtc_get_frequency_meter(u16 val, u16 measure_src, u16 window_size)
{
	u16 osc32con;
	u16 rdata;
	u16 fqmtr_data;

	if (val != 0) {
		rtc_read(RTC_BBPU, &rdata);
		rtc_write(RTC_BBPU, rdata | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
		rtc_write_trigger();
		rtc_read(RTC_OSC32CON, &rdata);
		osc32con = rdata & 0xFFE0;
		rtc_xosc_write(osc32con | (val & RTC_XOSCCALI_MASK));
	}

	/* RG_BANK_FQMTR_RST = 1 reset FQMTR */
	config_interface(RG_BANK_FQMTR_RST, 1, RG_BANK_FQMTR_RST_MASK,
			 RG_BANK_FQMTR_RST_SHIFT);
	udelay(20);

	/* RG_BANK_FQMTR_RST = 0 release FQMTR */
	config_interface(RG_BANK_FQMTR_RST, 0, RG_BANK_FQMTR_RST_MASK,
			 RG_BANK_FQMTR_RST_SHIFT);

	/* Write Protection Key to unlock TOP_CKPDN_CON0 */
	config_interface(TOP_DIG_WPK, 0x15, DIG_WPK_KEY_MASK, DIG_WPK_KEY_SHIFT);
	config_interface(TOP_DIG_WPK_H, 0x63, DIG_WPK_KEY_H_MASK, DIG_WPK_KEY_H_SHIFT);

	config_interface(RG_FQMTR_CLK_CK_PDN_CLR, 1, RG_FQMTR_CLK_CK_PDN_MASK,
			 RG_FQMTR_CLK_CK_PDN_SHIFT);

	config_interface(RG_FQMTR_32K_CK_PDN_CLR, 1, RG_FQMTR_32K_CK_PDN_MASK,
			 RG_FQMTR_32K_CK_PDN_SHIFT);

	config_interface(RG_FQMTR_DCXO26M_EN, 1, RG_FQMTR_DCXO26M_MASK,
			 RG_FQMTR_DCXO26M_SHIFT);

	/* Set freq meter window value (0=1X32K(fix clock)) */
	rtc_write(RG_FQMTR_WINSET, window_size);

	/* Select freq meter target clock */
	config_interface(RG_FQMTR_TCKSEL, measure_src,
			 RG_FQMTR_TCKSEL_MASK, RG_FQMTR_TCKSEL_SHIFT);

	udelay(100);

	/* Enable FQMTR */
	config_interface(RG_FQMTR_EN, 1, RG_FQMTR_EN_MASK, RG_FQMTR_EN_SHIFT);
	udelay(100);

	/* FQMTR read until ready */
	if (!wait_us(FQMTR_TIMEOUT_US,
		     !((rtc_read(RG_FQMTR_BUSY, &rdata), rdata) & FQMTR_BUSY))) {
		printk(BIOS_ERR, "%s: FQMTR read time out\n", __func__);
		return false;
	}

	/* Read data should be closed to 26MHz / 23K = (26 * 10^6) / (23 * 1024) ~= 794 */
	rtc_read(RG_FQMTR_DATA, &fqmtr_data);

	config_interface(RG_FQMTR_DCXO26M_EN, 0,
			 RG_FQMTR_DCXO26M_MASK, RG_FQMTR_DCXO26M_SHIFT);

	config_interface(RG_FQMTR_TCKSEL, 0,
			 RG_FQMTR_DCXO26M_MASK, RG_FQMTR_DCXO26M_SHIFT);
	udelay(100);

	/* Disable FQMTR */
	config_interface(RG_FQMTR_EN, 0, RG_FQMTR_EN_MASK, RG_FQMTR_EN_SHIFT);

	printk(BIOS_INFO, "%s: input=%#x, output=%u\n", __func__, val, fqmtr_data);

	config_interface(RG_FQMTR_CLK_CK_PDN_SET, 1,
			 RG_FQMTR_CLK_CK_PDN_MASK, RG_FQMTR_CLK_CK_PDN_SHIFT);

	config_interface(RG_FQMTR_32K_CK_PDN_SET, 1,
			 RG_FQMTR_32K_CK_PDN_MASK, RG_FQMTR_32K_CK_PDN_SHIFT);

	return fqmtr_data;
}

static void rtc_measure_all_clock(struct rtc_clk_freq *result)
{
	/* Select 26M as fixed clock */
	config_interface(RG_FQMTR_CKSEL, FQMTR_FIX_CLK_26M, RG_FQMTR_CKSEL_MASK,
			 RG_FQMTR_CKSEL_SHIFT);
	udelay(100);
	result->fqm26m_ck = rtc_get_frequency_meter(0, FQMTR_FQM26M_CK, 4);

	/* Select DCXO_32 as target clock */
	config_interface(RG_FQMTR_CKSEL, FQMTR_FIX_CLK_26M, RG_FQMTR_CKSEL_MASK,
			 RG_FQMTR_CKSEL_SHIFT);
	udelay(100);
	result->dcxo_f32k_ck = rtc_get_frequency_meter(0, FQMTR_DCXO_F32K_CK, 3970);

	/* Select 26M as target clock */
	config_interface(RG_FQMTR_CKSEL, FQMTR_FIX_CLK_EOSC_32K, RG_FQMTR_CKSEL_MASK,
			 RG_FQMTR_CKSEL_SHIFT);
	udelay(100);
	result->fqm26m_target_ck = rtc_get_frequency_meter(0, FQMTR_FQM26M_CK, 4);
}

static bool rtc_frequency_meter_check(void)
{
	struct rtc_clk_freq result;
	u16 osc32con, val;

	if (!wait_us(EOSC_CHECK_CLK_TIMEOUT_US,
		     (rtc_measure_all_clock(&result), rtc_eosc_check_clock(&result)))) {
		printk(BIOS_ERR, "%s: eosc clock check time out!\n", __func__);
		return false;
	}

	val = rtc_eosc_cali();
	printk(BIOS_DEBUG, "before set ENB_HW_Mode: EOSC cali val = %#x\n", val);
	/* ENB_HW_Mode */
	osc32con = OSC32CON_ANALOG_SETTING | RTC_REG_XOSC32_ENB;
	val = (val & RTC_XOSCCALI_MASK) | osc32con;
	printk(BIOS_DEBUG, "after set ENB_HW_Mode: EOSC cali val = %#x\n", val);
	rtc_xosc_write(val);

	return true;
}

static bool rtc_clrset_trigger(u16 addr, u16 clr_bits, u16 set_bits)
{
	u16 rdata;

	rtc_read(addr, &rdata);

	rdata &= ~clr_bits;
	rdata |= set_bits;

	rtc_write(addr, rdata);

	return rtc_write_trigger();
}

bool rtc_gpio_init(void)
{
	/* GPI mode and pull enable + pull down */
	rtc_clrset_trigger(RTC_CON,
			   (u16)(~(RTC_CON_LPSTA_RAW | RTC_CON_LPRST |
				   RTC_XOSC32_LPEN | RTC_EOSC32_LPEN) |
				 RTC_CON_GPU | RTC_CON_F32KOB),
			   RTC_CON_GPEN | RTC_CON_GOE);

	return rtc_write_trigger();
}

static bool rtc_hw_init(void)
{
	struct stopwatch sw;
	u16 rdata;

	stopwatch_init_usecs_expire(&sw, BBPU_RELOAD_TIMEOUT_US);

	rtc_read(RTC_BBPU, &rdata);
	rtc_write(RTC_BBPU,
		  rdata | RTC_BBPU_KEY | RTC_BBPU_RESET_ALARM |
		  (RTC_BBPU_RESET_SPAR & (~RTC_BBPU_SPAR_SW)));
	rtc_write_trigger();

	do {
		rtc_read(RTC_BBPU, &rdata);
		rtc_write(RTC_BBPU, rdata | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
		rtc_write_trigger();
		rtc_read(RTC_BBPU, &rdata);

		if (!(rdata & (RTC_BBPU_RESET_ALARM | RTC_BBPU_RESET_SPAR)))
			return true;

	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "%s time out!\n", __func__);

	return false;
}

static bool rtc_lpd_init(void)
{
	u16 rdata;

	/* Enable EOSC LPD only */
	printk(BIOS_INFO, "Enable EOSC LPD only\n");

	if (!rtc_clrset_trigger(RTC_AL_SEC, RTC_LPD_OPT_MASK, RTC_LPD_OPT_EOSC_LPD))
		return false;

	if (!rtc_clrset_trigger(RTC_CON, RTC_CON_LPRST, RTC_XOSC32_LPEN))
		return false;

	if (!rtc_clrset_trigger(RTC_CON, 0, RTC_CON_LPRST))
		return false;

	if (!rtc_clrset_trigger(RTC_CON, RTC_CON_LPRST, 0))
		return false;

	if (!rtc_clrset_trigger(RTC_CON, RTC_CON_LPRST, RTC_XOSC32_LPEN))
		return false;

	if (!rtc_clrset_trigger(RTC_CON, 0, RTC_CON_LPRST))
		return false;

	if (!rtc_clrset_trigger(RTC_CON, RTC_CON_LPRST, 0))
		return false;

	rtc_read(RTC_CON, &rdata);
	printk(BIOS_INFO, "%s: RTC_CON=%#x\n", __func__, rdata);

	/* bit 7 for low power detected in preloader */
	rtc_read(RTC_CON, &rdata);
	rtc_write(RTC_SPAR0, rdata | RTC_PDN1_PWRON_TIME);
	if (!rtc_write_trigger())
		return false;

	return true;
}

static void secure_write_trigger(void)
{
	rtc_write(RTC_WRTGR_SEC, 1);
}

static void secure_rtc_set_ck(void)
{
	u8 val;
	u16 rtc_sec_dsn;
	u16 rtc_sec_dsn_rev0;
	u16 rtc_sec_ck_pdn;

	printk(BIOS_INFO, "%s: Enter\n", __func__);

	val = RG_RTC_SEC_MCLK_PDN_MASK |
	      (RG_RTC_SEC_32K_CK_PDN_MASK << RG_RTC_SEC_32K_CK_PDN_SHIFT);

	config_interface(SCK_TOP_CKPDN_CON0_L_CLR, val,
			 val, SCK_TOP_CKPDN_CON0_L_CLR_SHIFT);

	udelay(50);
	rtc_write(RTC_SEC_CK_PDN, 1);
	secure_write_trigger();

	val = rtc_read_byte(SCK_TOP_CKPDN_CON0_L);
	rtc_read(RTC_SEC_DSN_ID, &rtc_sec_dsn);
	rtc_read(RTC_SEC_DSN_REV0, &rtc_sec_dsn_rev0);

	printk(BIOS_INFO, "RTC_SEC_DSN_ID[%#x]=%#x, RTC_SEC_DSN_REV0[%#x]=%#x\n",
	       RTC_SEC_DSN_ID, rtc_sec_dsn, RTC_SEC_DSN_REV0, rtc_sec_dsn_rev0);

	rtc_read(RTC_SEC_CK_PDN, &rtc_sec_ck_pdn);
	printk(BIOS_INFO, "SCK_TOP_CKPDN_CON0[%#x]=0x%x, RTC_SEC_CK_PDN[%#x]=%#x\n",
	       SCK_TOP_CKPDN_CON0_L, val, RTC_SEC_CK_PDN, rtc_sec_ck_pdn);
}

static void secure_rtc_init(void)
{
	printk(BIOS_INFO, "%s: Enter\n", __func__);
	secure_rtc_set_ck();
	udelay(100);

	rtc_write(RTC_TC_YEA_SEC, RTC_DEFAULT_YEA - RTC_MIN_YEAR);
	rtc_write(RTC_TC_MTH_SEC, RTC_DEFAULT_MTH);
	rtc_write(RTC_TC_DOM_SEC, RTC_DEFAULT_DOM);
	rtc_write(RTC_TC_DOW_SEC, 1);
	rtc_write(RTC_TC_HOU_SEC, 0);
	rtc_write(RTC_TC_MIN_SEC, 0);
	rtc_write(RTC_TC_SEC_SEC, 0);
	secure_write_trigger();
}

static bool rtc_init_after_recovery(void)
{
	/* write powerkeys */
	if (!rtc_powerkey_init())
		return false;
	if (!mt6685_writeif_unlock())
		return false;
	if (!rtc_gpio_init())
		return false;
	if (!rtc_hw_init())
		return false;
	if (!rtc_reg_init())
		return false;
	if (!rtc_lpd_init())
		return false;

	if (!rtc_powerkey_init())
		return false;
	if (!mt6685_writeif_unlock())
		return false;

	secure_rtc_init();

	printk(BIOS_INFO, "%s: done\n", __func__);
	return true;
}

static void rtc_recovery_flow(void)
{
	printk(BIOS_INFO, "%s: enter\n", __func__);

	config_interface(SCK_TOP_XTAL_SEL_ADDR, 1, SCK_TOP_XTAL_SEL_MASK,
			 SCK_TOP_XTAL_SEL_SHIFT);
	udelay(100);

	if (!retry(RECOVERY_RETRY_COUNT,
		   rtc_frequency_meter_check() &&
		   rtc_init_after_recovery())) {
		printk(BIOS_ERR, "%s: rtc recovery retry failed!!\n", __func__);
	}
}

static bool rtc_first_boot_init(void)
{
	u16 rdata;
	printk(BIOS_INFO, "%s: Enter\n", __func__);

	rtc_read(RTC_BBPU, &rdata);
	rtc_write(RTC_BBPU, rdata | RTC_BBPU_KEY | RTC_BBPU_RESET_SPAR);
	if (!rtc_write_trigger())
		return false;

	if (!mt6685_writeif_unlock()) {
		printk(BIOS_ERR,
		       "%s: mt6685_writeif_unlock failed after BBPU written\n", __func__);
		return false;
	}

	if (!rtc_gpio_init())
		return false;

	/* write powerkeys */
	rtc_read(RTC_AL_SEC, &rdata);
	rtc_write(RTC_AL_SEC, rdata & (~RTC_K_EOSC32_VTCXO_ON_SEL));
	rtc_read(RTC_AL_YEA, &rdata);
	rtc_write(RTC_AL_YEA, (rdata & RTC_AL_YEA_MASK) | RTC_K_EOSC_RSV_7 | RTC_K_EOSC_RSV_6);

	if (!rtc_powerkey_init()) {
		printk(BIOS_ERR,
		       "%s: rtc_powerkey_init failed\n", __func__);
		return false;
	}

	if (!mt6685_writeif_unlock()) {
		printk(BIOS_ERR,
		       "%s: mt6685_writeif_unlock failed after POWERKEY written\n", __func__);
		return false;
	}

	rtc_read(RTC_BBPU, &rdata);
	rtc_write(RTC_BBPU, rdata | RTC_BBPU_KEY | RTC_BBPU_RESET_SPAR);

	if (!rtc_write_trigger()) {
		printk(BIOS_ERR,
		       "%s rtc_write_trigger failed after BBPU written\n", __func__);
		return false;
	}

	if (!mt6685_writeif_unlock()) {
		printk(BIOS_ERR,
		       "%s mt6685_writeif_unlock failed after BBPU written\n", __func__);
		return false;
	}

	if (!rtc_lpd_init())
		return false;

	/* MT6685 needs to write POWERKEY again to unlock RTC */
	if (!rtc_powerkey_init()) {
		printk(BIOS_ERR,
		       "%s: rtc_powerkey_init failed after lpd init\n", __func__);
		return false;
	}

	if (!mt6685_writeif_unlock()) {
		printk(BIOS_ERR,
		       "%s mt6685_writeif_unlock failed after POWERKEY written\n", __func__);
		return false;
	}

	if (!rtc_frequency_meter_check())
		return false;

	if (!rtc_hw_init())
		return false;

	if (!rtc_reg_init())
		return false;

	secure_rtc_init();

	return true;
}

static void rtc_enable_dcxo(void)
{
	u16 rdata;
	u16 con, osc32con, sec;

	/* Unlock for reload */
	if (!mt6685_writeif_unlock())
		printk(BIOS_ERR, "mt6685_writeif_unlock() failed\n");

	rtc_read(RTC_BBPU, &rdata);
	rtc_write(RTC_BBPU, rdata | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();
	rtc_read(RTC_OSC32CON, &rdata);

	/* 0: f32k_ck src = dcxo_ck */
	rtc_xosc_write(rdata & ~RTC_EMBCK_SRC_SEL);

	rtc_read(RTC_BBPU, &rdata);
	rtc_write(RTC_BBPU, rdata | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	rtc_read(RTC_CON, &rdata);
	con = rdata;

	rtc_read(RTC_OSC32CON, &rdata);
	osc32con = rdata;

	rtc_read(RTC_AL_SEC, &rdata);
	sec = rdata;

	printk(BIOS_INFO, "%s con = %#x, osc32con = %#x, sec = %#x\n", __func__,
	       con, osc32con, sec);
}

void rtc_boot(void)
{
	bool need_secure_rtc_set_ck = false;
	struct rtc_clk_freq result;
	u16 rdata;
	u16 rtc_diff;
	u16 rtc_con;
	u16 rtc_pwrkey1;
	u16 rtc_pwrkey2;
	u16 rtc_bbpu;
	u16 rtc_osc32con;
	u16 rtc_al_sec;
	u16 rtc_al_yea;

	/* If EOSC cali is enabled in last power off. Needing to switch to DCXO clock source, */
	/* Or the FQMTR can't measure DCXO clock source. */
	rtc_read(RTC_DIFF, &rtc_diff);
	rtc_read(RTC_CON, &rtc_con);
	rtc_read(RTC_POWERKEY1, &rtc_pwrkey1);
	rtc_read(RTC_POWERKEY2, &rtc_pwrkey2);

	if ((rtc_diff & RTC_POWER_DETECTED) &&
	   !(rtc_con & RTC_CON_LPSTA_RAW) &&
	   (rtc_pwrkey1 == RTC_POWERKEY1_KEY &&
	   rtc_pwrkey2 == RTC_POWERKEY2_KEY))
		rtc_enable_dcxo();

	/* Check if clock sources match existing 32K */
	rtc_measure_all_clock(&result);

	if (!rtc_eosc_check_clock(&result))
		printk(BIOS_INFO, "Enter first boot init.\n");

	rtc_read(RTC_POWERKEY1, &rtc_pwrkey1);
	rtc_read(RTC_POWERKEY2, &rtc_pwrkey2);
	rtc_read(RTC_CON, &rtc_con);
	printk(BIOS_INFO, "%s: powerkey1 = %#x, powerkey2 = %#x, %s LPD\n", __func__,
	       rtc_pwrkey1, rtc_pwrkey2,
	       (rtc_con & RTC_CON_LPSTA_RAW) ? "with" : "without");

	rtc_read(RTC_BBPU, &rtc_bbpu);
	rtc_read(RTC_CON, &rtc_con);
	rtc_read(RTC_OSC32CON, &rtc_osc32con);
	rtc_read(RTC_AL_SEC, &rtc_al_sec);
	rtc_read(RTC_AL_YEA, &rtc_al_yea);

	printk(BIOS_INFO, "bbpu = %#x, con = %#x, osc32con = %#x, sec = %#x, yea = %#x\n",
	       rtc_bbpu, rtc_con, rtc_osc32con,
	       rtc_al_sec, rtc_al_yea);

	rtc_read(RTC_BBPU, &rtc_bbpu);
	rtc_write(RTC_BBPU, rtc_bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);

	if (!rtc_write_trigger() || !mt6685_writeif_unlock()) {
		rtc_recovery_flow();
	} else {
		rtc_read(RTC_POWERKEY1, &rtc_pwrkey1);
		rtc_read(RTC_POWERKEY2, &rtc_pwrkey2);
		if (rtc_pwrkey1 != RTC_POWERKEY1_KEY ||
		    rtc_pwrkey2 != RTC_POWERKEY2_KEY) {
			printk(BIOS_INFO, "%s: powerkey1 = %#x, powerkey2 = %#x\n", __func__,
			       rtc_pwrkey1, rtc_pwrkey2);
			if (!rtc_first_boot_init())
				rtc_recovery_flow();
		} else {
			rtc_osc_init();
			need_secure_rtc_set_ck = true;
		}
	}

	/* Make sure RTC get the latest register info. */
	rtc_read(RTC_BBPU, &rtc_bbpu);
	rtc_write(RTC_BBPU, rtc_bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	/* HW K EOSC mode whatever power off (including plug out battery) */
	rtc_read(RTC_AL_YEA, &rtc_al_yea);
	/* HW K EOSC mode whatever power off (including plug out battery) */
	rtc_write(RTC_AL_YEA, ((rtc_al_yea | RTC_K_EOSC_RSV_0) &
		  (~RTC_K_EOSC_RSV_1)) & (~RTC_K_EOSC_RSV_2));

	/* Write Protection Key to unlock RG_OCT1_RTC32K_1V8_0 */
	config_interface(TMA_KEY, 0x7a, TMA_KEY_MASK, TMA_KEY_SHIFT);
	config_interface(TMA_KEY_H, 0x99, TMA_KEY_H_MASK, TMA_KEY_H_SHIFT);

	/* Set 32K1V8_0_driving to 8mA */
	config_interface(RG_OCT1_RTC32K_1V8_0, 1,
			 RG_OCT1_RTC32K_1V8_0_MASK, RG_OCT1_RTC32K_1V8_0_SHIFT);

	/* Set 32K1V8_F_driving to 8mA */
	config_interface(RG_OCT1_RTC32K_1V8_F, 1,
			 RG_OCT1_RTC32K_1V8_F_MASK, RG_OCT1_RTC32K_1V8_F_SHIFT);

	/* Write Protection Key to unlock TOP_CKPDN_CON0 */
	config_interface(TOP_DIG_WPK, 0x15, DIG_WPK_KEY_MASK, DIG_WPK_KEY_SHIFT);
	config_interface(TOP_DIG_WPK_H, 0x63, DIG_WPK_KEY_H_MASK, DIG_WPK_KEY_H_SHIFT);

	/* Select RTC_32K1V8_0_CK clock */
	config_interface(SCK_TOP_CKSEL_CON, 1, R_SCK32K_CK_MASK, R_SCK32K_CK_SHIFT);
	rtc_read(SCK_TOP_CKSEL_CON, &rdata);
	printk(BIOS_INFO, "SCK_TOP_CKSEL_CON = %#x\n", rdata);

	/* Turn off eosc cali mode clock */
	config_interface(SCK_TOP_CKPDN_CON0_L_SET, 1,
			 RG_RTC_EOSC32_CK_PDN_MASK, RG_RTC_EOSC32_CK_PDN_SHIFT);

	/* Set register to let MD know 32k status */
	rtc_read(RTC_SPAR0, &rdata);
	rtc_write(RTC_SPAR0, (rdata & ~RTC_SPAR0_32K_LESS));

	printk(BIOS_INFO, "32k-less mode\n");
	rtc_write_trigger();

	rtc_read(RTC_BBPU, &rtc_bbpu);
	rtc_write(RTC_BBPU, rtc_bbpu | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	/* Clear ONESHOT bit to solve alarm issue */
	rtc_read(RTC_IRQ_EN, &rdata);
	rtc_write(RTC_IRQ_EN, rdata & ~RTC_IRQ_EN_ONESHOT);
	rtc_write_trigger();
	rtc_read(RTC_IRQ_EN, &rdata);
	printk(BIOS_INFO, "check RTC_IRQ_EN = %#x\n", rdata);

	if (need_secure_rtc_set_ck)
		secure_rtc_set_ck();
}

static void rtc_get_tick(struct rtc_time *tm)
{
	u16 rdata;
	rtc_read(RTC_BBPU, &rdata);
	rtc_write(RTC_BBPU, rdata | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	rtc_write_trigger();

	rtc_read(RTC_TC_SEC, &rdata);
	tm->sec = rdata;

	rtc_read(RTC_TC_MIN, &rdata);
	tm->min = rdata;

	rtc_read(RTC_TC_HOU, &rdata);
	tm->hour = rdata;

	rtc_read(RTC_TC_DOM, &rdata);
	tm->mday = rdata;

	rtc_read(RTC_TC_MTH, &rdata);
	tm->mon = rdata & RTC_TC_MTH_MASK;

	rtc_read(RTC_TC_YEA, &rdata);
	tm->year = rdata;
}

void rtc_get_time(struct rtc_time *tm)
{
	u16 rdata;
	rtc_get_tick(tm);
	rtc_read(RTC_TC_SEC, &rdata);

	/* SEC has carried */
	if (rdata < tm->sec)
		rtc_get_tick(tm);

	tm->year += RTC_MIN_YEAR;
}
