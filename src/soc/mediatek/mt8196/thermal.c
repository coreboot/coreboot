/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/thermal.h>
#include <soc/thermal_internal.h>

#define LVTS_DEVICE_ACCESS_DELAY_US		3
#define CHECK_DEVICE_ACCESS_RETRY_CNT		100
#define THERMAL_LVTS_MSR_OFT			64552
#define LVTS_READ_ID_DELAY_US			3
#define LVTS_READ_ID_RETRY_CNT			5
#define LVTS_SINGLE_SENSE_MODE_EN		BIT(9)
#define CHECK_SENSING_POINTS_IDLE_RETRY_CNT	100
#define LVTS_DEVICE_WRITE_CONFIG		0x8103
#define LVTS_DEVICE_READ_CONFIG			0x8502
#define LVTS_SENSOR_POINT_SELECTION_SETTING	0x13121110
#define LVTS_CALCULATION_SCALING_RULE		0x00000300

/*
 * module            LVTS Plan
 *=====================================================
 * Dsu0, Core3, Core7_0/1  LVTS1-0, LVTS1-1, LVTS1-2, LVTS1-3
 * Core4_0/1, Core1, Core2 LVTS2-0, LVTS2-1, LVTS2-2, LVTS2-3
 * Dsu2, Dsu1, Core6_0/1   LVTS3-0, LVTS3-1, LVTS3-2, LVTS3-3
 * Dsu3, Core0, Core5_0/1  LVTS4-0, LVTS4-1, LVTS4-2, LVTS4-3
 * APUa                    LVTS5-0, LVTS5-1, LVTS5-2, LVTS5-3
 * GPUa                    LVTS7-0, LVTS7-1
 * SOC-TOP                 LVTS11-0, LVTS11-1, LVTS11-2, LVTS11-3
 * SOC-BOT                 LVTS12-0, LVTS12-1, LVTS12-2, LVTS12-3
 * MD-AP                   LVTS13-0, LVTS13-1, LVTS13-2, LVTS13-3
 * SOC-ADCT                LVTS14-0, LVTS14-3
 * ptp_therm_ctrl_AP  Base address: 0x1441_4000, 0x1441_4100,
 *                                  0x1441_4200
 * ptp_therm_ctrl_MCU Base address: 0x0C23_0000, 0x0C23_0100,
 *                                  0x0C23_0200, 0x0C23_0300,
 */
static const struct lvts_thermal_controller lvts_tscpu_g_tc[LVTS_CONTROLLER_NUM] = {
	[LVTS_AP_CONTROLLER0] = { /* SOC-TOP */
		.ts = {L_TS_LVTS11_0, L_TS_LVTS11_1, L_TS_LVTS11_2, L_TS_LVTS11_3},
		.sensor_on_off = {SEN_ON, SEN_ON, SEN_ON, SEN_ON},
		.ts_number = 4,
		.reboot_temperature = 118800,
		.dominator_ts_idx = 0,
		.reboot_msr_sram_idx = 0,
		.has_reboot_temp_sram = true,
		.speed = {
			.group_interval_delay = 0x7fff,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001,
		},
		.regs = mtk_lvts_ap_controller0,
	},
	[LVTS_AP_CONTROLLER1] = { /* SOC-BOT */
		.ts = {L_TS_LVTS12_0, L_TS_LVTS12_1, L_TS_LVTS12_2, L_TS_LVTS12_3},
		.sensor_on_off = {SEN_ON, SEN_ON, SEN_ON, SEN_ON},
		.ts_number = 4,
		.reboot_temperature = 118800,
		.dominator_ts_idx = 0,
		.reboot_msr_sram_idx = 1,
		.has_reboot_temp_sram = false,
		.speed = {
			.group_interval_delay = 0x7fff,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001,
		},
		.regs = mtk_lvts_ap_controller1,
	},
};

static uint32_t golden_temp;
static uint32_t ts_edata[L_TS_LVTS_NUM];
static uint8_t op_cali[LVTS_CONTROLLER_NUM];

