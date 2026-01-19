/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/cb_err.h>
#include <commonlib/bsd/stdlib.h>
#include <commonlib/mipi/cmd.h>
#include <delay.h>

enum cb_err mipi_panel_parse_commands(const void *buf, mipi_cmd_func_t cmd_func,
				      void *user_data)
{
	const struct panel_command *command = buf;
	enum mipi_dsi_transaction type;

	/*
	 * The given commands should be in a buffer containing a packed array of
	 * panel_command and each element may be in variable size so we have
	 * to parse and scan.
	 */

	for (; command->cmd != PANEL_CMD_END; command = buf) {
		/*
		 * For some commands like DELAY, the command->len should not be
		 * counted for buf.
		 */
		buf += sizeof(*command);

		u32 cmd = command->cmd, len = command->len;

		if (cmd == PANEL_CMD_DELAY) {
			mdelay(len);
			continue;
		}

		switch (cmd) {
		case PANEL_CMD_DCS:
			switch (len) {
			case 0:
				printk(BIOS_ERR, "%s: DCS command length 0?\n", __func__);
				return CB_ERR;
			case 1:
				type = MIPI_DSI_DCS_SHORT_WRITE;
				break;
			case 2:
				type = MIPI_DSI_DCS_SHORT_WRITE_PARAM;
				break;
			default:
				type = MIPI_DSI_DCS_LONG_WRITE;
				break;
			}
			break;
		case PANEL_CMD_GENERIC:
			switch (len) {
			case 0:
				type = MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM;
				break;
			case 1:
				type = MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM;
				break;
			case 2:
				type = MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM;
				break;
			default:
				type = MIPI_DSI_GENERIC_LONG_WRITE;
				break;
			}
			break;
		default:
			printk(BIOS_ERR, "%s: Unknown command code: %d, "
			       "abort panel initialization.\n", __func__, cmd);
			return CB_ERR;
		}

		enum cb_err ret = cmd_func(type, command->data, len, user_data);
		if (ret != CB_SUCCESS)
			return ret;
		buf += len;
	}

	return CB_SUCCESS;
}

size_t mipi_panel_get_commands_len(const void *buf)
{
	const void *const buf_start = buf;
	const struct panel_command *command = buf;

	if (command->cmd == PANEL_CMD_END)
		return 0;

	for (; command->cmd != PANEL_CMD_END; command = buf) {
		buf += sizeof(*command);

		switch (command->cmd) {
		case PANEL_CMD_DELAY:
			/*
			 * For PANEL_CMD_DELAY, the command->len should not be
			 * counted for buf.
			 */
			break;
		case PANEL_CMD_GENERIC:
		case PANEL_CMD_DCS:
			buf += command->len;
			break;
		default:
			printk(BIOS_ERR, "%s: Unknown command code: %d.\n",
			       __func__, command->cmd);
			return 0;
		}
	}

	/* Add 1 byte for PANEL_CMD_END. */
	return buf - buf_start + 1;
}
