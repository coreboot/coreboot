/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_SPM_COMMON_H
#define SOC_MEDIATEK_SPM_COMMON_H

#include <soc/mcu_common.h>

struct pcm_desc {
	u32 pmem_words;
	u32 total_words;
	u32 pmem_start;
	u32 dmem_start;
};

struct dyna_load_pcm {
	u8 *buf;		/* binary array */
	struct pcm_desc desc;
};

const struct pwr_ctrl *get_pwr_ctrl(void);
void spm_code_swapping(void);
void spm_set_power_control(const struct pwr_ctrl *pwrctrl);
void spm_register_init(void);
void spm_reset_and_init_pcm(void);
void spm_set_wakeup_event(const struct pwr_ctrl *pwrctrl);
void spm_extern_initialize(void);
int spm_init(void);
void spm_set_pcm_flags(const struct pwr_ctrl *pwrctrl);
void spm_init_pcm_register(void);
void spm_kick_pcm_to_run(const struct pwr_ctrl *pwrctrl);

#endif  /* SOC_MEDIATEK_SPM_COMMON_H */
