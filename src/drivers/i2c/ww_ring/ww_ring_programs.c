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

#include "drivers/i2c/ww_ring/ww_ring_programs.h"

/****************************************************************
 *   LED ring program definitions for different patterns.
 *
 * Comments below are real lp55231 source code, they are compiled using
 * lasm.exe, the TI tool available from their Web site (search for lp55231)
 * and running only on Windows :P.
 *
 * Different hex dumps are results of tweaking the source code parameters to
 * achieve desirable LED ring behavior. It is possible to use just one code
 * dump and replace certain values in the body to achieve different behaviour
 * with the same basic dump, but keeping track of location to tweak with every
 * code change would be quite tedious.
 */

/*
 * Solid LED display, the arguments of the set_pwm commands set intensity and
 * color of the display:

row_red:   dw 0000000000000001b
row_green: dw 0000000000000010b
row_blue:  dw 0000000000000100b

.segment program1
	mux_map_addr row_red
	set_pwm 0
	end

.segment program2
	mux_map_addr row_green
	set_pwm 0
	end

.segment program3
	mux_map_addr row_blue
	set_pwm 0
	end
*/

/* RGB set to 000000, resulting in all LEDs off. */
static const uint8_t solid_000000_text[] = {
	0x00, 0x01, 0x00, 0x02, 0x00, 0x04, 0x9F, 0x80,
	0x40,    0, 0xC0, 0x00, 0x9F, 0x81, 0x40,    0,
	0xC0, 0x00, 0x9F, 0x82, 0x40,    0, 0xC0, 0x00
};

/* Rgb set to 128, resulting in a brightish white color. */
static const uint8_t solid_808080_text[] = {
	0x00, 0x01, 0x00, 0x02, 0x00, 0x04, 0x9F, 0x80,
	0x40,  128, 0xC0, 0x00, 0x9F, 0x81, 0x40,  128,
	0xC0, 0x00, 0x9F, 0x82, 0x40,  128, 0xC0, 0x00
};

static const TiLp55231Program solid_808080_program = {
	solid_808080_text,
	sizeof(solid_808080_text),
	0,
	{ 3, 6, 9 }
};

static const TiLp55231Program solid_000000_program = {
	solid_000000_text,
	sizeof(solid_000000_text),
	0,
	{ 3, 6, 9 }
};

/*
 * Blinking patterns are trickier then solid ones.
 *
 * The three internal engines seem to be competing for resources and get out
 * of sync in seconds if left running asynchronously.
 *
 * When solid patterns are deployed with instanteneous color intensity
 * changes, all three LEDs can be controlled by one engine in sequential
 * accesses. But the controllers still neeed to be synchronized.
 *
 * The maximum timer duration of lp55231 is .48 seconds. To achieve longer
 * blinking intervals the loops delays are deployed. Only the first controller
 * intervals need to be changed, as the second one is in lockstep with the
 * first.
 *
 * The time granularity is set at .1 second (see commands 'wait 0.1' in the
 * code), and then the loop counters can be set up to 63 (registers rb and rc),
 * which allows to generate intervals up to 6.3 seconds in .1 second
 * increments.
 */
