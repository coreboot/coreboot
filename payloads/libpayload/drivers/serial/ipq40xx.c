/*
 * Copyright (c) 2010-2012, 2014, 2016, The Linux Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <libpayload.h>

#define UART_DM_CLK_RX_TX_BIT_RATE 0xFF

enum MSM_BOOT_UART_DM_PARITY_MODE {
	MSM_BOOT_UART_DM_NO_PARITY,
	MSM_BOOT_UART_DM_ODD_PARITY,
	MSM_BOOT_UART_DM_EVEN_PARITY,
	MSM_BOOT_UART_DM_SPACE_PARITY
};

/* UART Stop Bit Length */
enum MSM_BOOT_UART_DM_STOP_BIT_LEN {
	MSM_BOOT_UART_DM_SBL_9_16,
	MSM_BOOT_UART_DM_SBL_1,
	MSM_BOOT_UART_DM_SBL_1_9_16,
	MSM_BOOT_UART_DM_SBL_2
};

/* UART Bits per Char */
enum MSM_BOOT_UART_DM_BITS_PER_CHAR {
	MSM_BOOT_UART_DM_5_BPS,
	MSM_BOOT_UART_DM_6_BPS,
	MSM_BOOT_UART_DM_7_BPS,
	MSM_BOOT_UART_DM_8_BPS
};

/* 8-N-1 Configuration */
#define MSM_BOOT_UART_DM_8_N_1_MODE	(MSM_BOOT_UART_DM_NO_PARITY | \
					 (MSM_BOOT_UART_DM_SBL_1 << 2) | \
					 (MSM_BOOT_UART_DM_8_BPS << 4))

/* UART_DM Registers */

/* UART Operational Mode Register */
#define MSM_BOOT_UART_DM_MR1(base)		((base) + 0x00)
#define MSM_BOOT_UART_DM_MR2(base)		((base) + 0x04)
#define MSM_BOOT_UART_DM_RXBRK_ZERO_CHAR_OFF	(1 << 8)
#define MSM_BOOT_UART_DM_LOOPBACK		(1 << 7)

#define PERIPH_BLK_BLSP			1

/* UART Clock Selection Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_CSR(base)		((base) + 0xA0)
#else
#define MSM_BOOT_UART_DM_CSR(base)		((base) + 0x08)
#endif

/* UART DM TX FIFO Registers - 4 */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_TF(base, x)		((base) + 0x100+(4*(x)))
#else
#define MSM_BOOT_UART_DM_TF(base, x)		((base) + 0x70+(4*(x)))
#endif

/* UART Command Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_CR(base)		((base) + 0xA8)
#else
#define MSM_BOOT_UART_DM_CR(base)		((base) + 0x10)
#endif
#define MSM_BOOT_UART_DM_CR_RX_ENABLE		(1 << 0)
#define MSM_BOOT_UART_DM_CR_RX_DISABLE		(1 << 1)
#define MSM_BOOT_UART_DM_CR_TX_ENABLE		(1 << 2)
#define MSM_BOOT_UART_DM_CR_TX_DISABLE		(1 << 3)

/* UART Channel Command */
#define MSM_BOOT_UART_DM_CR_CH_CMD_LSB(x)	((x & 0x0f) << 4)
#define MSM_BOOT_UART_DM_CR_CH_CMD_MSB(x)	((x >> 4) << 11)
#define MSM_BOOT_UART_DM_CR_CH_CMD(x)		\
	(MSM_BOOT_UART_DM_CR_CH_CMD_LSB(x) | MSM_BOOT_UART_DM_CR_CH_CMD_MSB(x))
