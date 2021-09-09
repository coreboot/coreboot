/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * MIPI DSI Bus
 *
 * Andrzej Hajda <a.hajda@samsung.com>
 */

#ifndef __SOC_MIPI_DSI_H__
#define __SOC_MIPI_DSI_H__

#include <types.h>

struct mipi_dsi_host;
struct mipi_dsi_device;

/* request ACK from peripheral */
#define MIPI_DSI_MSG_REQ_ACK	BIT(0)
/* use Low Power Mode to transmit message */
#define MIPI_DSI_MSG_USE_LPM	BIT(1)

/**
 * @brief mipi_dsi_msg - read/write DSI buffer
 */
struct mipi_dsi_msg {
	u8 channel;	/**< virtual channel id */
	u8 type;	/**< payload data type */
	u16 flags;	/**< flags controlling this message transmission */

	size_t tx_len;	/**< length of tx_buf */
	const void *tx_buf;	/**< data to be written */

	size_t rx_len;	/**< length of rx_buf */
	void *rx_buf;	/**< data to be read, or NULL */
};

/**
 * @brief mipi_dsi_host_ops - DSI bus operations
 * @var mipi_dsi_host_ops::attach
 *      attach DSI device to DSI host
 * @var mipi_dsi_host_ops::detach
 *      detach DSI device from DSI host
 * @var mipi_dsi_host_ops::transfer
 *     transmit a DSI packet
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
 * @brief mipi_dsi_host - DSI host device
 */
struct mipi_dsi_host {
	//struct device *dev;
	void *dev;	/**< driver model device node for this DSI host */
	const struct mipi_dsi_host_ops *ops;	/**< DSI host operations */
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
 * @brief mipi_dsi_device - DSI peripheral device
 *
 * For dual-channel interfaces, the master interface can be identified by the
 * fact that it's .slave field is set to non-NULL. The slave interface will
 * have the .master field set to non-NULL.
 */
struct mipi_dsi_device {
	struct mipi_dsi_host *host;	/**< DSI host for this peripheral */

	unsigned int channel;	/**< virtual channel assigned to the peripheral */
	unsigned int lanes;	/**< number of active data lanes */
	enum mipi_dsi_pixel_format format;	/**< pixel format for video mode */
	unsigned long mode_flags;	/**< DSI operation mode related flags */

	const struct mipi_dsi_master_ops *ops;	/**< callbacks for master/slave setup */
	struct mipi_dsi_device *master;	/**< master interface for dual-channel peripherals */
	struct mipi_dsi_device *slave;	/**< slave interface for dual-channel peripherals */
};

int mipi_dsi_attach(struct mipi_dsi_device *dsi);
int mipi_dsi_detach(struct mipi_dsi_device *dsi);
int mipi_dsi_enslave(struct mipi_dsi_device *master,
		     struct mipi_dsi_device *slave);
int mipi_dsi_liberate(struct mipi_dsi_device *master,
		      struct mipi_dsi_device *slave);

/**
 * @brief mipi_dsi_dcs_tear_mode - Tearing Effect Output Line mode
 * @var mipi_dsi_dcs_tear_mode::MIPI_DSI_DCS_TEAR_MODE_VBLANK
 *      The TE output line consists of V-Blanking information only
 * @var mipi_dsi_dcs_tear_mode::MIPI_DSI_DCS_TEAR_MODE_VHBLANK
 *      The TE output line consists of both V-Blanking and H-Blanking
 *      information
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
#define MIPI_CAL_CTRL_NOISE_FILTER(x)	(((x) & 0xf) << 26)
#define MIPI_CAL_CTRL_PRESCALE(x)	(((x) & 0x3) << 24)
#define MIPI_CAL_CTRL_CLKEN_OVR		(1 << 4)
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
#define MIPI_CAL_CONFIG_CSIF		0x0a
#define MIPI_CAL_CONFIG_DSIA		0x0e
#define MIPI_CAL_CONFIG_DSIB		0x0f
#define MIPI_CAL_CONFIG_DSIC		0x10
#define MIPI_CAL_CONFIG_DSID		0x11

#define MIPI_CAL_CONFIG_DSIA_CLK	0x19
#define MIPI_CAL_CONFIG_DSIB_CLK	0x1a
#define MIPI_CAL_CONFIG_CSIAB_CLK	0x1b
#define MIPI_CAL_CONFIG_DSIC_CLK	0x1c
#define MIPI_CAL_CONFIG_CSICD_CLK	0x1c
#define MIPI_CAL_CONFIG_DSID_CLK	0x1d
#define MIPI_CAL_CONFIG_CSIE_CLK	0x1d

/* for data and clock lanes */
#define MIPI_CAL_CONFIG_SELECT		(1 << 21)

/* for data lanes */
#define MIPI_CAL_CONFIG_HSPDOS(x)	(((x) & 0x1f) << 16)
#define MIPI_CAL_CONFIG_HSPUOS(x)	(((x) & 0x1f) <<  8)
#define MIPI_CAL_CONFIG_TERMOS(x)	(((x) & 0x1f) <<  0)

/* for clock lanes */
#define MIPI_CAL_CONFIG_HSCLKPDOSD(x)	(((x) & 0x1f) <<  8)
#define MIPI_CAL_CONFIG_HSCLKPUOSD(x)	(((x) & 0x1f) <<  0)

#define MIPI_CAL_BIAS_PAD_CFG0		0x16
#define MIPI_CAL_BIAS_PAD_PDVCLAMP	(1 << 1)
#define MIPI_CAL_BIAS_PAD_E_VCLAMP_REF	(1 << 0)

#define MIPI_CAL_BIAS_PAD_CFG1		0x17
#define MIPI_CAL_BIAS_PAD_DRV_DN_REF(x) (((x) & 0x7) << 16)
#define MIPI_CAL_BIAS_PAD_DRV_UP_REF(x) (((x) & 0x7) << 8)

#define MIPI_CAL_BIAS_PAD_CFG2		0x18
#define MIPI_CAL_BIAS_PAD_VCLAMP(x)	(((x) & 0x7) << 16)
#define MIPI_CAL_BIAS_PAD_VAUXP(x)	(((x) & 0x7) << 4)
#define MIPI_CAL_BIAS_PAD_PDVREG	(1 << 1)

struct tegra_mipi_pad {
	unsigned long data;
	unsigned long clk;
};

struct tegra_mipi_soc {
	int has_clk_lane;
	const struct tegra_mipi_pad *pads;
	unsigned int num_pads;

	int clock_enable_override;
	int needs_vclamp_ref;

	/* bias pad configuration settings */
	u8 pad_drive_down_ref;
	u8 pad_drive_up_ref;

	u8 pad_vclamp_level;
	u8 pad_vauxp_level;

	/* calibration settings for data lanes */
	u8 hspdos;
	u8 hspuos;
	u8 termos;

	/* calibration settings for clock lanes */
	u8 hsclkpdos;
	u8 hsclkpuos;
};

struct tegra_mipi {
	const struct tegra_mipi_soc *soc;
	void *regs;
};

struct tegra_mipi_device {
	struct tegra_mipi *mipi;
	unsigned long pads;
};

struct tegra_mipi_device *tegra_mipi_request(struct tegra_mipi_device *device,
						int device_index);
int tegra_mipi_calibrate(struct tegra_mipi_device *device);
#endif /* __SOC_MIPI_DSI_H__ */
