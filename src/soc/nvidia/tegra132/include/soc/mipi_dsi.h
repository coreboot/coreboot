/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */
/*
 * MIPI DSI Bus
 *
 * Copyright (C) 2012-2013, Samsung Electronics, Co., Ltd.
 * Andrzej Hajda <a.hajda@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __MIPI_DSI_H__
#define __MIPI_DSI_H__

struct mipi_dsi_host;
struct mipi_dsi_device;

/* request ACK from peripheral */
#define MIPI_DSI_MSG_REQ_ACK	BIT(0)
/* use Low Power Mode to transmit message */
#define MIPI_DSI_MSG_USE_LPM	BIT(1)

/**
 * struct mipi_dsi_msg - read/write DSI buffer
 * @channel: virtual channel id
 * @type: payload data type
 * @flags: flags controlling this message transmission
 * @tx_len: length of @tx_buf
 * @tx_buf: data to be written
 * @rx_len: length of @rx_buf
 * @rx_buf: data to be read, or NULL
 */
struct mipi_dsi_msg {
	u8 channel;
	u8 type;
	u16 flags;

	size_t tx_len;
	const void *tx_buf;

	size_t rx_len;
	void *rx_buf;
};

/**
 * struct mipi_dsi_host_ops - DSI bus operations
 * @attach: attach DSI device to DSI host
 * @detach: detach DSI device from DSI host
 * @transfer: transmit a DSI packet
 *
 * DSI packets transmitted by .transfer() are passed in as mipi_dsi_msg
 * structures. This structure contains information about the type of packet
 * being transmitted as well as the transmit and receive buffers. When an
 * error is encountered during transmission, this function will return a
 * negative error code. On success it shall return the number of bytes
 * transmitted for write packets or the number of bytes received for read
 * packets.
 *
 * Note that typically DSI packet transmission is atomic, so the .transfer()
 * function will seldomly return anything other than the number of bytes
 * contained in the transmit buffer on success.
 */
struct mipi_dsi_host_ops {
	int (*attach)(struct mipi_dsi_host *host,
		      struct mipi_dsi_device *dsi);
	int (*detach)(struct mipi_dsi_host *host,
		      struct mipi_dsi_device *dsi);
	ssize_t (*transfer)(struct mipi_dsi_host *host,
			    const struct mipi_dsi_msg *msg);
};

/**
 * struct mipi_dsi_host - DSI host device
 * @dev: driver model device node for this DSI host
 * @ops: DSI host operations
 */
struct mipi_dsi_host {
	//struct device *dev;
	void *dev;
	const struct mipi_dsi_host_ops *ops;
};

int mipi_dsi_host_register(struct mipi_dsi_host *host);

/* DSI mode flags */

/* video mode */
#define MIPI_DSI_MODE_VIDEO		BIT(0)
/* video burst mode */
#define MIPI_DSI_MODE_VIDEO_BURST	BIT(1)
/* video pulse mode */
#define MIPI_DSI_MODE_VIDEO_SYNC_PULSE	BIT(2)
/* enable auto vertical count mode */
#define MIPI_DSI_MODE_VIDEO_AUTO_VERT	BIT(3)
/* enable hsync-end packets in vsync-pulse and v-porch area */
#define MIPI_DSI_MODE_VIDEO_HSE		BIT(4)
/* disable hfront-porch area */
#define MIPI_DSI_MODE_VIDEO_HFP		BIT(5)
/* disable hback-porch area */
#define MIPI_DSI_MODE_VIDEO_HBP		BIT(6)
/* disable hsync-active area */
#define MIPI_DSI_MODE_VIDEO_HSA		BIT(7)
/* flush display FIFO on vsync pulse */
#define MIPI_DSI_MODE_VSYNC_FLUSH	BIT(8)
/* disable EoT packets in HS mode */
#define MIPI_DSI_MODE_EOT_PACKET	BIT(9)
/* device supports non-continuous clock behavior (DSI spec 5.6.1) */
#define MIPI_DSI_CLOCK_NON_CONTINUOUS	BIT(10)

enum mipi_dsi_pixel_format {
	MIPI_DSI_FMT_RGB888,
	MIPI_DSI_FMT_RGB666,
	MIPI_DSI_FMT_RGB666_PACKED,
	MIPI_DSI_FMT_RGB565,
};

struct mipi_dsi_master_ops {
	int (*enslave)(struct mipi_dsi_device *master,
		       struct mipi_dsi_device *slave);
	int (*liberate)(struct mipi_dsi_device *master,
			struct mipi_dsi_device *slave);
};

/**
 * struct mipi_dsi_device - DSI peripheral device
 * @host: DSI host for this peripheral
 * @dev: driver model device node for this peripheral
 * @channel: virtual channel assigned to the peripheral
 * @format: pixel format for video mode
 * @lanes: number of active data lanes
 * @mode_flags: DSI operation mode related flags
 * @ops: callbacks for master/slave setup
 * @master: master interface for dual-channel peripherals
 * @slave: slave interface for dual-channel peripherals
 *
 * For dual-channel interfaces, the master interface can be identified by the
 * fact that it's .slave field is set to non-NULL. The slave interface will
 * have the .master field set to non-NULL.
 */
struct mipi_dsi_device {
	struct mipi_dsi_host *host;

	unsigned int channel;
	unsigned int lanes;
	enum mipi_dsi_pixel_format format;
	unsigned long mode_flags;

	const struct mipi_dsi_master_ops *ops;
	struct mipi_dsi_device *master;
	struct mipi_dsi_device *slave;
};

int mipi_dsi_attach(struct mipi_dsi_device *dsi);
int mipi_dsi_detach(struct mipi_dsi_device *dsi);
int mipi_dsi_enslave(struct mipi_dsi_device *master,
		     struct mipi_dsi_device *slave);
