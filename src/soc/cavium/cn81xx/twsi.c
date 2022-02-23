/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#include <console/console.h>
#include <soc/twsi.h>
#include <soc/clock.h>
#include <device/i2c.h>
#include <device/i2c_simple.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <timer.h>

#define TWSI_THP		24

#define TWSI_SW_TWSI		0x1000
#define TWSI_TWSI_SW		0x1008
#define TWSI_INT		0x1010
#define TWSI_SW_TWSI_EXT	0x1018

union twsx_sw_twsi {
	u64 u;
	struct {
		u64 data:32;
		u64 eop_ia:3;
		u64 ia:5;
		u64 addr:10;
		u64 scr:2;
		u64 size:3;
		u64 sovr:1;
		u64 r:1;
		u64 op:4;
		u64 eia:1;
		u64 slonly:1;
		u64 v:1;
	} s;
};

union twsx_sw_twsi_ext {
	u64 u;
	struct {
		u64	data:32;
		u64	ia:8;
		u64	:24;
	} s;
};

union twsx_int {
	u64 u;
	struct {
		u64	st_int:1;	/** TWSX_SW_TWSI register update int */
		u64	ts_int:1;	/** TWSX_TWSI_SW register update int */
		u64	core_int:1;	/** TWSI core interrupt, ignored for HLC */
		u64	:5;		/** Reserved */
		u64	sda_ovr:1;	/** SDA testing override */
		u64	scl_ovr:1;	/** SCL testing override */
		u64	sda:1;		/** SDA signal */
		u64	scl:1;		/** SCL signal */
		u64	:52;		/** Reserved */
	} s;
};

enum {
	TWSI_OP_WRITE	= 0,
	TWSI_OP_READ	= 1,
};

enum {
	TWSI_EOP_SLAVE_ADDR = 0,
	TWSI_EOP_CLK_CTL = 3,
	TWSI_SW_EOP_IA   = 6,
};

enum {
	TWSI_SLAVEADD     = 0,
	TWSI_DATA         = 1,
	TWSI_CTL          = 2,
	TWSI_CLKCTL       = 3,
	TWSI_STAT         = 3,
	TWSI_SLAVEADD_EXT = 4,
	TWSI_RST          = 7,
};

enum {
	TWSI_CTL_AAK	= (1 << 2),
	TWSI_CTL_IFLG	= (1 << 3),
	TWSI_CTL_STP	= (1 << 4),
	TWSI_CTL_STA	= (1 << 5),
	TWSI_CTL_ENAB	= (1 << 6),
	TWSI_CTL_CE	= (1 << 7),
};