#define MSM_BOOT_UART_DM_CMD_NULL		MSM_BOOT_UART_DM_CR_CH_CMD(0)
#define MSM_BOOT_UART_DM_CMD_RESET_RX		MSM_BOOT_UART_DM_CR_CH_CMD(1)
#define MSM_BOOT_UART_DM_CMD_RESET_TX		MSM_BOOT_UART_DM_CR_CH_CMD(2)
#define MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT	MSM_BOOT_UART_DM_CR_CH_CMD(3)
#define MSM_BOOT_UART_DM_CMD_RES_BRK_CHG_INT	MSM_BOOT_UART_DM_CR_CH_CMD(4)
#define MSM_BOOT_UART_DM_CMD_START_BRK		MSM_BOOT_UART_DM_CR_CH_CMD(5)
#define MSM_BOOT_UART_DM_CMD_STOP_BRK		MSM_BOOT_UART_DM_CR_CH_CMD(6)
#define MSM_BOOT_UART_DM_CMD_RES_CTS_N		MSM_BOOT_UART_DM_CR_CH_CMD(7)
#define MSM_BOOT_UART_DM_CMD_RES_STALE_INT	MSM_BOOT_UART_DM_CR_CH_CMD(8)
#define MSM_BOOT_UART_DM_CMD_PACKET_MODE	MSM_BOOT_UART_DM_CR_CH_CMD(9)
#define MSM_BOOT_UART_DM_CMD_MODE_RESET		MSM_BOOT_UART_DM_CR_CH_CMD(C)
#define MSM_BOOT_UART_DM_CMD_SET_RFR_N		MSM_BOOT_UART_DM_CR_CH_CMD(D)
#define MSM_BOOT_UART_DM_CMD_RES_RFR_N		MSM_BOOT_UART_DM_CR_CH_CMD(E)
#define MSM_BOOT_UART_DM_CMD_RES_TX_ERR		MSM_BOOT_UART_DM_CR_CH_CMD(10)
#define MSM_BOOT_UART_DM_CMD_CLR_TX_DONE	MSM_BOOT_UART_DM_CR_CH_CMD(11)
#define MSM_BOOT_UART_DM_CMD_RES_BRKSTRT_INT	MSM_BOOT_UART_DM_CR_CH_CMD(12)
#define MSM_BOOT_UART_DM_CMD_RES_BRKEND_INT	MSM_BOOT_UART_DM_CR_CH_CMD(13)
#define MSM_BOOT_UART_DM_CMD_RES_PER_FRM_INT	MSM_BOOT_UART_DM_CR_CH_CMD(14)

/*UART General Command */
#define MSM_UART_DM_CR_GENERAL_CMD(x)		((x) << 8)

#define MSM_BOOT_UART_DM_GCMD_NULL		MSM_UART_DM_CR_GENERAL_CMD(0)
#define MSM_BOOT_UART_DM_GCMD_CR_PROT_EN	MSM_UART_DM_CR_GENERAL_CMD(1)
#define MSM_BOOT_UART_DM_GCMD_CR_PROT_DIS	MSM_UART_DM_CR_GENERAL_CMD(2)
#define MSM_BOOT_UART_DM_GCMD_RES_TX_RDY_INT	MSM_UART_DM_CR_GENERAL_CMD(3)
#define MSM_BOOT_UART_DM_GCMD_SW_FORCE_STALE	MSM_UART_DM_CR_GENERAL_CMD(4)
#define MSM_BOOT_UART_DM_GCMD_ENA_STALE_EVT	MSM_UART_DM_CR_GENERAL_CMD(5)
#define MSM_BOOT_UART_DM_GCMD_DIS_STALE_EVT	MSM_UART_DM_CR_GENERAL_CMD(6)

/* UART Interrupt Mask Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_IMR(base)		((base) + 0xB0)
#else
#define MSM_BOOT_UART_DM_IMR(base)		((base) + 0x14)
#endif

#define MSM_BOOT_UART_DM_TXLEV			(1 << 0)
#define MSM_BOOT_UART_DM_RXHUNT			(1 << 1)
#define MSM_BOOT_UART_DM_RXBRK_CHNG		(1 << 2)
#define MSM_BOOT_UART_DM_RXSTALE		(1 << 3)
#define MSM_BOOT_UART_DM_RXLEV			(1 << 4)
#define MSM_BOOT_UART_DM_DELTA_CTS		(1 << 5)
#define MSM_BOOT_UART_DM_CURRENT_CTS		(1 << 6)
#define MSM_BOOT_UART_DM_TX_READY		(1 << 7)
#define MSM_BOOT_UART_DM_TX_ERROR		(1 << 8)
#define MSM_BOOT_UART_DM_TX_DONE		(1 << 9)
#define MSM_BOOT_UART_DM_RXBREAK_START		(1 << 10)
#define MSM_BOOT_UART_DM_RXBREAK_END		(1 << 11)
#define MSM_BOOT_UART_DM_PAR_FRAME_ERR_IRQ	(1 << 12)

#define MSM_BOOT_UART_DM_IMR_ENABLED	(MSM_BOOT_UART_DM_TX_READY |	\
					 MSM_BOOT_UART_DM_TXLEV |	\
					 MSM_BOOT_UART_DM_RXSTALE)

/* UART Interrupt Programming Register */
#define MSM_BOOT_UART_DM_IPR(base)		((base) + 0x18)
#define MSM_BOOT_UART_DM_STALE_TIMEOUT_LSB	0x0f
#define MSM_BOOT_UART_DM_STALE_TIMEOUT_MSB	0	/* Not used currently */