static int lvts_write_device(uint16_t config, uint8_t dev_reg_idx, uint8_t data, int tc_num)
{
	const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[tc_num];
	uint32_t config_data = (config << 16) | (dev_reg_idx << 8) | data;

	write32(&tc->regs->lvts_config_0, config_data);

	/*
	 * LVTS Device Register Setting takes 1us (by 26MHz clock source)
	 * interface latency to access.
	 * So we set 2~3 us delay could guarantee access complete.
	 */
	udelay(LVTS_DEVICE_ACCESS_DELAY_US);

	/*
	 * Check ASIF bus status for transaction finished
	 * Wait until DEVICE_ACCESS_START = 0
	 */
	if (!retry(CHECK_DEVICE_ACCESS_RETRY_CNT,
		   !(read32(&tc->regs->lvts_config_0) & DEVICE_ACCESS_START_BIT), udelay(2))) {
		printk(BIOS_ERR, "DEVICE_ACCESS_START didn't ready, reg0x%x\n",
		       dev_reg_idx);
	}

	return 1;
}

static int lvts_raw_to_temp(uint16_t msr_raw, enum lvts_sensor ts_name)
{
	/* In millidegree Celsius */
	int temp_mc;
	int64_t temp1, coff_a;

	coff_a = LVTS_COF_T_SLP_GLD;

	temp1 = (coff_a * msr_raw) / (1 << 14);
	temp_mc = temp1 + golden_temp * 500 - coff_a;

	return temp_mc;
}

/*
 * The return value is NOT the same as the argument `msr_raw` of lvts_raw_to_temp.
 * Instead, it is equal to "(1 << 28) / msr_raw".
 */
static uint16_t lvts_temp_to_raw(int temp_mc, enum lvts_sensor ts_name)
{
	uint32_t msr_raw = 0;
	int64_t coff_a = 0;
	int64_t temp1;

	coff_a = LVTS_COF_T_SLP_GLD;

	temp1 = (int64_t)temp_mc - (golden_temp * 500) + coff_a;
	assert(temp1 > 0);
	msr_raw = ((coff_a << 14) / temp1) & 0xFFFF;

	printk(BIOS_DEBUG, "%s: msr_raw=%u, temp_mc=%d\n", __func__, msr_raw, temp_mc);

	return msr_raw;
}

static void lvts_efuse_setting(void)
{
	int i, j, s_index;
	uint32_t efuse_data;
	uint32_t val_0, val_1;

	printk(BIOS_INFO, "%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[i];
		val_0 = 0;
		val_1 = 0;
		for (j = 0; j < tc->ts_number; j++) {
			if (tc->sensor_on_off[j] == SEN_OFF)
				continue;

			s_index = tc->ts[j];

			efuse_data = ts_edata[s_index] + THERMAL_LVTS_MSR_OFT;

			switch (j) {
			case 0:
				write32(&tc->regs->lvtsedata[0], efuse_data);
				printk(BIOS_INFO, "efuse LVTSEDATA00_%d %#x\n", i,
				       read32(&tc->regs->lvtsedata[0]));
				val_0 |= ((LVTS_COF_T_SLP_GLD + 500) / 1000);
				break;

			case 1:
				write32(&tc->regs->lvtsedata[1], efuse_data);
				printk(BIOS_INFO, "efuse LVTSEDATA01_%d %#x\n", i,
				       read32(&tc->regs->lvtsedata[1]));
				val_0 |= (((LVTS_COF_T_SLP_GLD + 500) / 1000) << 10);
				break;
			case 2:
				write32(&tc->regs->lvtsedata[2], efuse_data);
				printk(BIOS_INFO, "efuse LVTSEDATA02_%d %#x\n", i,
				       read32(&tc->regs->lvtsedata[2]));
				val_1 |= ((LVTS_COF_T_SLP_GLD + 500) / 1000);
				break;
			case 3:
				write32(&tc->regs->lvtsedata[3], efuse_data);
				printk(BIOS_INFO, "efuse LVTSEDATA03_%d %#x\n", i,
				       read32(&tc->regs->lvtsedata[3]));
				val_1 |= (((LVTS_COF_T_SLP_GLD + 500) / 1000) << 10);
				break;
			default:
				printk(BIOS_ERR, "%s, illegal ts order : %d!!\n", __func__, j);
				break;
			}
		}
		val_0 |= (golden_temp << 20);
		printk(BIOS_INFO, "%s, spare setting: %#x, %#x\n", __func__, val_0, val_1);
		write32(&tc->regs->lvtsmsroft_0, THERMAL_LVTS_MSR_OFT);
		write32(&tc->regs->lvtsspare[1], val_0);
		write32(&tc->regs->lvtsspare[2], val_1);
	}
}