enum {
	/** Bus error */
	TWSI_STAT_BUS_ERROR		= 0x00,
	/** Start condition transmitted */
	TWSI_STAT_START			= 0x08,
	/** Repeat start condition transmitted */
	TWSI_STAT_RSTART		= 0x10,
	/** Address + write bit transmitted, ACK received */
	TWSI_STAT_TXADDR_ACK		= 0x18,
	/** Address + write bit transmitted, /ACK received */
	TWSI_STAT_TXADDR_NAK		= 0x20,
	/** Data byte transmitted in master mode, ACK received */
	TWSI_STAT_TXDATA_ACK		= 0x28,
	/** Data byte transmitted in master mode, ACK received */
	TWSI_STAT_TXDATA_NAK		= 0x30,
	/** Arbitration lost in address or data byte */
	TWSI_STAT_TX_ARB_LOST		= 0x38,
	/** Address + read bit transmitted, ACK received */
	TWSI_STAT_RXADDR_ACK		= 0x40,
	/** Address + read bit transmitted, /ACK received */
	TWSI_STAT_RXADDR_NAK		= 0x48,
	/** Data byte received in master mode, ACK transmitted */
	TWSI_STAT_RXDATA_ACK_SENT	= 0x50,
	/** Data byte received, NACK transmitted */
	TWSI_STAT_RXDATA_NAK_SENT	= 0x58,
	/** Slave address received, sent ACK */
	TWSI_STAT_SLAVE_RXADDR_ACK	= 0x60,
	/**
	 * Arbitration lost in address as master, slave address + write bit
	 * received, ACK transmitted
	 */
	TWSI_STAT_TX_ACK_ARB_LOST	= 0x68,
	/** General call address received, ACK transmitted */
	TWSI_STAT_RX_GEN_ADDR_ACK	= 0x70,
	/**
	 * Arbitration lost in address as master, general call address
	 * received, ACK transmitted
	 */
	TWSI_STAT_RX_GEN_ADDR_ARB_LOST	= 0x78,
	/** Data byte received after slave address received, ACK transmitted */
	TWSI_STAT_SLAVE_RXDATA_ACK	= 0x80,
	/** Data byte received after slave address received, /ACK transmitted */
	TWSI_STAT_SLAVE_RXDATA_NAK	= 0x88,
	/**
	 * Data byte received after general call address received, ACK
	 * transmitted
	 */
	TWSI_STAT_GEN_RXADDR_ACK	= 0x90,
	/**
	 * Data byte received after general call address received, /ACK
	 * transmitted
	 */
	TWSI_STAT_GEN_RXADDR_NAK	= 0x98,
	/** STOP or repeated START condition received in slave mode */
	TWSI_STAT_STOP_MULTI_START	= 0xA0,
	/** Slave address + read bit received, ACK transmitted */
	TWSI_STAT_SLAVE_RXADDR2_ACK	= 0xA8,
	/**
	 * Arbitration lost in address as master, slave address + read bit
	 * received, ACK transmitted
	 */
	TWSI_STAT_RXDATA_ACK_ARB_LOST	= 0xB0,
	/** Data byte transmitted in slave mode, ACK received */
	TWSI_STAT_SLAVE_TXDATA_ACK	= 0xB8,
	/** Data byte transmitted in slave mode, /ACK received */
	TWSI_STAT_SLAVE_TXDATA_NAK	= 0xC0,
	/** Last byte transmitted in slave mode, ACK received */
	TWSI_STAT_SLAVE_TXDATA_END_ACK	= 0xC8,
	/** Second address byte + write bit transmitted, ACK received */
	TWSI_STAT_TXADDR2DATA_ACK	= 0xD0,
	/** Second address byte + write bit transmitted, /ACK received */
	TWSI_STAT_TXADDR2DATA_NAK	= 0xD8,
	/** No relevant status information */
	TWSI_STAT_IDLE		= 0xF8
};

/**
 * Returns true if we lost arbitration
 *
 * @param	code		status code
 * @param	final_read	true if this is the final read operation
 *
 * @return	true if arbitration has been lost, false if it hasn't been lost.
 */
static int twsi_i2c_lost_arb(u8 code, int final_read)
{
	switch (code) {
	/* Arbitration lost */
	case TWSI_STAT_TX_ARB_LOST:
	case TWSI_STAT_TX_ACK_ARB_LOST:
	case TWSI_STAT_RX_GEN_ADDR_ARB_LOST:
	case TWSI_STAT_RXDATA_ACK_ARB_LOST:
		return -1;

	/* Being addressed as slave, should back off and listen */
	case TWSI_STAT_SLAVE_RXADDR_ACK:
	case TWSI_STAT_RX_GEN_ADDR_ACK:
	case TWSI_STAT_GEN_RXADDR_ACK:
	case TWSI_STAT_GEN_RXADDR_NAK:
		return -1;

	/* Core busy as slave */
	case TWSI_STAT_SLAVE_RXDATA_ACK:
	case TWSI_STAT_SLAVE_RXDATA_NAK:
	case TWSI_STAT_STOP_MULTI_START:
	case TWSI_STAT_SLAVE_RXADDR2_ACK:
	case TWSI_STAT_SLAVE_TXDATA_ACK:
	case TWSI_STAT_SLAVE_TXDATA_NAK:
	case TWSI_STAT_SLAVE_TXDATA_END_ACK:
		return  -1;

	/* Ack allowed on pre-terminal bytes only */
	case TWSI_STAT_RXDATA_ACK_SENT:
		if (!final_read)
			return 0;
		return -1;

	/* NAK allowed on terminal byte only */
	case TWSI_STAT_RXDATA_NAK_SENT:
		if (!final_read)
			return 0;
		return -1;

	case TWSI_STAT_TXDATA_NAK:
	case TWSI_STAT_TXADDR_NAK:
	case TWSI_STAT_RXADDR_NAK:
	case TWSI_STAT_TXADDR2DATA_NAK:
		return -1;
	}
	return 0;
}