/* UART Transmit/Receive FIFO Watermark Register */
#define MSM_BOOT_UART_DM_TFWR(base)		((base) + 0x1C)
/* Interrupt is generated when FIFO level is less than or equal to this value */
#define MSM_BOOT_UART_DM_TFW_VALUE		0

#define MSM_BOOT_UART_DM_RFWR(base)		((base) + 0x20)
/*Interrupt generated when no of words in RX FIFO is greater than this value */
#define MSM_BOOT_UART_DM_RFW_VALUE		0

/* UART Hunt Character Register */
#define MSM_BOOT_UART_DM_HCR(base)		((base) + 0x24)

/* Used for RX transfer initialization */
#define MSM_BOOT_UART_DM_DMRX(base)		((base) + 0x34)

/* Default DMRX value - any value bigger than FIFO size would be fine */
#define MSM_BOOT_UART_DM_DMRX_DEF_VALUE		0x220

/* Register to enable IRDA function */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_IRDA(base)		((base) + 0xB8)
#else
#define MSM_BOOT_UART_DM_IRDA(base)		((base) + 0x38)
#endif

/* UART Data Mover Enable Register */
#define MSM_BOOT_UART_DM_DMEN(base)		((base) + 0x3C)

/* Number of characters for Transmission */
#define MSM_BOOT_UART_DM_NO_CHARS_FOR_TX(base) ((base) + 0x040)

/* UART RX FIFO Base Address */
#define MSM_BOOT_UART_DM_BADR(base)		((base) + 0x44)

/* UART Status Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_SR(base)		((base) + 0x0A4)
#else
#define MSM_BOOT_UART_DM_SR(base)		((base) + 0x008)
#endif
#define MSM_BOOT_UART_DM_SR_RXRDY		(1 << 0)
#define MSM_BOOT_UART_DM_SR_RXFULL		(1 << 1)
#define MSM_BOOT_UART_DM_SR_TXRDY		(1 << 2)
#define MSM_BOOT_UART_DM_SR_TXEMT		(1 << 3)
#define MSM_BOOT_UART_DM_SR_UART_OVERRUN	(1 << 4)
#define MSM_BOOT_UART_DM_SR_PAR_FRAME_ERR	(1 << 5)
#define MSM_BOOT_UART_DM_RX_BREAK		(1 << 6)
#define MSM_BOOT_UART_DM_HUNT_CHAR		(1 << 7)
#define MSM_BOOT_UART_DM_RX_BRK_START_LAST	(1 << 8)

/* UART Receive FIFO Registers - 4 in numbers */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_RF(base, x)		((base) + 0x140 + (4*(x)))
#else
#define MSM_BOOT_UART_DM_RF(base, x)		((base) + 0x70 + (4*(x)))
#endif

/* UART Masked Interrupt Status Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_MISR(base)		((base) + 0xAC)
#else
#define MSM_BOOT_UART_DM_MISR(base)		((base) + 0x10)
#endif

/* UART Interrupt Status Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_ISR(base)		((base) + 0xB4)
#else
#define MSM_BOOT_UART_DM_ISR(base)		((base) + 0x14)
#endif

/* Number of characters received since the end of last RX transfer */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_RX_TOTAL_SNAP(base)  ((base) + 0xBC)
#else
#define MSM_BOOT_UART_DM_RX_TOTAL_SNAP(base)  ((base) + 0x38)
#endif

/* UART TX FIFO Status Register */
#define MSM_BOOT_UART_DM_TXFS(base)		((base) + 0x4C)
#define MSM_BOOT_UART_DM_TXFS_STATE_LSB(x)	\
		MSM_BOOT_UART_DM_EXTR_BITS(x, 0, 6)
#define MSM_BOOT_UART_DM_TXFS_STATE_MSB(x)	\
		MSM_BOOT_UART_DM_EXTR_BITS(x, 14, 31)
