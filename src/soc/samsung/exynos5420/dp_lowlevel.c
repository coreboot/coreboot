/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <soc/dp.h>
#include <soc/fimd.h>
#include <soc/i2c.h>
#include <soc/power.h>
#include <soc/sysreg.h>

/* FIXME: I think the DP controller shouldn't be hardcoded here... */
static struct exynos_dp * const dp_regs = (void *)EXYNOS5_DP1_BASE;

/* for debugging, it's nice to get control on a per-file basis.
 * I had a bit of a discussion with myself (boring!) about
 * how to do this and for the moment this is the easiest way.
 * These debugging statements allowed me to find the final bugs.
 */

#if 0
static inline void fwadl(unsigned long l,void *v) {
	writel(l, v);
	printk(BIOS_SPEW, "W %p %p\n", v, (void *)l);
}
#define lwrite32(a,b) fwadl((unsigned long)(a), (void *)(b))

static inline unsigned long fradl(void *v) {
	unsigned long l = readl(v);
	printk(BIOS_SPEW, "R %p %p\n", v, (void *)l);
	return l;
}

#define lread32(a) fradl((void *)(a))
#else
#define lwrite32(a,b) write32((void *)(b), (unsigned long)(a))
#define lread32(a) read32((void *)(a))
#endif

static void exynos_dp_enable_video_input(u32 enable)
{
	u32 reg;

	reg = lread32(&dp_regs->video_ctl1);
	reg &= ~VIDEO_EN_MASK;

	/* enable video input*/
	if (enable)
		reg |= VIDEO_EN_MASK;

	lwrite32(reg, &dp_regs->video_ctl1);
}

void exynos_dp_disable_video_bist(void)
{
	u32 reg;
	reg = lread32(&dp_regs->video_ctl4);
	reg &= ~VIDEO_BIST_MASK;
	lwrite32(reg, &dp_regs->video_ctl4);
}

void exynos_dp_enable_video_mute(unsigned int enable)
{
	u32 reg;

	reg = lread32(&dp_regs->video_ctl1);
	reg &= ~(VIDEO_MUTE_MASK);
	if (enable)
		reg |= VIDEO_MUTE_MASK;

	lwrite32(reg, &dp_regs->video_ctl1);
}

static void exynos_dp_init_analog_param(void)
{
	u32 reg;

	/*
	 * Set termination
	 * Normal bandgap, Normal swing, Tx terminal resistor 61 ohm
	 * 24M Phy clock, TX digital logic power is 100:1.0625V
	 */
	reg = SEL_BG_NEW_BANDGAP | TX_TERMINAL_CTRL_61_OHM |
		SWING_A_30PER_G_NORMAL;
	lwrite32(reg, &dp_regs->analog_ctl1);

	reg = SEL_24M | TX_DVDD_BIT_1_0625V;
	lwrite32(reg, &dp_regs->analog_ctl2);

	/*
	 * Set power source for internal clk driver to 1.0625v.
	 * Select current reference of TX driver current to 00:Ipp/2+Ic/2.
	 * Set VCO range of PLL +- 0uA
	 */
	reg = DRIVE_DVDD_BIT_1_0625V | SEL_CURRENT_DEFAULT | VCO_BIT_000_MICRO;
	lwrite32(reg, &dp_regs->analog_ctl3);

	/*
	 * Set AUX TX terminal resistor to 102 ohm
	 * Set AUX channel amplitude control
	*/
	reg = PD_RING_OSC | AUX_TERMINAL_CTRL_52_OHM | TX_CUR1_2X | TX_CUR_4_MA;
	lwrite32(reg, &dp_regs->pll_filter_ctl1);

	/*
	 * PLL loop filter bandwidth
	 * For 2.7Gbps: 175KHz, For 1.62Gbps: 234KHz
	 * PLL digital power select: 1.2500V
	 */
	reg = CH3_AMP_0_MV | CH2_AMP_0_MV | CH1_AMP_0_MV | CH0_AMP_0_MV;

	lwrite32(reg, &dp_regs->amp_tuning_ctl);

	/*
	 * PLL loop filter bandwidth
	 * For 2.7Gbps: 175KHz, For 1.62Gbps: 234KHz
	 * PLL digital power select: 1.1250V
	 */
	reg = DP_PLL_LOOP_BIT_DEFAULT | DP_PLL_REF_BIT_1_1250V;
	lwrite32(reg, &dp_regs->pll_ctl);
}