#define RST_BOOT_PNR_MUL(Val)  ((Val >> 33) & 0x1F)

/**
 * Writes to the MIO_TWS(0..5)_SW_TWSI register
 *
 * @param	baseaddr	Base address of i2c registers
 * @param	sw_twsi		value to write
 *
 * @return	0 for success, otherwise error
 */
static u64 twsi_write_sw(void *baseaddr, union twsx_sw_twsi sw_twsi)
{
	unsigned long timeout = 500000;

	sw_twsi.s.r = 0;
	sw_twsi.s.v = 1;

	printk(BIOS_SPEW, "%s(%p, 0x%llx)\n", __func__, baseaddr, sw_twsi.u);
	write64(baseaddr + TWSI_SW_TWSI, sw_twsi.u);
	do {
		sw_twsi.u = read64(baseaddr + TWSI_SW_TWSI);
		timeout--;
	} while (sw_twsi.s.v != 0 && timeout > 0);

	if (sw_twsi.s.v)
		printk(BIOS_ERR, "%s: timed out\n", __func__);
	return sw_twsi.u;
}

/**
 * Reads the MIO_TWS(0..5)_SW_TWSI register
 *
 * @param	baseaddr	Base address of i2c registers
 * @param	sw_twsi		value for eia and op, etc. to read
 *
 * @return	value of the register
 */
static u64 twsi_read_sw(void *baseaddr, union twsx_sw_twsi sw_twsi)
{
	unsigned long timeout = 500000;
	sw_twsi.s.r = 1;
	sw_twsi.s.v = 1;

	printk(BIOS_SPEW, "%s(%p, 0x%llx)\n", __func__, baseaddr, sw_twsi.u);
	write64(baseaddr + TWSI_SW_TWSI, sw_twsi.u);

	do {
		sw_twsi.u = read64(baseaddr + TWSI_SW_TWSI);
		timeout--;
	} while (sw_twsi.s.v != 0 && timeout > 0);

	if (sw_twsi.s.v)
		printk(BIOS_ERR, "%s: Error writing 0x%llx\n", __func__,
		       sw_twsi.u);

	printk(BIOS_SPEW, "%s: Returning 0x%llx\n", __func__, sw_twsi.u);
	return sw_twsi.u;
}

/**
 * Write control register
 *
 * @param	baseaddr	Base address for i2c registers
 * @param	data		data to write
 */
static void twsi_write_ctl(void *baseaddr, const u8 data)
{
	union twsx_sw_twsi twsi_sw;

	printk(BIOS_SPEW, "%s(%p, 0x%x)\n", __func__, baseaddr, data);
	twsi_sw.u = 0;

	twsi_sw.s.op = TWSI_SW_EOP_IA;
	twsi_sw.s.eop_ia = TWSI_CTL;
	twsi_sw.s.data  = data;

	twsi_write_sw(baseaddr, twsi_sw);
}

/**
 * Reads the TWSI Control Register
 *
 * @param[in]	baseaddr	Base address for i2c
 *
 * @return	8-bit TWSI control register
 */
static u32 twsi_read_ctl(void *baseaddr)
{
	union twsx_sw_twsi sw_twsi;

	sw_twsi.u  = 0;
	sw_twsi.s.op  = TWSI_SW_EOP_IA;
	sw_twsi.s.eop_ia = TWSI_CTL;

	sw_twsi.u = twsi_read_sw(baseaddr, sw_twsi);
	printk(BIOS_SPEW, "%s(%p): 0x%x\n", __func__, baseaddr, sw_twsi.s.data);
	return sw_twsi.s.data;
}

