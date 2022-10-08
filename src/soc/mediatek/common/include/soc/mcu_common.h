/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MTLIB_COMMON_H
#define SOC_MEDIATEK_MTLIB_COMMON_H

struct mtk_mcu {
	const char *firmware_name;	/* The firmware file name in CBFS */
	void *run_address;		/* The address for running the firmware */
	size_t run_size;		/* The buffer for loading the firmware */
	void *load_buffer;		/* The buffer size */
	size_t buffer_size;		/* The firmware real size */
	void *priv;			/* The additional data required by the reset callback */
	void (*reset)(struct mtk_mcu *mcu);	/* The reset callback */
};

enum cb_err mtk_init_mcu(struct mtk_mcu *mcu);

#endif /* SOC_MEDIATEK_MTLIB_COMMON_H */