static void exynos_dp_init_interrupt(void)
{
	/* Set interrupt registers to initial states */

	/*
	 * Disable interrupt
	 * INT pin assertion polarity. It must be configured
	 * correctly according to ICU setting.
	 * 1 = assert high, 0 = assert low
	 */
	lwrite32(INT_POL, &dp_regs->int_ctl);

	/* Clear pending registers */
	lwrite32(0xff, &dp_regs->common_int_sta1);
	lwrite32(0xff, &dp_regs->common_int_sta2);
	lwrite32(0xff, &dp_regs->common_int_sta3);
	lwrite32(0xff, &dp_regs->common_int_sta4);
	lwrite32(0xff, &dp_regs->int_sta);

	/* 0:mask,1: unmask */
	lwrite32(0x00, &dp_regs->int_sta_mask1);
	lwrite32(0x00, &dp_regs->int_sta_mask2);
	lwrite32(0x00, &dp_regs->int_sta_mask3);
	lwrite32(0x00, &dp_regs->int_sta_mask4);
	lwrite32(0x00, &dp_regs->int_sta_mask);
}

void exynos_dp_reset(void)
{
	u32 reg_func_1;

	/*dp tx sw reset*/
	lwrite32(RESET_DP_TX, &dp_regs->tx_sw_reset);

	exynos_dp_enable_video_input(DP_DISABLE);
	exynos_dp_disable_video_bist();
	exynos_dp_enable_video_mute(DP_DISABLE);

	/* software reset */
	reg_func_1 = MASTER_VID_FUNC_EN_N | SLAVE_VID_FUNC_EN_N |
		AUD_FIFO_FUNC_EN_N | AUD_FUNC_EN_N |
		HDCP_FUNC_EN_N | SW_FUNC_EN_N;

	lwrite32(reg_func_1, &dp_regs->func_en1);
	lwrite32(reg_func_1, &dp_regs->func_en2);

	mdelay(1);

	exynos_dp_init_analog_param();
	exynos_dp_init_interrupt();
}

void exynos_dp_enable_sw_func(unsigned int enable)
{
	u32 reg;

	reg = lread32(&dp_regs->func_en1);
	reg &= ~(SW_FUNC_EN_N);

	if (!enable)
		reg |= SW_FUNC_EN_N;

	lwrite32(reg, &dp_regs->func_en1);
}

unsigned int exynos_dp_set_analog_power_down(unsigned int block, u32 enable)
{
	u32 reg;

	reg = lread32(&dp_regs->phy_pd);
	switch (block) {
	case AUX_BLOCK:
		reg &= ~(AUX_PD);
		if (enable)
			reg |= AUX_PD;
		break;
	case CH0_BLOCK:
		reg &= ~(CH0_PD);
		if (enable)
			reg |= CH0_PD;
		break;
	case CH1_BLOCK:
		reg &= ~(CH1_PD);
		if (enable)
			reg |= CH1_PD;
		break;
	case CH2_BLOCK:
		reg &= ~(CH2_PD);
		if (enable)
			reg |= CH2_PD;
		break;
	case CH3_BLOCK:
		reg &= ~(CH3_PD);
		if (enable)
			reg |= CH3_PD;
		break;
	case ANALOG_TOTAL:
		reg &= ~PHY_PD;
		if (enable)
			reg |= PHY_PD;
		break;
	case POWER_ALL:
		reg &= ~(PHY_PD | AUX_PD | CH0_PD | CH1_PD | CH2_PD |
			CH3_PD);
		if (enable)
			reg |= (PHY_PD | AUX_PD | CH0_PD | CH1_PD |
				CH2_PD | CH3_PD);
		break;
	default:
		printk(BIOS_ERR, "DP undefined block number : %d\n",  block);
		return -1;
	}

	lwrite32(reg, &dp_regs->phy_pd);

	return 0;
}

unsigned int exynos_dp_get_pll_lock_status(void)
{
	u32 reg;

	reg = lread32(&dp_regs->debug_ctl);

	if (reg & PLL_LOCK)
		return PLL_LOCKED;
	else
		return PLL_UNLOCKED;
}

static void exynos_dp_set_pll_power(unsigned int enable)
{
	u32 reg;

	reg = lread32(&dp_regs->pll_ctl);
	reg &= ~(DP_PLL_PD);

	if (!enable)
		reg |= DP_PLL_PD;

	lwrite32(reg, &dp_regs->pll_ctl);
}

int exynos_dp_init_analog_func(void)
{
	int ret = EXYNOS_DP_SUCCESS;
	unsigned int retry_cnt = 10;
	u32 reg;

	/*Power On All Analog block */
	exynos_dp_set_analog_power_down(POWER_ALL, DP_DISABLE);

	reg = PLL_LOCK_CHG;
	lwrite32(reg, &dp_regs->common_int_sta1);

	reg = lread32(&dp_regs->debug_ctl);
	reg &= ~(F_PLL_LOCK | PLL_LOCK_CTRL);
	lwrite32(reg, &dp_regs->debug_ctl);

	/*Assert DP PLL Reset*/
	reg = lread32(&dp_regs->pll_ctl);
	reg |= DP_PLL_RESET;
	lwrite32(reg, &dp_regs->pll_ctl);

	mdelay(1);

	/*Deassert DP PLL Reset*/
	reg = lread32(&dp_regs->pll_ctl);
	reg &= ~(DP_PLL_RESET);
	lwrite32(reg, &dp_regs->pll_ctl);

	exynos_dp_set_pll_power(DP_ENABLE);

	while (exynos_dp_get_pll_lock_status() == PLL_UNLOCKED) {
		mdelay(1);
		retry_cnt--;
		if (retry_cnt == 0) {
			printk(BIOS_ERR, "DP dp's pll lock failed : retry : %d\n",
					retry_cnt);
			return -1;
		}
	}

	printk(BIOS_DEBUG, "dp's pll lock success(%d)\n", retry_cnt);

	/* Enable Serdes FIFO function and Link symbol clock domain module */
	reg = lread32(&dp_regs->func_en2);
	reg &= ~(SERDES_FIFO_FUNC_EN_N | LS_CLK_DOMAIN_FUNC_EN_N
		| AUX_FUNC_EN_N);
	lwrite32(reg, &dp_regs->func_en2);

	return ret;
}

