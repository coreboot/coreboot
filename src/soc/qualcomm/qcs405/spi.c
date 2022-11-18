/* SPDX-License-Identifier: BSD-3-Clause */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include <soc/iomap.h>
#include <soc/spi.h>
#include <soc/clock.h>
#include <spi_flash.h>
#include <timer.h>

static const struct blsp_spi spi_reg[] = {
	/* BLSP0 registers for SPI interface */
	{
		BLSP0_SPI_CONFIG_REG,
		BLSP0_SPI_IO_CONTROL_REG,
		BLSP0_SPI_ERROR_FLAGS_REG,
		BLSP0_SPI_ERROR_FLAGS_EN_REG,
		BLSP0_QUP_CONFIG_REG,
		BLSP0_QUP_ERROR_FLAGS_REG,
		BLSP0_QUP_ERROR_FLAGS_EN_REG,
		BLSP0_QUP_OPERATIONAL_REG,
		BLSP0_QUP_IO_MODES_REG,
		BLSP0_QUP_STATE_REG,
		BLSP0_QUP_INPUT_FIFOc_REG(0),
		BLSP0_QUP_OUTPUT_FIFOc_REG(0),
		BLSP0_QUP_MX_INPUT_COUNT_REG,
		BLSP0_QUP_MX_OUTPUT_COUNT_REG,
		BLSP0_QUP_SW_RESET_REG,
		0,
		0,
		BLSP0_QUP_OPERATIONAL_MASK,
		BLSP0_SPI_DEASSERT_WAIT_REG,
	},
	{0}, {0}, {0},
	/* BLSP4 registers for SPI interface */
	{
		BLSP4_SPI_CONFIG_REG,
		BLSP4_SPI_IO_CONTROL_REG,
		BLSP4_SPI_ERROR_FLAGS_REG,
		BLSP4_SPI_ERROR_FLAGS_EN_REG,
		BLSP4_QUP_CONFIG_REG,
		BLSP4_QUP_ERROR_FLAGS_REG,
		BLSP4_QUP_ERROR_FLAGS_EN_REG,
		BLSP4_QUP_OPERATIONAL_REG,
		BLSP4_QUP_IO_MODES_REG,
		BLSP4_QUP_STATE_REG,
		BLSP4_QUP_INPUT_FIFOc_REG(0),
		BLSP4_QUP_OUTPUT_FIFOc_REG(0),
		BLSP4_QUP_MX_INPUT_COUNT_REG,
		BLSP4_QUP_MX_OUTPUT_COUNT_REG,
		BLSP4_QUP_SW_RESET_REG,
		0,
		0,
		BLSP4_QUP_OPERATIONAL_MASK,
		BLSP4_SPI_DEASSERT_WAIT_REG,
	},
	/* BLSP5 registers for SPI interface */
	{
		BLSP5_SPI_CONFIG_REG,
		BLSP5_SPI_IO_CONTROL_REG,
		BLSP5_SPI_ERROR_FLAGS_REG,
		BLSP5_SPI_ERROR_FLAGS_EN_REG,
		BLSP5_QUP_CONFIG_REG,
		BLSP5_QUP_ERROR_FLAGS_REG,
		BLSP5_QUP_ERROR_FLAGS_EN_REG,
		BLSP5_QUP_OPERATIONAL_REG,
		BLSP5_QUP_IO_MODES_REG,
		BLSP5_QUP_STATE_REG,
		BLSP5_QUP_INPUT_FIFOc_REG(0),
		BLSP5_QUP_OUTPUT_FIFOc_REG(0),
		BLSP5_QUP_MX_INPUT_COUNT_REG,
		BLSP5_QUP_MX_OUTPUT_COUNT_REG,
		BLSP5_QUP_SW_RESET_REG,
		0,
		0,
		BLSP5_QUP_OPERATIONAL_MASK,
		BLSP5_SPI_DEASSERT_WAIT_REG,
	},
};

