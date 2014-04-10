/*
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
 */

#include <common.h>
#include <spi.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/errno.h>
#include "ipq_spi.h"
#include <asm/arch-ipq806x/gpio.h>
#include <asm/arch-ipq806x/iomap.h>

#define SUCCESS		0

#define DUMMY_DATA_VAL		0
#define TIMEOUT_CNT		100
#define CS_ASSERT		1
#define CS_DEASSERT		0
#define NUM_PORTS		3
#define NUM_GSBI_PINS		3
#define TLMM_ARGS		6
#define NUM_CS			4
#define GSBI_PIN_IDX		0
#define FUNC_SEL_IDX		1
#define GPIO_DIR_IDX		2
#define PULL_CONF_IDX		3
#define DRV_STR_IDX		4
#define GPIO_EN_IDX		5

#define GSBI_IDX_TO_GSBI(idx)   (idx + 5)

/*
 * TLMM Configuration for SPI NOR
 * gsbi_pin_conf[bus_num][GPIO_NUM, FUNC_SEL, I/O,
 *			       PULL UP/DOWN, DRV_STR, GPIO_FUNC]
 * gsbi_pin_conf[0][x][y] -- GSBI5
 * gsbi_pin_conf[1][x][y] -- GSBI6
 * gsbi_pin_conf[2][x][y] -- GSBI7
*/
static unsigned int gsbi_pin_conf[NUM_PORTS][NUM_GSBI_PINS][TLMM_ARGS] = {
	{
		/* GSBI5 CLK */
		{
			GSBI5_SPI_CLK,  FUNC_SEL_1, GPIO_INPUT,
			GPIO_PULL_DOWN, GPIO_DRV_STR_11MA, GPIO_FUNC_DISABLE
		},
		/* GSBI5 MISO */
		{
			GSBI5_SPI_MISO, FUNC_SEL_1, GPIO_INPUT,
			GPIO_PULL_DOWN, GPIO_DRV_STR_10MA, GPIO_FUNC_DISABLE
		},
		/* GSBI5 MOSI */
		{
			GSBI5_SPI_MOSI, FUNC_SEL_1, GPIO_INPUT,
			GPIO_PULL_DOWN, GPIO_DRV_STR_10MA, GPIO_FUNC_DISABLE
		}
	},
	{
		/* GSBI6 CLK */
		{
			GSBI6_SPI_CLK,  FUNC_SEL_3, GPIO_INPUT,
			GPIO_PULL_DOWN, GPIO_DRV_STR_11MA, GPIO_FUNC_DISABLE
		},
		/* GSBI6 MISO */
		{
			GSBI6_SPI_MISO, FUNC_SEL_3, GPIO_INPUT,
			GPIO_PULL_DOWN, GPIO_DRV_STR_10MA, GPIO_FUNC_DISABLE
		},
		/* GSBI6 MOSI */
		{
			GSBI6_SPI_MOSI, FUNC_SEL_3, GPIO_INPUT,
			GPIO_PULL_DOWN, GPIO_DRV_STR_10MA, GPIO_FUNC_DISABLE
		}
	},
	{
		/* GSBI7 CLK */
		{
			GSBI7_SPI_CLK,  FUNC_SEL_1, GPIO_INPUT,
			GPIO_PULL_DOWN, GPIO_DRV_STR_11MA, GPIO_FUNC_DISABLE
		},
		/* GSBI7 MISO */
		{
			GSBI7_SPI_MISO, FUNC_SEL_1, GPIO_INPUT,
			GPIO_PULL_DOWN, GPIO_DRV_STR_10MA, GPIO_FUNC_DISABLE
		},
		/* GSBI7 MOSI */
		{
			GSBI7_SPI_MOSI, FUNC_SEL_1, GPIO_INPUT,
			GPIO_PULL_DOWN, GPIO_DRV_STR_10MA, GPIO_FUNC_DISABLE
		}
	}
};

/*
 * CS GPIO number array cs_gpio_array[port_num][cs_num]
 * cs_gpio_array[0][x] -- GSBI5
 * cs_gpio_array[1][x] -- GSBI6
 * cs_gpio_array[2][x] -- GSBI7
 */
