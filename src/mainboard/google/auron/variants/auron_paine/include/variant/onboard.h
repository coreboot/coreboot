/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ONBOARD_H
#define ONBOARD_H

#define BOARD_TRACKPAD_NAME		"trackpad"
#define BOARD_TRACKPAD_IRQ		37	/* PIRQV */
#define BOARD_TRACKPAD_WAKE_GPIO	12	/* GPIO12 */
#define BOARD_TRACKPAD_I2C_BUS		1	/* I2C0 */
#define BOARD_TRACKPAD_I2C_ADDR		0x15

#define BOARD_WLAN_WAKE_GPIO		10	/* GPIO10 */
#define BOARD_PP3300_CODEC_GPIO		45	/* GPIO45 */
#define BOARD_WLAN_DISABLE_GPIO		46	/* GPIO46 */

#endif
