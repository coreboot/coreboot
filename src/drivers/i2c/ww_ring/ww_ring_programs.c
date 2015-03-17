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

row_red:   dw 0000000001001001b
row_green: dw 0000000010010010b
row_blue:  dw 0000000100100100b

.segment program1
	mux_map_addr row_red
	set_pwm 1
	end

.segment program2
	mux_map_addr row_green
	set_pwm 1
	end

.segment program3
	mux_map_addr row_blue
	set_pwm 1
	end
*/

/* RGB set to 000000, resulting in all LEDs off. */
static const uint8_t solid_000000_text[] = {
	0x00, 0x49, 0x00, 0x92, 0x01, 0x24, 0x9F, 0x80,
	0x40, 0x00, 0xC0, 0x00, 0x9F, 0x81, 0x40, 0x00,
	0xC0, 0x00, 0x9F, 0x82, 0x40, 0x00, 0xC0, 0x00
};

/* RGB set to 010101, resulting in a bleak greyish color. */
static const uint8_t solid_010101_text[] = {
	0x00, 0x49, 0x00, 0x92, 0x01, 0x24, 0x9F, 0x80,
	0x40, 0x01, 0xC0, 0x00, 0x9F, 0x81, 0x40, 0x01,
	0xC0, 0x00, 0x9F, 0x82, 0x40, 0x01, 0xC0, 0x00
};

