/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 9.13
 */

#include <assert.h>
#include <delay.h>
#include <soc/efuse.h>
#include <soc/thermal.h>

#define LVTS_DEVICE_WRITE_CONFIG	0xC103

static uint32_t golden_temp;

static u32 ts_edata[L_TS_LVTS_NUM];
static u32 op_cali[LVTS_CONTROLLER_NUM];
static u32 count_rc_now[L_TS_LVTS_NUM];

/*
 * module            LVTS Plan
 *=====================================================
 * MCU_BIG core0/1 LVTS1-0, LVTS1-1, LVTS1-2, LVTS1-3
 * MCU_BIG core2/3 LVTS2-0, LVTS2-1, LVTS2-2, LVTS2-3
 * MCU_LITTLE      LVTS3-0, LVTS3-1, LVTS3-2, LVTS3-3
 * SOC-TOP,APU     LVTS4-0, LVTS4-1, LVTS4-2, LVTS4-3
 * GPU             LVTS5-0, LVTS5-1
 *
 * ptp_therm_ctrl_AP  Base address: (+0x1031_5000), 0x1031_5100, 0x1031_5200
 * ptp_therm_ctrl_MCU Base address: (+0x1031_6000), 0x1031_6100, 0x1031_6200, 0x1031_6300
 */
static const struct lvts_thermal_controller lvts_tscpu_g_tc[LVTS_CONTROLLER_NUM] = {
	[LVTS_MCU_CONTROLLER0] = { /* MCU_BIG core0/1 */
		.ts = {L_TS_LVTS1_0, L_TS_LVTS1_1, L_TS_LVTS1_2, L_TS_LVTS1_3},
		.sensor_on_off = {SEN_ON, SEN_ON, SEN_ON, SEN_ON},
		.ts_number = 4,
		.reboot_temperature = 119000,
		.dominator_ts_idx = 0,
		.reboot_msr_sram_idx = 0,
		.has_reboot_msr_sram = false,
		.has_reboot_temp_sram = false,
		.speed = {
			.group_interval_delay = 0x0F6,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001
		},
		.regs = mtk_lvts_mcu_controller0,
	},
	[LVTS_MCU_CONTROLLER1] = { /* MCU_BIG core2/3 */
		.ts = {L_TS_LVTS2_0, L_TS_LVTS2_1, L_TS_LVTS2_2, L_TS_LVTS2_3},
		.sensor_on_off = {SEN_ON, SEN_ON, SEN_ON, SEN_ON},
		.ts_number = 4,
		.reboot_temperature = 119000,
		.dominator_ts_idx = 0,
		.reboot_msr_sram_idx = 0,
		.has_reboot_msr_sram = false,
		.has_reboot_temp_sram = false,
		.speed = {
			.group_interval_delay = 0x0F6,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001
		},
		.regs = mtk_lvts_mcu_controller1,
	},
	[LVTS_MCU_CONTROLLER2] = { /* MCU_LITTLE */
		.ts = {L_TS_LVTS3_0, L_TS_LVTS3_1, L_TS_LVTS3_2, L_TS_LVTS3_3},
		.sensor_on_off = {SEN_ON, SEN_ON, SEN_ON, SEN_ON},
		.ts_number = 4,
		.reboot_temperature = 119000,
		.dominator_ts_idx = 0,
		.reboot_msr_sram_idx = 0,
		.has_reboot_msr_sram = false,
		.has_reboot_temp_sram = false,
		.speed = {
			.group_interval_delay = 0x1C1,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001
		},
		.regs = mtk_lvts_mcu_controller2,
	},
	[LVTS_AP_CONTROLLER0] = { /* SOC-TOP, APU */
		.ts = {L_TS_LVTS4_0, L_TS_LVTS4_1, L_TS_LVTS4_2, L_TS_LVTS4_3},
		.sensor_on_off = {SEN_ON, SEN_ON, SEN_ON, SEN_ON},
		.ts_number = 4,
		.reboot_temperature = 119000,
		.dominator_ts_idx = 0,
		.reboot_msr_sram_idx = 0,
		.has_reboot_msr_sram = false,
		.has_reboot_temp_sram = false,
		.speed = {
			.group_interval_delay = 0x0D2,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001
		},
		.regs = mtk_lvts_ap_controller0,
	},
	[LVTS_MFG_CONTROLLER0] = { /* MFG */
		.ts = {L_TS_LVTS5_0, L_TS_LVTS5_1},
		.sensor_on_off = {SEN_ON, SEN_ON},
		.ts_number = 2,
		.reboot_temperature = 119000,
		.dominator_ts_idx = 0,
		.reboot_msr_sram_idx = 0,
		.has_reboot_msr_sram = false,
		.has_reboot_temp_sram = false,
		.speed = {
			.group_interval_delay = 0x08A,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001
		},
		.regs = mtk_lvts_mfg_controller0,
	},
};

