/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/thermal.h>
#include <timer.h>

#define LVTS_SINGLE_SENSE_MODE_EN		BIT(9)
#define LVTS_SENSOR_POINT_SELECTION_SETTING	0x13121110
#define LVTS_CALCULATION_SCALING_RULE		0x300

__weak void lvts_set_device_single_mode(int tc_num)
{
}

__weak void lvts_clock_gate_disable(void)
{
}

static int lvts_raw_to_temp(uint16_t msr_raw, enum lvts_sensor ts_name, uint32_t golden_temp)
{
	int temp_mc;
	int64_t temp1, coeff_a;

	coeff_a = LVTS_COEFF_A;

	temp1 = (coeff_a * msr_raw) / (1 << 14);
	temp_mc = temp1 + golden_temp * 500 - coeff_a;

	return temp_mc;
}

int lvts_read_tc_raw_and_temp(void *msr_reg, enum lvts_sensor ts_name, uint32_t golden_temp)
{
	int temp;
	uint32_t msr_data;
	uint16_t msr_raw;

	msr_data = read32(msr_reg);
	msr_raw = msr_data & 0xFFFF;

	if (msr_raw > 0) {
		temp = lvts_raw_to_temp(msr_raw, ts_name, golden_temp);
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

static void lvts_tscpu_thermal_initial_all_tc(void)
{
	u32 i = 0;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
		/* Set sensor index of LVTS */
		write32(&tc->regs->lvtstssel_0, LVTS_SENSOR_POINT_SELECTION_SETTING);
		/* Set calculation scale rules */
		write32(&tc->regs->lvtscalscale_0, LVTS_CALCULATION_SCALING_RULE);
		/* Set Device Single mode */
		lvts_set_device_single_mode(i);

		lvts_configure_polling_speed_and_filter(tc);
	}

}

void lvts_configure_polling_speed_and_filter(const struct lvts_thermal_controller *tc)
{
	u32 lvtsMonCtl1, lvtsMonCtl2;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	lvtsMonCtl1 = ((tc->speed.group_interval_delay << 20) &
		LVTS_GROUP_INTERVAL_DELAY_MASK) | (tc->speed.period_unit & GENMASK(9, 0));

	lvtsMonCtl2 = ((tc->speed.filter_interval_delay << 16) & GENMASK(25, 16)) |
		      (tc->speed.sensor_interval_delay & GENMASK(9, 0));

	/*
	 * Calculating period unit in Module clock x 256, and the Module clock
	 * will be changed to 26M when Infrasys enters Sleep mode.
	 */

	/*
	 * Bus clock 66M counting unit is
	 *           12 * 1/66M * 256 = 12 * 3.879us = 46.545 us
	 */
	write32(&tc->regs->lvtsmonctl1_0, lvtsMonCtl1);
	/*
	 * Filt interval is 1 * 46.545us = 46.545us,
	 * sen interval is 429 * 46.545us = 19.968ms
	 */
	write32(&tc->regs->lvtsmonctl2_0, lvtsMonCtl2);

	/* Temperature sampling control, 1 sample */
	write32(&tc->regs->lvtsmsrctl0_0, 0);

	udelay(1);
	printk(BIOS_INFO,
	       "%s,LVTSMONCTL1_0= %#x,LVTSMONCTL2_0= %#x,LVTSMSRCTL0_0= %#x\n",
	       __func__,
	       read32(&tc->regs->lvtsmonctl1_0),
	       read32(&tc->regs->lvtsmonctl2_0),
	       read32(&tc->regs->lvtsmsrctl0_0));
}

static int lvts_check_all_sensing_points_idle(void)
{
	uint32_t mask, temp;
	int i;

	mask = BIT(10) | BIT(7) | BIT(0);
	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
		temp = read32(&tc->regs->lvtsmsrctl1_0);
		if ((temp & mask) != 0)
			return -1;
	}

	return 0;
}

static void lvts_wait_all_sensing_points_idle(void)
{
	if (!retry(CHECK_DEVICE_ACCESS_RETRY_CNT,
		   lvts_check_all_sensing_points_idle() == 0,
		   udelay(2)))
		printk(BIOS_ERR, "%s timeout\n", __func__);
}

static bool lvts_lk_init_check(void)
{
	int i;

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
		/* Check LVTS device ID */
		if ((read32(&tc->regs->lvtsspare[0]) & GENMASK(11, 0)) != LVTS_MAGIC)
			return false;
	}

	return true;
}

int lvts_write_device(uint16_t config, uint8_t dev_reg_idx, uint8_t data, int tc_num)
{
	const struct lvts_thermal_controller *tc = lvts_get_controller(tc_num);
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
	 * Wait until DEVICE_ACCESS_START_BIT = 0
	 */
	if (!retry(CHECK_DEVICE_ACCESS_RETRY_CNT,
		   !(read32(&tc->regs->lvts_config_0) & DEVICE_ACCESS_START_BIT), udelay(2))) {
		printk(BIOS_ERR, "DEVICE_ACCESS_START_BIT didn't ready, reg0x%x\n",
		       dev_reg_idx);
	}
	return 1;
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

	printk(BIOS_INFO, "%s,value in LVTSMONCTL0_0 = %#x\n", __func__, value);
}

/*
 * disable ALL periodoc temperature sensing point
 */
static void lvts_disable_all_sensing_points(void)
{
	int i;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
		write32(&tc->regs->lvtsmonctl0_0, LVTS_SINGLE_SENSE_MODE_EN);
	}
}

static void lvts_enable_all_sensing_points(void)
{
	int i = 0;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
		lvts_enable_sensing_points(tc);
	}
}

static void lvts_set_init_flag(void)
{
	u32 i;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	/*write init done flag to inform kernel */

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
		printk(BIOS_INFO, "%s %d:%zu, tc_base_addr:%p\n", __func__, i,
		       tc->ts_number, tc->regs);

		write32(&tc->regs->lvtsspare[0], LVTS_MAGIC);
	}
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
	if (tc->has_reboot_msr_sram)
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

void lvts_config_all_tc_hw_protect(void)
{
	int i;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
		lvts_set_tc_trigger_hw_protect(tc);
	}
}

static void read_all_tc_lvts_temperature(void)
{
	int i, j;

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
		for (j = 0; j < tc->ts_number; j++)
			lvts_tscpu_thermal_read_tc_temp(tc, j);
	}
}

void lvts_thermal_init(void)
{
	printk(BIOS_INFO, "===== %s begin ======\n", __func__);

	if (lvts_lk_init_check())
		return;

	lvts_clock_gate_disable();

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