static void lvts_device_identification(void)
{
	uint32_t dev_id, data;
	int i;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);
	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[i];
		/* Enable LVTS_CTRL Clock */
		write32(&tc->regs->lvtsclken_0, 0x00000001);

		/* Turn On LDO & set DIV_MODE and wait 10 us */
		write32(&tc->regs->lvtsgslope_0, LDO_ON_SETTING);
		udelay(10);

		/* Reset All Devices */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0xFF, 0xFF, i);

		/* Read back Dev_ID with Update */
		lvts_write_device(LVTS_DEVICE_READ_CONFIG, 0xFC, 0x55, i);

		dev_id = 0x8B + i;

		if (!retry(LVTS_READ_ID_RETRY_CNT,
			   (data = read32(&tc->regs->lvts_id_0) & GENMASK(7, 0)) == dev_id,
			   udelay(LVTS_READ_ID_DELAY_US)))
			printk(BIOS_ERR,
			       "LVTS_TC_%d read timeout, addr:0x%lx, Device ID should be 0x%x, but 0x%x\n",
			       i, (uintptr_t)(&tc->regs->lvts_id_0), dev_id, data);
	}
}

static void lvts_device_enable_init_all_devices(void)
{
	int i;
	uint8_t cali_0, cali_1;

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		/* Stop Counting (RG_TSFM_ST=0) */
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x03, 0x00, i);

		/* Set Bandgap Calibration */
		if ((op_cali[i] & (1 << 5)) != 0) {
			cali_0 = (op_cali[i] & 0xf) << 4;
			cali_1 = ((op_cali[i] >> 4) & 1) << 4;
		} else {
			cali_0 = (op_cali[i] & 0xf);
			cali_1 = ((op_cali[i] >> 4) & 1) << 3;
		}
		cali_1 |= 0xA0;

		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x20, cali_0, i);
		lvts_write_device(LVTS_DEVICE_WRITE_CONFIG, 0x21, cali_1, i);
	}
}