#define MSM_BOOT_UART_DM_TXFS_BUF_STATE(x)	\
		MSM_BOOT_UART_DM_EXTR_BITS(x, 7, 9)
#define MSM_BOOT_UART_DM_TXFS_ASYNC_STATE(x)	\
		MSM_BOOT_UART_DM_EXTR_BITS(x, 10, 13)

/* UART RX FIFO Status Register */
#define MSM_BOOT_UART_DM_RXFS(base)		((base) + 0x50)
#define MSM_BOOT_UART_DM_RXFS_STATE_LSB(x)	\
		MSM_BOOT_UART_DM_EXTR_BITS(x, 0, 6)
#define MSM_BOOT_UART_DM_RXFS_STATE_MSB(x)	\
		MSM_BOOT_UART_DM_EXTR_BITS(x, 14, 31)
#define MSM_BOOT_UART_DM_RXFS_BUF_STATE(x)	\
		MSM_BOOT_UART_DM_EXTR_BITS(x, 7, 9)
#define MSM_BOOT_UART_DM_RXFS_ASYNC_STATE(x)	\
		MSM_BOOT_UART_DM_EXTR_BITS(x, 10, 13)

/* Macros for Common Errors */
#define MSM_BOOT_UART_DM_E_SUCCESS		0
#define MSM_BOOT_UART_DM_E_FAILURE		1
#define MSM_BOOT_UART_DM_E_TIMEOUT		2
#define MSM_BOOT_UART_DM_E_INVAL		3
#define MSM_BOOT_UART_DM_E_MALLOC_FAIL		4
#define MSM_BOOT_UART_DM_E_RX_NOT_READY		5

#define UART1_DM_BASE				((void *)0x078af000)
#define UART2_DM_BASE				((void *)0x078b0000)

enum {
	BLSP1_UART1,
	BLSP1_UART2,
};

#define FIFO_DATA_SIZE	4

typedef struct {
	void *uart_dm_base;
	unsigned blsp_uart;
} uart_params_t;

/*
 * All constants lifted from u-boot's
 * board/qcom/ipq40xx_cdp/ipq40xx_board_param.h
 */
static const uart_params_t uart_board_param = {
	.uart_dm_base = UART1_DM_BASE,
	.blsp_uart = BLSP1_UART1,
};

#define write32(addr, val)		writel(val, addr)
#define read32(addr)			readl(addr)
/**
 * msm_boot_uart_dm_init_rx_transfer - Init Rx transfer
 * @uart_dm_base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_init_rx_transfer(void *uart_dm_base)
{
	/* Reset receiver */
	write32(MSM_BOOT_UART_DM_CR(uart_dm_base),
		MSM_BOOT_UART_DM_CMD_RESET_RX);

	/* Enable receiver */
	write32(MSM_BOOT_UART_DM_CR(uart_dm_base),
		MSM_BOOT_UART_DM_CR_RX_ENABLE);
	write32(MSM_BOOT_UART_DM_DMRX(uart_dm_base),
		MSM_BOOT_UART_DM_DMRX_DEF_VALUE);

	/* Clear stale event */
	write32(MSM_BOOT_UART_DM_CR(uart_dm_base),
		MSM_BOOT_UART_DM_CMD_RES_STALE_INT);

	/* Enable stale event */
	write32(MSM_BOOT_UART_DM_CR(uart_dm_base),
		MSM_BOOT_UART_DM_GCMD_ENA_STALE_EVT);

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

static unsigned int msm_boot_uart_dm_init(void  *uart_dm_base);

/* Received data is valid or not */
static int valid_data = 0;

/* Received data */
static unsigned int word = 0;

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
	void *base;
	uint32_t status_reg;

	base = uart_board_param.uart_dm_base;

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
	if (*data == 0)
		return MSM_BOOT_UART_DM_E_RX_NOT_READY;

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

void serial_putchar(unsigned data)
{
	int num_of_chars = 1;
	void *base = uart_board_param.uart_dm_base;

	if (data == '\n') {
		num_of_chars++;
		data = (data << 8) | '\r';
	}

	/* Wait until transmit FIFO is empty. */
	while (!(read32(MSM_BOOT_UART_DM_SR(base)) &
		 MSM_BOOT_UART_DM_SR_TXEMT))
		udelay(1);
	/*
	 * TX FIFO is ready to accept new character(s). First write number of
	 * characters to be transmitted.
	 */
	write32(MSM_BOOT_UART_DM_NO_CHARS_FOR_TX(base), num_of_chars);

	/* And now write the character(s) */
	write32(MSM_BOOT_UART_DM_TF(base, 0), data);
}

