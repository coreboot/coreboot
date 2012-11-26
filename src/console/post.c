/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#if CONFIG_CMOS_POST
#include <pc80/mc146818rtc.h>
#endif
#include <elog.h>

/* Write POST information */

/* someday romcc will be gone. */
#ifndef __ROMCC__
/* Some mainboards have very nice features beyond just a simple display.
 * They can override this function.
 */
void __attribute__((weak)) mainboard_post(uint8_t value)
{
}

#else
/* This just keeps the number of #ifs to a minimum */
#define mainboard_post(x)
#endif

#if CONFIG_CMOS_POST

#if !defined(__PRE_RAM__)
void cmos_post_log(void)
{
	u8 code;

	/* Get post code from other bank */
	switch (cmos_read(CMOS_POST_BANK_OFFSET)) {
	case CMOS_POST_BANK_0_MAGIC:
		code = cmos_read(CMOS_POST_BANK_1_OFFSET);
		break;
	case CMOS_POST_BANK_1_MAGIC:
		code = cmos_read(CMOS_POST_BANK_0_OFFSET);
		break;
	default:
		return;
	}

	/* Check last post code in previous boot against normal list */
	switch (code) {
	case POST_OS_BOOT:
	case POST_OS_RESUME:
	case POST_ENTER_ELF_BOOT:
	case 0:
		break;
	default:
		printk(BIOS_WARNING, "POST: Unexpected post code "
		       "in previous boot: 0x%02x\n", code);
#if CONFIG_ELOG
		elog_add_event_word(ELOG_TYPE_LAST_POST_CODE, code);
#endif
	}
}
#endif /* !__PRE_RAM__ */

static void cmos_post_code(u8 value)
{
	switch (cmos_read(CMOS_POST_BANK_OFFSET)) {
	case CMOS_POST_BANK_0_MAGIC:
		cmos_write(value, CMOS_POST_BANK_0_OFFSET);
		break;
	case CMOS_POST_BANK_1_MAGIC:
		cmos_write(value, CMOS_POST_BANK_1_OFFSET);
		break;
	}
}
#endif /* CONFIG_CMOS_POST */

void post_code(uint8_t value)
{
#if !CONFIG_NO_POST
#if CONFIG_CONSOLE_POST
	print_emerg("POST: 0x");
	print_emerg_hex8(value);
	print_emerg("\n");
#endif
#if CONFIG_CMOS_POST
	cmos_post_code(value);
#endif
#if CONFIG_IO_POST
	outb(value, CONFIG_IO_POST_PORT);
#endif
#endif
	mainboard_post(value);
}
