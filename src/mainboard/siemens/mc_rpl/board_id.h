/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MAINBOARD_COMMON_BOARD_ID_H_
#define _MAINBOARD_COMMON_BOARD_ID_H_

/*
 * Returns board information (board id[15:8] and
 * Fab info[7:0]) on success and < 0 on error
 */
int get_board_id(void);

#endif /* _MAINBOARD_COMMON_BOARD_ID_H_ */