static int check_bit_state(void *reg_addr, int mask,
				int val, int us_delay)
{
	unsigned int count = TIMEOUT_CNT;

	while ((read32(reg_addr) & mask) != val) {
		count--;
		if (count == 0)
			return -ETIMEDOUT;
		udelay(us_delay);
	}

	return SUCCESS;
}

/*
 * Check whether QUPn State is valid
 */
static int check_qup_state_valid(struct qcs_spi_slave *ds)
{
	return check_bit_state(ds->regs->qup_state, QUP_STATE_VALID_MASK,
				QUP_STATE_VALID, 1);
}

/*
 * Configure QUPn Core state
 */
static int config_spi_state(struct qcs_spi_slave *ds, unsigned int state)
{
	uint32_t val;
	int ret = SUCCESS;

	ret = check_qup_state_valid(ds);
	if (ret != SUCCESS)
		return ret;

	switch (state) {
	case QUP_STATE_RUN:
		/* Set the state to RUN */
		val = ((read32(ds->regs->qup_state) & ~QUP_STATE_MASK)
				| QUP_STATE_RUN);
		write32(ds->regs->qup_state, val);
		ret = check_qup_state_valid(ds);
		break;
	case QUP_STATE_RESET:
		/* Set the state to RESET */
		val = ((read32(ds->regs->qup_state) & ~QUP_STATE_MASK)
				| QUP_STATE_RESET);
		write32(ds->regs->qup_state, val);
		ret = check_qup_state_valid(ds);
		break;
	default:
		printk(BIOS_ERR, "unsupported QUP SPI state : %d\n", state);
		ret = -EINVAL;
		break;
	}

	return ret;
}

/*
 * Set QUPn SPI Mode
 */
static void spi_set_mode(struct qcs_spi_slave *ds, unsigned int mode)
{
	unsigned int clk_idle_state;
	unsigned int input_first_mode;
	uint32_t val;

	switch (mode) {
	case SPI_MODE0:
		clk_idle_state = 0;
		input_first_mode = SPI_CONFIG_INPUT_FIRST;
		break;
	case SPI_MODE1:
		clk_idle_state = 0;
		input_first_mode = 0;
		break;
	case SPI_MODE2:
		clk_idle_state = 1;
		input_first_mode = SPI_CONFIG_INPUT_FIRST;
		break;
	case SPI_MODE3:
		clk_idle_state = 1;
		input_first_mode = 0;
		break;
	default:
		printk(BIOS_ERR, "unsupported spi mode : %d\n", mode);
		return;
	}

	val = read32(ds->regs->spi_config);
	val |= input_first_mode;
	write32(ds->regs->spi_config, val);
	val = read32(ds->regs->io_control);

	if (clk_idle_state)
		val |= SPI_IO_CTRL_CLOCK_IDLE_HIGH;
	else
		val &= ~SPI_IO_CTRL_CLOCK_IDLE_HIGH;

	write32(ds->regs->io_control, val);
}

/*
 * Reset entire QUP and all mini cores
 */
static void spi_reset(struct qcs_spi_slave *ds)
{
	write32(ds->regs->qup_sw_reset, 0x1);
	udelay(5);
	check_qup_state_valid(ds);
}

static struct qcs_spi_slave spi_slave_pool[3];

static struct qcs_spi_slave *to_qcs_spi(const struct spi_slave *slave)
{
	struct qcs_spi_slave *ds;
	size_t i;

	for (i = 0; i < ARRAY_SIZE(spi_slave_pool); i++) {
		ds = spi_slave_pool + i;

		if (!ds->allocated)
			continue;

		if ((ds->slave.bus == slave->bus) &&
		    (ds->slave.cs == slave->cs))
			return ds;
	}

	return NULL;
}

