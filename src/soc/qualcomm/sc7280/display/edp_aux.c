// SPDX-License-Identifier: GPL-2.0-only

#include <device/mmio.h>
#include <console/console.h>
#include <edid.h>
#include <delay.h>
#include <timer.h>
#include <types.h>
#include <string.h>
#include <soc/display/edp_aux.h>
#include <soc/display/edp_reg.h>

#define AUX_CMD_FIFO_LEN		144
#define AUX_CMD_NATIVE_MAX		16
#define AUX_CMD_I2C_MAX			128
#define AUX_INTR_I2C_DONE		BIT(0)
#define AUX_INTR_WRONG_ADDR		BIT(1)
#define AUX_INTR_CONSECUTIVE_TIMEOUT	BIT(2)
#define AUX_INTR_CONSECUTIVE_NACK_DEFER	BIT(3)
#define AUX_INTR_WRONG_RD_DATA_CNT	BIT(4)
#define AUX_INTR_NACK_I2C		BIT(5)
#define AUX_INTR_DEFER_I2C		BIT(6)
#define AUX_INTR_DPPHY_AUX_ERR		BIT(7)
#define EDP_AUX_INTERRUPT		(AUX_INTR_I2C_DONE | AUX_INTR_WRONG_ADDR | \
					AUX_INTR_CONSECUTIVE_TIMEOUT | \
					AUX_INTR_CONSECUTIVE_NACK_DEFER | \
					AUX_INTR_WRONG_RD_DATA_CNT | AUX_INTR_DEFER_I2C | \
					AUX_INTR_NACK_I2C | AUX_INTR_DPPHY_AUX_ERR)

static void edp_wait_for_aux_done(void)
{
	u32 intr_status = 0;

	if (!wait_ms(100, read32(&edp_auxclk->status) & EDP_AUX_INTERRUPT)) {
		printk(BIOS_ERR, "ERROR: AUX SEND not acknowledged\n");
		return;
	}

	intr_status = read32(&edp_auxclk->status);
	if (!(intr_status & AUX_INTR_I2C_DONE)) {
		printk(BIOS_ERR, "ERROR: AUX command failed, status = %#x\n", intr_status);
		return;
	}

	write32(&edp_ahbclk->interrupt_status, 0x0);
}

static int edp_msg_fifo_tx(unsigned int address, u8 request, void *buffer, size_t size)
{
	u32 data[4];
	u32 reg, len;
	bool native = (request == DP_AUX_NATIVE_WRITE) || (request == DP_AUX_NATIVE_READ);
	bool read = (request == DP_AUX_I2C_READ) || (request == DP_AUX_NATIVE_READ);
	u8 *msgdata = buffer;
	int i;

	if (read)
		len = 4;
	else
		len = size + 4;

	/*
	 * cmd fifo only has depth of 144 bytes
	 */
	if (len > AUX_CMD_FIFO_LEN)
		return -1;

	/* Pack cmd and write to HW */
	data[0] = (address >> 16) & 0xf;	/* addr[19:16] */
	if (read)
		data[0] |=  AUX_CMD_READ;		/* R/W */

	data[1] = (address >> 8) & 0xff;	/* addr[15:8] */
	data[2] = address & 0xff;		/* addr[7:0] */
	data[3] = (size - 1) & 0xff;		/* len[7:0] */

	for (i = 0; i < len; i++) {
		reg = (i < 4) ? data[i] : msgdata[i - 4];
		reg = EDP_AUX_DATA_DATA(reg); /* index = 0, write */
		if (i == 0)
			reg |= EDP_AUX_DATA_INDEX_WRITE;
		write32(&edp_auxclk->aux_data, reg);
	}

	/* clear old aux transaction control */
	write32(&edp_auxclk->aux_trans_ctrl, 0);
	reg = RX_STOP_ERR | RX_DEC_ERR | RX_SYNC_ERR | RX_ALIGN_ERR | TX_REQ_ERR;
	write32(&edp_phy->aux_interrupt_clr, reg);
	write32(&edp_phy->aux_interrupt_clr, reg | GLOBE_REQ_CLR);
	write32(&edp_phy->aux_interrupt_clr, 0x0);

	reg = 0; /* Transaction number is always 1 */
	if (!native) /* i2c */
		reg |= EDP_AUX_TRANS_CTRL_I2C | EDP_AUX_TRANS_CTRL_NO_SEND_ADDR |
			EDP_AUX_TRANS_CTRL_NO_SEND_STOP;

	reg |= EDP_AUX_TRANS_CTRL_GO;
	write32(&edp_auxclk->aux_trans_ctrl, reg);
	edp_wait_for_aux_done();

	return 0;
}

