/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MIPI_PANEL_H__
#define __MIPI_PANEL_H__

#include <edid.h>
#include <mipi/dsi.h>
#include <types.h>

/* Definitions for cmd in panel_init_command */
enum panel_init_cmd {
	PANEL_CMD_END = 0,
	PANEL_CMD_DELAY = 1,
	PANEL_CMD_GENERIC = 2,
	PANEL_CMD_DCS = 3,
};

struct panel_init_command {
	u8 cmd;
	u8 len;
	u8 data[];
};

/*
 * The data to be serialized and put into CBFS.
 * Note some fields, for example edid.mode.name, were actually pointers and
 * cannot be really serialized.
 */
struct panel_serializable_data {
	struct edid edid;  /* edid info of this panel */
	u8 init[]; /* A packed array of panel_init_command */
};

typedef enum cb_err (*mipi_cmd_func_t)(enum mipi_dsi_transaction type, const u8 *data, u8 len);

/* Parse a command array and call cmd_func() for each entry. Delays get handled internally. */
enum cb_err mipi_panel_parse_init_commands(const void *buf, mipi_cmd_func_t cmd_func);

#define PANEL_DCS(...) \
	PANEL_CMD_DCS, \
	sizeof((u8[]){__VA_ARGS__}), \
	__VA_ARGS__

#define PANEL_GENERIC(...) \
	PANEL_CMD_GENERIC, \
	sizeof((u8[]){__VA_ARGS__}), \
	__VA_ARGS__

#define PANEL_DELAY(delay) \
	PANEL_CMD_DELAY, \
	delay

#define PANEL_END \
	PANEL_CMD_END

#endif /* __MIPI_PANEL_H__ */
