/* SPDX-License-Identifier: GPL-2.0-only */
#include <stdint.h>
#include <string.h>
#include <spi-generic.h>
#include <spi_sdcard.h>
#include <crc_byte.h>
#include <commonlib/helpers.h>
#include <console/console.h>

//#define SPI_SDCARD_DEBUG

#ifdef SPI_SDCARD_DEBUG
#define dprintk(fmt, args...) \
	printk(BIOS_DEBUG, fmt, ##args)
#else
#define dprintk(fmt, args...) \
	do {} while (0)
#endif

#define SDCARD_TYPE_SDSC	1
#define SDCARD_TYPE_SDHC	2
#define SDCARD_TYPE_SDXC	3

/* CMD */
#define GO_IDLE_STATE           0
#define SEND_OP_COND            1
#define SWITCH_FUNC             6
#define SEND_IF_COND            8
#define SEND_CSD                9
#define SEND_CID                10
#define STOP_TRANSMISSION       12
#define SEND_STATUS             13
#define SET_BLOCKLEN            16
#define READ_SINGLE_BLOCK       17
#define READ_MULTIPLEBLOCK      18
#define WRITE_BLOCK             24
#define WRITE_MULTIPLEBLOCK     25
#define PROGRAM_CSD             27
#define SET_WRITE_PROT          28
#define CLR_WRITE_PROT          29
#define SEND_WRITE_PROT         30
#define ERASE_WR_BLK_START_ADDR 32
#define ERASE_WR_BLK_END_ADDR   33
#define ERASE                   38
#define LOCK_UNLOCK             42
#define APP_CMD                 55
#define GEN_CMD                 56
#define READ_OCR                58
#define CRC_ON_OFF              59

/* ACMD */
#define SD_STATUS               13
#define SEND_NUM_WR_BLOCKS      22
#define SET_WR_BLK_ERASE_COUNT  23
#define SD_SEND_OP_COND         41
#define SET_CLR_CARD_DETECT     42
#define SEND_SCR                51

/* control tokens */
#define CT_BLOCK_START                  0xfe
#define CT_MULTIPLE_BLOCK_START         0xfc
#define CT_MULTIPLE_BLOCK_STOP          0xfd
#define CT_RESPONSE_MASK                0x1f
#define  CT_RESPONSE_ACCEPTED           0x05
#define  CT_RESPONSE_REJECTED_CRC       0x0b
#define  CT_RESPONSE_REJECTED_WRITE_ERR 0x0d

/* response type */
#define RSP_R1  0
#define RSP_R1b 1
#define RSP_R2  2
#define RSP_R3  3
#define RSP_R4  4
#define RSP_R5  5
#define RSP_R7  7

#define RSP_ERR_CARD_IS_LOCKED	(1 << 0)
#define RSP_ERR_WP_ERASE_SKIP	(1 << 1)
#define RSP_ERR_GENERAL		(1 << 2)
#define RSP_ERR_CC		(1 << 3)
#define RSP_ERR_ECC		(1 << 4)
#define RSP_ERR_WP_VIOLATION	(1 << 5)
#define RSP_ERR_ERASE_PARAM	(1 << 6)
#define RSP_ERR_OUT_OF_RANGE	(1 << 7)
#define RSP_ERR_IN_IDLE		(1 << 8)
#define RSP_ERR_ERASE_RESET	(1 << 9)
#define RSP_ERR_ILLEGAL_COMMAND	(1 << 10)
#define RSP_ERR_COM_CRC		(1 << 11)
#define RSP_ERR_ERASE_SEQUENCE	(1 << 12)
#define RSP_ERR_ADDRESS		(1 << 13)
#define RSP_ERR_PARAMETER	(1 << 14)

#define BLOCK_SIZE	512

static unsigned long long extract_bits(uint8_t *buff,
	int width, int start, int end)
{
	unsigned long long r = 0;
	for (int i = end; i >= start; i--) {
		int bitpos = width - i - 1;
		int b = bitpos / 8;
		int shift = 7 - bitpos % 8;
		r = (r << 1) | ((buff[b] >> shift) & 1);
	}
	return r;
}

static void spi_sdcard_enable_cs(const struct spi_sdcard *card)
{
	spi_claim_bus(&card->slave);
}

static void spi_sdcard_disable_cs(const struct spi_sdcard *card)
{
	spi_release_bus(&card->slave);
}

static void spi_sdcard_sendbyte(const struct spi_sdcard *card, uint8_t b)
{
	dprintk("sdcard -> %#x\n", b);
	spi_xfer(&card->slave, &b, 1, NULL, 0);
}

static uint8_t spi_sdcard_recvbyte(const struct spi_sdcard *card)
{
	uint8_t b, t = 0xff;
	spi_xfer(&card->slave, &t, 1, &b, 1);
	dprintk("sdcard <- %#x\n", b);
	return b;
}

static uint8_t spi_sdcard_calculate_command_crc(uint8_t cmd, uint32_t argument)
{
	uint8_t crc = 0;
	crc = crc7_byte(crc, (cmd | 0x40) & 0x7f);
	crc = crc7_byte(crc, (argument >> (3 * 8)) & 0xff);
	crc = crc7_byte(crc, (argument >> (2 * 8)) & 0xff);
	crc = crc7_byte(crc, (argument >> (1 * 8)) & 0xff);
	crc = crc7_byte(crc, (argument >> (0 * 8)) & 0xff);
	return crc | 1;
}

static int lookup_cmd_response_type(uint8_t cmd)
{
	switch (cmd) {
	case GO_IDLE_STATE:
	case SEND_OP_COND:
	case SWITCH_FUNC:
	case SEND_CSD:
	case SEND_CID:
	case SET_BLOCKLEN:
	case READ_SINGLE_BLOCK:
	case READ_MULTIPLEBLOCK:
	case WRITE_BLOCK:
	case WRITE_MULTIPLEBLOCK:
	case PROGRAM_CSD:
	case SEND_WRITE_PROT:
	case ERASE_WR_BLK_START_ADDR:
	case ERASE_WR_BLK_END_ADDR:
	case LOCK_UNLOCK:
	case APP_CMD:
	case GEN_CMD:
	case CRC_ON_OFF:
		return RSP_R1;
	case STOP_TRANSMISSION:
	case SET_WRITE_PROT:
	case CLR_WRITE_PROT:
	case ERASE:
		return RSP_R1b;
	case SEND_STATUS:
		return RSP_R2;
	case READ_OCR:
		return RSP_R3;
	case SEND_IF_COND:
		return RSP_R7;
	}
	return -1;
}

static int lookup_acmd_response_type(uint8_t cmd)
{
	switch (cmd) {
	case SEND_NUM_WR_BLOCKS:
	case SET_WR_BLK_ERASE_COUNT:
	case SD_SEND_OP_COND:
	case SET_CLR_CARD_DETECT:
	case SEND_SCR:
		return RSP_R1;
	case SD_STATUS:
		return RSP_R2;
	}
	return -1;
}

static int lookup_response_length(int response_type)
{
	switch (response_type) {
	case RSP_R1:
	case RSP_R1b:
		return 1;
	case RSP_R2:
		return 2;
	case RSP_R3:
	case RSP_R7:
		return 5;
	}
	return -1;
}

static int response_resolve(int response_type, uint8_t *response,
	uint32_t *out_register)
{
	__maybe_unused static const char * const sd_err[] = {
		"Card is locked",
		"wp erase skip | lock/unlok cmd failed",
		"error",
		"CC error",
		"card err failed",
		"wp violation",
		"erase param",
		"out of range | csd overwrite",
		"in idle state",
		"erase reset",
		"illegal command",
		"com crc error",
		"erase sequence error",
		"address error",
		"parameter error"
	};
	uint8_t r1 = 0, r2 = 0;

	if ((response_type == RSP_R1)
		|| (response_type == RSP_R1b)
		|| (response_type == RSP_R2)
		|| (response_type == RSP_R3)
		|| (response_type == RSP_R7))
		r1 = response[0];

	if (response_type == RSP_R2)
		r2 = response[1];

	if (((response_type == RSP_R3) || (response_type == RSP_R7))
			&& (out_register != NULL)) {
		*out_register = 0;
		*out_register = (*out_register << 8) | response[1];
		*out_register = (*out_register << 8) | response[2];
		*out_register = (*out_register << 8) | response[3];
		*out_register = (*out_register << 8) | response[4];
	}

	if (r1 != 0 || r2 != 0) {
		int i = 0;
		uint16_t r = (r1 << 8) | r2;
		while (r) {
			if (r & 1)
				dprintk("SDCARD ERROR: %s\n", sd_err[i]);
			r = r >> 1;
			i++;
		}
		return (r1 << 8) | r2;
	}

	return 0;
}

static int spi_sdcard_do_command_help(const struct spi_sdcard *card,
		int is_acmd,
		uint8_t cmd,
		uint32_t argument,
		uint32_t *out_register)
{
	int ret, type, length, wait;
	uint8_t crc, c, response[5];

	/* calculate crc for command */
	crc = spi_sdcard_calculate_command_crc(cmd, argument);

	if (is_acmd)
		dprintk("\nsdcard execute acmd%d, argument = %#x, crc = %#x\n",
				cmd, argument, crc);
	else
		dprintk("\nsdcard execute  cmd%d, argument = %#x, crc = %#x\n",
				cmd, argument, crc);

	/* lookup response type of command */
	if (!is_acmd)
		type = lookup_cmd_response_type(cmd);
	else
		type = lookup_acmd_response_type(cmd);

	/* lookup response length of command */
	length = lookup_response_length(type);

	/* enable cs  */
	spi_sdcard_enable_cs(card);

	/* just delay 8 clocks */
	spi_sdcard_recvbyte(card);

	/* send command */
	spi_sdcard_sendbyte(card, (cmd | 0x40) & 0x7f);
	/* send argument */
	spi_sdcard_sendbyte(card, (argument >> (8 * 3)) & 0xff);
	spi_sdcard_sendbyte(card, (argument >> (8 * 2)) & 0xff);
	spi_sdcard_sendbyte(card, (argument >> (8 * 1)) & 0xff);
	spi_sdcard_sendbyte(card, (argument >> (8 * 0)) & 0xff);
	/* send crc */
	spi_sdcard_sendbyte(card, crc);

	/* waitting for response */
	wait = 0xffff;
	while (((c = spi_sdcard_recvbyte(card)) & 0x80) && --wait)
		;
	if (!wait) {
		spi_sdcard_disable_cs(card);
		return -1; /* timeout */
	}

	/* obtain response */
	for (int i = 0; i < length; i++) {
		response[i] = c;
		c = spi_sdcard_recvbyte(card);
	}

	if (type == RSP_R1b) {
		/* waitting done */
		wait = 0xffffff;
		while (c == 0 && --wait)
			c = spi_sdcard_recvbyte(card);
		if (!wait) {
			spi_sdcard_disable_cs(card);
			return -1; /* timeout */
		}
	}

	spi_sdcard_disable_cs(card);

	ret = response_resolve(type, response, out_register);

	return ret;
}

static int spi_sdcard_do_command(const struct spi_sdcard *card,
		uint8_t cmd,
		uint32_t argument,
		uint32_t *out_register)
{
	return spi_sdcard_do_command_help(card, 0, cmd, argument, out_register);
}

static int spi_sdcard_do_app_command(const struct spi_sdcard *card,
		uint8_t cmd,
		uint32_t argument,
		uint32_t *out_register)
{
	/* CMD55 */
	if (spi_sdcard_do_command(card, APP_CMD, 0, NULL))
		return -1;

	return spi_sdcard_do_command_help(card, 1, cmd, argument, out_register);
}

size_t spi_sdcard_size(const struct spi_sdcard *card)
{
	int wait;
	uint8_t csd[16];
	uint16_t c = 0;

	/* CMD9, send csd (128bits register) */
	if (spi_sdcard_do_command(card, SEND_CSD, 0, NULL))
		return -1;

	/* enable CS */
	spi_sdcard_enable_cs(card);

	/* waitting start block token */
	wait = 0xffff;
	while ((spi_sdcard_recvbyte(card) != CT_BLOCK_START) && --wait)
		;
	if (!wait) {
		spi_sdcard_disable_cs(card);
		return -1;
	}

	/* receive data */
	for (int i = 0; i < 16; i++) {
		csd[i] = spi_sdcard_recvbyte(card);
		c = crc16_byte(c, csd[i]);
	}

	/* receive crc and verify check sum */
	if (((c >> 8) & 0xff) != spi_sdcard_recvbyte(card)) {
		spi_sdcard_disable_cs(card);
		return -1;
	}
	if (((c >> 0) & 0xff) != spi_sdcard_recvbyte(card)) {
		spi_sdcard_disable_cs(card);
		return -1;
	}

	/* disable cs */
	spi_sdcard_disable_cs(card);

	if (extract_bits(csd, 128, 126, 127) == 0) {
		/* csd version 1.0 */
		size_t c_size = extract_bits(csd, 128, 62, 73);
		size_t mult = extract_bits(csd, 128, 47, 49);
		size_t read_bl_len = extract_bits(csd, 128, 80, 83);
		return (c_size + 1) * mult * (1 << read_bl_len);
	}

	if (extract_bits(csd, 128, 126, 127) == 1) {
		/* csd version 2.0 */
		size_t c_size = extract_bits(csd, 128, 48, 69);
		return (c_size + 1) * 512 * 1024;
	}

	return -1;
}

int spi_sdcard_init(struct spi_sdcard *card,
		const unsigned int bus, const unsigned int cs)
{
	int resolve, wait;
	uint32_t ocr;

	/* initialize spi controller */
	spi_setup_slave(bus, cs, &card->slave);

	/* must wait at least 74 clock ticks after reset
	 * disable cs pin to enter spi mode */
	spi_sdcard_disable_cs(card);
	for (int i = 0; i < 10; i++)
		spi_sdcard_sendbyte(card, 0xff);

	/* CMD0, reset sdcard */
	wait = 0xffff;
	while ((spi_sdcard_do_command(card, GO_IDLE_STATE, 0, NULL)
			!= RSP_ERR_IN_IDLE) && --wait)
		;
	if (!wait)
		return -1; /* timeout */

	/* CMD8 */
	resolve = spi_sdcard_do_command(card, SEND_IF_COND, 0x1aa, NULL);
	if (resolve & RSP_ERR_ILLEGAL_COMMAND) {
		/* ACMD41, initialize card */
		wait = 0xffff;
		while ((resolve = spi_sdcard_do_app_command(card,
			SD_SEND_OP_COND, 0, NULL)) && --wait)
			;
		if ((resolve & RSP_ERR_ILLEGAL_COMMAND) || !wait) {
			wait = 0xffff;
			/* CMD1, initialize card for 2.1mm SD Memory Card */
			while (spi_sdcard_do_app_command(card, SEND_OP_COND,
				0, NULL) && --wait)
				;
			if (!wait)
				return -1; /* unknown card */
		}
	} else {
		/* ACMD41, initialize card */
		wait = 0xffff;
		while (spi_sdcard_do_app_command(card, SD_SEND_OP_COND,
				0x40000000, NULL) && --wait)
			;
		if (!wait)
			return -1;
	}

	/* CMD58, read ocr register */
	if (spi_sdcard_do_command(card, READ_OCR, 0, &ocr))
		return -1;

	/* CMD16, set block length to 512 bytes */
	if (spi_sdcard_do_command(card, SET_BLOCKLEN, 512, NULL))
		return -1;

	/* CCS is bit30 of ocr register
	 * CCS = 0 -> SDSC
	 * CCS = 1 -> SDHC/SDXC
	 * */
	if ((ocr & 0x40000000) == 0)
		card->type = SDCARD_TYPE_SDSC;
	else {
		/* size > 32G -> SDXC */
		if (spi_sdcard_size(card) > 32LL * 1024 * 1024 * 1024)
			card->type = SDCARD_TYPE_SDXC;
		else
			card->type = SDCARD_TYPE_SDHC;
	}

	return 0;
}

int spi_sdcard_single_read(const struct spi_sdcard *card,
		size_t block_address,
		void *buff)
{
	int wait;
	uint16_t c = 0;

	if (card->type == SDCARD_TYPE_SDSC)
		block_address = block_address * 512;

	/* CMD17, start single block read */
	if (spi_sdcard_do_command(card, READ_SINGLE_BLOCK, block_address, NULL))
		return -1;

	/* enable cs */
	spi_sdcard_enable_cs(card);

	/* waitting start block token */
	wait = 0xffff;
	while ((spi_sdcard_recvbyte(card) != CT_BLOCK_START) && --wait)
		;
	if (!wait) { /* timeout */
		spi_sdcard_disable_cs(card);
		return -1;
	}

	/* receive data */
	for (int i = 0; i < 512; i++) {
		((uint8_t *)buff)[i] = spi_sdcard_recvbyte(card);
		c = crc16_byte(c, ((uint8_t *)buff)[i]);
	}

	/* receive crc and verify check sum */
	if (((c >> 8) & 0xff) != spi_sdcard_recvbyte(card)) {
		spi_sdcard_disable_cs(card);
		return -1;
	}
	if (((c >> 0) & 0xff) != spi_sdcard_recvbyte(card)) {
		spi_sdcard_disable_cs(card);
		return -1;
	}

	/* disable cs */
	spi_sdcard_disable_cs(card);

	return 0;
}

int spi_sdcard_multiple_read(const struct spi_sdcard *card,
		size_t start_block_address,
		size_t end_block_address,
		void *buff)
{
	int wait;
	int block_num = end_block_address - start_block_address + 1;
	if (card->type == SDCARD_TYPE_SDSC) {
		start_block_address = start_block_address * 512;
		end_block_address = end_block_address * 512;
	}
	/* CMD18, start multiple block read */
	if (spi_sdcard_do_command(card,
			READ_MULTIPLEBLOCK, start_block_address, NULL))
		return -1;

	/* enable cs */
	spi_sdcard_enable_cs(card);

	for (int i = 0; i < block_num; i++) {
		uint16_t c = 0;

		/* waitting start block token */
		wait = 0xffff;
		while ((spi_sdcard_recvbyte(card) != CT_BLOCK_START) && --wait)
			;
		if (!wait) { /* timeout */
			spi_sdcard_disable_cs(card);
			return -1;
		}

		/* receive data */
		for (int k = 0; k < 512; k++) {
			uint8_t tmp = spi_sdcard_recvbyte(card);
			((uint8_t *)buff)[512 * i + k] = tmp;
			c = crc16_byte(c, tmp);
		}

		/* receive crc and verify check sum */
		if (((c >> 8) & 0xff) != spi_sdcard_recvbyte(card)) {
			spi_sdcard_disable_cs(card);
			return -1;
		}
		if (((c >> 0) & 0xff) != spi_sdcard_recvbyte(card)) {
			spi_sdcard_disable_cs(card);
			return -1;
		}
	}

	/* disable cs */
	spi_sdcard_disable_cs(card);

	if (spi_sdcard_do_command(card, STOP_TRANSMISSION, 0, NULL))
		if (spi_sdcard_do_command(card, SEND_STATUS, 0, NULL))
			return -1;

	return 0;
}

int spi_sdcard_read(const struct spi_sdcard *card,
		void *dest,
		size_t offset,
		size_t count)
{
	size_t start_block_address = offset / BLOCK_SIZE;
	size_t end_block_address = (offset + count - 1) / BLOCK_SIZE;
	size_t has_begin = !!(offset % BLOCK_SIZE);
	size_t has_end = !!((offset + count) % BLOCK_SIZE);

	if (start_block_address == end_block_address) {
		uint8_t tmp[BLOCK_SIZE];
		size_t o = offset % BLOCK_SIZE;
		size_t l = count;
		if (spi_sdcard_single_read(card, start_block_address, tmp))
			return -1;
		memcpy(dest, tmp + o, l);
		return 0;
	}

	if (has_begin) {
		uint8_t tmp[BLOCK_SIZE];
		size_t o = offset % BLOCK_SIZE;
		size_t l = BLOCK_SIZE - o;
		if (spi_sdcard_single_read(card, start_block_address, tmp))
			return -1;
		memcpy(dest, tmp + o, l);
	}

	if (start_block_address + has_begin <= end_block_address - has_end) {
		size_t start_lba = start_block_address + has_begin;
		size_t end_lba = end_block_address - has_end;
		size_t o = has_begin ? BLOCK_SIZE - offset % BLOCK_SIZE : 0;
		if (start_lba < end_lba) {
			if (spi_sdcard_multiple_read(card, start_lba, end_lba,
					dest + o))
				return -1;
		} else {
			if (spi_sdcard_single_read(card, start_lba, dest + o))
				return -1;
		}
	}

	if (has_end) {
		uint8_t tmp[BLOCK_SIZE];
		size_t o = 0;
		size_t l = (offset + count) % BLOCK_SIZE;
		if (spi_sdcard_single_read(card, end_block_address, tmp))
			return -1;
		memcpy(dest + count - l, tmp + o, l);
	}

	return 0;
}

int spi_sdcard_single_write(const struct spi_sdcard *card,
		size_t block_address,
		void *buff)
{
	int wait;
	uint16_t c = 0;
	if (card->type == SDCARD_TYPE_SDSC)
		block_address = block_address * 512;

	if (spi_sdcard_do_command(card, WRITE_BLOCK, block_address, NULL))
		return -1;

	/* eanbele cs */
	spi_sdcard_enable_cs(card);

	/* send start block token */
	spi_sdcard_sendbyte(card, CT_BLOCK_START);

	/* send data */
	for (int i = 0; i < 512; i++) {
		spi_sdcard_sendbyte(card, ((uint8_t *)buff)[i]);
		c = crc16_byte(c, ((uint8_t *)buff)[i]);
	}

	/* send crc check sum */
	spi_sdcard_sendbyte(card, 0xff & (c >> 8));
	spi_sdcard_sendbyte(card, 0xff & (c >> 0));

	/* receive and verify data response token */
	c = spi_sdcard_recvbyte(card);
	if ((c & CT_RESPONSE_MASK) != CT_RESPONSE_ACCEPTED) {
		spi_sdcard_disable_cs(card);
		return -1;
	}

	wait = 0xffff;
	while ((spi_sdcard_recvbyte(card) == 0) && --wait)
		;/* wait for complete */
	if (!wait) {
		spi_sdcard_disable_cs(card);
		return -1;
	}

	/* disable cs */
	spi_sdcard_disable_cs(card);

	return 0;
}

int spi_sdcard_multiple_write(const struct spi_sdcard *card,
		size_t start_block_address,
		size_t end_block_address,
		void *buff)
{
	int wait, ret = 0;
	int block_num = end_block_address - start_block_address + 1;
	if (card->type == SDCARD_TYPE_SDSC) {
		start_block_address = start_block_address * 512;
		end_block_address = end_block_address * 512;
	}

	if (spi_sdcard_do_command(card, WRITE_MULTIPLEBLOCK,
		start_block_address, NULL))
		return -1;

	/* enable cs */
	spi_sdcard_enable_cs(card);

	for (int i = 0; i < block_num; i++) {
		uint16_t c = 0;

		ret = -1;

		/* send start block token */
		spi_sdcard_sendbyte(card, CT_MULTIPLE_BLOCK_START);

		/* send data */
		for (int k = 0; k < 512; k++) {
			uint8_t tmp = ((uint8_t *)buff)[512 * i + k];
			spi_sdcard_sendbyte(card, tmp);
			c = crc16_byte(c, tmp);
		}

		/* send crc check sum */
		spi_sdcard_sendbyte(card, 0xff & (c >> 8));
		spi_sdcard_sendbyte(card, 0xff & (c >> 0));

		/* receive and verify data response token */
		c = spi_sdcard_recvbyte(card);
		if ((c & CT_RESPONSE_MASK) != CT_RESPONSE_ACCEPTED)
			break;

		wait = 0xffff;
		while ((spi_sdcard_recvbyte(card) == 0) && --wait)
			;/* wait for complete */
		if (!wait)
			break;

		ret = 0;
	}

	/* send stop transmission token */
	spi_sdcard_sendbyte(card, CT_MULTIPLE_BLOCK_STOP);

	/* disable cs */
	spi_sdcard_disable_cs(card);

	if (spi_sdcard_do_command(card, STOP_TRANSMISSION, 0, NULL))
		if (spi_sdcard_do_command(card, SEND_STATUS, 0, NULL))
			return -1;

	return ret;
}

int spi_sdcard_erase(const struct spi_sdcard *card,
		size_t start_block_address,
		size_t end_block_address)
{
	if (card->type == SDCARD_TYPE_SDSC) {
		start_block_address = start_block_address * 512;
		end_block_address = end_block_address * 512;
	}

	/* CMD32, set erase start address */
	if (spi_sdcard_do_command(card, ERASE_WR_BLK_START_ADDR,
			start_block_address, NULL))
		return -1;

	/* CMD33, set erase end address */
	if (spi_sdcard_do_command(card, ERASE_WR_BLK_END_ADDR,
			end_block_address, NULL))
		return -1;

	/* CMD38, erase */
	if (spi_sdcard_do_command(card, ERASE, 0, NULL))
		return -1;

	return 0;
}

int spi_sdcard_erase_all(const struct spi_sdcard *card)
{
	return spi_sdcard_erase(card, 0, spi_sdcard_size(card) / BLOCK_SIZE);
}
