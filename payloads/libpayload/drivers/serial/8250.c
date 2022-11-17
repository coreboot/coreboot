/*
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 Ulf Jordan <jordan@chalmers.se>
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

#include <libpayload-config.h>
#include <libpayload.h>

static struct cb_serial cb_serial;

#define IOBASE cb_serial.baseaddr
#define MEMBASE (phys_to_virt(IOBASE))

static int serial_hardware_is_present = 0;
static int serial_is_mem_mapped = 0;

static uint8_t serial_read_reg(int offset)
{
	offset *= cb_serial.regwidth;

#if CONFIG(LP_IO_ADDRESS_SPACE)
	if (!serial_is_mem_mapped)
		return inb(IOBASE + offset);
	else
#endif
		if (cb_serial.regwidth == 4)
			return readl(MEMBASE + offset) & 0xff;
		else
			return readb(MEMBASE + offset);
}

static void serial_write_reg(uint8_t val, int offset)
{
	offset *= cb_serial.regwidth;

#if CONFIG(LP_IO_ADDRESS_SPACE)
	if (!serial_is_mem_mapped)
		outb(val, IOBASE + offset);
	else
#endif
		if (cb_serial.regwidth == 4)
			writel(val & 0xff, MEMBASE + offset);
		else
			writeb(val, MEMBASE + offset);
}

#if CONFIG(LP_SERIAL_SET_SPEED)
static void serial_hardware_init(int speed, int word_bits,
				 int parity, int stop_bits)
{
#if !CONFIG(LP_PL011_SERIAL_CONSOLE)
	unsigned char reg;

	/* Disable interrupts. */
	serial_write_reg(0, 0x01);

	/* Assert RTS and DTR. */
	serial_write_reg(3, 0x04);

	/* Set the divisor latch. */
	reg = serial_read_reg(0x03);
	serial_write_reg(reg | 0x80, 0x03);

	/* Write the divisor. */
	uint16_t divisor = 115200 / speed;
	serial_write_reg(divisor & 0xFF, 0x00);
	serial_write_reg(divisor >> 8, 0x01);

	/* Restore the previous value of the divisor.
	 * And set 8 bits per character */
	serial_write_reg((reg & ~0x80) | 3, 0x03);
#endif
}
#endif

static struct console_input_driver consin = {
	.havekey = &serial_havechar,
	.getchar = &serial_getchar,
	.input_type = CONSOLE_INPUT_TYPE_UART,
};

static struct console_output_driver consout = {
	.putchar = &serial_putchar
};

void serial_init(void)
{
	serial_is_mem_mapped = (cb_serial.type == CB_SERIAL_TYPE_MEMORY_MAPPED);

	if (!serial_is_mem_mapped) {
#if CONFIG(LP_IO_ADDRESS_SPACE)
		if ((inb(IOBASE + 0x05) == 0xFF) &&
				(inb(IOBASE + 0x06) == 0xFF)) {
			printf("IO space mapped serial not present.");
			return;
		}
#else
		printf("IO space mapped serial not supported.");
		return;
#endif
	}

#if CONFIG(LP_SERIAL_SET_SPEED)
	serial_hardware_init(CONFIG_LP_SERIAL_BAUD_RATE, 8, 0, 1);
#endif
	serial_hardware_is_present = 1;
}

void serial_console_init(void)
{
	if (!lib_sysinfo.cb_serial)
		return;
	cb_serial = *(struct cb_serial *)phys_to_virt(lib_sysinfo.cb_serial);

	serial_init();

	console_add_input_driver(&consin);
	console_add_output_driver(&consout);
}

void serial_putchar(unsigned int c)
{
	if (!serial_hardware_is_present)
		return;
#if !CONFIG(LP_PL011_SERIAL_CONSOLE)
	while ((serial_read_reg(0x05) & 0x20) == 0) ;
#endif
	serial_write_reg(c, 0x00);
	if (c == '\n')
		serial_putchar('\r');
}

int serial_havechar(void)
{
	if (!serial_hardware_is_present)
		return 0;
	return serial_read_reg(0x05) & 0x01;
}

int serial_getchar(void)
{
	if (!serial_hardware_is_present)
		return -1;
	while (!serial_havechar()) ;
	return serial_read_reg(0x00);
}