static void write_force_cs(const struct spi_slave *slave, int assert)
{
	struct qcs_spi_slave *ds = to_qcs_spi(slave);
	if (assert)
		clrsetbits32(ds->regs->io_control,
			SPI_IO_CTRL_FORCE_CS_MSK, SPI_IO_CTRL_FORCE_CS_EN);
	else
		clrsetbits32(ds->regs->io_control,
			SPI_IO_CTRL_FORCE_CS_MSK, SPI_IO_CTRL_FORCE_CS_DIS);
}

/*
 * BLSP QUPn SPI Hardware Initialisation
 */
static int spi_hw_init(struct qcs_spi_slave *ds)
{
	int ret;

	ds->initialized = 0;

	/* QUPn module configuration */
	spi_reset(ds);

	/* Set the QUPn state */
	ret = config_spi_state(ds, QUP_STATE_RESET);
	if (ret)
		return ret;

	/*
	 * Configure Mini core to SPI core with Input Output enabled,
	 * SPI master, N = 8 bits
	 */
	clrsetbits32(ds->regs->qup_config, QUP_CONFIG_MINI_CORE_MSK |
						QUP_CONF_INPUT_MSK |
						QUP_CONF_OUTPUT_MSK |
						QUP_CONF_N_MASK,
						QUP_CONFIG_MINI_CORE_SPI |
						QUP_CONF_INPUT_ENA |
						QUP_CONF_OUTPUT_ENA |
						QUP_CONF_N_SPI_8_BIT_WORD);

	/*
	 * Configure Input first SPI protocol,
	 * SPI master mode and no loopback
	 */
	clrsetbits32(ds->regs->spi_config, SPI_CONFIG_LOOP_BACK_MSK |
						SPI_CONFIG_NO_SLAVE_OPER_MSK,
						SPI_CONFIG_NO_LOOP_BACK |
						SPI_CONFIG_NO_SLAVE_OPER);

	/*
	 * Configure SPI IO Control Register
	 * CLK_ALWAYS_ON = 0
	 * MX_CS_MODE = 0
	 * NO_TRI_STATE = 1
	 */
	write32(ds->regs->io_control, SPI_IO_CTRL_CLK_ALWAYS_ON |
		SPI_IO_CTRL_NO_TRI_STATE | SPI_IO_CTRL_MX_CS_MODE);

	/*
	 * Configure SPI IO Modes.
	 * OUTPUT_BIT_SHIFT_EN = 1
	 * INPUT_MODE = Block Mode
	 * OUTPUT MODE = Block Mode
	 */
	clrsetbits32(ds->regs->qup_io_modes,
				QUP_IO_MODES_OUTPUT_BIT_SHIFT_MSK |
				QUP_IO_MODES_INPUT_MODE_MSK |
				QUP_IO_MODES_OUTPUT_MODE_MSK,
				QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN |
				QUP_IO_MODES_INPUT_BLOCK_MODE |
				QUP_IO_MODES_OUTPUT_BLOCK_MODE);

	spi_set_mode(ds, ds->mode);

	/* Disable Error mask */
	write32(ds->regs->error_flags_en, 0);
	write32(ds->regs->qup_error_flags_en, 0);
	write32(ds->regs->qup_deassert_wait, 0);

	ds->initialized = 1;

	return SUCCESS;
}