const struct lvts_thermal_controller *lvts_get_controller(int tc_num)
{
	assert(tc_num < LVTS_CONTROLLER_NUM);

	return &lvts_tscpu_g_tc[tc_num];
}

void lvts_clock_gate_disable(void)
{
	write32(&mtk_infracfg_ao->infracfg_ao_module_cg_0_clr, THERM_CG);
}

void lvts_tscpu_reset_thermal(void)
{
	/* Reset AP thermal control */
	write32(&mtk_infracfg->ap_rst_set, BIT(23));
	write32(&mtk_infracfg->ap_rst_clr, BIT(23));

	/* Reset MCU thermal control */
	write32(&mtk_infracfg->mcu_rst_set, BIT(12));
	write32(&mtk_infracfg->mcu_rst_clr, BIT(12));
}

void lvts_thermal_cal_prepare(void)
{
	u32 temp[LVTS_ADDRESS_INDEX_NUM];
	int i;
	bool efuse_calibrated = false;
	enum lvts_sensor sensor_groups[] = {L_TS_LVTS1_0, L_TS_LVTS2_0,
					    L_TS_LVTS3_0, L_TS_LVTS4_0,
					    L_TS_LVTS5_0};
	const size_t sensor_group_size = 4;
	int idx;

	for (i = 0; i < ARRAY_SIZE(temp); i++) {
		temp[i] = read32(&mtk_efuse->lvts_reg[i]);
		printk(BIOS_INFO, "[lvts_cali] %d: 0x%x\n", i, temp[i]);

		if (temp[i] != 0)
			efuse_calibrated = true;
	}

	/* 0x11F1_01A4,LVTS1 */
	golden_temp = (temp[0] & GENMASK(7, 0));
	idx = 0;

	for (i = 0; i < ARRAY_SIZE(sensor_groups); i++) {
		idx++;
		for (int j = 0; j < sensor_group_size; j++) {
			enum lvts_sensor sensor = sensor_groups[i] + j;
			if (sensor >= L_TS_LVTS_NUM)
				break;
			ts_edata[sensor] = temp[idx++] & GENMASK(23, 0);
		}
	}

	/* 0x11F1_01A4,LVTS1 */
	op_cali[LVTS_MCU_CONTROLLER0] = ((temp[0] & GENMASK(31, 8)) >> 8);
	/* 0x11F1_01B8,LVTS2 */
	op_cali[LVTS_MCU_CONTROLLER1] = (temp[5] & GENMASK(23, 0));
	/* 0x11F1_01CC,LVTS3 */
	op_cali[LVTS_MCU_CONTROLLER2] = (temp[10] & GENMASK(23, 0));

	/* 0x11F1_01E0,LVTS4 */
	op_cali[LVTS_AP_CONTROLLER0] = (temp[15] & GENMASK(23, 0));

	/* 0x11F1_01F4,LVTS5 */
	op_cali[LVTS_MFG_CONTROLLER0] = (temp[20] & GENMASK(23, 0));

	if (!efuse_calibrated) {
		/* It means all efuse data are equal to 0 */
		printk(BIOS_ERR,
		       "[lvts_cal] The efuses of this sample are all zero, "
		       "meaning it is not calibrated, fake!!!\n");
		golden_temp = DEFAULT_EFUSE_GOLDEN_TEMP;
		for (i = 0; i < L_TS_LVTS_NUM; i++)
			ts_edata[i] = DEFAULT_EFUSE_COUNT;

		for (i = 0; i < LVTS_CONTROLLER_NUM; i++)
			op_cali[i] = DEFAULT_EFUSE_COUNT_RC;
	}

	printk(BIOS_INFO, "[lvts_cal] golden_temp = %d mdegC\n", golden_temp);

	printk(BIOS_INFO, "[lvts_cal] num:ts_edata:op_cali ");
	for (i = 0; i < ARRAY_SIZE(ts_edata); i++) {
		if (i < ARRAY_SIZE(op_cali))
			printk(BIOS_INFO, "%d:%d:%d ", i, ts_edata[i], op_cali[i]);
		else
			printk(BIOS_INFO, "%d:%d ", i, ts_edata[i]);
	}
	printk(BIOS_INFO, "\n");

	write32(&lvts_tscpu_g_tc[0].regs->lvtsspare[1], golden_temp);
}