static unsigned int cs_gpio_array[NUM_PORTS][NUM_CS] = {
	{
		GSBI5_SPI_CS_0, GSBI5_SPI_CS_1, GSBI5_SPI_CS_2, GSBI5_SPI_CS_3
	},
	{
		GSBI6_SPI_CS_0,              0,              0,              0
	},
	{
		GSBI7_SPI_CS_0,              0,              0,              0
	}
};

/*
 * GSBI HCLK state register bit
 * hclk_state[0] -- GSBI5
 * hclk_state[1] -- GSBI6
 * hclk_state[2] -- GSBI7
*/
static unsigned int hclk_state[NUM_PORTS] = {
	GSBI5_HCLK,
	GSBI6_HCLK,
	GSBI7_HCLK
};

/*
 * GSBI QUP_APPS_CLK state register bit
 * qup_apps_clk_state[0] -- GSBI5
 * qup_apps_clk_state[1] -- GSBI6
 * qup_apps_clk_state[2] -- GSBI7
*/
static unsigned int qup_apps_clk_state[NUM_PORTS] = {
	GSBI5_QUP_APPS_CLK,
	GSBI6_QUP_APPS_CLK,
	GSBI7_QUP_APPS_CLK
};


static int check_bit_state(uint32_t reg_addr, int bit_num, int val, int us_delay)
{
	unsigned int count = TIMEOUT_CNT;
	unsigned int bit_val = ((readl(reg_addr) >> bit_num) & 0x01);

	while (bit_val != val) {
		count--;
		if (count == 0)
			return -ETIMEDOUT;
		udelay(us_delay);
		bit_val = ((readl(reg_addr) >> bit_num) & 0x01);
	}

	return SUCCESS;
}

/*
 * Check whether GSBIn_QUP State is valid
 */
static int check_qup_state_valid(struct ipq_spi_slave *ds)
{

	return check_bit_state(ds->regs->qup_state, QUP_STATE_VALID_BIT,
				QUP_STATE_VALID, 1);

}

/*
 * Configure GSBIn Core state
 */
static int config_spi_state(struct ipq_spi_slave *ds, unsigned int state)
{
	uint32_t val;
	int ret = SUCCESS;

	ret = check_qup_state_valid(ds);
	if (ret != SUCCESS)
		return ret;

	switch (state) {
	case SPI_RUN_STATE:
		/* Set the state to RUN */
		val = ((readl(ds->regs->qup_state) & ~QUP_STATE_MASK)
					| QUP_STATE_RUN_STATE);
		writel(val, ds->regs->qup_state);
		ret = check_qup_state_valid(ds);
		if (ret != SUCCESS)
			return ret;
		ds->core_state = SPI_CORE_RUNNING;
		break;
	case SPI_RESET_STATE:
		/* Set the state to RESET */
		val = ((readl(ds->regs->qup_state) & ~QUP_STATE_MASK)
					| QUP_STATE_RESET_STATE);
		writel(val, ds->regs->qup_state);
		ret = check_qup_state_valid(ds);
		if (ret != SUCCESS)
			return ret;
		ds->core_state = SPI_CORE_RESET;
		break;
	default:
		printf("err: unsupported GSBI SPI state : %d\n", state);
		ret = -EINVAL;
		break;
	}

	return ret;
}

/*
 * Set GSBIn SPI Mode
 */
static void spi_set_mode(struct ipq_spi_slave *ds, unsigned int mode)
{
	unsigned int clk_idle_state;
	unsigned int input_first_mode;
	uint32_t val;

	switch (mode) {
	case GSBI_SPI_MODE_0:
		clk_idle_state = 0;
		input_first_mode = SPI_INPUT_FIRST_MODE;
		break;
	case GSBI_SPI_MODE_1:
		clk_idle_state = 0;
		input_first_mode = 0;
		break;
	case GSBI_SPI_MODE_2:
		clk_idle_state = 1;
		input_first_mode = SPI_INPUT_FIRST_MODE;
		break;
	case GSBI_SPI_MODE_3:
		clk_idle_state = 1;
		input_first_mode = 0;
		break;
	default:
		printf("err : unsupported spi mode : %d\n", mode);
		return;
	}

	val = readl(ds->regs->spi_config);
	val |= input_first_mode;
	writel(val, ds->regs->spi_config);

	val = readl(ds->regs->io_control);
	if (clk_idle_state)
		val |= SPI_IO_CONTROL_CLOCK_IDLE_HIGH;
	else
		val &= ~SPI_IO_CONTROL_CLOCK_IDLE_HIGH;

	writel(val, ds->regs->io_control);
}