void exynos_dp_init_hpd(void)
{
	u32 reg;

	/* Clear interrupts related to Hot Plug Detect */
	reg = HOTPLUG_CHG | HPD_LOST | PLUG;
	lwrite32(reg, &dp_regs->common_int_sta4);

	reg = INT_HPD;
	lwrite32(reg, &dp_regs->int_sta);

	reg = lread32(&dp_regs->sys_ctl3);
	reg &= ~(F_HPD | HPD_CTRL);
	lwrite32(reg, &dp_regs->sys_ctl3);
}

static inline void exynos_dp_reset_aux(void)
{
	u32 reg;

	/* Disable AUX channel module */
	reg = lread32(&dp_regs->func_en2);
	reg |= AUX_FUNC_EN_N;
	lwrite32(reg, &dp_regs->func_en2);
}

void exynos_dp_init_aux(void)
{
	u32 reg;

	/* Clear interrupts related to AUX channel */
	reg = RPLY_RECEIV | AUX_ERR;
	lwrite32(reg, &dp_regs->int_sta);

	exynos_dp_reset_aux();

	/* Disable AUX transaction H/W retry */
	reg = AUX_BIT_PERIOD_EXPECTED_DELAY(3) | AUX_HW_RETRY_COUNT_SEL(3)|
		AUX_HW_RETRY_INTERVAL_600_MICROSECONDS;
	lwrite32(reg, &dp_regs->aux_hw_retry_ctl);

	/* Receive AUX Channel DEFER commands equal to DEFFER_COUNT*64 */
	reg = DEFER_CTRL_EN | DEFER_COUNT(1);
	lwrite32(reg, &dp_regs->aux_ch_defer_ctl);

	/* Enable AUX channel module */
	reg = lread32(&dp_regs->func_en2);
	reg &= ~AUX_FUNC_EN_N;
	lwrite32(reg, &dp_regs->func_en2);
}

void exynos_dp_config_interrupt(void)
{
	u32 reg;

	/* 0: mask, 1: unmask */
	reg = COMMON_INT_MASK_1;
	lwrite32(reg, &dp_regs->common_int_mask1);

	reg = COMMON_INT_MASK_2;
	lwrite32(reg, &dp_regs->common_int_mask2);

	reg = COMMON_INT_MASK_3;
	lwrite32(reg, &dp_regs->common_int_mask3);

	reg = COMMON_INT_MASK_4;
	lwrite32(reg, &dp_regs->common_int_mask4);

	reg = INT_STA_MASK;
	lwrite32(reg, &dp_regs->int_sta_mask);
}

unsigned int exynos_dp_get_plug_in_status(void)
{
	u32 reg;

	reg = lread32(&dp_regs->sys_ctl3);
	if (reg & HPD_STATUS)
		return 0;

	return -1;
}

unsigned int exynos_dp_detect_hpd(void)
{
	int timeout_loop = DP_TIMEOUT_LOOP_COUNT;

	mdelay(2);

	while (exynos_dp_get_plug_in_status() != 0) {
		if (timeout_loop == 0)
			return -1;
		mdelay(1);
		timeout_loop--;
	}

	return EXYNOS_DP_SUCCESS;
}

unsigned int exynos_dp_start_aux_transaction(void)
{
	u32 reg;
	unsigned int ret = 0;
	unsigned int retry_cnt;

	/* Enable AUX CH operation */
	reg = lread32(&dp_regs->aux_ch_ctl2);
	reg |= AUX_EN;
	lwrite32(reg, &dp_regs->aux_ch_ctl2);

	retry_cnt = 10;
	while (retry_cnt) {
		reg = lread32(&dp_regs->int_sta);
		if (!(reg & RPLY_RECEIV)) {
			if (retry_cnt == 0) {
				printk(BIOS_ERR, "DP Reply Timeout!!\n");
				ret = -1;
				return ret;
			}
			mdelay(1);
			retry_cnt--;
		} else
			break;
	}

	/* Clear interrupt source for AUX CH command reply */
	lwrite32(reg, &dp_regs->int_sta);

	/* Clear interrupt source for AUX CH access error */
	reg = lread32(&dp_regs->int_sta);
	if (reg & AUX_ERR) {
		printk(BIOS_ERR, "DP Aux Access Error\n");
		lwrite32(AUX_ERR, &dp_regs->int_sta);
		ret = -1;
		return ret;
	}

	/* Check AUX CH error access status */
	reg = lread32(&dp_regs->aux_ch_sta);
	if ((reg & AUX_STATUS_MASK) != 0) {
		printk(BIOS_DEBUG, "DP AUX CH error happens: %x\n", reg & AUX_STATUS_MASK);
		ret = -1;
		return ret;
	}
	return EXYNOS_DP_SUCCESS;
}