void lvts_device_identification(void)
{
	u32 dev_id, data;
	int i;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);
	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
		/* Enable LVTS_CTRL Clock */
		write32(&tc->regs->lvtsclken_0, 0x00000001);

		/* Reset All Devices */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0xFF, 0xFF, i);

		/*  Read back Dev_ID with Update */
		lvts_write_device(0xC502, 0xFC, 0x55, i);

		dev_id = 0x81 + i;

		if (!retry(LVTS_READ_ID_RETRY_CNT,
			   (data = read32(&tc->regs->lvts_id_0) & GENMASK(7, 0)) == dev_id,
			   udelay(LVTS_READ_ID_DELAY_US)))
			printk(BIOS_ERR,
			       "LVTS_TC_%d read timeout, addr:0x%lx, Device ID should be 0x%x,"
			       " but 0x%x\n",
			       i, (uintptr_t)(&tc->regs->lvts_id_0), dev_id, data);
	}
}

void lvts_device_enable_init_all_devices(void)
{
	int i;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		/*  Stop Counting (RG_TSFM_ST=0) */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x03, 0x00, i);
		/* RG_TSFM_LPDLY[1:0]=2' 10 */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x07, 0xA6, i);
		/* Set LVTS device counting window 20us */
		/* Counting Window */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x05, 0x00, i);
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x04, 0x20, i);
		/* TSV2F_CHOP_CKSEL & TSV2F_EN */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x0A, 0x8C, i);
		/* TSBG_DEM_CKSEL * TSBG_CHOP_EN */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x0C, 0xFC, i);
		/* Set TS_RSV */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x09, 0x8D, i);
		/* Set TS_CHOP control */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x08, 0xF1, i);
	}
}

static u32 lvts_read_device(u32 config, u32 dev_reg_idx, int tc_num)
{
	u32 data;
	u32 *lvts_config_reg = &lvts_tscpu_g_tc[tc_num].regs->lvts_config_0;

	config |= (dev_reg_idx & 0xFF) << 8;

	write32(lvts_config_reg, config);

	/* Wait 2us + 3us buffer */
	udelay(5);

	if (!retry(CHECK_DEVICE_ACCESS_RETRY_CNT,
		   !(read32(lvts_config_reg) & DEVICE_ACCESS_START_BIT),
		   udelay(2)))
		printk(BIOS_ERR,
		       "read err: DEVICE_ACCESS_START_BIT is not ready, "
		       "ASIF transaction not finished, reg0x%x\n",
		       dev_reg_idx);

	data = read32(&lvts_tscpu_g_tc[tc_num].regs->lvtsrdata0_0);

	return data;
}

static void lvts_device_check_counting_status(const struct lvts_thermal_controller *tc)
{
	/*
	 * Check this when LVTS device is counting for a temperature or
	 * a RC (Resistor-Capacitor Calibration) now
	 */

	if (!retry(CHECK_DEVICE_ACCESS_RETRY_CNT,
		   !(read32(&tc->regs->lvts_config_0) & DEVICE_SENSING_STATUS),
		   udelay(2)))
		printk(BIOS_ERR, "Error: DEVICE_SENSING_STATUS is not ready\n");
}

