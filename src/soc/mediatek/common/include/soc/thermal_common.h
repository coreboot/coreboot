/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_COMMON_INCLUDE_SOC_THERMAL_COMMON_H__
#define __SOC_MEDIATEK_COMMON_INCLUDE_SOC_THERMAL_COMMON_H__

#include <soc/thermal_internal.h>

#define CHECK_DEVICE_ACCESS_RETRY_CNT	100
#define LVTS_DEVICE_ACCESS_DELAY_US	3
#define LVTS_READ_ID_RETRY_CNT		5
#define LVTS_READ_ID_DELAY_US		3

enum sensor_switch_status {
	SEN_OFF,
	SEN_ON,
};

struct lvts_thermal_controller_speed {
	u32 group_interval_delay;
	u32 period_unit;
	u32 filter_interval_delay;
	u32 sensor_interval_delay;
};

struct lvts_thermal_controller {
	enum lvts_sensor ts[MAX_TS_NUMBER]; /* sensor point */
	enum sensor_switch_status sensor_on_off[MAX_TS_NUMBER];
	size_t ts_number;
	int reboot_temperature;
	int dominator_ts_idx;
	unsigned int reboot_msr_sram_idx;
	bool has_reboot_temp_sram;
	bool has_reboot_msr_sram;
	struct lvts_thermal_controller_speed speed;
	struct mtk_thermal_controller_regs *regs;
};

int lvts_write_device(uint16_t config, uint8_t dev_reg_idx, uint8_t data, int tc_num);
const struct lvts_thermal_controller *lvts_get_controller(int tc_num);
void lvts_configure_polling_speed_and_filter(const struct lvts_thermal_controller *tc);
int lvts_read_tc_raw_and_temp(void *msr_reg, enum lvts_sensor ts_name, uint32_t golden_temp);
void lvts_thermal_init(void);
void lvts_tscpu_reset_thermal(void);
void lvts_thermal_cal_prepare(void);
void lvts_device_identification(void);
void lvts_device_enable_init_all_devices(void);
void lvts_efuse_setting(void);
void lvts_config_all_tc_hw_protect(void);
uint16_t lvts_temp_to_raw(int temp_mc, enum lvts_sensor ts_name);
void lvts_tscpu_thermal_read_tc_temp(const struct lvts_thermal_controller *tc, int order);

void thermal_cls_sram(void);
void thermal_stat_cls_sram(void);
void thermal_sram_init(void);
void thermal_init(void);
void thermal_write_reboot_temp_sram(uint32_t value);
void thermal_write_reboot_msr_sram(unsigned int idx, uint32_t value);

#endif /* __SOC_MEDIATEK_COMMON_INCLUDE_SOC_THERMAL_COMMON_H__ */
