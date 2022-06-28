/*
 * Copyright (c) 2014 ChromiumOS authors
 */

#include <libpayload.h>

enum IPQ_UART_DM_PARITY_MODE {
        IPQ_UART_DM_NO_PARITY,
        IPQ_UART_DM_ODD_PARITY,
        IPQ_UART_DM_EVEN_PARITY,
        IPQ_UART_DM_SPACE_PARITY
};

/* UART Stop Bit Length */
enum IPQ_UART_DM_STOP_BIT_LEN {
        IPQ_UART_DM_SBL_9_16,
        IPQ_UART_DM_SBL_1,
        IPQ_UART_DM_SBL_1_9_16,
        IPQ_UART_DM_SBL_2
};

/* UART Bits per Char */
enum IPQ_UART_DM_BITS_PER_CHAR {
        IPQ_UART_DM_5_BPS,
        IPQ_UART_DM_6_BPS,
        IPQ_UART_DM_7_BPS,
        IPQ_UART_DM_8_BPS
};

#define IPQ_UART_DM_CR(base)		(((u8 *)(base)) + 0x10)
#define IPQ_UART_DM_DMEN(base)		(((u8 *)(base)) + 0x3C)
#define IPQ_UART_DM_DMRX(base)		(((u8 *)(base)) + 0x34)
#define IPQ_UART_DM_HCR(base)		(((u8 *)(base)) + 0x24)
#define IPQ_UART_DM_IMR(base)		(((u8 *)(base)) + 0x14)
#define IPQ_UART_DM_IPR(base)		(((u8 *)(base)) + 0x18)
#define IPQ_UART_DM_IRDA(base)		(((u8 *)(base)) + 0x38)
#define IPQ_UART_DM_MISR(base)	 	(((u8 *)(base)) + 0x10)
#define IPQ_UART_DM_MR1(base)		(((u8 *)(base)) + 0x00)
#define IPQ_UART_DM_MR2(base)		(((u8 *)(base)) + 0x04)
#define IPQ_UART_DM_NO_CHARS_FOR_TX(base) (((u8 *)(base)) + 0x040)
#define IPQ_UART_DM_RF(base, x)		(((u8 *)(base)) + 0x70 + 4*(x))
#define IPQ_UART_DM_RFWR(base)		(((u8 *)(base)) + 0x20)
#define IPQ_UART_DM_RX_TOTAL_SNAP(base)	(((u8 *)(base)) + 0x38)
#define IPQ_UART_DM_SR(base)		(((u8 *)(base)) + 0x008)
#define IPQ_UART_DM_TF(base, x)		(((u8 *)(base)) + 0x70 + 4*(x))
#define IPQ_UART_DM_TFWR(base)		(((u8 *)(base)) + 0x1C)

#define IPQ_UART_DM_TXLEV               (1 << 0)
#define IPQ_UART_DM_TX_READY            (1 << 7)

#define IPQ_UART_DM_CR_CH_CMD_LSB(x)    ((x & 0x0f) << 4)
#define IPQ_UART_DM_CR_CH_CMD_MSB(x)    ((x >> 4 ) << 11 )
#define IPQ_UART_DM_CR_CH_CMD(x)        (IPQ_UART_DM_CR_CH_CMD_LSB(x)\
                                             | IPQ_UART_DM_CR_CH_CMD_MSB(x))

#define IPQ_UART_DM_CR_GENERAL_CMD(x)   ((x) << 8)

#define IPQ_UART_DM_8_N_1_MODE          (IPQ_UART_DM_NO_PARITY | \
                                             (IPQ_UART_DM_SBL_1 << 2) | \
                                             (IPQ_UART_DM_8_BPS << 4))
#define IPQ_UART_DM_CR_RX_ENABLE        (1 << 0)
#define IPQ_UART_DM_CR_TX_ENABLE        (1 << 2)
#define IPQ_UART_DM_CMD_RESET_RX        IPQ_UART_DM_CR_CH_CMD(1)
#define IPQ_UART_DM_CMD_RESET_TX        IPQ_UART_DM_CR_CH_CMD(2)
#define IPQ_UART_DM_CMD_RESET_ERR_STAT  IPQ_UART_DM_CR_CH_CMD(3)
#define IPQ_UART_DM_CMD_RES_STALE_INT   IPQ_UART_DM_CR_CH_CMD(8)
#define IPQ_UART_DM_CMD_RES_TX_ERR      IPQ_UART_DM_CR_CH_CMD(10)
#define IPQ_UART_DM_GCMD_ENA_STALE_EVT   IPQ_UART_DM_CR_GENERAL_CMD(5)
#define IPQ_UART_DM_RXSTALE             (1 << 3)
#define IPQ_UART_DM_IMR_ENABLED         (IPQ_UART_DM_TX_READY | \
                                              IPQ_UART_DM_TXLEV    | \
                                              IPQ_UART_DM_RXSTALE)
