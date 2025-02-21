/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8196_THERMAL_H
#define SOC_MEDIATEK_MT8196_THERMAL_H

void thermal_sram_init(void);
void thermal_init(void);
void thermal_write_reboot_temp_sram(uint32_t value);
void thermal_write_reboot_msr_sram(unsigned int idx, uint32_t value);

#endif /* SOC_MEDIATEK_MT8196_THERMAL_H */
