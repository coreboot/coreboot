/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <console/simnow.h>

#define AMD_SIMNOW_PORT			0x80
#define AMD_SIMNOW_PORT_DATA_BEGIN	0x5f535452ul
#define AMD_SIMNOW_PORT_DATA_END	0x5f454e44ul

void simnow_console_init(void)
{
	outl(AMD_SIMNOW_PORT_DATA_BEGIN, AMD_SIMNOW_PORT);
}

void simnow_console_tx_byte(unsigned char data)
{
	outb(data, AMD_SIMNOW_PORT);

	if (data == '\n') {
		outl(AMD_SIMNOW_PORT_DATA_END, AMD_SIMNOW_PORT);
		outl(AMD_SIMNOW_PORT_DATA_BEGIN, AMD_SIMNOW_PORT);
	}
}