/*
 * Check for HCLK state
 */
static int check_hclk_state(unsigned int core_num, int enable)
{
	if (clk_is_dummy())
		return 0;

	return check_bit_state(CLK_HALT_CFPB_STATEB_REG,
		hclk_state[core_num], enable, 5);
}

/*
 * Check for QUP APPS CLK state
 */
static int check_qup_clk_state(unsigned int core_num, int enable)
{
	if (clk_is_dummy())
		return 0;

	return check_bit_state(CLK_HALT_CFPB_STATEB_REG,
		qup_apps_clk_state[core_num], enable, 5);
}

/*
 * Function to assert and De-assert chip select
 */
static void CS_change(int port_num, int cs_num, int enable)
{
	unsigned int cs_gpio = cs_gpio_array[port_num][cs_num];
	uint32_t addr = GPIO_IN_OUT_ADDR(cs_gpio);
	uint32_t val = readl(addr);

	val &= (~(1 << GPIO_OUT));
	if (!enable)
		val |= (1 << GPIO_OUT);
	writel(val, addr);
}

/*
 * GSBIn TLMM configuration
 */
static void gsbi_pin_config(unsigned int port_num, int cs_num)
{
	unsigned int gpio;
	unsigned int i;
	/* Hold the GSBIn (core_num) core in reset */
	clrsetbits_le32(GSBIn_RESET_REG(GSBI_IDX_TO_GSBI(port_num)),
			GSBI1_RESET_MSK, GSBI1_RESET);

	/*
	 * Configure SPI_CLK, SPI_MISO and SPI_MOSI
	 */
	for (i = 0; i < NUM_GSBI_PINS; i++) {
		unsigned int func_sel;
		unsigned int io_config;
		unsigned int pull_config;
		unsigned int drv_strength;
		unsigned int gpio_en;
		unsigned int *ptr;

		ptr = gsbi_pin_conf[port_num][i];
		gpio		= *(ptr + GSBI_PIN_IDX);
		func_sel	= *(ptr + FUNC_SEL_IDX);
		io_config	= *(ptr + GPIO_DIR_IDX);
		pull_config	= *(ptr + PULL_CONF_IDX);
		drv_strength	= *(ptr + DRV_STR_IDX);
		gpio_en	= *(ptr + GPIO_EN_IDX);

		gpio_tlmm_config(gpio, func_sel, io_config,
				 pull_config, drv_strength, gpio_en);
	}

	gpio = cs_gpio_array[port_num][cs_num];
	/* configure CS */
	gpio_tlmm_config(gpio, FUNC_SEL_GPIO, GPIO_OUTPUT, GPIO_PULL_UP,
				GPIO_DRV_STR_10MA, GPIO_FUNC_ENABLE);
	CS_change(port_num, cs_num, CS_DEASSERT);
}

/*
 * Clock configuration for GSBIn Core
 */