static void lvts_device_read_count_rc_n(void)
{
	/* Perform RC (Resistor-Capacitor Calibration) and read current count (count_rc_n) */
	int i, j, s_index;
	u32 data;

	/* Set LVTS MANUAL-RCK operation */
	for (i = 0; i < LVTS_CONTROLLER_NUM; i++)
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x0E, 0x00, i);

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);

		/* Set LVTS Manual-RCK operation */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x0E, 0x00, i);

		for (j = 0; j < tc->ts_number; j++) {
			if (tc->sensor_on_off[j] == SEN_OFF)
				continue;

			s_index = tc->ts[j];

			/* Select sensor-N with RCK */
			lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x0D, j, i);
			/* Set Device Low-Power Single mode */
			lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x06, 0x78, i);
			/* Set TS_EN and DEV_EN */
			lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x08, 0xF5, i);
			/* Toggle VCO_RST */
			lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x08, 0xFD, i);
			/* Set TS_EN and DEV_EN */
			lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x08, 0xF5, i);
			/* wait 10us */
			udelay(10);
			lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x03, 0x02, i);
			/* polling counting status */
			lvts_device_check_counting_status(tc);
			/* disable TS_EN */
			lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x08, 0xF1, i);
			/* wait 40us at sensor0 */
			/* 2us is margin for different IC */
			if (j == 0)
				udelay(42);

			/* Get RCK count data (sensor-N) */
			data = lvts_read_device(0xC1020000, 0x00, i);
			/* wait 2us + 3us buffer */
			udelay(5);

			/* Get RCK value from LSB[23:0] */
			count_rc_now[s_index] = data & GENMASK(23, 0);

			/*
			 * Recover Setting for Normal Access on
			 * temperature fetch
			 */
			/* Select Sensor-N without RCK */
			lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x0D, (0x10 | j), i);
		}
	}

	printk(BIOS_INFO, "[COUNT_RC_NOW] ");
	for (i = 0; i < L_TS_LVTS_NUM; i++)
		printk(BIOS_INFO, "%d:%d ", i, count_rc_now[i]);

	printk(BIOS_INFO, "\n");
}

void lvts_efuse_setting(void)
{
	int i, j, s_index;
	uint32_t efuse_data;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	lvts_device_read_count_rc_n();

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);

		assert(tc->ts_number <= ARRAY_SIZE(tc->regs->lvtsedata));
		for (j = 0; j < tc->ts_number; j++) {
			if (tc->sensor_on_off[j] == SEN_OFF)
				continue;

			s_index = tc->ts[j];

			efuse_data = ((u64)count_rc_now[s_index] * ts_edata[s_index]) >> 14;

			write32(&tc->regs->lvtsedata[j], efuse_data);
			printk(BIOS_INFO,
			       "efuse LVTSEDATA[%d]_%d %#x\n", j, i,
			       read32(&tc->regs->lvtsedata[j]));
		}
	}
}

void lvts_tscpu_thermal_read_tc_temp(const struct lvts_thermal_controller *tc, int order)
{
	enum lvts_sensor ts_name = tc->ts[order];
	int temperature;
	assert(order < ARRAY_SIZE(tc->regs->lvtsmsr));
	temperature = lvts_read_tc_raw_and_temp(&tc->regs->lvtsmsr[order],
						ts_name,
						golden_temp);

	printk(BIOS_INFO,
	       "%s order %d ts_name %d temp %d mdegC\n",
	       __func__, order, ts_name, temperature);
}

void lvts_set_device_single_mode(int tc_num)
{
	lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x06, 0xF8, tc_num);
}

uint16_t lvts_temp_to_raw(int temp_mc, enum lvts_sensor ts_name)
{
	/*
	 * MSR_RAW = ((temp_mc - GOLDEN_TEMP/2 - b) * 16384) / a
	 * a = -250.46
	 * b =  250.46
	 */
	uint32_t msr_raw;
	int64_t temp1;

	temp1 = (int64_t)golden_temp * 500 - LVTS_COEFF_A - temp_mc;
	assert(temp1 >= 0);
	msr_raw = ((temp1 << 14) / (-1 * LVTS_COEFF_A)) & 0xFFFF;

	printk(BIOS_INFO, "%s msr_raw = %u, temp_mc = %d\n", __func__, msr_raw, temp_mc);
	return msr_raw;
}

void thermal_init(void)
{
	lvts_thermal_init();
	thermal_sram_init();
}
