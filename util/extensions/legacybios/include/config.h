/* tag: compile time configuration options
 *
 * Copyright (C) 2003 Patrick Mauritz, Stefan Reinauer
 *
 * See the file "COPYING" for further information about
 * the copyright and warranty status of this work.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* Console options
 * 
 * DEBUG_CONSOLE_VGA
 *   use vga textmode and keyboard console
 *
 * DEBUG_CONSOLE_SERIAL
 *   use serial console. If this is enabled, see 
 *   options below.
 *
 * SERIAL_PORT:
 *   0 for none, 1 for ttyS0, 2 for ttyS1
 *
 * SERIAL_SPEED:
 *   supported speeds are: 115200, 57600, 38400, 19200, 9600
 */

#define DEBUG_CONSOLE_SERIAL
// #define DEBUG_CONSOLE_VGA

#define SERIAL_PORT  1
#define SERIAL_SPEED 115200


/* Debugging switches
 * 
 * these switches enable debugging code snippets spread all over 
 * the code. You should not need to change these settings unless
 * you know what you are doing.
 * 
 * DEBUG_BOOT: 
 *   early boot code (multiboot parsing etc)
 *
 * DEBUG_CONSOLE:
 *   use builtin C console code for user interaction. There is no
 *   real alternative to this until someone writes a display/kbd or
 *   serial driver in forth.
 */

#if 0
#define DEBUG_BOOT
#endif
#define DEBUG_CONSOLE

#endif
