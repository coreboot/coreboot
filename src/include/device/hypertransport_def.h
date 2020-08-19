#ifndef DEVICE_HYPERTRANSPORT_DEF_H
#define DEVICE_HYPERTRANSPORT_DEF_H

#define HT_FREQ_200Mhz   0
#define HT_FREQ_300Mhz   1
#define HT_FREQ_400Mhz   2
#define HT_FREQ_500Mhz   3
#define HT_FREQ_600Mhz   4
#define HT_FREQ_800Mhz   5
#define HT_FREQ_1000Mhz  6
#define HT_FREQ_1200Mhz  7
#define HT_FREQ_1400Mhz  8
#define HT_FREQ_1600Mhz  9
#define HT_FREQ_1800Mhz 10
#define HT_FREQ_2000Mhz 11
#define HT_FREQ_2200Mhz 12
#define HT_FREQ_2400Mhz 13
#define HT_FREQ_2600Mhz 14
#define HT_FREQ_VENDOR  15  /* AMD defines this to be 100Mhz */

static inline bool offset_unit_id(bool is_sb_ht_chain)
{
	bool need_offset = (CONFIG_HT_CHAIN_UNITID_BASE != 1)
		|| (CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20);
	return need_offset && is_sb_ht_chain;
}

#endif /* DEVICE_HYPERTRANSPORT_DEF_H */