unsigned int exynos_dp_write_byte_to_dpcd(u32 reg_addr, u8 data)
{
	u32 reg;
	unsigned int ret;

	/* Clear AUX CH data buffer */
	reg = BUF_CLR;
	lwrite32(reg, &dp_regs->buffer_data_ctl);

	/* Select DPCD device address */
	reg = AUX_ADDR_7_0(reg_addr);
	lwrite32(reg, &dp_regs->aux_addr_7_0);
	reg = AUX_ADDR_15_8(reg_addr);
	lwrite32(reg, &dp_regs->aux_addr_15_8);
	reg = AUX_ADDR_19_16(reg_addr);
	lwrite32(reg, &dp_regs->aux_addr_19_16);

	/* Write data buffer */
	reg = data;
	lwrite32(reg, &dp_regs->buf_data0);

	/*
	 * Set DisplayPort transaction and write 1 byte
	 * If bit 3 is 1, DisplayPort transaction.
	 * If Bit 3 is 0, I2C transaction.
	 */
	reg = AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_WRITE;
	lwrite32(reg, &dp_regs->aux_ch_ctl1);

	/* Start AUX transaction */
	ret = exynos_dp_start_aux_transaction();
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP Aux transaction failed\n");
	}

	return ret;
}

unsigned int exynos_dp_read_byte_from_dpcd(u32 reg_addr,
		unsigned char *data)
{
	u32 reg;
	int retval;

	/* Clear AUX CH data buffer */
	reg = BUF_CLR;
	lwrite32(reg, &dp_regs->buffer_data_ctl);

	/* Select DPCD device address */
	reg = AUX_ADDR_7_0(reg_addr);
	lwrite32(reg, &dp_regs->aux_addr_7_0);
	reg = AUX_ADDR_15_8(reg_addr);
	lwrite32(reg, &dp_regs->aux_addr_15_8);
	reg = AUX_ADDR_19_16(reg_addr);
	lwrite32(reg, &dp_regs->aux_addr_19_16);

	/*
	 * Set DisplayPort transaction and read 1 byte
	 * If bit 3 is 1, DisplayPort transaction.
	 * If Bit 3 is 0, I2C transaction.
	 */
	reg = AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_READ;
	lwrite32(reg, &dp_regs->aux_ch_ctl1);

	/* Start AUX transaction */
	retval = exynos_dp_start_aux_transaction();
	if (retval != EXYNOS_DP_SUCCESS)
		printk(BIOS_DEBUG, "DP Aux Transaction fail!\n");

	/* Read data buffer */
	reg = lread32(&dp_regs->buf_data0);
	*data = (unsigned char)(reg & 0xff);

	return retval;
}

unsigned int exynos_dp_write_bytes_to_dpcd(u32 reg_addr,
				unsigned int count,
				unsigned char data[])
{
	u32 reg;
	unsigned int start_offset;
	unsigned int cur_data_count;
	unsigned int cur_data_idx;
	unsigned int retry_cnt;
	unsigned int ret = 0;

	/* Clear AUX CH data buffer */
	reg = BUF_CLR;
	lwrite32(reg, &dp_regs->buffer_data_ctl);

	start_offset = 0;
	while (start_offset < count) {
		/* Buffer size of AUX CH is 16 * 4bytes */
		if ((count - start_offset) > 16)
			cur_data_count = 16;
		else
			cur_data_count = count - start_offset;

		retry_cnt = 5;
		while (retry_cnt) {
			/* Select DPCD device address */
			reg = AUX_ADDR_7_0(reg_addr + start_offset);
			lwrite32(reg, &dp_regs->aux_addr_7_0);
			reg = AUX_ADDR_15_8(reg_addr + start_offset);
			lwrite32(reg, &dp_regs->aux_addr_15_8);
			reg = AUX_ADDR_19_16(reg_addr + start_offset);
			lwrite32(reg, &dp_regs->aux_addr_19_16);

			for (cur_data_idx = 0; cur_data_idx < cur_data_count;
					cur_data_idx++) {
				reg = data[start_offset + cur_data_idx];
				lwrite32(reg, (void *)((unsigned int)&dp_regs->buf_data0 +
						      (4 * cur_data_idx)));
			}
			/*
			* Set DisplayPort transaction and write
			* If bit 3 is 1, DisplayPort transaction.
			* If Bit 3 is 0, I2C transaction.
			*/
			reg = AUX_LENGTH(cur_data_count) |
				AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_WRITE;
			lwrite32(reg, &dp_regs->aux_ch_ctl1);

			/* Start AUX transaction */
			ret = exynos_dp_start_aux_transaction();
			if (ret != EXYNOS_DP_SUCCESS) {
				if (retry_cnt == 0) {
					printk(BIOS_ERR, "DP Aux Transaction failed\n");
					return ret;
				}
				retry_cnt--;
			} else
				break;
		}
		start_offset += cur_data_count;
	}

	return ret;
}