static int spi_ctrlr_claim_bus(const struct spi_slave *slave)
{
	struct qcs_spi_slave *ds = to_qcs_spi(slave);
	unsigned int ret;

	ret = spi_hw_init(ds);
	if (ret)
		return -EIO;
	switch (slave->bus) {
	case 4:
		gpio_configure
		(GPIO(37), 2, GPIO_PULL_DOWN, GPIO_6MA, GPIO_INPUT); // MOSI
		gpio_configure
		(GPIO(38), 2, GPIO_PULL_DOWN, GPIO_6MA, GPIO_OUTPUT); // MISO
		gpio_configure
		(GPIO(117), 2, GPIO_NO_PULL, GPIO_6MA, GPIO_OUTPUT); // CS
		gpio_configure
		(GPIO(118), 2, GPIO_PULL_DOWN, GPIO_6MA, GPIO_OUTPUT);// CLK
		break;
	case 5:
		gpio_configure
		(GPIO(26), 3, GPIO_NO_PULL, GPIO_16MA, GPIO_INPUT); // MOSI
		gpio_configure
		(GPIO(27), 3, GPIO_NO_PULL, GPIO_16MA, GPIO_INPUT); // MISO
		gpio_configure
		(GPIO(28), 4, GPIO_PULL_UP, GPIO_16MA, GPIO_INPUT); // CS
		gpio_configure
		(GPIO(29), 4, GPIO_NO_PULL, GPIO_16MA, GPIO_INPUT); // CLK
		break;
	default:
		printk(BIOS_ERR, "SPI error: unsupported bus %d "
			"(Supported buses 0, 1, 2, 3, 4, 5)\n", slave->bus);
		break;
	}
	write_force_cs(slave, 1);

	return SUCCESS;
}

static void spi_ctrlr_release_bus(const struct spi_slave *slave)
{
	struct qcs_spi_slave *ds = to_qcs_spi(slave);

	/* Reset the SPI hardware */
	write_force_cs(slave, 0);
	spi_reset(ds);
	ds->initialized = 0;
}

/*
 * Function to write data to OUTPUT FIFO
 */
static void spi_write_byte(struct qcs_spi_slave *ds, unsigned char data)
{
	/* Wait for space in the FIFO */
	while ((read32(ds->regs->qup_operational) & OUTPUT_FIFO_FULL))
		udelay(1);

	/* Write the byte of data */
	write32(ds->regs->qup_output_fifo, data);
}

/*
 * Function to read data from Input FIFO
 */
static unsigned char spi_read_byte(struct qcs_spi_slave *ds)
{
	/* Wait for Data in FIFO */
	while (!(read32(ds->regs->qup_operational) & INPUT_FIFO_NOT_EMPTY))
		udelay(1);

	/* Read a byte of data */
	return read32(ds->regs->qup_input_fifo) & 0xff;
}

/*
 * Function to check whether Input or Output FIFO
 * has data to be serviced
 */
static int check_fifo_status(void *reg_addr)
{
	unsigned int count = TIMEOUT_CNT;
	unsigned int status_flag;
	unsigned int val;

	do {
		val = read32(reg_addr);
		count--;
		if (count == 0)
			return -ETIMEDOUT;
		status_flag = ((val & OUTPUT_SERVICE_FLAG) |
					(val & INPUT_SERVICE_FLAG));
	} while (!status_flag);

	return SUCCESS;
}

/*
 * Function to configure Input and Output enable/disable
 */
static void enable_io_config(struct qcs_spi_slave *ds,
				uint32_t write_cnt, uint32_t read_cnt)
{

	if (write_cnt) {
		clrsetbits32(ds->regs->qup_config,
				QUP_CONF_OUTPUT_MSK, QUP_CONF_OUTPUT_ENA);
	} else {
		clrsetbits32(ds->regs->qup_config,
				QUP_CONF_OUTPUT_MSK, QUP_CONF_NO_OUTPUT);
	}

	if (read_cnt) {
		clrsetbits32(ds->regs->qup_config,
				QUP_CONF_INPUT_MSK, QUP_CONF_INPUT_ENA);
	} else {
		clrsetbits32(ds->regs->qup_config,
				QUP_CONF_INPUT_MSK, QUP_CONF_NO_INPUT);
	}
}

/*
 * Function to read bytes number of data from the Input FIFO
 */
