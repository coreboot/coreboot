/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * MIPI DSI Bus
 *
 * Andrzej Hajda <a.hajda@samsung.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <console/console.h>
#include <string.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <device/device.h>
#include <soc/nvidia/tegra/types.h>
#include <soc/display.h>
#include <soc/mipi_dsi.h>
#include <soc/mipi_display.h>
#include <soc/tegra_dsi.h>
#include <types.h>

struct mipi_dsi_device mipi_dsi_device_data[NUM_DSI] = {
	{
		.master = NULL,
		.slave = &mipi_dsi_device_data[DSI_B],
	},
	{
		.master = &mipi_dsi_device_data[DSI_A],
		.slave = NULL,
	},
};

static struct mipi_dsi_device *
mipi_dsi_device_alloc(struct mipi_dsi_host *host)
{
	static int index = 0;
	struct mipi_dsi_device *dsi;

	if (index >= NUM_DSI)
		return (void *)-EPTR;

	dsi = &mipi_dsi_device_data[index++];
	dsi->host = host;
	return dsi;
}

static struct mipi_dsi_device *
of_mipi_dsi_device_add(struct mipi_dsi_host *host)
{
	struct mipi_dsi_device *dsi;
	u32 reg = 0;

	dsi = mipi_dsi_device_alloc(host);
	if (IS_ERR_PTR(dsi)) {
		printk(BIOS_ERR, "failed to allocate DSI device\n");
		return dsi;
	}

	dsi->channel = reg;
	host->dev = (void *)dsi;

	return dsi;
}

int mipi_dsi_host_register(struct mipi_dsi_host *host)
{
	of_mipi_dsi_device_add(host);
	return 0;
}

/**
 * mipi_dsi_attach - attach a DSI device to its DSI host
 * @param dsi: DSI peripheral
 */
int mipi_dsi_attach(struct mipi_dsi_device *dsi)
{
	const struct mipi_dsi_host_ops *ops = dsi->host->ops;

	if (!ops || !ops->attach)
		return -ENOSYS;

	return ops->attach(dsi->host, dsi);
}

/**
 * mipi_dsi_detach - detach a DSI device from its DSI host
 * @param dsi: DSI peripheral
 */
int mipi_dsi_detach(struct mipi_dsi_device *dsi)
{
	const struct mipi_dsi_host_ops *ops = dsi->host->ops;

	if (!ops || !ops->detach)
		return -ENOSYS;

	return ops->detach(dsi->host, dsi);
}

/**
 * mipi_dsi_enslave() - use a MIPI DSI peripheral as slave for dual-channel
 *    operation
 * @param master: master DSI peripheral device
 * @param slave: slave DSI peripheral device
 *
 * @return 0 on success or a negative error code on failure.
 */
int mipi_dsi_enslave(struct mipi_dsi_device *master,
		     struct mipi_dsi_device *slave)
{
	int err = 0;

	slave->master = master;
	master->slave = slave;

	if (master->ops && master->ops->enslave)
		err = master->ops->enslave(master, slave);

	return err;
}

/**
 * mipi_dsi_liberate() - stop using a MIPI DSI peripheral as slave for dual-
 *    channel operation
 * @param master: master DSI peripheral device
 * @param slave: slave DSI peripheral device
 *
 * @return 0 on success or a negative error code on failure.
 */
int mipi_dsi_liberate(struct mipi_dsi_device *master,
		      struct mipi_dsi_device *slave)
{
	int err = 0;

	if (master->ops && master->ops->liberate)
		err = master->ops->liberate(master, slave);

	master->slave = NULL;
	slave->master = NULL;

	return err;
}

/**
 * mipi_dsi_dcs_write() - send DCS write command
 * @param dsi: DSI peripheral device
 * @param cmd: DCS command
 * @param data: buffer containing the command payload
 * @param len: command payload length
 *
 * This function will automatically choose the right data type depending on
 * the command payload length.
 *
 * @return The number of bytes successfully transmitted or a negative error code on failure.
 */
ssize_t mipi_dsi_dcs_write(struct mipi_dsi_device *dsi, u8 cmd,
			   const void *data, size_t len)
{
	struct mipi_dsi_msg msg;
	ssize_t err;
	size_t size;

	u8 buffer[MAX_DSI_HOST_FIFO_DEPTH + 4];
	u8 *tx = buffer;

	if (len > MAX_DSI_HOST_FIFO_DEPTH) {
		printk(BIOS_ERR, "%s: Error: too large payload length: %zu\n",
			__func__, len);

		return -EINVAL;
	}

	if (len > 0) {
		unsigned int offset = 0;

		/*
		 * DCS long write packets contain the word count in the header
		 * bytes 1 and 2 and have a payload containing the DCS command
		 * byte followed by word count minus one bytes.
		 *
		 * DCS short write packets encode the DCS command and up to
		 * one parameter in header bytes 1 and 2.
		 */
		if (len > 1)
			size = 3 + len;
		else
			size = 1 + len;

		/* write word count to header for DCS long write packets */
		if (len > 1) {
			tx[offset++] = ((1 + len) >> 0) & 0xff;
			tx[offset++] = ((1 + len) >> 8) & 0xff;
		}

		/* write the DCS command byte followed by the payload */
		tx[offset++] = cmd;
		memcpy(tx + offset, data, len);
	} else {
		tx = &cmd;
		size = 1;
	}

	memset(&msg, 0, sizeof(msg));
	msg.flags = MIPI_DSI_MSG_USE_LPM;
	msg.channel = dsi->channel;
	msg.tx_len = size;
	msg.tx_buf = tx;

	switch (len) {
	case 0:
		msg.type = MIPI_DSI_DCS_SHORT_WRITE;
		break;
	case 1:
		msg.type = MIPI_DSI_DCS_SHORT_WRITE_PARAM;
		break;
	default:
		msg.type = MIPI_DSI_DCS_LONG_WRITE;
		break;
	}

	err = dsi->host->ops->transfer(dsi->host, &msg);

	return err;
}

