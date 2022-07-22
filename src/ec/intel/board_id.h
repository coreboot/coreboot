/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _MAINBOARD_COMMON_BOARD_ID_H_
#define _MAINBOARD_COMMON_BOARD_ID_H_

/* Board/FAB ID Command */
#define EC_FAB_ID_CMD	0x0d
/* Bit 5:0 for Board ID */
#define BOARD_ID_MASK	0x3f

/*
 * Returns board information (board id[15:8] and
 * Fab info[7:0]) on success and < 0 on error
 */
int get_rvp_board_id(void);

#endif /* _MAINBOARD_COMMON_BOARD_ID_H_ */
