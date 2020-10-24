/*
 *
 * Copyright 2018 Google LLC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __DRIVERS_I8042_I8042_H__
#define __DRIVERS_I8042_I8042_H__

/* Port 0x64 commands */
#define I8042_CMD_RD_CMD_BYTE		0x20
#define I8042_CMD_WR_CMD_BYTE		0x60
#define   I8042_CMD_BYTE_XLATE		(1 << 6)
#define I8042_CMD_DIS_AUX		0xa7
#define I8042_CMD_EN_AUX		0xa8
#define I8042_CMD_AUX_TEST		0xa9
#define I8042_CMD_SELF_TEST		0xaa
#define   I8042_SELF_TEST_RSP		0x55
#define I8042_CMD_KB_TEST		0xab
#define I8042_CMD_DIAG_DUMP		0xac
#define I8042_CMD_DIS_KB		0xad
#define I8042_CMD_EN_KB			0xae
#define I8042_CMD_RD_INPUT_PORT		0xc0
#define I8042_CMD_RD_OUTPUT_PORT	0xd0
#define I8042_CMD_WR_OUTPUT_PORT	0xd1
#define I8042_CMD_RD_TEST_INPUTS	0xe0

/* Port 0x60 keyboard commands */
#define I8042_KBCMD_SET_MODE_IND	0xed
#define   I8042_MODE_CAPS_LOCK_ON	(1 << 2)
#define   I8042_MODE_CAPS_LOCK_OFF	(0 << 2)
#define   I8042_MODE_NUM_LOCK_ON	(1 << 1)
#define   I8042_MODE_NUM_LOCK_OFF	(0 << 1)
#define   I8042_MODE_SCROLL_LOCK_ON	(1 << 0)
#define   I8042_MODE_SCROLL_LOCK_OFF	(0 << 0)
#define I8042_KBCMD_SET_SCANCODE	0xf0
#define I8042_KBCMD_SET_TYPEMATIC	0xf3
#define I8042_KBCMD_EN			0xf4
#define I8042_KBCMD_DEFAULT_DIS		0xf5
#define I8042_KBCMD_SET_DEFAULT		0xf6
#define I8042_KBCMD_RESEND		0xfe
#define I8042_KBCMD_RESET		0xff

#endif /* __DRIVERS_I8042_I8042_H__ */