/*
 * msm_boot_uart_dm_reset - resets UART controller
 * @base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_reset(void *base)
{
	write32(MSM_BOOT_UART_DM_CR(base), MSM_BOOT_UART_DM_CMD_RESET_RX);
	write32(MSM_BOOT_UART_DM_CR(base), MSM_BOOT_UART_DM_CMD_RESET_TX);
	write32(MSM_BOOT_UART_DM_CR(base),
		MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT);
	write32(MSM_BOOT_UART_DM_CR(base), MSM_BOOT_UART_DM_CMD_RES_TX_ERR);
	write32(MSM_BOOT_UART_DM_CR(base), MSM_BOOT_UART_DM_CMD_RES_STALE_INT);

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

/*
 * msm_boot_uart_dm_init - initilaizes UART controller
 * @uart_dm_base: UART controller base address
 */
unsigned int msm_boot_uart_dm_init(void  *uart_dm_base)
{
	/* Configure UART mode registers MR1 and MR2 */
	/* Hardware flow control isn't supported */
	write32(MSM_BOOT_UART_DM_MR1(uart_dm_base), 0x0);

	/* 8-N-1 configuration: 8 data bits - No parity - 1 stop bit */
	write32(MSM_BOOT_UART_DM_MR2(uart_dm_base),
		MSM_BOOT_UART_DM_8_N_1_MODE);

	/* Configure Interrupt Mask register IMR */
	write32(MSM_BOOT_UART_DM_IMR(uart_dm_base),
		MSM_BOOT_UART_DM_IMR_ENABLED);

	/*
	 * Configure Tx and Rx watermarks configuration registers
	 * TX watermark value is set to 0 - interrupt is generated when
	 * FIFO level is less than or equal to 0
	 */
	write32(MSM_BOOT_UART_DM_TFWR(uart_dm_base),
		MSM_BOOT_UART_DM_TFW_VALUE);

	/* RX watermark value */
	write32(MSM_BOOT_UART_DM_RFWR(uart_dm_base),
		MSM_BOOT_UART_DM_RFW_VALUE);

	/* Configure Interrupt Programming Register */
	/* Set initial Stale timeout value */
	write32(MSM_BOOT_UART_DM_IPR(uart_dm_base),
		MSM_BOOT_UART_DM_STALE_TIMEOUT_LSB);

	/* Configure IRDA if required */
	/* Disabling IRDA mode */
	write32(MSM_BOOT_UART_DM_IRDA(uart_dm_base), 0x0);

	/* Configure hunt character value in HCR register */
	/* Keep it in reset state */
	write32(MSM_BOOT_UART_DM_HCR(uart_dm_base), 0x0);

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
	write32(MSM_BOOT_UART_DM_DMEN(uart_dm_base), 0x0);

	/* Enable transmitter */
	write32(MSM_BOOT_UART_DM_CR(uart_dm_base),
		MSM_BOOT_UART_DM_CR_TX_ENABLE);

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
	if (valid_data)
		return 1;

	/* Read data from the FIFO */
	if (msm_boot_uart_dm_read(&word, &valid_data, 0) !=
	    MSM_BOOT_UART_DM_E_SUCCESS)
		return 0;

	return 1;
}

/**
 * ipq40xx_serial_getc - reads a character
 *
 * Returns the character read from serial port.
 */
int serial_getchar(void)
{
	uint8_t byte;

	while (!serial_havechar())
		;	/* wait for incoming data */

	byte = (uint8_t)(word & 0xff);
	word = word >> 8;
	valid_data--;

	return byte;
}

static struct console_input_driver consin = {};
static struct console_output_driver consout = {};

/* For simplicity sake let's rely on coreboot initalizing the UART. */
void serial_console_init(void)
{
	struct cb_serial *sc_ptr = lib_sysinfo.serial;

	if (!sc_ptr)
		return;

	consin.havekey = serial_havechar;
	consin.getchar = serial_getchar;

	consout.putchar = serial_putchar;

	console_add_output_driver(&consout);
	console_add_input_driver(&consin);
}