unsigned int exynos_dp_read_bytes_from_dpcd(u32 reg_addr,
				unsigned int count,
				unsigned char data[])
{
	u32 reg;
	unsigned int start_offset;
	unsigned int cur_data_count;
	unsigned int cur_data_idx;
	unsigned int retry_cnt;
	unsigned int ret = 0;

	/* Clear AUX CH data buffer */
	reg = BUF_CLR;
	lwrite32(reg, &dp_regs->buffer_data_ctl);

	start_offset = 0;
	while (start_offset < count) {
		/* Buffer size of AUX CH is 16 * 4bytes */
		if ((count - start_offset) > 16)
			cur_data_count = 16;
		else
			cur_data_count = count - start_offset;

		retry_cnt = 5;
		while (retry_cnt) {
			/* Select DPCD device address */
			reg = AUX_ADDR_7_0(reg_addr + start_offset);
			lwrite32(reg, &dp_regs->aux_addr_7_0);
			reg = AUX_ADDR_15_8(reg_addr + start_offset);
			lwrite32(reg, &dp_regs->aux_addr_15_8);
			reg = AUX_ADDR_19_16(reg_addr + start_offset);
			lwrite32(reg, &dp_regs->aux_addr_19_16);
			/*
			 * Set DisplayPort transaction and read
			 * If bit 3 is 1, DisplayPort transaction.
			 * If Bit 3 is 0, I2C transaction.
			 */
			reg = AUX_LENGTH(cur_data_count) |
				AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_READ;
			lwrite32(reg, &dp_regs->aux_ch_ctl1);

			/* Start AUX transaction */
			ret = exynos_dp_start_aux_transaction();
			if (ret != EXYNOS_DP_SUCCESS) {
				if (retry_cnt == 0) {
					printk(BIOS_ERR, "DP Aux Transaction failed\n");
					return ret;
				}
				retry_cnt--;
			} else
				break;
		}

		for (cur_data_idx = 0; cur_data_idx < cur_data_count;
				cur_data_idx++) {
			reg = lread32((void *)((u32)&dp_regs->buf_data0 +
					      4 * cur_data_idx));
			data[start_offset + cur_data_idx] = (unsigned char)reg;
		}

		start_offset += cur_data_count;
	}

	return ret;
}

int exynos_dp_select_i2c_device(u32 device_addr,
				u32 reg_addr)
{
	u32 reg;
	int retval;

	/* Set EDID device address */
	reg = device_addr;
	lwrite32(reg, &dp_regs->aux_addr_7_0);
	lwrite32(0x0, &dp_regs->aux_addr_15_8);
	lwrite32(0x0, &dp_regs->aux_addr_19_16);

	/* Set offset from base address of EDID device */
	lwrite32(reg_addr, &dp_regs->buf_data0);

	/*
	 * Set I2C transaction and write address
	 * If bit 3 is 1, DisplayPort transaction.
	 * If Bit 3 is 0, I2C transaction.
	 */
	reg = AUX_TX_COMM_I2C_TRANSACTION | AUX_TX_COMM_MOT |
		AUX_TX_COMM_WRITE;
	lwrite32(reg, &dp_regs->aux_ch_ctl1);

	/* Start AUX transaction */
	retval = exynos_dp_start_aux_transaction();
	if (retval != 0)
		printk(BIOS_DEBUG, "%s: DP Aux Transaction fail!\n", __func__);

	return retval;
}

int exynos_dp_read_byte_from_i2c(u32 device_addr,
				u32 reg_addr,
				unsigned int *data)
{
	u32 reg;
	int i;
	int retval;

	for (i = 0; i < 10; i++) {
		/* Clear AUX CH data buffer */
		reg = BUF_CLR;
		lwrite32(reg, &dp_regs->buffer_data_ctl);

		/* Select EDID device */
		retval = exynos_dp_select_i2c_device(device_addr, reg_addr);
		if (retval != 0) {
			printk(BIOS_DEBUG, "DP Select EDID device fail. retry !\n");
			continue;
		}

		/*
		 * Set I2C transaction and read data
		 * If bit 3 is 1, DisplayPort transaction.
		 * If Bit 3 is 0, I2C transaction.
		 */
		reg = AUX_TX_COMM_I2C_TRANSACTION |
			AUX_TX_COMM_READ;
		lwrite32(reg, &dp_regs->aux_ch_ctl1);

		/* Start AUX transaction */
		retval = exynos_dp_start_aux_transaction();
		if (retval != EXYNOS_DP_SUCCESS)
			printk(BIOS_DEBUG, "%s: DP Aux Transaction fail!\n", __func__);
	}

	/* Read data */
	if (retval == 0)
		*data = lread32(&dp_regs->buf_data0);

	return retval;
}