static int gsbi_clock_init(struct ipq_spi_slave *ds)
{
	int ret;

	/* Hold the GSBIn (core_num) core in reset */
	clrsetbits_le32(GSBIn_RESET_REG(GSBI_IDX_TO_GSBI(ds->slave.bus)),
			GSBI1_RESET_MSK, GSBI1_RESET);

	/* Disable GSBIn (core_num) QUP core clock branch */
	clrsetbits_le32(ds->regs->qup_ns_reg, QUP_CLK_BRANCH_ENA_MSK,
					QUP_CLK_BRANCH_DIS);

	ret = check_qup_clk_state(ds->slave.bus, 1);
	if (ret) {
		printf("QUP Clock Halt For GSBI%d failed!\n", ds->slave.bus);
		return ret;
	}

	/* Disable M/N:D counter and hold M/N:D counter in reset */
	clrsetbits_le32(ds->regs->qup_ns_reg, (MNCNTR_MSK | MNCNTR_RST_MSK),
					(MNCNTR_RST_ENA | MNCNTR_DIS));

	/* Disable GSBIn (core_num) QUP core clock root */
	clrsetbits_le32(ds->regs->qup_ns_reg, CLK_ROOT_ENA_MSK, CLK_ROOT_DIS);

	clrsetbits_le32(ds->regs->qup_ns_reg, GSBIn_PLL_SRC_MSK,
					GSBIn_PLL_SRC_PLL8);
	clrsetbits_le32(ds->regs->qup_ns_reg, GSBIn_PRE_DIV_SEL_MSK,
						(0 << GSBI_PRE_DIV_SEL_SHFT));

	/* Program M/N:D values for GSBIn_QUP_APPS_CLK @50MHz */
	clrsetbits_le32(ds->regs->qup_md_reg, GSBIn_M_VAL_MSK,
						(0x01 << GSBI_M_VAL_SHFT));
	clrsetbits_le32(ds->regs->qup_md_reg, GSBIn_D_VAL_MSK,
						(0xF7 << GSBI_D_VAL_SHFT));
	clrsetbits_le32(ds->regs->qup_ns_reg, GSBIn_N_VAL_MSK,
						(0xF8 << GSBI_N_VAL_SHFT));

	/* Set MNCNTR_MODE = 0: Bypass mode */
	clrsetbits_le32(ds->regs->qup_ns_reg, MNCNTR_MODE_MSK,
					MNCNTR_MODE_DUAL_EDGE);

	/* De-assert the M/N:D counter reset */
	clrsetbits_le32(ds->regs->qup_ns_reg, MNCNTR_RST_MSK, MNCNTR_RST_DIS);
	clrsetbits_le32(ds->regs->qup_ns_reg, MNCNTR_MSK, MNCNTR_EN);

	/*
	 * Enable the GSBIn (core_num) QUP core clock root.
	 * Keep MND counter disabled
	 */
	clrsetbits_le32(ds->regs->qup_ns_reg, CLK_ROOT_ENA_MSK, CLK_ROOT_ENA);

	/* Enable GSBIn (core_num) QUP core clock branch */
	clrsetbits_le32(ds->regs->qup_ns_reg, QUP_CLK_BRANCH_ENA_MSK,
						QUP_CLK_BRANCH_ENA);

	ret = check_qup_clk_state(ds->slave.bus, 0);
	if (ret) {
		printf("QUP Clock Enable For GSBI%d"
				" failed!\n", ds->slave.bus);
		return ret;
	}

	/* Enable GSBIn (core_num) core clock branch */
	clrsetbits_le32(GSBIn_HCLK_CTL_REG(GSBI_IDX_TO_GSBI(ds->slave.bus)),
			GSBI_CLK_BRANCH_ENA_MSK, GSBI_CLK_BRANCH_ENA);

	ret = check_hclk_state(ds->slave.bus, 0);
	if (ret) {
		printf("HCLK Enable For GSBI%d failed!\n", ds->slave.bus);
		return ret;
	}

	/* Release GSBIn (core_num) core from reset */
	clrsetbits_le32(GSBIn_RESET_REG(GSBI_IDX_TO_GSBI(ds->slave.bus)),
						GSBI1_RESET_MSK, 0);
	udelay(50);

	return SUCCESS;
}

/*
 * Reset entire QUP and all mini cores
 */
static void spi_reset(struct ipq_spi_slave *ds)
{
	writel(0x1, ds->regs->qup_sw_reset);
	udelay(5);
}

void spi_init()
{
	/* do nothing */

}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				unsigned int max_hz, unsigned int mode)
{
	struct ipq_spi_slave *ds;

	ds = malloc(sizeof(struct ipq_spi_slave));
	if (!ds) {
		printf("SPI error: malloc of SPI structure failed\n");
		return NULL;
	}

	/*
	 * IPQ GSBI (Generic Serial Bus Interface) supports SPI Flash
	 * on different GSBI5, GSBI6 and GSBI7
	 * with different number of chip selects (CS, channels):
	*/
	if ((bus < GSBI5_SPI) || (bus > GSBI7_SPI)
		|| ((bus == GSBI5_SPI) && (cs > 3))
		|| ((bus == GSBI6_SPI) && (cs > 0))
		|| ((bus == GSBI7_SPI) && (cs > 0))) {
		printf("SPI error: unsupported bus %d "
			"(Supported busses 0,1 and 2) or chipselect\n", bus);
		goto err;
	}
	ds->slave.bus	= bus;
	ds->slave.cs	= cs;

	ds->regs	= &spi_reg[bus];

	/* TODO For different clock frequency */
	if (max_hz > MSM_GSBI_MAX_FREQ) {
		printf("SPI error: unsupported frequency %d Hz "
			"Max frequency is %d Hz\n", max_hz, MSM_GSBI_MAX_FREQ);
		goto err;
	}
	ds->freq = max_hz;

	if (mode > GSBI_SPI_MODE_3) {
		printf("SPI error: unsupported SPI mode %d\n", mode);
		goto err;
	}
	ds->mode = mode;

	return &ds->slave;

err:
	free(ds);
	return NULL;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct ipq_spi_slave *ds = to_ipq_spi(slave);

	if (ds != NULL)
		free(ds);
}

