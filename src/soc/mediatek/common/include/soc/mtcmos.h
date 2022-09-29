/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_COMMON_MTCMOS_H__
#define __SOC_MEDIATEK_COMMON_MTCMOS_H__

struct power_domain_data {
	void *pwr_con;
	u32 pwr_sta_mask;
	u32 sram_pdn_mask;
	u32 sram_ack_mask;
	u32 ext_buck_iso_bits;
	u32 caps;
};

#define SCPD_SRAM_ISO		BIT(0)
#define SCPD_EXT_BUCK_ISO	BIT(1)

void mtcmos_set_scpd_ext_buck_iso(const struct power_domain_data *pd);

void mtcmos_power_on(const struct power_domain_data *pd);
void mtcmos_adsp_power_on(void);
void mtcmos_audio_power_on(void);
void mtcmos_display_power_on(void);

void mtcmos_protect_adsp_bus(void);
void mtcmos_protect_audio_bus(void);
void mtcmos_protect_display_bus(void);

#endif /* __SOC_MEDIATEK_COMMON_MTCMOS_H__ */