int exynos_dp_read_bytes_from_i2c(u32 device_addr,
		u32 reg_addr, unsigned int count, unsigned char edid[])
{
	u32 reg;
	unsigned int i, j;
	unsigned int cur_data_idx;
	unsigned int defer = 0;
	int retval = 0;

	for (i = 0; i < count; i += 16) { /* use 16 burst */
		for (j = 0; j < 100; j++) {
			/* Clear AUX CH data buffer */
			reg = BUF_CLR;
			lwrite32(reg, &dp_regs->buffer_data_ctl);

			/* Set normal AUX CH command */
			reg = lread32(&dp_regs->aux_ch_ctl2);
			reg &= ~ADDR_ONLY;
			lwrite32(reg, &dp_regs->aux_ch_ctl2);

			/*
			 * If Rx sends defer, Tx sends only reads
			 * request without sending address
			 */
			if (!defer)
				retval =
					exynos_dp_select_i2c_device(device_addr,
							reg_addr + i);
			else
				defer = 0;

			if (retval == EXYNOS_DP_SUCCESS) {
				/*
				 * Set I2C transaction and write data
				 * If bit 3 is 1, DisplayPort transaction.
				 * If Bit 3 is 0, I2C transaction.
				 */
				reg = AUX_LENGTH(16) |
					AUX_TX_COMM_I2C_TRANSACTION |
					AUX_TX_COMM_READ;
				lwrite32(reg, &dp_regs->aux_ch_ctl1);

				/* Start AUX transaction */
				retval = exynos_dp_start_aux_transaction();
				if (retval == 0)
					break;
				else
					printk(BIOS_ERR, "DP Aux Transaction fail!\n");
			}
			/* Check if Rx sends defer */
			reg = lread32(&dp_regs->aux_rx_comm);
			if (reg == AUX_RX_COMM_AUX_DEFER ||
				reg == AUX_RX_COMM_I2C_DEFER) {
				printk(BIOS_ERR, "DP Defer: %d\n\n", reg);
				defer = 1;
			}
		}

		for (cur_data_idx = 0; cur_data_idx < 16; cur_data_idx++) {
			reg = lread32((void *)((u32)&dp_regs->buf_data0
					      + 4 * cur_data_idx));
			edid[i + cur_data_idx] = (unsigned char)reg;
		}
	}

	return retval;
}

void exynos_dp_reset_macro(void)
{
	u32 reg;

	reg = lread32(&dp_regs->phy_test);
	reg |= MACRO_RST;
	lwrite32(reg, &dp_regs->phy_test);

	/* 10 us is the minimum Macro reset time. */
	udelay(50);

	reg &= ~MACRO_RST;
	lwrite32(reg, &dp_regs->phy_test);
}

void exynos_dp_set_link_bandwidth(unsigned char bwtype)
{
	u32 reg;

	reg = (u32)bwtype;

	 /* Set bandwidth to 2.7G or 1.62G */
	if ((bwtype == DP_LANE_BW_1_62) || (bwtype == DP_LANE_BW_2_70))
		lwrite32(reg, &dp_regs->link_bw_set);
}

unsigned char exynos_dp_get_link_bandwidth(void)
{
	unsigned char ret;
	u32 reg;

	reg = lread32(&dp_regs->link_bw_set);
	ret = (unsigned char)reg;

	return ret;
}

void exynos_dp_set_lane_count(unsigned char count)
{
	u32 reg;

	reg = (u32)count;

	if ((count == DP_LANE_CNT_1) || (count == DP_LANE_CNT_2) ||
			(count == DP_LANE_CNT_4))
		lwrite32(reg, &dp_regs->lane_count_set);
}

unsigned int exynos_dp_get_lane_count(void)
{
	u32 reg;

	reg = lread32(&dp_regs->lane_count_set);

	return reg;
}

unsigned char exynos_dp_get_lanex_pre_emphasis(unsigned char lanecnt)
{
	void *reg_list[DP_LANE_CNT_4] = {
		&dp_regs->ln0_link_training_ctl,
		&dp_regs->ln1_link_training_ctl,
		&dp_regs->ln2_link_training_ctl,
		&dp_regs->ln3_link_training_ctl,
	};

	return lread32(reg_list[lanecnt]);
}