int mipi_dsi_liberate(struct mipi_dsi_device *master,
		      struct mipi_dsi_device *slave);

/**
 * enum mipi_dsi_dcs_tear_mode - Tearing Effect Output Line mode
 * @MIPI_DSI_DCS_TEAR_MODE_VBLANK: the TE output line consists of V-Blanking
 *    information only
 * @MIPI_DSI_DCS_TEAR_MODE_VHBLANK : the TE output line consists of both
 *    V-Blanking and H-Blanking information
 */
enum mipi_dsi_dcs_tear_mode {
	MIPI_DSI_DCS_TEAR_MODE_VBLANK,
	MIPI_DSI_DCS_TEAR_MODE_VHBLANK,
};

#define MIPI_DSI_DCS_POWER_MODE_DISPLAY (1 << 2)
#define MIPI_DSI_DCS_POWER_MODE_NORMAL  (1 << 3)
#define MIPI_DSI_DCS_POWER_MODE_SLEEP   (1 << 4)
#define MIPI_DSI_DCS_POWER_MODE_PARTIAL (1 << 5)
#define MIPI_DSI_DCS_POWER_MODE_IDLE    (1 << 6)

ssize_t mipi_dsi_dcs_write(struct mipi_dsi_device *dsi, u8 cmd,
			   const void *data, size_t len);
int mipi_dsi_dcs_exit_sleep_mode(struct mipi_dsi_device *dsi);
int mipi_dsi_dcs_set_display_on(struct mipi_dsi_device *dsi);
int mipi_dsi_dcs_set_column_address(struct mipi_dsi_device *dsi, u16 start,
				    u16 end);
int mipi_dsi_dcs_set_page_address(struct mipi_dsi_device *dsi, u16 start,
				  u16 end);
int mipi_dsi_dcs_set_address_mode(struct mipi_dsi_device *dsi,
				  bool reverse_page_address,
				  bool reverse_col_address,
				  bool reverse_page_col_address,
				  bool refresh_from_bottom,
				  bool reverse_rgb,
				  bool latch_right_to_left,
				  bool flip_horizontal,
				  bool flip_vertical);
int mipi_dsi_dcs_set_tear_on(struct mipi_dsi_device *dsi,
			     enum mipi_dsi_dcs_tear_mode mode);
int mipi_dsi_dcs_set_pixel_format(struct mipi_dsi_device *dsi, u8 format);

#define MIPI_CAL_CTRL			0x00
#define MIPI_CAL_CTRL_START		(1 << 0)

#define MIPI_CAL_AUTOCAL_CTRL		0x01

#define MIPI_CAL_STATUS			0x02
#define MIPI_CAL_STATUS_DONE		(1 << 16)
#define MIPI_CAL_STATUS_ACTIVE		(1 <<  0)

#define MIPI_CAL_CONFIG_CSIA		0x05
#define MIPI_CAL_CONFIG_CSIB		0x06
#define MIPI_CAL_CONFIG_CSIC		0x07
#define MIPI_CAL_CONFIG_CSID		0x08
#define MIPI_CAL_CONFIG_CSIE		0x09
#define MIPI_CAL_CONFIG_DSIA		0x0e
#define MIPI_CAL_CONFIG_DSIB		0x0f
#define MIPI_CAL_CONFIG_DSIC		0x10
#define MIPI_CAL_CONFIG_DSID		0x11

#define MIPI_CAL_CONFIG_DSIAB_CLK	0x19
#define MIPI_CAL_CONFIG_DSICD_CLK	0x1a
#define MIPI_CAL_CONFIG_CSIAB_CLK	0x1b
#define MIPI_CAL_CONFIG_CSICD_CLK	0x1c
#define MIPI_CAL_CONFIG_CSIE_CLK	0x1d

#define MIPI_CAL_CONFIG_SELECT		(1 << 21)
#define MIPI_CAL_CONFIG_HSPDOS(x)	(((x) & 0x1f) << 16)
#define MIPI_CAL_CONFIG_HSPUOS(x)	(((x) & 0x1f) <<  8)
#define MIPI_CAL_CONFIG_TERMOS(x)	(((x) & 0x1f) <<  0)
#define MIPI_CAL_CONFIG_HSCLKPDOSD(x)	(((x) & 0x1f) << 8)
#define MIPI_CAL_CONFIG_HSCLKPUOSD(x)	(((x) & 0x1f) <<  0)

#define MIPI_CAL_BIAS_PAD_CFG0		0x16
#define MIPI_CAL_BIAS_PAD_PDVCLAMP	(1 << 1)
#define MIPI_CAL_BIAS_PAD_E_VCLAMP_REF	(1 << 0)

#define MIPI_CAL_BIAS_PAD_CFG1		0x17
#define MIPI_CAL_BIAS_PAD_CFG1_DEFAULT	(0x20000)

#define MIPI_CAL_BIAS_PAD_CFG2		0x18
#define MIPI_CAL_BIAS_PAD_PDVREG	(1 << 1)

struct calibration_regs {
	unsigned long data;
	unsigned long clk;
};

struct tegra_mipi_config {
	int calibrate_clk_lane;
	int num_pads;
	const struct calibration_regs *regs;
};

struct tegra_mipi {
	void *regs;
};

struct tegra_mipi_device {
	struct tegra_mipi *mipi;
	const struct tegra_mipi_config *config;
	unsigned long pads;
};

struct tegra_mipi_device *tegra_mipi_request(struct tegra_mipi_device *device,
						int device_index);
int tegra_mipi_calibrate(struct tegra_mipi_device *device);
#endif /* __MIPI_DSI_H__ */