static const TiLp55231Program solid_010101_program = {
	solid_010101_text,
	sizeof(solid_010101_text),
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
 * Blinking patterns are much tricker then solid ones.
 *
 * The three internal engines seem to be competing for resources and get out
 * of sync in seconds if left running asynchronously.
 *
 * When there are two separate controllers, with three engine each, they all
 * run away from each other in no time, resulting in some completely chaotic
 * LED behavior.
 *
 * To keep the ring in check internal and external triggers are used which
 * makes programs for controller1 and controller2 sligtly different.
 *
 * The first controller is loaded and started first, but it sits waiting for
 * the trigger from the second controller to actually start the cycle.
 *
 * In the middle of the cycle the first controller sends a sync back to the
 * second one. Both controllers' engine1 also synchs up their respective
 * engines 2 and 3.
 *
 * The maximum timer duration of lp55231 is .48 seconds. To achieve longer
 * blinking intervals the loops delays are deployed.
 *
 * The granularity is set at .1 second (see commands 'wait 0.1' in the code,
 * and then the loop counters can be set up to 63 (registers rb and rc), which
 * allows to generate intervals up to 6.3 seconds in .1 second increments.
 */
/* blink_solid1.src
row_red:   dw 0000000001001001b
row_green: dw 0000000010010010b
row_blue:  dw 0000000100100100b

.segment program1
	ld ra, 255     ; red intensity
	ld rb, 2       ; up time 200 ms
	ld rc, 2       ; down time 200 ms

	mux_map_addr row_red
loop1:  trigger w{e}	; wait for external trigger from 2nd controller
	trigger s{2|3}
	set_pwm ra
common1:
	wait 0.1
	branch rb, common1
	trigger s{2|3|e}
	set_pwm 0
wait1:
	wait 0.1
	branch rc, wait1
	branch 0, loop1


.segment program2
	mux_map_addr row_green
	ld ra, 255	; green intensity
loop2:  trigger w{1}
	set_pwm ra
common2:
	; engine 2 and 3 intervals are controlled by sync with engine 1
	wait 0.1
	branch 1, common2
	trigger w{1}
	set_pwm 0
wait2:
	wait 0.1
	branch 1, wait2
	branch 0, loop2


.segment program3
	ld ra, 0	; blue intensity
loop3:  trigger w{1}
	set_pwm ra
common3:
	wait 0.1
	branch 1, common3
	trigger w{1}
	set_pwm 0
wait3:
	wait 0.1
	branch 1, wait3
	branch 0, loop3
*/
/*  blink_solid2.src
row_red:   dw 0000000001001001b
row_green: dw 0000000010010010b
row_blue:  dw 0000000100100100b

.segment program1
	ld ra, 255     ; red intensity
	ld rb, 2       ; up time
	ld rc, 2       ; down time
	mux_map_addr row_red
loop1:  trigger s{2|3|e} ; send trigger to own engines and the first controller
	set_pwm ra
common1:
	wait 0.1
	branch rb, common1
	trigger w{e}
	trigger s{2|3}
	set_pwm 0
wait1:
	wait 0.1
	branch rc, wait1
	branch 0, loop1


.segment program2
	mux_map_addr row_green
	ld ra, 255
loop2:  trigger w{1}
	set_pwm ra
common2:
	wait 0.1
	branch 1, common2
	trigger w{1}
	set_pwm 0
wait2:
	wait 0.1
	branch 1, wait2
	branch 0, loop2


.segment program3
	mux_map_addr row_blue
	ld ra, 0
loop3:  trigger w{1}
	set_pwm ra
common3:
	wait 0.1
	branch 1, common3
	trigger w{1}
	set_pwm 0
wait3:
	wait 0.1
	branch 1, wait3
	branch 0, loop3
 */
static const uint8_t blink_wipeout1_text[] = {
	0x00,  0x49,  0x00,  0x92,  0x01,  0x24,  0x90,  0xff,
	0x94,  0x02,  0x98,  0x02,  0x9f,  0x80,  0xf0,  0x00,
	0xe0,  0x0c,  0x84,  0x60,  0x4c,  0x00,  0x86,  0x1d,
	0xe0,  0x4c,  0x40,  0x00,  0x4c,  0x00,  0x86,  0x2e,
	0xa0,  0x04,  0x9f,  0x81,  0x90,  0xff,  0xe0,  0x80,
	0x84,  0x60,  0x4c,  0x00,  0xa0,  0x84,  0xe0,  0x80,
	0x40,  0x00,  0x4c,  0x00,  0xa0,  0x88,  0xa0,  0x02,
	0x9f,  0x82,  0x90,  0x00,  0xe0,  0x80,  0x84,  0x60,
	0x4c,  0x00,  0xa0,  0x84,  0xe0,  0x80,  0x40,  0x00,
	0x4c,  0x00,  0xa0,  0x88,  0xa0,  0x02,  0x00,  0x00,
};

static const uint8_t blink_wipeout2_text[] = {
	0x00,  0x49,  0x00,  0x92,  0x01,  0x24,  0x90,  0xff,
	0x94,  0x02,  0x98,  0x02,  0x9f,  0x80,  0xe0,  0x4c,
	0x84,  0x60,  0x4c,  0x00,  0x86,  0x19,  0xf0,  0x00,
	0xe0,  0x0c,  0x40,  0x00,  0x4c,  0x00,  0x86,  0x2e,
	0xa0,  0x04,  0x9f,  0x81,  0x90,  0xff,  0xe0,  0x80,
	0x84,  0x60,  0x4c,  0x00,  0xa0,  0x84,  0xe0,  0x80,
	0x40,  0x00,  0x4c,  0x00,  0xa0,  0x88,  0xa0,  0x02,
	0x9f,  0x82,  0x90,  0x00,  0xe0,  0x80,  0x84,  0x60,
	0x4c,  0x00,  0xa0,  0x84,  0xe0,  0x80,  0x40,  0x00,
	0x4c,  0x00,  0xa0,  0x88,  0xa0,  0x02,  0x00,  0x00,
};

static const TiLp55231Program blink_wipeout1_program = {
	blink_wipeout1_text,
	sizeof(blink_wipeout1_text),
	0,
	{ 3,  17,  28,  }
};
static const TiLp55231Program blink_wipeout2_program = {
	blink_wipeout2_text,
	sizeof(blink_wipeout2_text),
	0,
	{ 3,  17,  28,  }
};

static const uint8_t blink_recovery1_text[] = {
	0x00,  0x49,  0x00,  0x92,  0x01,  0x24,  0x90,  0xff,
	0x94,  0x02,  0x98,  0x02,  0x9f,  0x80,  0xf0,  0x00,
	0xe0,  0x0c,  0x84,  0x60,  0x4c,  0x00,  0x86,  0x1d,
	0xe0,  0x4c,  0x40,  0x00,  0x4c,  0x00,  0x86,  0x2e,
	0xa0,  0x04,  0x9f,  0x81,  0x90,  0x3d,  0xe0,  0x80,
	0x84,  0x60,  0x4c,  0x00,  0xa0,  0x84,  0xe0,  0x80,
	0x40,  0x00,  0x4c,  0x00,  0xa0,  0x88,  0xa0,  0x02,
	0x90,  0x00,  0xe0,  0x80,  0x84,  0x60,  0x4c,  0x00,
	0xa0,  0x83,  0xe0,  0x80,  0x40,  0x00,  0x4c,  0x00,
	0xa0,  0x87,  0xa0,  0x01,  0x00,  0x00,  0x00,
};
static const TiLp55231Program blink_recovery1_program = {
	blink_recovery1_text,
	sizeof(blink_recovery1_text),
	0,
	{ 3,  17,  28,  }
};
static const uint8_t blink_recovery2_text[] = {
	0x00,  0x49,  0x00,  0x92,  0x01,  0x24,  0x90,  0xff,
	0x94,  0x02,  0x98,  0x02,  0x9f,  0x80,  0xe0,  0x4c,
	0x84,  0x60,  0x4c,  0x00,  0x86,  0x19,  0xf0,  0x00,
	0xe0,  0x0c,  0x40,  0x00,  0x4c,  0x00,  0x86,  0x2e,
	0xa0,  0x04,  0x9f,  0x81,  0x90,  0x3d,  0xe0,  0x80,
	0x84,  0x60,  0x4c,  0x00,  0xa0,  0x84,  0xe0,  0x80,
	0x40,  0x00,  0x4c,  0x00,  0xa0,  0x88,  0xa0,  0x02,
	0x9f,  0x82,  0x90,  0x00,  0xe0,  0x80,  0x84,  0x60,
	0x4c,  0x00,  0xa0,  0x84,  0xe0,  0x80,  0x40,  0x00,
	0x4c,  0x00,  0xa0,  0x88,  0xa0,  0x02,  0x00,  0x00,
	0x00,
};
static const TiLp55231Program blink_recovery2_program = {
	blink_recovery2_text,
	sizeof(blink_recovery2_text),
	0,
	{ 3,  17,  28,  }
};


const WwRingStateProg wwr_state_programs[] = {
	/*
	 * for test purposes the blank screen program is set to blinking, will
	 * be changed soon.
	 */
	{WWR_ALL_OFF, {&solid_000000_program, &solid_000000_program} },
	{WWR_RECOVERY_PUSHED, {&solid_010101_program, &solid_010101_program} },
	{WWR_WIPEOUT_REQUEST, {&blink_wipeout1_program,
			       &blink_wipeout2_program} },
	{WWR_RECOVERY_REQUEST, {&blink_recovery1_program,
				&blink_recovery2_program} },
	{}, /* Empty record to mark the end of the table. */
};