void exynos_dp_set_lanex_pre_emphasis(unsigned char request_val,
		unsigned char lanecnt)
{
	void *reg_list[DP_LANE_CNT_4] = {
		&dp_regs->ln0_link_training_ctl,
		&dp_regs->ln1_link_training_ctl,
		&dp_regs->ln2_link_training_ctl,
		&dp_regs->ln3_link_training_ctl,
	};

	lwrite32(request_val, reg_list[lanecnt]);
}

void exynos_dp_set_lane_pre_emphasis(unsigned int level, unsigned char lanecnt)
{
	unsigned int i;
	u32 reg;
	void *reg_list[DP_LANE_CNT_4] = {
		&dp_regs->ln0_link_training_ctl,
		&dp_regs->ln1_link_training_ctl,
		&dp_regs->ln2_link_training_ctl,
		&dp_regs->ln3_link_training_ctl,
	};
	u32 reg_shift[DP_LANE_CNT_4] = {
		PRE_EMPHASIS_SET_0_SHIFT,
		PRE_EMPHASIS_SET_1_SHIFT,
		PRE_EMPHASIS_SET_2_SHIFT,
		PRE_EMPHASIS_SET_3_SHIFT
	};

	for (i = 0; i < lanecnt; i++) {
		reg = level << reg_shift[i];
		lwrite32(reg, reg_list[i]);
	}
}

void exynos_dp_set_training_pattern(unsigned int pattern)
{
	u32 reg = 0;

	switch (pattern) {
	case PRBS7:
		reg = SCRAMBLING_ENABLE | LINK_QUAL_PATTERN_SET_PRBS7;
		break;
	case D10_2:
		reg = SCRAMBLING_ENABLE | LINK_QUAL_PATTERN_SET_D10_2;
		break;
	case TRAINING_PTN1:
		reg = SCRAMBLING_DISABLE | SW_TRAINING_PATTERN_SET_PTN1;
		break;
	case TRAINING_PTN2:
		reg = SCRAMBLING_DISABLE | SW_TRAINING_PATTERN_SET_PTN2;
		break;
	case DP_NONE:
		reg = SCRAMBLING_ENABLE | LINK_QUAL_PATTERN_SET_DISABLE |
			SW_TRAINING_PATTERN_SET_NORMAL;
		break;
	default:
		break;
	}

	lwrite32(reg, &dp_regs->training_ptn_set);
}

void exynos_dp_enable_enhanced_mode(unsigned char enable)
{
	u32 reg;

	reg = lread32(&dp_regs->sys_ctl4);
	reg &= ~ENHANCED;

	if (enable)
		reg |= ENHANCED;

	lwrite32(reg, &dp_regs->sys_ctl4);
}

void exynos_dp_enable_scrambling(unsigned int enable)
{
	u32 reg;

	reg = lread32(&dp_regs->training_ptn_set);
	reg &= ~(SCRAMBLING_DISABLE);

	if (!enable)
		reg |= SCRAMBLING_DISABLE;

	lwrite32(reg, &dp_regs->training_ptn_set);
}
int exynos_dp_init_video(void)
{
	unsigned int reg;

	/* Clear VID_CLK_CHG[1] and VID_FORMAT_CHG[3] and VSYNC_DET[7] */
	reg = VSYNC_DET | VID_FORMAT_CHG | VID_CLK_CHG;
	lwrite32(reg, &dp_regs->common_int_sta1);

	/* I_STRM__CLK detect : DE_CTL : Auto detect */
	reg &= ~DET_CTRL;
	lwrite32(reg, &dp_regs->sys_ctl1);
	return 0;
}

void exynos_dp_config_video_slave_mode(struct edp_video_info *video_info)
{
	u32 reg;

	/* Video Slave mode setting */
	reg = lread32(&dp_regs->func_en1);
	reg &= ~(MASTER_VID_FUNC_EN_N|SLAVE_VID_FUNC_EN_N);
	reg |= MASTER_VID_FUNC_EN_N;
	lwrite32(reg, &dp_regs->func_en1);

	/* Configure Interlaced for slave mode video */
	reg = lread32(&dp_regs->video_ctl10);
	reg &= ~INTERACE_SCAN_CFG;
	reg |= (video_info->interlaced << INTERACE_SCAN_CFG_SHIFT);
	printk(BIOS_SPEW, "interlaced %d\n", video_info->interlaced);
	lwrite32(reg, &dp_regs->video_ctl10);

	/* Configure V sync polarity for slave mode video */
	reg = lread32(&dp_regs->video_ctl10);
	reg &= ~VSYNC_POLARITY_CFG;
	reg |= (video_info->v_sync_polarity << V_S_POLARITY_CFG_SHIFT);
	lwrite32(reg, &dp_regs->video_ctl10);

	/* Configure H sync polarity for slave mode video */
	reg = lread32(&dp_regs->video_ctl10);
	reg &= ~HSYNC_POLARITY_CFG;
	reg |= (video_info->h_sync_polarity << H_S_POLARITY_CFG_SHIFT);
	lwrite32(reg, &dp_regs->video_ctl10);

	/*Set video mode to slave mode */
	reg = AUDIO_MODE_SPDIF_MODE | VIDEO_MODE_SLAVE_MODE;
	lwrite32(reg, &dp_regs->soc_general_ctl);
}