/*
 * blink_solid1.src
row_red:   dw 0000000000000001b
row_green: dw 0000000000000010b
row_blue:  dw 0000000000000100b

.segment program1
	ld ra, 2   # LED on duration
	ld rb, 2   # LED off duration
	mux_map_addr row_red
loop:
	ld rc, 1        ; red intensity
	set_pwm rc
	mux_map_addr row_green
	ld rc, 50       ; green intensity
	set_pwm rc
	mux_map_addr row_blue
	ld rc, 155      ; blue intensity
	set_pwm rc
wait1:
	wait 0.1
	branch ra, wait1
	set_pwm 0
	mux_map_addr row_green
	set_pwm 0
	mux_map_addr row_red
	set_pwm 0
wait2:
	wait 0.1
	branch rb, wait2
	branch 0, loop
.segment program2
	 end
.segment program3
	 end
*/
static const uint8_t blink_wipeout1_text[] = {
	0x00,  0x01,  0x00,  0x02,  0x00,  0x04,  0x90,  0x02,
	0x94,  0x02,  0x9f,  0x80,  0x98,     1,  0x84,  0x62,
	0x9f,  0x81,  0x98,    50,  0x84,  0x62,  0x9f,  0x82,
	0x98,   155,  0x84,  0x62,  0x4c,  0x00,  0x86,  0x2c,
	0x40,  0x00,  0x9f,  0x81,  0x40,  0x00,  0x9f,  0x80,
	0x40,  0x00,  0x4c,  0x00,  0x86,  0x49,  0xa0,  0x03,
	0xc0,  0x00,  0xc0,  0x00,  0x00,
};

static const TiLp55231Program blink_wipeout1_program = {
	blink_wipeout1_text,
	sizeof(blink_wipeout1_text),
	0,
	{ 3,  24,  25,  }
};

static const uint8_t blink_recovery1_text[] = {
	0x00,  0x01,  0x00,  0x02,  0x00,  0x04,  0x90,  0x02,
	0x94,  0x02,  0x9f,  0x80,  0x98,   255,  0x84,  0x62,
	0x9f,  0x81,  0x98,    50,  0x84,  0x62,  0x9f,  0x82,
	0x98,     0,  0x84,  0x62,  0x4c,  0x00,  0x86,  0x2c,
	0x40,  0x00,  0x9f,  0x81,  0x40,  0x00,  0x9f,  0x80,
	0x40,  0x00,  0x4c,  0x00,  0x86,  0x49,  0xa0,  0x03,
	0xc0,  0x00,  0xc0,  0x00,  0x00,
};
static const TiLp55231Program blink_recovery1_program = {
	blink_recovery1_text,
	sizeof(blink_recovery1_text),
	0,
	{ 3,  24,  25,  }
};

/*
 * fade_in1.src
 *
 row_red:   dw 0000000000000001b
 row_green: dw 0000000000000010b
 row_blue:  dw 0000000000000100b

 .segment program1
	mux_map_addr row_red
	set_pwm 1h
	trigger s{2|3}
	end
 .segment program2
	mux_map_addr row_green
	set_pwm 0h
	trigger w{1}
	ramp 2, 50
	end
.segment program3
	mux_map_addr row_blue
	set_pwm 0h
 loop3: trigger w{1}
	ramp 2, 155
	end
*/

static const uint8_t fade_in1_text[] = {
	0x00,  0x01,  0x00,  0x02,  0x00,  0x04,  0x9f,  0x80,
	0x40,  0x01,  0xe0,  0x0c,  0xc0,  0x00,  0x9f,  0x81,
	0x40,  0x00,  0xe0,  0x80,  0x46,  0x32,  0xc0,  0x00,
	0x9f,  0x82,  0x40,  0x00,  0xe0,  0x80,  0x34,  0x9B,
	0xc0,  0x00,  0x00,
};
static const TiLp55231Program fade_in1_program = {
	fade_in1_text,
	sizeof(fade_in1_text),
	0,
	{ 3,  7,  12,  }
};

const WwRingStateProg wwr_state_programs[] = {
	/*
	 * for test purposes the blank screen program is set to blinking, will
	 * be changed soon.
	 */
	{WWR_ALL_OFF, {&solid_000000_program} },
	{WWR_RECOVERY_PUSHED, {&solid_808080_program} },
	{WWR_WIPEOUT_REQUEST, {&blink_wipeout1_program} },
	{WWR_RECOVERY_REQUEST, {&blink_recovery1_program} },
	{WWR_NORMAL_BOOT, {&fade_in1_program} },
	{}, /* Empty record to mark the end of the table. */
};
