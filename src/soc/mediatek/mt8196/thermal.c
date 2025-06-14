/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/thermal.h>

#define LVTS_DEVICE_ACCESS_DELAY_US		3
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
		.has_reboot_msr_sram = true,
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
		.has_reboot_msr_sram = true,
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

const struct lvts_thermal_controller *lvts_get_controller(int tc_num)
{
	assert(tc_num < LVTS_CONTROLLER_NUM);

	return &lvts_tscpu_g_tc[tc_num];
}

/*
 * The return value is NOT the same as the argument `msr_raw` of lvts_raw_to_temp.
 * Instead, it is equal to "(1 << 28) / msr_raw".
 */
uint16_t lvts_temp_to_raw(int temp_mc, enum lvts_sensor ts_name)
{
	uint32_t msr_raw = 0;
	int64_t coff_a = 0;
	int64_t temp1;

	coff_a = LVTS_COEFF_A;

	temp1 = (int64_t)temp_mc - (golden_temp * 500) + coff_a;
	assert(temp1 > 0);
	msr_raw = ((coff_a << 14) / temp1) & 0xFFFF;

	printk(BIOS_DEBUG, "%s: msr_raw=%u, temp_mc=%d\n", __func__, msr_raw, temp_mc);

	return msr_raw;
}

void lvts_efuse_setting(void)
{
	int i, j, s_index;
	uint32_t efuse_data;
	uint32_t val_0, val_1;

	printk(BIOS_INFO, "%s\n", __func__);

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
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

void lvts_device_identification(void)
{
	uint32_t dev_id, data;
	int i;

	printk(BIOS_INFO, "===== %s begin ======\n", __func__);
	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
		const struct lvts_thermal_controller *tc = lvts_get_controller(i);
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

void lvts_device_enable_init_all_devices(void)
{
	int i;
	uint8_t cali_0, cali_1;

	for (i = 0; i < LVTS_CONTROLLER_NUM; i++) {
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

void lvts_thermal_cal_prepare(void)
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

void lvts_tscpu_thermal_read_tc_temp(const struct lvts_thermal_controller *tc, int order)
{
	uint32_t rg_temp;
	enum lvts_sensor ts_name = tc->ts[order];
	int temperature;

	ASSERT(order < ARRAY_SIZE(tc->regs->lvtsatp));

	temperature = lvts_read_tc_raw_and_temp(&tc->regs->lvtsatp[order],
						ts_name,
						golden_temp);
	rg_temp = read32(&tc->regs->lvtstemp[order]) & 0x7FFFFF;

	printk(BIOS_INFO, "%s order %d ts_name %d temp %d rg_temp %d(%d)\n", __func__, order,
	       ts_name, temperature, (rg_temp * 1000 / 1024), rg_temp);
}

void lvts_tscpu_reset_thermal(void)
{
	/* Enable thermal control software reset */
	write32p(AP_RST_SET, BIT(11));

	/* Clear thermal control software reset */
	write32p(AP_RST_CLR, BIT(11));
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