#define IPQ_UART_DM_TFW_VALUE           0
#define IPQ_UART_DM_RFW_VALUE           1
#define IPQ_UART_DM_DMRX_DEF_VALUE	0x220
#define IPQ_UART_DM_SR_TXEMT            (1 << 3)
#define IPQ_UART_DM_SR_UART_OVERRUN     (1 << 4)
#define IPQ_UART_DM_E_SUCCESS           0
#define IPQ_UART_DM_E_INVAL             3
#define IPQ_UART_DM_E_RX_NOT_READY      5

#define IPQ_UART_DM_STALE_TIMEOUT_LSB   0x0f

static struct console_input_driver consin = {};
static struct console_output_driver consout = {};

#define FIFO_DATA_SIZE	4

static void  *base_uart_addr;

/*
 * All constants lifted from u-boot's
 * board/qcom/ipq806x_cdp/ipq806x_board_param.h
 */
static unsigned int msm_boot_uart_dm_init(void *uart_dm_base);

/* Number of pending received characters */
static int uart_ready_data_count;

/* Received data as it came from 32 bit wide FIFO */
static unsigned int uart_rx_fifo_word;

/**
 * msm_boot_uart_dm_init_rx_transfer - Init Rx transfer
 * @uart_dm_base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_init_rx_transfer(void *uart_dm_base)
{
	/* Reset receiver */
	writel(IPQ_UART_DM_CMD_RESET_RX,
		IPQ_UART_DM_CR(uart_dm_base));

	/* Enable receiver */
	writel(IPQ_UART_DM_CR_RX_ENABLE,
		IPQ_UART_DM_CR(uart_dm_base));
	writel(IPQ_UART_DM_DMRX_DEF_VALUE,
		IPQ_UART_DM_DMRX(uart_dm_base));

	/* Clear stale event */
	writel(IPQ_UART_DM_CMD_RES_STALE_INT,
		IPQ_UART_DM_CR(uart_dm_base));

	/* Enable stale event */
	writel(IPQ_UART_DM_GCMD_ENA_STALE_EVT,
		IPQ_UART_DM_CR(uart_dm_base));

	return IPQ_UART_DM_E_SUCCESS;
}

/**
 * Reads a word from the RX FIFO or returns not ready.
 */
static unsigned msm_boot_uart_dm_read(void)
{
	static int total_rx_data = 0;
	static int rx_data_read = 0;
	void  *base = base_uart_addr;
	uint32_t status_reg;

	/* RXSTALE means RX FIFO is not empty. */
	status_reg = readl(IPQ_UART_DM_MISR(base));
	if (!(status_reg & IPQ_UART_DM_RXSTALE))
		return IPQ_UART_DM_E_RX_NOT_READY;

	/* Check for Overrun error. We'll just reset Error Status */
	if (readl(IPQ_UART_DM_SR(base)) &
			IPQ_UART_DM_SR_UART_OVERRUN) {
		writel(IPQ_UART_DM_CMD_RESET_ERR_STAT,
			IPQ_UART_DM_CR(base));
		total_rx_data = rx_data_read = 0;
		msm_boot_uart_dm_init(base);
		return IPQ_UART_DM_E_RX_NOT_READY;
	}

	/* Read UART_DM_RX_TOTAL_SNAP for actual number of bytes received */
	if (total_rx_data == 0)
		total_rx_data =  readl(IPQ_UART_DM_RX_TOTAL_SNAP(base));

	/* Data available in FIFO; read a word. */
	uart_rx_fifo_word = readl(IPQ_UART_DM_RF(base, 0));

	/*
	 * TODO(vbendeb): this is wrong and will be addressed shortly: there
	 * should be no zeros returned from the FIFO in case there are
	 * received characters (we don't expect to be receiving zeros).
	 * See http://crosbug.com/p/29313
	 */
	if (uart_rx_fifo_word == 0) {
		return IPQ_UART_DM_E_RX_NOT_READY;
	}

	/* increment the total count of chars we've read so far */
	rx_data_read += FIFO_DATA_SIZE;

	/* Actual count of valid data in word */
	uart_ready_data_count =
		((total_rx_data < rx_data_read) ?
		 (FIFO_DATA_SIZE - (rx_data_read - total_rx_data)) :
		 FIFO_DATA_SIZE);

	/* If there are still data left in FIFO we'll read them before
	 * initializing RX Transfer again
	 */
	if (rx_data_read < total_rx_data)
		return IPQ_UART_DM_E_SUCCESS;

	msm_boot_uart_dm_init_rx_transfer(base);
	total_rx_data = rx_data_read = 0;

	return IPQ_UART_DM_E_SUCCESS;
}

void serial_putchar(unsigned data)
{
	int num_of_chars = 0;
	unsigned tx_data = 0;
	void *base = base_uart_addr;

	if (data == '\n') {
		num_of_chars++;
		tx_data = '\r';
	}

	tx_data  |= data << (8 * num_of_chars++);

        /* Wait until transmit FIFO is empty. */
        while (!(readl(IPQ_UART_DM_SR(base)) &
		 IPQ_UART_DM_SR_TXEMT))
		udelay(1);

        /*
	 * TX FIFO is ready to accept new character(s). First write number of
	 * characters to be transmitted.
	 */
        writel(num_of_chars, IPQ_UART_DM_NO_CHARS_FOR_TX(base));

	/* And now write the character(s) */
	writel(tx_data, IPQ_UART_DM_TF(base, 0));
}