static void lvts_thermal_cal_prepare(void)
{
	uint32_t temp[7];
	int i, j;
	bool efuse_calibrated = false;

	temp[0] = read32p(LVTS_ADDRESS_INDEX_0);
	temp[1] = read32p(LVTS_ADDRESS_INDEX_12);
	temp[2] = read32p(LVTS_ADDRESS_INDEX_13);
	temp[3] = read32p(LVTS_ADDRESS_INDEX_14);
	temp[4] = read32p(LVTS_ADDRESS_INDEX_15);
	temp[5] = read32p(LVTS_ADDRESS_INDEX_27);
	temp[6] = read32p(LVTS_ADDRESS_INDEX_28);

	printk(BIOS_INFO,
	       "[lvts_cali] %d: %#x, %d: %#x, %d: %#x, %d: %#x, %d: %#x, %d: %#x, %d: %#x\n",
	       0, temp[0], 1, temp[1], 2, temp[2], 3, temp[3], 4, temp[4], 5, temp[5], 6, temp[6]);

	golden_temp = temp[0] & GENMASK(7, 0);

	ts_edata[L_TS_LVTS11_0] = (temp[1] & GENMASK(31, 16)) >> 16;
	ts_edata[L_TS_LVTS11_1] = temp[1] & GENMASK(15, 0);
	ts_edata[L_TS_LVTS11_2] = (temp[2] & GENMASK(31, 16)) >> 16;
	ts_edata[L_TS_LVTS11_3] = temp[2] & GENMASK(15, 0);
	ts_edata[L_TS_LVTS12_0] = (temp[3] & GENMASK(31, 16)) >> 16;
	ts_edata[L_TS_LVTS12_1] = temp[3] & GENMASK(15, 0);
	ts_edata[L_TS_LVTS12_2] = (temp[4] & GENMASK(31, 16)) >> 16;
	ts_edata[L_TS_LVTS12_3] = temp[4] & GENMASK(15, 0);
	op_cali[LVTS_AP_CONTROLLER0] = (temp[5] & GENMASK(29, 24)) >> 24;
	op_cali[LVTS_AP_CONTROLLER1] = (temp[6] & GENMASK(29, 24)) >> 24;

	for (i = 0; i < ARRAY_SIZE(temp); i++) {
		if (temp[i] != 0) {
			efuse_calibrated = true;
			break;
		}
	}

	if (!efuse_calibrated) {
		/* It means all efuse data are equal to 0 */
		printk(BIOS_ERR, "[lvts_cal] This sample is not calibrated; Use fake efuse\n");
		golden_temp = DEFAULT_EFUSE_GOLDEN_TEMP;
		for (i = 0; i < L_TS_LVTS_NUM; i++)
			ts_edata[i] = DEFAULT_EFUSE_COUNT;

		for (i = 0; i < LVTS_CONTROLLER_NUM; i++)
			op_cali[i] = 0;
	}
	printk(BIOS_INFO, "[lvts_cal] golden_temp = %d\n", golden_temp);

	printk(BIOS_INFO, "[lvts_cal] num:ts_edata ");
	for (i = 0; i < L_TS_LVTS_NUM; i++)
		printk(BIOS_INFO, "%d:%d ", i, ts_edata[i]);
	printk(BIOS_INFO, "\n");

	printk(BIOS_INFO, "num:op_cali ");
	for (j = 0; j < LVTS_CONTROLLER_NUM; j++)
		printk(BIOS_INFO, "%d:0x%x ", j, op_cali[j]);

	printk(BIOS_INFO, "\n");

	write32(&lvts_tscpu_g_tc[LVTS_AP_CONTROLLER0].regs->lvtsspare[1], golden_temp);
}

static int lvts_read_tc_raw_and_temp(void *msr_reg, enum lvts_sensor ts_name)
{
	int temp;
	uint32_t msr_data;
	uint16_t msr_raw;

	msr_data = read32(msr_reg);
	msr_raw = msr_data & 0xFFFF;

	if (msr_raw > 0) {
		temp = lvts_raw_to_temp(msr_raw, ts_name);
	} else {
		/*
		 * 26111 is magic num.
		 * This is to keep system alive for a while to wait until
		 * HW init is done, because 0 msr raw will translate to 28x'C
		 * and then 28x'C will trigger a SW reset.
		 *
		 * If HW init finishes, this msr raw will not be 0,
		 * system can report normal temperature.
		 * If wait over 60 times zero, this means something wrong with HW.
		 */
		temp = 26111;
	}

	printk(BIOS_INFO, "[LVTS_MSR] ts%d msr_all=%x, msr_temp=%d, temp=%d\n", ts_name,
	       msr_data, msr_raw, temp);

	return temp;
}

static void lvts_tscpu_thermal_read_tc_temp(const struct lvts_thermal_controller *tc, int order)
{
	uint32_t rg_temp;
	enum lvts_sensor ts_name = tc->ts[order];
	int temperature;

	ASSERT(order < ARRAY_SIZE(tc->regs->lvtsatp));

	temperature = lvts_read_tc_raw_and_temp(&tc->regs->lvtsatp[order], ts_name);
	rg_temp = read32(&tc->regs->lvtstemp[order]) & 0x7FFFFF;

	printk(BIOS_INFO, "%s order %d ts_name %d temp %d rg_temp %d(%d)\n", __func__, order,
	       ts_name, temperature, (rg_temp * 1000 / 1024), rg_temp);
}