/**
 * mipi_dsi_dcs_exit_sleep_mode() - enable all blocks inside the display
 *    module
 * @param dsi: DSI peripheral device
 *
 * @return 0 on success or a negative error code on failure.
 */
int mipi_dsi_dcs_exit_sleep_mode(struct mipi_dsi_device *dsi)
{
	ssize_t err;

	err = mipi_dsi_dcs_write(dsi, MIPI_DCS_EXIT_SLEEP_MODE, NULL, 0);
	if (err < 0)
		return err;

	return 0;
}

/**
 * mipi_dsi_dcs_set_display_on() - start displaying the image data on the
 *    display device
 * @param dsi: DSI peripheral device
 *
 * @return 0 on success or a negative error code on failure
 */
int mipi_dsi_dcs_set_display_on(struct mipi_dsi_device *dsi)
{
	ssize_t err;

	err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_DISPLAY_ON, NULL, 0);
	if (err < 0)
		return err;

	return 0;
}

/**
 * mipi_dsi_dcs_set_column_address() - define the column extent of the frame
 *    memory accessed by the host processor
 * @param dsi: DSI peripheral device
 * @param start: first column of frame memory
 * @param end: last column of frame memory
 *
 * @return 0 on success or a negative error code on failure.
 */
int mipi_dsi_dcs_set_column_address(struct mipi_dsi_device *dsi, u16 start,
				    u16 end)
{
	u8 payload[4] = { start >> 8, start & 0xff, end >> 8, end & 0xff };
	ssize_t err;

	err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_COLUMN_ADDRESS, payload,
				 sizeof(payload));
	if (err < 0)
		return err;

	return 0;
}

/**
 * mipi_dsi_dcs_set_page_address() - define the page extent of the frame
 *    memory accessed by the host processor
 * @param dsi: DSI peripheral device
 * @param start: first page of frame memory
 * @param end: last page of frame memory
 *
 * @return 0 on success or a negative error code on failure.
 */
int mipi_dsi_dcs_set_page_address(struct mipi_dsi_device *dsi, u16 start,
				  u16 end)
{
	u8 payload[4] = { start >> 8, start & 0xff, end >> 8, end & 0xff };
	ssize_t err;

	err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_PAGE_ADDRESS, payload,
				 sizeof(payload));
	if (err < 0)
		return err;

	return 0;
}

/**
 * mipi_dsi_dcs_set_tear_on() - turn on the display module's Tearing Effect
 *    output signal on the TE signal line.
 * @param dsi: DSI peripheral device
 * @param mode: the Tearing Effect Output Line mode
 *
 * @return 0 on success or a negative error code on failure
 */
int mipi_dsi_dcs_set_tear_on(struct mipi_dsi_device *dsi,
			     enum mipi_dsi_dcs_tear_mode mode)
{
	u8 value = mode;
	ssize_t err;

	err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_TEAR_ON, &value,
				 sizeof(value));
	if (err < 0)
		return err;

	return 0;
}

/**
 * mipi_dsi_dcs_set_pixel_format() - sets the pixel format for the RGB image
 *    data used by the interface
 * @param dsi: DSI peripheral device
 * @param format: pixel format
 *
 * @return 0 on success or a negative error code on failure.
 */
int mipi_dsi_dcs_set_pixel_format(struct mipi_dsi_device *dsi, u8 format)
{
	ssize_t err;

	err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_PIXEL_FORMAT, &format,
				 sizeof(format));
	if (err < 0)
		return err;

	return 0;
}

/**
 * mipi_dsi_dcs_set_address_mode() - sets the data order for forward transfers
 *    from the host to the peripheral
 * @param dsi: DSI peripheral device
 * @param reverse_page_address: reverses the page addressing to bottom->top
 * @param reverse_col_address: reverses the column addressing to right->left
 * @param reverse_page_col_address: reverses the page/column addressing order
 * @param refresh_from_bottom: refresh the display bottom to top
 * @param reverse_rgb: send pixel data bgr instead of rgb
 * @param latch_right_to_left: latch the incoming display data right to left
 * @param flip_horizontal: flip the image horizontally, left to right
 * @param flip_vertical: flip the image vertically, top to bottom
 *
 * @return 0 on success or a negative error code on failure.
 */
int mipi_dsi_dcs_set_address_mode(struct mipi_dsi_device *dsi,
			bool reverse_page_address,
			bool reverse_col_address,
			bool reverse_page_col_address,
			bool refresh_from_bottom,
			bool reverse_rgb,
			bool latch_right_to_left,
			bool flip_horizontal,
			bool flip_vertical)
{
	ssize_t err;
	u8 data;

	data = ((flip_vertical ? 1 : 0) << 0) |
		((flip_horizontal ? 1 : 0) << 1) |
		((latch_right_to_left ? 1 : 0) << 2) |
		((reverse_rgb ? 1 : 0) << 3) |
		((refresh_from_bottom ? 1 : 0) << 4) |
		((reverse_page_col_address ? 1 : 0) << 5) |
		((reverse_col_address ? 1 : 0) << 6) |
		((reverse_page_address ? 1 : 0) << 7);

	err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_ADDRESS_MODE, &data, 1);
	if (err < 0)
		return err;

	return 0;
}
