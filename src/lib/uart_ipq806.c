/*
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
 * Source : APQ8064 LK boot
 *
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <common.h>
#include <asm/arch-ipq806x/gsbi.h>
#include <asm/arch-ipq806x/clock.h>
#include <asm/arch-ipq806x/uart.h>
#include <serial.h>

#define FIFO_DATA_SIZE	4

extern board_ipq806x_params_t *gboard_param;

static unsigned int msm_boot_uart_dm_init(unsigned int  uart_dm_base);

/* Received data is valid or not */
static int valid_data = 0;

/* Received data */
static unsigned int word = 0;

/**
 * msm_boot_uart_dm_init_rx_transfer - Init Rx transfer
 * @uart_dm_base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_init_rx_transfer(unsigned int uart_dm_base)
{
	/* Reset receiver */
	writel(MSM_BOOT_UART_DM_CMD_RESET_RX,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	/* Enable receiver */
	writel(MSM_BOOT_UART_DM_CR_RX_ENABLE,
		MSM_BOOT_UART_DM_CR(uart_dm_base));
	writel(MSM_BOOT_UART_DM_DMRX_DEF_VALUE,
		MSM_BOOT_UART_DM_DMRX(uart_dm_base));

	/* Clear stale event */
	writel(MSM_BOOT_UART_DM_CMD_RES_STALE_INT,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	/* Enable stale event */
	writel(MSM_BOOT_UART_DM_GCMD_ENA_STALE_EVT,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

/**
 * msm_boot_uart_dm_read - reads a word from the RX FIFO.
 * @data: location where the read data is stored
 * @count: no of valid data in the FIFO
 * @wait: indicates blocking call or not blocking call
 *
 * Reads a word from the RX FIFO. If no data is available blocks if
 * @wait is true, else returns %MSM_BOOT_UART_DM_E_RX_NOT_READY.
 */
static unsigned int
msm_boot_uart_dm_read(unsigned int *data, int *count, int wait)
{
	static int total_rx_data = 0;
	static int rx_data_read = 0;
	unsigned int  base = 0;
	uint32_t status_reg;

	base = gboard_param->uart_dm_base;

	if (data == NULL)
		return MSM_BOOT_UART_DM_E_INVAL;

	status_reg = readl(MSM_BOOT_UART_DM_MISR(base));

	/* Check for DM_RXSTALE for RX transfer to finish */
	while (!(status_reg & MSM_BOOT_UART_DM_RXSTALE)) {
		status_reg = readl(MSM_BOOT_UART_DM_MISR(base));
		if (!wait)
			return MSM_BOOT_UART_DM_E_RX_NOT_READY;
	}

	/* Check for Overrun error. We'll just reset Error Status */
	if (readl(MSM_BOOT_UART_DM_SR(base)) &
			MSM_BOOT_UART_DM_SR_UART_OVERRUN) {
		writel(MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT,
			MSM_BOOT_UART_DM_CR(base));
		total_rx_data = rx_data_read = 0;
		msm_boot_uart_dm_init(base);
		return MSM_BOOT_UART_DM_E_RX_NOT_READY;
	}

	/* Read UART_DM_RX_TOTAL_SNAP for actual number of bytes received */
	if (total_rx_data == 0)
		total_rx_data =  readl(MSM_BOOT_UART_DM_RX_TOTAL_SNAP(base));

	/* Data available in FIFO; read a word. */
	*data = readl(MSM_BOOT_UART_DM_RF(base, 0));

	/* WAR for http://prism/CR/548280 */
	if (*data == 0) {
		return MSM_BOOT_UART_DM_E_RX_NOT_READY;
	}

	/* increment the total count of chars we've read so far */
	rx_data_read += FIFO_DATA_SIZE;

	/* actual count of valid data in word */
	*count = ((total_rx_data < rx_data_read) ?
			(FIFO_DATA_SIZE - (rx_data_read - total_rx_data)) :
			FIFO_DATA_SIZE);

	/* If there are still data left in FIFO we'll read them before
	 * initializing RX Transfer again
	 */
	if (rx_data_read < total_rx_data)
		return MSM_BOOT_UART_DM_E_SUCCESS;

	msm_boot_uart_dm_init_rx_transfer(base);
	total_rx_data = rx_data_read = 0;

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

/**
 * msm_boot_uart_replace_lr_with_cr - replaces "\n" with "\r\n"
 * @data_in:      characters to be converted
 * @num_of_chars: no. of characters
 * @data_out:     location where converted chars are stored
 *
 * Replace linefeed char "\n" with carriage return + linefeed
 * "\r\n". Currently keeping it simple than efficient.
 */
static unsigned int
msm_boot_uart_replace_lr_with_cr(char *data_in,
                                 int num_of_chars,
                                 char *data_out, int *num_of_chars_out)
{
        int i = 0, j = 0;

        if ((data_in == NULL) || (data_out == NULL) || (num_of_chars < 0))
                return MSM_BOOT_UART_DM_E_INVAL;

        for (i = 0, j = 0; i < num_of_chars; i++, j++) {
                if (data_in[i] == '\n')
                        data_out[j++] = '\r';

                data_out[j] = data_in[i];
        }

        *num_of_chars_out = j;

        return MSM_BOOT_UART_DM_E_SUCCESS;
}

/**
 * msm_boot_uart_dm_write - transmit data
 * @data:          data to transmit
 * @num_of_chars:  no. of bytes to transmit
 *
 * Writes the data to the TX FIFO. If no space is available blocks
 * till space becomes available.
 */
static unsigned int
msm_boot_uart_dm_write(char *data, unsigned int num_of_chars)
{
	unsigned int tx_word_count = 0;
	unsigned int tx_char_left = 0, tx_char = 0;
	unsigned int tx_word = 0;
	int i = 0;
	char *tx_data = NULL;
	char new_data[1024];
	unsigned int base = gboard_param->uart_dm_base;

        if ((data == NULL) || (num_of_chars <= 0))
                return MSM_BOOT_UART_DM_E_INVAL;

        /* Replace line-feed (/n) with carriage-return + line-feed (/r/n) */
        msm_boot_uart_replace_lr_with_cr(data, num_of_chars, new_data, &i);

        tx_data = new_data;
        num_of_chars = i;

        /* Write to NO_CHARS_FOR_TX register number of characters
        * to be transmitted. However, before writing TX_FIFO must
        * be empty as indicated by TX_READY interrupt in IMR register
        */
        /* Check if transmit FIFO is empty.
        * If not we'll wait for TX_READY interrupt. */

        if (!(readl(MSM_BOOT_UART_DM_SR(base)) & MSM_BOOT_UART_DM_SR_TXEMT)) {
                while (!(readl(MSM_BOOT_UART_DM_ISR(base)) & MSM_BOOT_UART_DM_TX_READY))
                        __udelay(1);
        }

        /* We are here. FIFO is ready to be written. */
        /* Write number of characters to be written */
        writel(num_of_chars, MSM_BOOT_UART_DM_NO_CHARS_FOR_TX(base));

        /* Clear TX_READY interrupt */
        writel(MSM_BOOT_UART_DM_GCMD_RES_TX_RDY_INT, MSM_BOOT_UART_DM_CR(base));

        /* We use four-character word FIFO. So we need to divide data into
        * four characters and write in UART_DM_TF register */
        tx_word_count = (num_of_chars % 4) ? ((num_of_chars / 4) + 1) :
                        (num_of_chars / 4);
        tx_char_left = num_of_chars;

        for (i = 0; i < (int)tx_word_count; i++) {
                tx_char = (tx_char_left < 4) ? tx_char_left : 4;
                PACK_CHARS_INTO_WORDS(tx_data, tx_char, tx_word);

                /* Wait till TX FIFO has space */
                while (!(readl(MSM_BOOT_UART_DM_SR(base)) & MSM_BOOT_UART_DM_SR_TXRDY))
                        __udelay(1);

                /* TX FIFO has space. Write the chars */
                writel(tx_word, MSM_BOOT_UART_DM_TF(base, 0));
                tx_char_left = num_of_chars - (i + 1) * 4;
                tx_data = tx_data + 4;
        }

        return MSM_BOOT_UART_DM_E_SUCCESS;
}

/*
 * msm_boot_uart_dm_reset - resets UART controller
 * @base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_reset(unsigned int base)
{
	writel(MSM_BOOT_UART_DM_CMD_RESET_RX, MSM_BOOT_UART_DM_CR(base));
	writel(MSM_BOOT_UART_DM_CMD_RESET_TX, MSM_BOOT_UART_DM_CR(base));
	writel(MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT, MSM_BOOT_UART_DM_CR(base));
	writel(MSM_BOOT_UART_DM_CMD_RES_TX_ERR, MSM_BOOT_UART_DM_CR(base));
	writel(MSM_BOOT_UART_DM_CMD_RES_STALE_INT, MSM_BOOT_UART_DM_CR(base));

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

/*
 * msm_boot_uart_dm_init - initilaizes UART controller
 * @uart_dm_base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_init(unsigned int  uart_dm_base)
{
	/* Configure UART mode registers MR1 and MR2 */
	/* Hardware flow control isn't supported */
	writel(0x0, MSM_BOOT_UART_DM_MR1(uart_dm_base));

	/* 8-N-1 configuration: 8 data bits - No parity - 1 stop bit */
	writel(MSM_BOOT_UART_DM_8_N_1_MODE, MSM_BOOT_UART_DM_MR2(uart_dm_base));

	/* Configure Interrupt Mask register IMR */
	writel(MSM_BOOT_UART_DM_IMR_ENABLED, MSM_BOOT_UART_DM_IMR(uart_dm_base));

	/*
	 * Configure Tx and Rx watermarks configuration registers
	 * TX watermark value is set to 0 - interrupt is generated when
	 * FIFO level is less than or equal to 0
	 */
	writel(MSM_BOOT_UART_DM_TFW_VALUE, MSM_BOOT_UART_DM_TFWR(uart_dm_base));

	/* RX watermark value */
	writel(MSM_BOOT_UART_DM_RFW_VALUE, MSM_BOOT_UART_DM_RFWR(uart_dm_base));

	/* Configure Interrupt Programming Register */
	/* Set initial Stale timeout value */
	writel(MSM_BOOT_UART_DM_STALE_TIMEOUT_LSB,
		MSM_BOOT_UART_DM_IPR(uart_dm_base));

	/* Configure IRDA if required */
	/* Disabling IRDA mode */
	writel(0x0, MSM_BOOT_UART_DM_IRDA(uart_dm_base));

	/* Configure hunt character value in HCR register */
	/* Keep it in reset state */
	writel(0x0, MSM_BOOT_UART_DM_HCR(uart_dm_base));

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
	writel(0x0, MSM_BOOT_UART_DM_DMEN(uart_dm_base));

	/* Enable transmitter */
	writel(MSM_BOOT_UART_DM_CR_TX_ENABLE,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	/* Initialize Receive Path */
	msm_boot_uart_dm_init_rx_transfer(uart_dm_base);

	return 0;
}

/**
 * uart_dm_init - initializes UART
 *
 * Initializes clocks, GPIO and UART controller.
 */
static int uart_dm_init(void)
{
	unsigned int dm_base, gsbi_base;

	dm_base = gboard_param->uart_dm_base;
	gsbi_base = gboard_param->uart_gsbi_base;
	ipq_configure_gpio(gboard_param->dbg_uart_gpio, NO_OF_DBG_UART_GPIOS);

	/* Configure the uart clock */
        uart_clock_config(gboard_param->uart_gsbi,
		gboard_param->mnd_value.m_value,
		gboard_param->mnd_value.n_value,
		gboard_param->mnd_value.d_value,
		gboard_param->clk_dummy);

	writel(GSBI_PROTOCOL_CODE_I2C_UART <<
		GSBI_CTRL_REG_PROTOCOL_CODE_S,
		GSBI_CTRL_REG(gsbi_base));
        writel(UART_DM_CLK_RX_TX_BIT_RATE, MSM_BOOT_UART_DM_CSR(dm_base));
	/* Intialize UART_DM */
	msm_boot_uart_dm_init(dm_base);

	return 0;
}

/**
 * ipq806x_serial_putc - transmits a character
 * @c: character to transmit
 */
static void ipq806x_serial_putc(char c)
{
        msm_boot_uart_dm_write(&c, 1);
}

/**
 * ipq806x_serial_puts - transmits a string of data
 * @s: string to transmit
 */
static void ipq806x_serial_puts(const char *s)
{
        while (*s != '\0')
                serial_putc(*s++);
}

/**
 * ipq806x_serial_tstc - checks if data available for reading
 *
 * Returns 1 if data available, 0 otherwise
 */
static int ipq806x_serial_tstc(void)
{
	/* Return if data is already read */
	if (valid_data)
		return 1;

	/* Read data from the FIFO */
	if (msm_boot_uart_dm_read(&word, &valid_data, 0) != MSM_BOOT_UART_DM_E_SUCCESS)
		return 0;

	return 1;
}

/**
 * ipq806x_serial_getc - reads a character
 *
 * Returns the character read from serial port.
 */
static int ipq806x_serial_getc(void)
{
	int byte;

	while (!serial_tstc()) {
		/* wait for incoming data */
	}

	byte = (int)word & 0xff;
	word = word >> 8;
	valid_data--;

	return byte;
}

static struct serial_device ipq_serial_device = {
	.name = "ipq_serial",
	.start = uart_dm_init,
	.getc = ipq806x_serial_getc,
	.tstc = ipq806x_serial_tstc,
	.putc = ipq806x_serial_putc,
	.puts = ipq806x_serial_puts,
};

__weak struct serial_device *default_serial_console(void)
{
	return &ipq_serial_device;
}

/**
 * ipq806x_serial_init - initializes serial controller
 */
void ipq806x_serial_initialize(void)
{
	serial_register(&ipq_serial_device);
}