static int edp_msg_fifo_rx(void *buffer, size_t size)
{
	u32 data;
	u8 *dp;
	int i;
	u32 len = size;

	clrbits32(&edp_auxclk->aux_trans_ctrl, EDP_AUX_TRANS_CTRL_GO);
	write32(&edp_auxclk->aux_data,
		EDP_AUX_DATA_INDEX_WRITE | EDP_AUX_DATA_READ); /* index = 0 */

	dp = buffer;

	/* discard first byte */
	data = read32(&edp_auxclk->aux_data);
	for (i = 0; i < len; i++) {
		data = read32(&edp_auxclk->aux_data);
		dp[i] = (u8)((data >> 8) & 0xff);
	}

	return 0;
}

ssize_t edp_aux_transfer(unsigned int address, u8 request, void *buffer, size_t size)
{
	ssize_t ret;
	bool native = (request == DP_AUX_NATIVE_WRITE) || (request == DP_AUX_NATIVE_READ);
	bool read = (request == DP_AUX_I2C_READ) || (request == DP_AUX_NATIVE_READ);

	/* Ignore address only message */
	if ((size == 0) || (buffer == NULL)) {
		printk(BIOS_ERR, "%s: invalid size or buffer\n", __func__);
		return size;
	}

	/* msg sanity check */
	if ((native && (size > AUX_CMD_NATIVE_MAX)) ||
		(size > AUX_CMD_I2C_MAX)) {
		printk(BIOS_ERR, "%s: invalid msg: size(%zu), request(%x)\n",
		       __func__, size, request);
		return -1;
	}

	ret = edp_msg_fifo_tx(address, request, buffer, size);
	if (ret < 0) {
		printk(BIOS_ERR, "edp aux transfer tx failed\n");
		return ret;
	}

	if (read) {
		ret = edp_msg_fifo_rx(buffer, size);
		if (ret < 0) {
			printk(BIOS_ERR, "edp aux transfer rx failed\n");
			return ret;
		}
	}

	/* Return requested size for success or retry */
	ret = size;

	return ret;
}

int edp_read_edid(struct edid *out)
{
	int err;
	u8 edid[EDID_LENGTH * 2];
	int edid_size = EDID_LENGTH;

	uint8_t reg_addr = 0;
	err = edp_aux_transfer(EDID_I2C_ADDR, DP_AUX_I2C_WRITE, &reg_addr, 1);
	if (err > 0)
		err = edp_aux_transfer(EDID_I2C_ADDR, DP_AUX_I2C_READ, edid, EDID_LENGTH);

	if (err < EDID_LENGTH) {
		printk(BIOS_ERR, "ERROR: Failed to read EDID. :%d\n", err);
		return err;
	}

	if (edid[EDID_EXTENSION_FLAG]) {
		printk(BIOS_ERR, " read EDID ext block.\n");
		edid_size += EDID_LENGTH;
		reg_addr = EDID_LENGTH;
		err = edp_aux_transfer(EDID_I2C_ADDR, DP_AUX_I2C_WRITE, &reg_addr, 1);
		if (err > 0)
			err = edp_aux_transfer(EDID_I2C_ADDR, DP_AUX_I2C_READ,
					       &edid[EDID_LENGTH], EDID_LENGTH);

		if (err < EDID_LENGTH) {
			printk(BIOS_ERR, "Failed to read EDID ext block.\n");
			return err;
		}
	}

	if (decode_edid(edid, edid_size, out) != EDID_CONFORMANT) {
		printk(BIOS_ERR, "ERROR: Failed to decode EDID.\n");
		return CB_ERR;
	}


	return CB_SUCCESS;
}

void edp_aux_ctrl(int enable)
{
	u32 data;
	data = read32(&edp_auxclk->aux_ctrl);

	if (!enable) {
		data &= ~EDP_AUX_CTRL_ENABLE;
		write32(&edp_auxclk->aux_ctrl, data);
		return;
	}

	data |= EDP_AUX_CTRL_RESET;
	write32(&edp_auxclk->aux_ctrl, data);

	data &= ~EDP_AUX_CTRL_RESET;
	write32(&edp_auxclk->aux_ctrl, data);

	write32(&edp_auxclk->timeout_count, 0xffff);
	write32(&edp_auxclk->aux_limits, 0xffff);

	data |= EDP_AUX_CTRL_ENABLE;
	write32(&edp_auxclk->aux_ctrl, data);
}