void exynos_dp_set_video_color_format(struct edp_video_info *video_info)
{
	u32 reg;

	/* Configure the input color depth, color space, dynamic range */
	reg = (video_info->dynamic_range << IN_D_RANGE_SHIFT) |
		(video_info->color_depth << IN_BPC_SHIFT) |
		(video_info->color_space << IN_COLOR_F_SHIFT);
	lwrite32(reg, &dp_regs->video_ctl2);

	/* Set Input Color YCbCr Coefficients to ITU601 or ITU709 */
	reg = lread32(&dp_regs->video_ctl3);
	reg &= ~IN_YC_COEFFI_MASK;
	if (video_info->ycbcr_coeff)
		reg |= IN_YC_COEFFI_ITU709;
	else
		reg |= IN_YC_COEFFI_ITU601;
	lwrite32(reg, &dp_regs->video_ctl3);
}

unsigned int exynos_dp_is_slave_video_stream_clock_on(void)
{
	u32 reg;

	/* Update Video stream clk detect status */
	reg = lread32(&dp_regs->sys_ctl1);
	lwrite32(reg, &dp_regs->sys_ctl1);

	reg = lread32(&dp_regs->sys_ctl1);

	if (!(reg & DET_STA)) {
		printk(BIOS_DEBUG, "DP Input stream clock not detected.\n");
		return -1;
	}

	return EXYNOS_DP_SUCCESS;
}

void exynos_dp_set_video_cr_mn(unsigned int type, unsigned int m_value,
		unsigned int n_value)
{
	u32 reg;

	if (type == REGISTER_M) {
		reg = lread32(&dp_regs->sys_ctl4);
		reg |= FIX_M_VID;
		lwrite32(reg, &dp_regs->sys_ctl4);
		reg = M_VID0_CFG(m_value);
		lwrite32(reg, &dp_regs->m_vid0);
		reg = M_VID1_CFG(m_value);
		lwrite32(reg, &dp_regs->m_vid1);
		reg = M_VID2_CFG(m_value);
		lwrite32(reg, &dp_regs->m_vid2);

		reg = N_VID0_CFG(n_value);
		lwrite32(reg, &dp_regs->n_vid0);
		reg = N_VID1_CFG(n_value);
		lwrite32(reg, &dp_regs->n_vid1);
		reg = N_VID2_CFG(n_value);
		lwrite32(reg, &dp_regs->n_vid2);
	} else  {
		reg = lread32(&dp_regs->sys_ctl4);
		reg &= ~FIX_M_VID;
		lwrite32(reg, &dp_regs->sys_ctl4);
	}
}

void exynos_dp_set_video_timing_mode(unsigned int type)
{
	u32 reg;

	reg = lread32(&dp_regs->video_ctl10);
	reg &= ~FORMAT_SEL;

	if (type != VIDEO_TIMING_FROM_CAPTURE)
		reg |= FORMAT_SEL;

	lwrite32(reg, &dp_regs->video_ctl10);
}

void exynos_dp_enable_video_master(unsigned int enable)
{
	u32 reg;

	reg = lread32(&dp_regs->soc_general_ctl);
	if (enable) {
		reg &= ~VIDEO_MODE_MASK;
		reg |= VIDEO_MASTER_MODE_EN | VIDEO_MODE_MASTER_MODE;
	} else {
		reg &= ~VIDEO_MODE_MASK;
		reg |= VIDEO_MODE_SLAVE_MODE;
	}

	lwrite32(reg, &dp_regs->soc_general_ctl);
}

void exynos_dp_start_video(void)
{
	u32 reg;

	/* Enable Video input and disable Mute */
	reg = lread32(&dp_regs->video_ctl1);
	reg |= VIDEO_EN;
	lwrite32(reg, &dp_regs->video_ctl1);
}

unsigned int exynos_dp_is_video_stream_on(void)
{
	u32 reg;

	/* Update STRM_VALID */
	reg = lread32(&dp_regs->sys_ctl3);
	lwrite32(reg, &dp_regs->sys_ctl3);

	reg = lread32(&dp_regs->sys_ctl3);

	if (!(reg & STRM_VALID))
		return -1;

	return EXYNOS_DP_SUCCESS;
}

void dp_phy_control(unsigned int enable)
{
	u32 cfg;

	cfg = lread32(&exynos_power->dptx_phy_control);
	if (enable)
		cfg |= EXYNOS_DP_PHY_ENABLE;
	else
		cfg &= ~EXYNOS_DP_PHY_ENABLE;
	lwrite32(cfg, &exynos_power->dptx_phy_control);
}