/*
 * GSBIn SPI Hardware Initialisation
 */
static int spi_hw_init(struct ipq_spi_slave *ds)
{
	int ret;

	ds->initialized = 0;

	/* GSBI module configuration */
	spi_reset(ds);

	/* Set the GSBIn QUP state */
	ret = config_spi_state(ds, SPI_RESET_STATE);
	if (ret)
		return ret;

	/* Configure GSBI_CTRL register to set protocol_mode to SPI:011 */
	clrsetbits_le32(ds->regs->gsbi_ctrl, PROTOCOL_CODE_MSK,
					PROTOCOL_CODE_SPI);

	/*
	 * Configure Mini core to SPI core with Input Output enabled,
	 * SPI master, N = 8 bits
	 */
	clrsetbits_le32(ds->regs->qup_config, (QUP_CONFIG_MINI_CORE_MSK |
					       SPI_QUP_CONF_INPUT_MSK |
					       SPI_QUP_CONF_OUTPUT_MSK |
					       SPI_BIT_WORD_MSK),
					      (QUP_CONFIG_MINI_CORE_SPI |
					       SPI_QUP_CONF_INPUT_ENA |
					       SPI_QUP_CONF_OUTPUT_ENA |
					       SPI_8_BIT_WORD));

	/*
	 * Configure Input first SPI protocol,
	 * SPI master mode and no loopback
	 */
	clrsetbits_le32(ds->regs->spi_config, (LOOP_BACK_MSK |
					       SLAVE_OPERATION_MSK),
					      (NO_LOOP_BACK |
					       SLAVE_OPERATION));

	/*
	 * Configure SPI IO Control Register
	 * CLK_ALWAYS_ON = 0
	 * MX_CS_MODE = 0
	 * NO_TRI_STATE = 1
	 */
	writel((CLK_ALWAYS_ON | MX_CS_MODE | NO_TRI_STATE),
				ds->regs->io_control);

	/*
	 * Configure SPI IO Modes.
	 * OUTPUT_BIT_SHIFT_EN = 1
	 * INPUT_MODE = Block Mode
	 * OUTPUT MODE = Block Mode
	 */
	clrsetbits_le32(ds->regs->qup_io_modes, (OUTPUT_BIT_SHIFT_MSK |
						 INPUT_BLOCK_MODE_MSK |
						 OUTPUT_BLOCK_MODE_MSK),
						(OUTPUT_BIT_SHIFT_EN |
						 INPUT_BLOCK_MODE |
						 OUTPUT_BLOCK_MODE));

	spi_set_mode(ds, ds->mode);

	/* Disable Error mask */
	writel(0, ds->regs->error_flags_en);
	writel(0, ds->regs->qup_error_flags_en);

	ds->initialized = 1;

	return SUCCESS;
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct ipq_spi_slave *ds = to_ipq_spi(slave);
	unsigned int ret;

	/* GPIO Configuration for SPI port */
	gsbi_pin_config(ds->slave.bus, ds->slave.cs);

	/* Clock configuration */
	ret = gsbi_clock_init(ds);
	if (ret)
		return ret;

	ret = spi_hw_init(ds);
	if (ret)
		return -EIO;

	return SUCCESS;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct ipq_spi_slave *ds = to_ipq_spi(slave);

	/* Reset the SPI hardware */
	spi_reset(ds);
	ds->initialized = 0;
}

/* Drain input fifo
 * If input fifo is not empty drain the input FIFO. When the
 * input fifo is drained make sure that the output fifo is also
 * empty and break when the input fifo is completely drained.
 */
static void flush_fifos(struct ipq_spi_slave *ds)
{
	unsigned int fifo_data;

	while (1) {
		if (readl(ds->regs->qup_operational) &
			QUP_DATA_AVAILABLE_FOR_READ) {
			fifo_data = readl(ds->regs->qup_input_fifo);
		} else {
			if (!(readl(ds->regs->qup_operational) &
				QUP_OUTPUT_FIFO_NOT_EMPTY)) {
				if (!(readl(ds->regs->qup_operational) &
					QUP_DATA_AVAILABLE_FOR_READ))
					break;
			}
		}
	}

	(void)fifo_data;
}