/**
 * Read i2c status register
 *
 * @param	baseaddr	Base address of i2c registers
 *
 * @return	value of status register
 */
static u8 twsi_read_status(void *baseaddr)
{
	union twsx_sw_twsi twsi_sw;

	twsi_sw.u = 0;
	twsi_sw.s.op = TWSI_SW_EOP_IA;
	twsi_sw.s.eop_ia = TWSI_STAT;

	return twsi_read_sw(baseaddr, twsi_sw);
}

/**
 * Waits for an i2c operation to complete
 *
 * @param	baseaddr	Base address of registers
 *
 * @return	0 for success, 1 if timeout
 */
static int twsi_wait(void *baseaddr, struct stopwatch *timeout)
{
	u8 twsi_ctl;

	printk(BIOS_SPEW, "%s(%p)\n", __func__, baseaddr);
	do {
		twsi_ctl = twsi_read_ctl(baseaddr);
		twsi_ctl &= TWSI_CTL_IFLG;
	} while (!twsi_ctl && !stopwatch_expired(timeout));

	printk(BIOS_SPEW, "  return: %u\n", !twsi_ctl);
	return !twsi_ctl;
}

/**
 * Sends an i2c stop condition
 *
 * @param	baseaddr	register base address
 *
 * @return	0 for success, -1 if error
 */
static int twsi_stop(void *baseaddr)
{
	u8 stat;
	twsi_write_ctl(baseaddr, TWSI_CTL_STP | TWSI_CTL_ENAB);

	stat = twsi_read_status(baseaddr);
	if (stat != TWSI_STAT_IDLE) {
		printk(BIOS_ERR, "%s: Bad status on bus@%p\n", __func__,
		       baseaddr);
		return -1;
	}
	return 0;
}

/**
 * Manually clear the I2C bus and send a stop
 */
static void twsi_unblock(void *baseaddr)
{
	int i;
	union twsx_int	int_reg;

	int_reg.u = 0;
	for (i = 0; i < 9; i++) {
		int_reg.s.scl_ovr = 0;
		write64(baseaddr + TWSI_INT, int_reg.u);
		udelay(5);
		int_reg.s.scl_ovr = 1;
		write64(baseaddr + TWSI_INT, int_reg.u);
		udelay(5);
	}
	int_reg.s.sda_ovr = 1;
	write64(baseaddr + TWSI_INT, int_reg.u);
	udelay(5);
	int_reg.s.scl_ovr = 0;
	write64(baseaddr + TWSI_INT, int_reg.u);
	udelay(5);
	int_reg.u = 0;
	write64(baseaddr + TWSI_INT, int_reg.u);
	udelay(5);
}

/**
 * Unsticks the i2c bus
 *
 * @param	baseaddr	base address of registers
 */
static int twsi_start_unstick(void *baseaddr)
{
	twsi_stop(baseaddr);

	twsi_unblock(baseaddr);

	return 0;
}

/**
 * Sends an i2c start condition
 *
 * @param	baseaddr	base address of registers
 *
 * @return	0 for success, otherwise error
 */
static int twsi_start(void *baseaddr)
{
	int result;
	u8 stat;
	struct stopwatch timeout;

	printk(BIOS_SPEW, "%s(%p)\n", __func__, baseaddr);
	stopwatch_init_usecs_expire(&timeout, CONFIG_I2C_TRANSFER_TIMEOUT_US);
	twsi_write_ctl(baseaddr, TWSI_CTL_STA | TWSI_CTL_ENAB);
	result = twsi_wait(baseaddr, &timeout);
	if (result) {
		stat = twsi_read_status(baseaddr);
		printk(BIOS_SPEW, "%s: result: 0x%x, status: 0x%x\n", __func__,
		      result, stat);
		switch (stat) {
		case TWSI_STAT_START:
		case TWSI_STAT_RSTART:
			return 0;
		case TWSI_STAT_RXADDR_ACK:
		default:
			return twsi_start_unstick(baseaddr);
		}
	}
	printk(BIOS_SPEW, "%s: success\n", __func__);
	return 0;
}

