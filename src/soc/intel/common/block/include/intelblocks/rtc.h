/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_RTC_H
#define SOC_INTEL_COMMON_BLOCK_RTC_H

/* Top swap feature enable/disable config */
enum ts_config {
	TS_DISABLE,
	TS_ENABLE
};

void enable_rtc_upper_bank(void);

/* Expect return rtc failed bootlean in case of coin removal */
int soc_get_rtc_failed(void);

void rtc_init(void);

/*
 * set/unset RTC backed top swap bit in the BUC register.
 *  TS_ENABLE - PCH will invert A16, A17 or A18 for cycles
 *      going to the BIOS space based on PCH strap setting.
 *  TS_DISABLE - PCH will not invert A16, A17 or A18.
 */
void configure_rtc_buc_top_swap(enum ts_config ts_state);

/*
 * Return the current top swap state which is reflected by the
 * RTC backed top swap bit in the BUC register.
 *   TS_ENABLE - Top swap enabled.
 *   TS_DISABLE - Top swap disabled.
 */
enum ts_config get_rtc_buc_top_swap_status(void);

/* Set RTC Configuration BILD bit. */
void rtc_conf_set_bios_interface_lockdown(void);
#endif	/* SOC_INTEL_COMMON_BLOCK_RTC_H */