static void read_all_tc_lvts_temperature(void)
{
	int i, j;

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[i];
		for (j = 0; j < tc->ts_number; j++)
			lvts_tscpu_thermal_read_tc_temp(tc, j);
	}
}

static void lvts_enable_sensing_points(const struct lvts_thermal_controller *tc)
{
	int i;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);
	uint32_t value = LVTS_SINGLE_SENSE_MODE_EN;

	for (i = 0; i < tc->ts_number; i++) {
		if (tc->sensor_on_off[i] == SEN_ON)
			value |= BIT(i);
	}

	write32(&tc->regs->lvtsmonctl0_0, value);

	printk(BIOS_INFO, "%s, value in LVTSMONCTL0_0 = %#x\n", __func__, value);
}

/*
 * disable ALL periodoc temperature sensing point
 */
static void lvts_disable_all_sensing_points(void)
{
	int i = 0;

	printk(BIOS_INFO, "%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[i];
		write32(&tc->regs->lvtsmonctl0_0, LVTS_SINGLE_SENSE_MODE_EN);
	}
}

static int lvts_check_all_sensing_points_idle(void)
{
	uint32_t mask, temp;
	int i;

	mask = BIT(10) | BIT(7) | BIT(0);
	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[i];
		temp = read32(&tc->regs->lvtsmsrctl1_0);
		if ((temp & mask) != 0)
			return -1;
	}

	return 0;
}

static void lvts_wait_all_sensing_points_idle(void)
{
	if (!retry(CHECK_SENSING_POINTS_IDLE_RETRY_CNT,
		   lvts_check_all_sensing_points_idle() == 0,
		   udelay(2)))
		printk(BIOS_ERR, "%s timeout\n", __func__);
}

static void lvts_enable_all_sensing_points(void)
{
	int i = 0;

	printk(BIOS_INFO, "%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[i];
		lvts_enable_sensing_points(tc);
	}
}

static void lvts_set_init_flag(void)
{
	int i;

	printk(BIOS_INFO, "%s\n", __func__);

	/* write init done flag to inform kernel */

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[i];
		printk(BIOS_INFO, "%s %d:%zu, tc_base_addr:%p\n", __func__, i,
		       tc->ts_number, tc->regs);

		write32(&tc->regs->lvtsspare[0], LVTS_MAGIC);
	}
}

static void lvts_configure_polling_speed_and_filter(const struct lvts_thermal_controller *tc)
{
	uint32_t lvts_mon_ctl1, lvts_mon_ctl2;

	lvts_mon_ctl1 = (((tc->speed.group_interval_delay << 17) & GENMASK(31, 17)) |
		(tc->speed.period_unit & GENMASK(9, 0)));

	lvts_mon_ctl2 = (((tc->speed.filter_interval_delay << 16) & GENMASK(25, 16)) |
		(tc->speed.sensor_interval_delay & GENMASK(9, 0)));

	/*
	 * Calculate period unit in Module clock x 256, and the Module clock
	 * will be changed to 26M when Infrasys enters Sleep mode.
	 */

	/*
	 * bus clock 66M counting unit is
	 * 12 * 1/66M * 256 = 12 * 3.879us = 46.545 us
	 */
	write32(&tc->regs->lvtsmonctl1_0, lvts_mon_ctl1);
	/*
	 * filt interval is 1 * 46.545us = 46.545us,
	 * sen interval is 429 * 46.545us = 19.968ms
	 */
	write32(&tc->regs->lvtsmonctl2_0, lvts_mon_ctl2);

	/* temperature sampling control, 1 sample */
	write32(&tc->regs->lvtsmsrctl0_0, 0);

	udelay(1);
	printk(BIOS_INFO, "%s, LVTSMONCTL1_0= 0x%x,LVTSMONCTL2_0= 0x%x,LVTSMSRCTL0_0= 0x%x\n",
	       __func__, read32(&tc->regs->lvtsmonctl1_0), read32(&tc->regs->lvtsmonctl2_0),
	       read32(&tc->regs->lvtsmsrctl0_0));
}