/**
 * Writes data to the i2c bus
 *
 * @param	baseraddr	register base address
 * @param	slave_addr	address of slave to write to
 * @param	buffer		Pointer to buffer to write
 * @param	length		Number of bytes in buffer to write
 *
 * @return	0 for success, otherwise error
 */
static int twsi_write_data(void *baseaddr, const u8 slave_addr,
			   const u8 *buffer, const unsigned int length)
{
	union twsx_sw_twsi twsi_sw;
	unsigned int curr = 0;
	int result;
	struct stopwatch timeout;

	printk(BIOS_SPEW, "%s(%p, 0x%x, %p, 0x%x)\n", __func__, baseaddr,
	       slave_addr, buffer, length);
	stopwatch_init_usecs_expire(&timeout, CONFIG_I2C_TRANSFER_TIMEOUT_US);
	result = twsi_start(baseaddr);
	if (result) {
		printk(BIOS_ERR, "%s: Could not start BUS transaction\n",
		       __func__);
		return -1;
	}

	result = twsi_wait(baseaddr, &timeout);
	if (result) {
		printk(BIOS_ERR, "%s: wait failed\n", __func__);
		return result;
	}

	twsi_sw.u = 0;
	twsi_sw.s.op = TWSI_SW_EOP_IA;
	twsi_sw.s.eop_ia = TWSI_DATA;
	twsi_sw.s.data = (u32) (slave_addr << 1) | TWSI_OP_WRITE;

	twsi_write_sw(baseaddr, twsi_sw);
	twsi_write_ctl(baseaddr, TWSI_CTL_ENAB);

	printk(BIOS_SPEW, "%s: Waiting\n", __func__);
	result = twsi_wait(baseaddr, &timeout);
	if (result) {
		printk(BIOS_ERR, "%s: Timed out writing slave address 0x%x\n",
		      __func__, slave_addr);
		return result;
	}
	result = twsi_read_status(baseaddr);
	if ((result = twsi_read_status(baseaddr)) != TWSI_STAT_TXADDR_ACK) {
		twsi_stop(baseaddr);
		return twsi_i2c_lost_arb(result, 0);
	}

	while (curr < length) {
		twsi_sw.u = 0;
		twsi_sw.s.op = TWSI_SW_EOP_IA;
		twsi_sw.s.eop_ia = TWSI_DATA;
		twsi_sw.s.data = buffer[curr++];

		twsi_write_sw(baseaddr, twsi_sw);
		twsi_write_ctl(baseaddr, TWSI_CTL_ENAB);

		result = twsi_wait(baseaddr, &timeout);
		if (result) {
			printk(BIOS_ERR, "%s: Timed out writing data to 0x%x\n",
			      __func__, slave_addr);
			return result;
		}
	}

	printk(BIOS_SPEW, "%s: Stopping\n", __func__);
	return twsi_stop(baseaddr);
}

/**
 * Performs a read transaction on the i2c bus
 *
 * @param	baseaddr	Base address of twsi registers
 * @param	slave_addr	i2c bus address to read from
 * @param	buffer		buffer to read into
 * @param	length		number of bytes to read
 *
 * @return	0 for success, otherwise error
 */
