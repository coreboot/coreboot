/* SPDX-License-Identifier: GPL-2.0-only */

#include <fmap.h>
#include <intelblocks/pcr.h>
#include <intelblocks/rtc.h>
#include <option.h>
#include <pc80/mc146818rtc.h>
#include <reset.h>
#include <soc/pcr_ids.h>

/* RTC PCR configuration */
#define PCR_RTC_CONF		0x3400
#define PCR_RTC_CONF_UCMOS_EN	(1 << 2)
#define PCR_RTC_CONF_LCMOS_LOCK	(1 << 3)
#define PCR_RTC_CONF_UCMOS_LOCK	(1 << 4)
#define PCR_RTC_CONF_BILD	(1 << 31)
/* RTC backed up control register */
#define PCR_RTC_BUC		0x3414
#define  PCR_RTC_BUC_TOP_SWAP	(1 << 0)

void enable_rtc_upper_bank(void)
{
	/* Enable upper 128 bytes of CMOS */
	pcr_or32(PID_RTC, PCR_RTC_CONF, PCR_RTC_CONF_UCMOS_EN);
}

__weak int soc_get_rtc_failed(void)
{
	return 0;
}

void rtc_init(void)
{
	/* Ensure the date is set including century byte. */
	cmos_check_update_date();

	cmos_init(soc_get_rtc_failed());
}

void rtc_conf_set_bios_interface_lockdown(void)
{
	pcr_rmw32(PID_RTC, PCR_RTC_CONF, ~PCR_RTC_CONF_BILD,
					PCR_RTC_CONF_BILD);
}

#if CONFIG(INTEL_HAS_TOP_SWAP)
void configure_rtc_buc_top_swap(enum ts_config ts_state)
{
	pcr_rmw32(PID_RTC, PCR_RTC_BUC, ~PCR_RTC_BUC_TOP_SWAP, ts_state);
}

enum ts_config get_rtc_buc_top_swap_status(void)
{
	if (pcr_read32(PID_RTC, PCR_RTC_BUC) & PCR_RTC_BUC_TOP_SWAP)
		return TS_ENABLE;
	else
		return TS_DISABLE;
}

void sync_rtc_buc_top_swap(void)
{
	uint8_t cmos_slotb_option, topswap_control_bit;
	cmos_slotb_option = get_uint_option(TOP_SWAP_ENABLE_CMOS_OPTION, 0);
	topswap_control_bit = get_rtc_buc_top_swap_status();
	printk(BIOS_INFO, "Top Swap: CMOS option state: %d\n", cmos_slotb_option);
	printk(BIOS_INFO, "Top Swap: RTC BUC control bit: %d\n", topswap_control_bit);
	if (cmos_slotb_option != topswap_control_bit) {
		configure_rtc_buc_top_swap(cmos_slotb_option);
		printk(BIOS_INFO, "Top Swap: RTC BUC control bit set to: %d, platform reset is necessary\n", get_rtc_buc_top_swap_status());
		board_reset();
	}
}

/*
 * Select the FMAP region to continue booting from, depending on the state of
 * Top Swap
 */
const char *cbfs_fmap_region_hint(void)
{
	if (CONFIG(INTEL_TOP_SWAP_OPTION_CONTROL) && get_rtc_buc_top_swap_status())
		return "COREBOOT_TS";
	else
		return "COREBOOT";
}
#endif