static void lvts_tscpu_thermal_initial_all_tc(void)
{
	uint32_t i = 0;

	printk(BIOS_INFO, "%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[i];
		/*  set sensor index of LVTS */
		write32(&tc->regs->lvtstssel_0, LVTS_SENSOR_POINT_SELECTION_SETTING);
		/*  set calculation scale rules */
		write32(&tc->regs->lvtscalscale_0, LVTS_CALCULATION_SCALING_RULE);

		lvts_configure_polling_speed_and_filter(tc);
	}

}

static void lvts_tscpu_reset_thermal(void)
{
	/* Enable thermal control software reset */
	write32p(AP_RST_SET, BIT(11));

	/* Clear thermal control software reset */
	write32p(AP_RST_CLR, BIT(11));
}

static void lvts_set_tc_trigger_hw_protect(const struct lvts_thermal_controller *tc)
{
	int d_index, i;
	uint32_t raw_high;
	uint16_t raw;
	enum lvts_sensor ts_name;

	if (tc->dominator_ts_idx < tc->ts_number) {
		d_index = tc->dominator_ts_idx;
	} else {
		printk(BIOS_ERR, "LVTS, dominator_ts_idx %d >= ts_number %zu; use idx 0\n",
		       tc->dominator_ts_idx, tc->ts_number);
		d_index = 0;
	}

	ts_name = tc->ts[d_index];

	printk(BIOS_INFO, "%s, the dominator ts_name is %d\n", __func__, ts_name);

	/* Maximum of 4 sensing points */
	raw_high = 0;
	for (i = 0; i < tc->ts_number; i++) {
		ts_name = tc->ts[i];
		raw = lvts_temp_to_raw(tc->reboot_temperature, ts_name);
		raw_high = MAX(raw_high, raw);
	}

	thermal_write_reboot_msr_sram(tc->reboot_msr_sram_idx, raw_high);
	if (tc->has_reboot_temp_sram)
		thermal_write_reboot_temp_sram(tc->reboot_temperature);

	setbits32(&tc->regs->lvtsprotctl_0, 0x3FFF);
	/* disable trigger SPM interrupt */
	write32(&tc->regs->lvtsmonint_0, 0);

	clrsetbits32(&tc->regs->lvtsprotctl_0, 0xF << 16, BIT(16));

	write32(&tc->regs->lvtsprottc_0, raw_high);

	/* enable trigger Hot SPM interrupt */
	write32(&tc->regs->lvtsmonint_0, STAGE3_INT_EN);

	clrbits32(&tc->regs->lvtsprotctl_0, 0xFFFF);
}

static void lvts_config_all_tc_hw_protect(void)
{
	int i = 0;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[i];
		lvts_set_tc_trigger_hw_protect(tc);
	}
}

static bool lvts_lk_init_check(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		const struct lvts_thermal_controller *tc = &lvts_tscpu_g_tc[i];
		/* Check LVTS device ID */
		if ((read32(&tc->regs->lvtsspare[0]) & GENMASK(11, 0)) != LVTS_MAGIC)
			return false;
	}

	return true;
}

static void lvts_thermal_init(void)
{
	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	if (lvts_lk_init_check())
		return;

	lvts_tscpu_reset_thermal();

	lvts_thermal_cal_prepare();
	lvts_device_identification();
	lvts_device_enable_init_all_devices();
	lvts_efuse_setting();

	lvts_disable_all_sensing_points();
	lvts_wait_all_sensing_points_idle();
	lvts_tscpu_thermal_initial_all_tc();
	lvts_config_all_tc_hw_protect();
	lvts_enable_all_sensing_points();

	lvts_set_init_flag();

	printk(BIOS_INFO, "%s: thermal initialized\n", __func__);

	read_all_tc_lvts_temperature();
}

static void reset_cpu_lvts(void)
{
	write32p(CPU_LVTS_RESET_ADDR, 1);
	udelay(10);
	write32p(CPU_LVTS_RESET_ADDR, 0);

	printk(BIOS_INFO, "%s done\n", __func__);
}

void thermal_init(void)
{
	reset_cpu_lvts();
	lvts_thermal_init();
	thermal_sram_init();
}
