/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __COMMONLIB_MIPI_CMD_H__
#define __COMMONLIB_MIPI_CMD_H__

#include <commonlib/bsd/cb_err.h>
#include <commonlib/mipi/dsi.h>
#include <stdint.h>

/* Definitions for cmd in panel_command */
enum panel_cmd {
	PANEL_CMD_END = 0,
	PANEL_CMD_DELAY = 1,
	PANEL_CMD_GENERIC = 2,
	PANEL_CMD_DCS = 3,
};

struct panel_command {
	u8 cmd;
	u8 len;
	u8 data[];
};

#define PANEL_DELAY(delay) \
	PANEL_CMD_DELAY, \
	delay

#define PANEL_GENERIC(...) \
	PANEL_CMD_GENERIC, \
	sizeof((u8[]){__VA_ARGS__}), \
	__VA_ARGS__

#define PANEL_DCS(...) \
	PANEL_CMD_DCS, \
	sizeof((u8[]){__VA_ARGS__}), \
	__VA_ARGS__

#define PANEL_END \
	PANEL_CMD_END

/*
 * Callback function type for mipi_panel_parse_commands().
 * @param type		MIPI DSI transaction type.
 * @param data		panel_command data.
 * @param len		panel_command len.
 * @param user_data	Arbitrary user data passed from mipi_panel_parse_commands().
 */
typedef enum cb_err (*mipi_cmd_func_t)(enum mipi_dsi_transaction type, const u8 *data, u8 len,
				       void *user_data);

/* Parse a command array and call cmd_func() for each entry. Delays get handled internally. */
enum cb_err mipi_panel_parse_commands(const void *buf, mipi_cmd_func_t cmd_func,
				      void *user_data);

#endif /* __COMMONLIB_MIPI_CMD_H__ */