static int twsi_read_data(void *baseaddr, const u8 slave_addr,
			  u8 *buffer, const unsigned int length)
{
	union twsx_sw_twsi twsi_sw;
	unsigned int curr = 0;
	int result;
	struct stopwatch timeout;

	printk(BIOS_SPEW, "%s(%p, 0x%x, %p, %u)\n", __func__, baseaddr,
	       slave_addr, buffer, length);
	stopwatch_init_usecs_expire(&timeout, CONFIG_I2C_TRANSFER_TIMEOUT_US);
	result = twsi_start(baseaddr);
	if (result) {
		printk(BIOS_ERR, "%s: start failed\n", __func__);
		return result;
	}

	result = twsi_wait(baseaddr, &timeout);
	if (result) {
		printk(BIOS_ERR, "%s: wait failed\n", __func__);
		return result;
	}

	twsi_sw.u = 0;

	twsi_sw.s.op = TWSI_SW_EOP_IA;
	twsi_sw.s.eop_ia = TWSI_DATA;

	twsi_sw.s.data  = (u32) (slave_addr << 1) | TWSI_OP_READ;

	twsi_write_sw(baseaddr, twsi_sw);
	twsi_write_ctl(baseaddr, TWSI_CTL_ENAB);

	result = twsi_wait(baseaddr, &timeout);
	if (result) {
		printk(BIOS_ERR, "%s: waiting for sending addr failed\n", __func__);
		return result;
	}

	result = twsi_read_status(baseaddr);
	if (result != TWSI_STAT_RXADDR_ACK) {
		twsi_stop(baseaddr);
		return twsi_i2c_lost_arb(result, 0);
	}

	while (curr < length) {
		twsi_write_ctl(baseaddr, TWSI_CTL_ENAB |
				((curr < length - 1) ? TWSI_CTL_AAK : 0));

		result = twsi_wait(baseaddr, &timeout);
		if (result) {
			printk(BIOS_ERR, "%s: waiting for data failed\n",
			       __func__);
			return result;
		}

		twsi_sw.u = twsi_read_sw(baseaddr, twsi_sw);
		buffer[curr++] = twsi_sw.s.data;
	}

	twsi_stop(baseaddr);

	return 0;
}

static int twsi_set_speed(void *baseaddr, const unsigned int speed)
{
	u64 io_clock_hz;
	int n_div;
	int m_div;
	union twsx_sw_twsi sw_twsi;

	io_clock_hz = thunderx_get_io_clock();

	/* Set the TWSI clock to a conservative TWSI_BUS_FREQ.  Compute the
	 * clocks M divider based on the SCLK.
	 * TWSI freq = (core freq) / (20 x (M+1) x (thp+1) x 2^N)
	 * M = ((core freq) / (20 x (TWSI freq) x (thp+1) x 2^N)) - 1
	 */
	for (n_div = 0; n_div < 8; n_div++) {
		m_div = io_clock_hz / (20 * speed * (TWSI_THP + 1));
		m_div /= 1 << n_div;
		m_div -= 1;
		if (m_div < 16)
			break;
	}
	if (m_div >= 16)
		return -1;

	sw_twsi.u = 0;
	sw_twsi.s.v = 1;
	sw_twsi.s.op = 0x6;	/* See EOP field */
	sw_twsi.s.r = 0;	/* Select CLKCTL when R = 0 */
	sw_twsi.s.eop_ia = 3;	/* R=0 selects CLKCTL, R=1 selects STAT */
	sw_twsi.s.data = ((m_div & 0xf) << 3) | ((n_div & 0x7) << 0);

	twsi_write_sw(baseaddr, sw_twsi);
	return 0;
}

int twsi_init(unsigned int bus, enum i2c_speed hz)
{
	void *baseaddr = (void *)MIO_TWSx_PF_BAR0(bus);
	if (!baseaddr)
		return -1;

	if (twsi_set_speed(baseaddr, hz) < 0)
		return -1;

	/* Enable TWSI, HLC disable, STOP, NAK */
	twsi_write_ctl(baseaddr, TWSI_CTL_ENAB);

	return 0;
}

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments,
			  int seg_count)
{
	int result;
	void *baseaddr = (void *)MIO_TWSx_PF_BAR0(bus);
	if (!baseaddr)
		return -1;

	printk(BIOS_SPEW, "%s: %d messages\n", __func__, seg_count);
	for (; seg_count > 0; seg_count--, segments++) {
		if (segments->flags & I2C_M_RD) {
			result = twsi_read_data(baseaddr, segments->slave,
						segments->buf, segments->len);
		} else {
			result = twsi_write_data(baseaddr, segments->slave,
						 segments->buf, segments->len);
		}
		if (result) {
			printk(BIOS_ERR, "%s: error transmitting data\n",
			       __func__);
			return -1;
		}
	}

	return 0;
}
