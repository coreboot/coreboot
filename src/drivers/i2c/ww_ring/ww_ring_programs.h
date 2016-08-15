/*
 * Copyright (C) 2015 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * This is a driver for the Whirlwind LED ring, which is equipped with two LED
 * microcontrollers TI LP55231 (http://www.ti.com/product/lp55231), each of
 * them driving three multicolor LEDs.
 *
 * The only connection between the ring and the main board is an i2c bus.
 *
 * This driver imitates a depthcharge display device. On initialization the
 * driver sets up the controllers to prepare them to accept programs to run.
 *
 * When a certain vboot state needs to be indicated, the program for that
 * state is loaded into the controllers, resulting in the state appropriate
 * LED behavior.
 */

#ifndef __THIRD_PARTY_COREBOOT_SRC_DRIVERS_I2C_WW_RING_WW_RING_PROGRAMS_H__
#define __THIRD_PARTY_COREBOOT_SRC_DRIVERS_I2C_WW_RING_WW_RING_PROGRAMS_H__

#include <stdint.h>
#include "drivers/i2c/ww_ring/ww_ring.h"

/* There are threee independent engines/cores in the controller. */
#define LP55231_NUM_OF_ENGINES 3

/* Number of lp55321 controllers on the ring */
#define WW_RING_NUM_LED_CONTROLLERS 1

/*
 * Structure to describe an lp55231 program: pointer to the text of the
 * program, its size and load address (load addr + size sould not exceed
 * LP55231_MAX_PROG_SIZE), and start addresses for all of the three
 * engines.
 */
typedef struct {
	const uint8_t *program_text;
	uint8_t program_size;
	uint8_t  load_addr;
	uint8_t  engine_start_addr[LP55231_NUM_OF_ENGINES];
} TiLp55231Program;

/* A structure to bind controller programs to a vboot state. */
typedef struct {
	enum display_pattern   led_pattern;
	const TiLp55231Program *programs[WW_RING_NUM_LED_CONTROLLERS];
} WwRingStateProg;

extern const WwRingStateProg wwr_state_programs[];

#endif