static int __blsp_spi_read(struct qcs_spi_slave *ds, u8 *data_buffer,
				unsigned int bytes)
{
	uint32_t val;
	unsigned int i;
	unsigned int fifo_count;
	int ret = SUCCESS;
	int state_config;
	struct stopwatch sw;

	/* Configure no of bytes to read */
	state_config = config_spi_state(ds, QUP_STATE_RESET);
	if (state_config)
		return state_config;

	/* Configure input and output enable */
	enable_io_config(ds, 0, bytes);

	write32(ds->regs->qup_mx_input_count, bytes);

	state_config = config_spi_state(ds, QUP_STATE_RUN);
	if (state_config)
		return state_config;

	while (bytes) {
		ret = check_fifo_status(ds->regs->qup_operational);
		if (ret != SUCCESS)
			goto out;

		val = read32(ds->regs->qup_operational);
		if (val & INPUT_SERVICE_FLAG) {
			/*
			 * acknowledge to hw that software will
			 * read input data
			 */
			val &= INPUT_SERVICE_FLAG;
			write32(ds->regs->qup_operational, val);

			fifo_count = ((bytes > SPI_INPUT_BLOCK_SIZE) ?
					SPI_INPUT_BLOCK_SIZE : bytes);

			for (i = 0; i < fifo_count; i++) {
				*data_buffer = spi_read_byte(ds);
				data_buffer++;
				bytes--;
			}
		}
	}

	stopwatch_init_msecs_expire(&sw, 10);

	do {
		val = read32(ds->regs->qup_operational);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "SPI FIFO read timeout\n");
			ret = -ETIMEDOUT;
			goto out;
		}
	} while (!(val & MAX_INPUT_DONE_FLAG));

out:
	/*
	 * Put the SPI Core back in the Reset State
	 * to end the transfer
	 */
	(void)config_spi_state(ds, QUP_STATE_RESET);
	return ret;
}

static int blsp_spi_read(struct qcs_spi_slave *ds, u8 *data_buffer,
				unsigned int bytes)
{
	int length, ret;

	while (bytes) {
		length = (bytes < MAX_COUNT_SIZE) ? bytes : MAX_COUNT_SIZE;

		ret = __blsp_spi_read(ds, data_buffer, length);
		if (ret != SUCCESS)
			return ret;

		data_buffer += length;
		bytes -= length;
	}

	return 0;
}

/*
 * Function to write data to the Output FIFO
 */
static int __blsp_spi_write(struct qcs_spi_slave *ds, const u8 *cmd_buffer,
				unsigned int bytes)
{
	uint32_t val;
	unsigned int i;
	unsigned int write_len = bytes;
	unsigned int read_len = bytes;
	unsigned int fifo_count;
	int ret = SUCCESS;
	int state_config;
	struct stopwatch sw;

	state_config = config_spi_state(ds, QUP_STATE_RESET);
	if (state_config)
		return state_config;

	/* Configure input and output enable */
	enable_io_config(ds, write_len, read_len);
	/* No of bytes to be written in Output FIFO */
	write32(ds->regs->qup_mx_output_count, bytes);
	write32(ds->regs->qup_mx_input_count, bytes);
	state_config = config_spi_state(ds, QUP_STATE_RUN);

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

		val = read32(ds->regs->qup_operational);
		if (val & OUTPUT_SERVICE_FLAG) {
			/*
			 * acknowledge to hw that software will write
			 * expected output data
			 */
			val &= OUTPUT_SERVICE_FLAG;
			write32(ds->regs->qup_operational, val);

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
			write32(ds->regs->qup_operational, val);

			if (read_len > SPI_INPUT_BLOCK_SIZE)
				fifo_count = SPI_INPUT_BLOCK_SIZE;
			else
				fifo_count = read_len;

			for (i = 0; i < fifo_count; i++) {
				/* Read dummy data for the data written */
				(void)spi_read_byte(ds);

				/* Decrement the read count after reading the
				 * dummy data from the device. This is to make
				 * sure we read dummy data before we write the
				 * data to fifo
				 */
				read_len--;
			}
		}
	}

	stopwatch_init_msecs_expire(&sw, 10);

	do {
		val = read32(ds->regs->qup_operational);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "SPI FIFO write timeout\n");
			ret = -ETIMEDOUT;
			goto out;
		}

	} while (!(val & MAX_OUTPUT_DONE_FLAG));

