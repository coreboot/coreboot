/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __COMMON_INCLUDE_MTK_FSP_COMMON_H__
#define __COMMON_INCLUDE_MTK_FSP_COMMON_H__

#include <stdarg.h>
#include <stdint.h>

#define INTF_MAJOR_VER 1
#define INTF_MINOR_VER 0

enum fsp_status {
	FSP_STATUS_SUCCESS = 0,
	FSP_STATUS_INVALID_VERSION,
	FSP_STATUS_INVALID_PHASE,
	FSP_STATUS_PARAM_NOT_FOUND,
	FSP_STATUS_PARAM_INVALID_SIZE,
	FSP_STATUS_INVALID_STORAGE,
	FSP_STATUS_INVALID_PI_IMG,
};

enum fsp_phase {
	ROMSTAGE_INIT = 0x30,
	RAMSTAGE_MAINBOARD_INIT = 0x40,
	RAMSTAGE_SOC_INIT = 0x50,
};

enum fsp_param_io {
	FSP_PARAM_IO_INVALID = 0,
	FSP_PARAM_IO_IN,
	FSP_PARAM_IO_OUT,
	FSP_PARAM_IO_INOUT,
};

#define FSP_PARAM_IO_ENCODE(x)	((uint32_t)(x) << 30)

enum fsp_param_type {
	FSP_PARAM_TYPE_INVALID = 0,
	/* 0x40000000+ reserved for input type params */
	FSP_PARAM_TYPE_IN = FSP_PARAM_IO_ENCODE(FSP_PARAM_IO_IN),
	FSP_PARAM_TYPE_STORAGE,
	FSP_PARAM_TYPE_PI_IMG,
	FSP_PARAM_TYPE_PI_IMG_CSRAM,

	/* 0x80000000+ reserved for output type params */
	FSP_PARAM_TYPE_OUT = FSP_PARAM_IO_ENCODE(FSP_PARAM_IO_OUT),

	/* 0xC0000000+ reserved for the params that support both input and output */
	FSP_PARAM_TYPE_INOUT = FSP_PARAM_IO_ENCODE(FSP_PARAM_IO_INOUT),
};

struct mtk_fsp_intf {
	uint8_t major_version;
	uint8_t minor_version;
	uint16_t header_size;
	uint32_t phase; /* enum fsp_phase */
	int32_t status; /* enum fsp_status */
	uint16_t entry_size;
	uint8_t num_entries;
	uint8_t reserved;
	int (*do_vprintf)(const char *fmt, va_list args);
	struct mtk_fsp_param {
		uint32_t param_type;
		uint32_t param_size;
		void *param;
	} entries[];
};

#endif /* __COMMON_INCLUDE_MTK_FSP_COMMON_H__ */