/*
 * Function to write data to OUTPUT FIFO
 */
static void spi_write_byte(struct ipq_spi_slave *ds, unsigned char data)
{
	/* Wait for space in the FIFO */
	while ((readl(ds->regs->qup_operational) & QUP_OUTPUT_FIFO_FULL))
		udelay(1);

	/* Write the byte of data */
	writel(data, ds->regs->qup_output_fifo);
}

/*
 * Function to read data from Input FIFO
 */
static unsigned char spi_read_byte(struct ipq_spi_slave *ds)
{
	/* Wait for Data in FIFO */
	while (!(readl(ds->regs->qup_operational) &
			QUP_DATA_AVAILABLE_FOR_READ)) {
		udelay(1);
	}

	/* Read a byte of data */
	return readl(ds->regs->qup_input_fifo) & 0xff;
}

/*
 * Function to check wheather Input or Output FIFO
 * has data to be serviced
 */
static int check_fifo_status(uint32_t reg_addr)
{
	unsigned int count = TIMEOUT_CNT;
	unsigned int status_flag;
	unsigned int val;

	do {
		val = readl(reg_addr);
		count--;
		if (count == 0)
			return -ETIMEDOUT;
		status_flag = ((val & OUTPUT_SERVICE_FLAG) | (val & INPUT_SERVICE_FLAG));
	} while (!status_flag);

	return SUCCESS;
}

/*
 * Function to read bytes number of data from the Input FIFO
 */
static int gsbi_spi_read(struct ipq_spi_slave *ds, u8 *data_buffer,
				unsigned int bytes, unsigned long flags)
{
	uint32_t val;
	unsigned int i;
	unsigned int read_bytes = bytes;
	unsigned int fifo_count;
	int ret = SUCCESS;
	int state_config;

	if (flags & SPI_XFER_BEGIN) {
		/* Assert chip select */
		CS_change(ds->slave.bus, ds->slave.cs, CS_ASSERT);
	}

	/* Configure no of bytes to read */
	state_config = config_spi_state(ds, SPI_RESET_STATE);
	if (state_config)
		return state_config;

	writel(bytes, ds->regs->qup_mx_output_count);
	writel(bytes, ds->regs->qup_mx_input_count);

	state_config = config_spi_state(ds, SPI_RUN_STATE);
	if (state_config)
		return state_config;

	while (read_bytes) {

		ret = check_fifo_status(ds->regs->qup_operational);
		if (ret != SUCCESS)
			goto out;

		val = readl(ds->regs->qup_operational);
		if (val & INPUT_SERVICE_FLAG) {
			/*
			 * acknowledge to hw that software will
			 * read input data
			 */
			val &= INPUT_SERVICE_FLAG;
			writel(val, ds->regs->qup_operational);

			fifo_count = ((read_bytes > SPI_INPUT_BLOCK_SIZE) ?
					SPI_INPUT_BLOCK_SIZE : read_bytes);

			for (i = 0; i < fifo_count; i++) {
				*data_buffer = spi_read_byte(ds);
				data_buffer++;
				read_bytes--;
			}
		}

		if (val & OUTPUT_SERVICE_FLAG) {
			/*
			 * acknowledge to hw that software will
			 * write output data
			 */
			val &= OUTPUT_SERVICE_FLAG;
			writel(val, ds->regs->qup_operational);

			fifo_count = ((read_bytes > SPI_OUTPUT_BLOCK_SIZE) ?
					SPI_OUTPUT_BLOCK_SIZE : read_bytes);

			for (i = 0; i < fifo_count; i++) {
				/*
				 * Write dummy data byte for the device
				 * to shift in actual data. Most of the SPI devices
				 * accepts dummy data value as 0. In case of any
				 * other value change DUMMY_DATA_VAL.
				 */
				spi_write_byte(ds, DUMMY_DATA_VAL);
			}
		}
	}

	if (flags & SPI_XFER_END) {
		flush_fifos(ds);
		goto out;
	}

	return ret;

out:
	/* Deassert CS */
	CS_change(ds->slave.bus, ds->slave.cs, CS_DEASSERT);

	/*
	 * Put the SPI Core back in the Reset State
	 * to end the transfer
	 */
	(void)config_spi_state(ds, SPI_RESET_STATE);

	return ret;

}