out:
	/*
	 * Put the SPI Core back in the Reset State
	 * to end the transfer
	 */
	(void)config_spi_state(ds, QUP_STATE_RESET);

	return ret;
}

static int blsp_spi_write(struct qcs_spi_slave *ds, u8 *cmd_buffer,
				unsigned int bytes)
{
	int length, ret;

	while (bytes) {
		length = (bytes < MAX_COUNT_SIZE) ? bytes : MAX_COUNT_SIZE;

		ret = __blsp_spi_write(ds, cmd_buffer, length);
		if (ret != SUCCESS) {
			printk(BIOS_ERR, "SPI:DBG write not success\n");
			return ret;
		}

		cmd_buffer += length;
		bytes -= length;
	}

	return 0;
}

/*
 * This function is invoked with either tx_buf or rx_buf.
 * Calling this function with both null does a chip select change.
 */
static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
			size_t out_bytes, void *din, size_t in_bytes)
{
	struct qcs_spi_slave *ds = to_qcs_spi(slave);
	u8 *txp = (u8 *)dout;
	u8 *rxp = (u8 *)din;
	int ret;

	ret = config_spi_state(ds, QUP_STATE_RESET);
	if (ret != SUCCESS)
		return ret;

	if (dout != NULL) {
		ret = blsp_spi_write(ds, txp, (unsigned int)out_bytes);
		if (ret != SUCCESS)
			goto out;
	}

	if (din != NULL) {
		ret = blsp_spi_read(ds, rxp, in_bytes);
		if (ret != SUCCESS)
			goto out;
	}

out:
	/*
	 * Put the SPI Core back in the Reset State
	 * to end the transfer
	 */
	(void)config_spi_state(ds, QUP_STATE_RESET);

	return ret;
}

static int spi_ctrlr_setup(const struct spi_slave *slave)
{
	struct qcs_spi_slave *ds = NULL;
	int i;
	unsigned int bus = slave->bus;
	unsigned int cs = slave->cs;
	int qup = 0;
	int blsp = 2;

	if (((bus != BLSP4_SPI) && (bus != BLSP5_SPI)) || cs != 0) {
		printk(BIOS_ERR,
			"SPI error: unsupported bus %d or cs %d\n", bus, cs);
		return -1;
	}

	for (i = 0; i < ARRAY_SIZE(spi_slave_pool); i++) {
		if (spi_slave_pool[i].allocated)
			continue;
		ds = spi_slave_pool + i;
		ds->slave.bus = bus;
		ds->slave.cs = cs;
		ds->regs = &spi_reg[bus];
		ds->mode = SPI_MODE0;
		ds->freq = 50000000;

		if (bus == BLSP4_SPI) {
			ds->freq = 1000000;
			qup = 4;
			blsp = 1;
		}

		clock_configure_spi(blsp, qup, ds->freq);
		clock_enable_spi(blsp, qup);

		ds->allocated = 1;

		return 0;
	}

	printk(BIOS_ERR, "SPI error: all %d pools busy\n", i);
	return -1;
}

static int xfer_vectors(const struct spi_slave *slave,
			struct spi_op vectors[], size_t count)
{
	return spi_flash_vector_helper(slave, vectors, count, spi_ctrlr_xfer);
}

static const struct spi_ctrlr spi_ctrlr = {
	.setup = spi_ctrlr_setup,
	.claim_bus = spi_ctrlr_claim_bus,
	.release_bus = spi_ctrlr_release_bus,
	.xfer = spi_ctrlr_xfer,
	.xfer_vector = xfer_vectors,
	.max_xfer_size = MAX_PACKET_COUNT,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = BLSP5_SPI,
		.bus_end = BLSP5_SPI,
	},
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = BLSP4_SPI,
		.bus_end = BLSP4_SPI,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