/*
 * msm_boot_uart_dm_reset - resets UART controller
 * @base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_reset(void *base)
{
	writel(IPQ_UART_DM_CMD_RESET_RX, IPQ_UART_DM_CR(base));
	writel(IPQ_UART_DM_CMD_RESET_TX, IPQ_UART_DM_CR(base));
	writel(IPQ_UART_DM_CMD_RESET_ERR_STAT,
		 IPQ_UART_DM_CR(base));
	writel(IPQ_UART_DM_CMD_RES_TX_ERR, IPQ_UART_DM_CR(base));
	writel(IPQ_UART_DM_CMD_RES_STALE_INT, IPQ_UART_DM_CR(base));

	return IPQ_UART_DM_E_SUCCESS;
}

/*
 * msm_boot_uart_dm_init - Initializes UART controller
 * @uart_dm_base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_init(void  *uart_dm_base)
{
	/* Configure UART mode registers MR1 and MR2 */
	/* Hardware flow control isn't supported */
	writel(0x0, IPQ_UART_DM_MR1(uart_dm_base));

	/* 8-N-1 configuration: 8 data bits - No parity - 1 stop bit */
	writel(IPQ_UART_DM_8_N_1_MODE,
		 IPQ_UART_DM_MR2(uart_dm_base));

	/* Configure Interrupt Mask register IMR */
	writel(IPQ_UART_DM_IMR_ENABLED,
		 IPQ_UART_DM_IMR(uart_dm_base));

	/*
	 * Configure Tx and Rx watermarks configuration registers
	 * TX watermark value is set to 0 - interrupt is generated when
	 * FIFO level is less than or equal to 0
	 */
	writel(IPQ_UART_DM_TFW_VALUE,
		 IPQ_UART_DM_TFWR(uart_dm_base));

	/* RX watermark value */
	writel(IPQ_UART_DM_RFW_VALUE,
		 IPQ_UART_DM_RFWR(uart_dm_base));

	/* Configure Interrupt Programming Register */
	/* Set initial Stale timeout value */
	writel(IPQ_UART_DM_STALE_TIMEOUT_LSB,
		IPQ_UART_DM_IPR(uart_dm_base));

	/* Configure IRDA if required */
	/* Disabling IRDA mode */
	writel(0x0, IPQ_UART_DM_IRDA(uart_dm_base));

	/* Configure hunt character value in HCR register */
	/* Keep it in reset state */
	writel(0x0, IPQ_UART_DM_HCR(uart_dm_base));

	/*
	 * Configure Rx FIFO base address
	 * Both TX/RX shares same SRAM and default is half-n-half.
	 * Sticking with default value now.
	 * As such RAM size is (2^RAM_ADDR_WIDTH, 32-bit entries).
	 * We have found RAM_ADDR_WIDTH = 0x7f
	 */

	/* Issue soft reset command */
	msm_boot_uart_dm_reset(uart_dm_base);

	/* Enable/Disable Rx/Tx DM interfaces */
	/* Data Mover not currently utilized. */
	writel(0, IPQ_UART_DM_DMEN(uart_dm_base));

	/* Enable transmitter */
	writel(IPQ_UART_DM_CR_TX_ENABLE,
		IPQ_UART_DM_CR(uart_dm_base));

	/* Initialize Receive Path */
	msm_boot_uart_dm_init_rx_transfer(uart_dm_base);

	return 0;
}

/**
 * serial_havechar - checks if data available for reading
 *
 * Returns 1 if data available, 0 otherwise
 */
int serial_havechar(void)
{
	/* Return if data is already read */
	if (uart_ready_data_count)
		return 1;

	/* Read data from the FIFO */
	if (msm_boot_uart_dm_read() != IPQ_UART_DM_E_SUCCESS)
		return 0;

	return 1;
}

/**
 * ipq806x_serial_getc - reads a character
 *
 * Returns the character read from serial port.
 */
int serial_getchar(void)
{
	uint8_t byte;

	while (!serial_havechar()) {
		/* wait for incoming data */
	}

	byte = (uint8_t)(uart_rx_fifo_word & 0xff);
	uart_rx_fifo_word = uart_rx_fifo_word >> 8;
	uart_ready_data_count--;

	return byte;
}

/* For simplicity's sake, let's rely on coreboot initializing the UART. */
void serial_console_init(void)
{
	struct cb_serial *sc_ptr = phys_to_virt(lib_sysinfo.cb_serial);

	if (!lib_sysinfo.cb_serial)
		return;

	base_uart_addr = (void *) sc_ptr->baseaddr;

	consin.havekey = serial_havechar;
	consin.getchar = serial_getchar;
	consin.input_type = CONSOLE_INPUT_TYPE_UART;

	consout.putchar = serial_putchar;

	console_add_output_driver(&consout);
	console_add_input_driver(&consin);
}