/*
 * Function to write data to the Output FIFO
 */
static int gsbi_spi_write(struct ipq_spi_slave *ds, const u8 *cmd_buffer,
				unsigned int bytes, unsigned long flags)
{
	uint32_t val;
	unsigned int i;
	unsigned int write_len = bytes;
	unsigned int read_len = bytes;
	unsigned int fifo_count;
	int ret = SUCCESS;
	int state_config;

	if (flags & SPI_XFER_BEGIN) {
		/* Select the chip select */
		CS_change(ds->slave.bus, ds->slave.cs, CS_ASSERT);
	}

	state_config = config_spi_state(ds, SPI_RESET_STATE);
	if (state_config)
		return state_config;

	/* No of bytes to be written in Output FIFO */
	writel(bytes, ds->regs->qup_mx_output_count);
	writel(bytes, ds->regs->qup_mx_input_count);
	state_config = config_spi_state(ds, SPI_RUN_STATE);
	if (state_config)
		return state_config;

	/*
	 * read_len considered to ensure that we read the dummy data for the
	 * write we performed. This is needed to ensure with WR-RD transaction
	 * to get the actual data on the subsequent read cycle that happens
	 */
	while (write_len || read_len) {

		ret = check_fifo_status(ds->regs->qup_operational);
		if (ret != SUCCESS)
			goto out;

		val = readl(ds->regs->qup_operational);
		if (val & OUTPUT_SERVICE_FLAG) {
			/*
			 * acknowledge to hw that software will write
			 * expected output data
			 */
			val &= OUTPUT_SERVICE_FLAG;
			writel(val, ds->regs->qup_operational);

			if (write_len > SPI_OUTPUT_BLOCK_SIZE)
                                fifo_count = SPI_OUTPUT_BLOCK_SIZE;
                        else
                                fifo_count = write_len;

			for (i = 0; i < fifo_count; i++) {
				/* Write actual data to output FIFO */
				spi_write_byte(ds, *cmd_buffer);
				cmd_buffer++;
				write_len--;
			}
		}
		if (val & INPUT_SERVICE_FLAG) {
			/*
			 * acknowledge to hw that software
			 * will read input data
			 */
			val &= INPUT_SERVICE_FLAG;
			writel(val, ds->regs->qup_operational);

			if (read_len > SPI_INPUT_BLOCK_SIZE)
				fifo_count = SPI_INPUT_BLOCK_SIZE;
			else
				fifo_count = read_len;

			for (i = 0; i < fifo_count; i++) {
				/* Read dummy data for the data written */
				(void)spi_read_byte(ds);

				/* Decrement the write count after reading the dummy data
				 * from the device. This is to make sure we read dummy data
				 * before we write the data to fifo
				 */
				read_len--;
			}
		}
	}

	if (flags & SPI_XFER_END) {
		flush_fifos(ds);
		goto out;
	}

	return ret;

out:
	/* Deassert CS */
	CS_change(ds->slave.bus, ds->slave.cs, CS_DEASSERT);

	/*
	 * Put the SPI Core back in the Reset State
	 * to end the transfer
	 */
	(void)config_spi_state(ds, SPI_RESET_STATE);

	return ret;
}

/*
 * This function is invoked with either tx_buf or rx_buf.
 * Calling this function with both null does a chip select change.
 */
int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	struct ipq_spi_slave *ds = to_ipq_spi(slave);
	unsigned int len;
	const u8 *txp = dout;
	u8 *rxp = din;
	int ret;

	if (bitlen & 0x07) {
		printf("err : Invalid bit length");
		return -EINVAL;
	}

	len = bitlen >> 3;

	if (dout != NULL) {
		ret = gsbi_spi_write(ds, txp, len, flags);
		if (ret != SUCCESS)
			return ret;
	}

	if (din != NULL)
		return gsbi_spi_read(ds, rxp, len, flags);

	if ((din == NULL) && (dout == NULL))
		/* To handle only when chip select change is needed */
		ret = gsbi_spi_write(ds, NULL, 0, flags);

	return ret;
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return 1;
}

void spi_cs_activate(struct spi_slave *slave)
{

}

void spi_cs_deactivate(struct spi_slave *slave)
{

}
