/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/console.h>
#include <arch/io.h>
#include <delay.h>

#include <ec/acpi/ec.h>
#include "m3885.h"

#define TH0LOW	80
#define TH0HIGH	85
#define TH0CRIT	120
#define TH1LOW	75
#define TH1HIGH	80

static u8 variables[] = {
     /* Offs,  AND,   OR */
	0x08, 0x48, 0x6C,  /* Keyboard ScanCode Set & LED Data (kState1) */
	0x0a, 0x01, 0x00,  /* Keyboard Shift flags (kState3) */
	0x0c, 0x80, 0x08,  /* Keyboard State flags (kState5) */
	0x11, 0xff, 0x06,  /* Make/Break Debounce #'s (debounce) */
	0x13, 0xff, 0x00,  /* HotKey1 ScanCode (hotKey1) */
	0x14, 0xff, 0x00,  /* HotKey2 ScanCode (hotKey2) */
	0x15, 0xff, 0x3f,  /* HotKey3 ScanCode (hotKey3) */
	0x16, 0xff, 0x00,  /* HotKey4 ScanCode (hotKey4) */
	0x17, 0xff, 0x00,  /* HotKey5 ScanCode (hotKey5) */
	0x18, 0xff, 0x0e,  /* HotKey6 ScanCode (hotKey6) */
	0x19, 0xff, 0x9f,  /* HotKey1 Task = c5 Command Data (keyTsk1) */
	0x1a, 0xff, 0x9f,  /* HotKey2 Task = c5 Command Data (keyTsk2) */
	0x1b, 0xff, 0x6a,  /* HotKey3 Task = c5 Command Data (keyTsk3) */
	0x1c, 0xff, 0x9f,  /* HotKey4 Task = c5 Command Data (keyTsk4) */
	0x1d, 0xff, 0x9f,  /* HotKey5 Task = c5 Command Data (keyTsk5) */
	0x1e, 0xff, 0x87,  /* FuncKey Task = c5 Command Data (funcTsk) */
	0x1f, 0xff, 0x9f,  /* Delayed Task = c5 Command Data (dlyTsk1) */
	0x20, 0xff, 0x9f,  /* Wake-Up Task = c5 Command Data (wakeTsk) */

	0x21, 0xff, 0x08,  /* WigglePin Pulse Width * 2.4ms (tmPulse) */
	0x24, 0xff, 0x30,  /* Keyboard State Flags (kState7) */

	0x2b, 0xff, 0x00,
	0x2c, 0xff, 0x80,  /* Set Fn-Key 8 */
	0x2d, 0xff, 0x02,  /* Set Fn-Key 9 */
	0x2e, 0xff, 0x00,  /* Set Fn-Key 1-8 task  (0 = SMI) */
	0x2f, 0xff, 0x00,  /* Set Fn-Key 9-12 task (1 = SCI) */
};

static u8 matrix[] = {
	0xc1,0xc0,0xd8,0xdb,0xbf,0x05,0x76,0xbf,  /* (0x00-0x07) */
	0xbf,0x80,0x78,0xbf,0xbf,0x07,0x88,0xc2,  /* (0x08-0x0f) */
	0x03,0x09,0xd9,0x16,0xbf,0x06,0x0e,0x81,  /* (0x10-0x17) */
	0xbf,0xbf,0xee,0xbf,0xbf,0x55,0x9a,0x89,  /* (0x18-0x1f) */
	0x1e,0x15,0x36,0xda,0xe8,0xbf,0x0d,0xbf,  /* (0x20-0x27) */
	0xbf,0xbf,0xbf,0xa3,0xbf,0x4e,0x66,0x8b,  /* (0x28-0x2f) */
	0x1d,0x2e,0xe6,0xe7,0xe5,0x1c,0x58,0xbf,  /* (0x30-0x37) */
	0x82,0xbf,0xf0,0xbf,0xbf,0x5b,0x5d,0x8c,  /* (0x38-0x3f) */
	0x22,0x25,0x2c,0x35,0xe1,0x1a,0x96,0xbf,  /* (0x40-0x47) */
	0xbf,0xbf,0xec,0xbf,0xbf,0x54,0xf1,0x8f,  /* (0x48-0x4f) */
	0x1b,0x2a,0x2b,0x32,0xe9,0x31,0x29,0x61,  /* (0x50-0x57) */
	0xbf,0xbf,0x8d,0xbf,0x86,0xc3,0x92,0x93,  /* (0x58-0x5f) */
	0x21,0x23,0x34,0x33,0x41,0xe0,0xbf,0xbf,  /* (0x60-0x67) */
	0xbf,0x85,0xeb,0xbf,0xb6,0xbf,0x91,0xbf,  /* (0x68-0x6f) */
	0x26,0x24,0x2d,0xe3,0xe2,0xe4,0xbf,0xbf,  /* (0x70-0x77) */
	0x87,0xbf,0xea,0xbf,0xbf,0x52,0x90,0x8e,  /* (0x78-0x7f) */
};

static u8 function_ram[] = {
	0x04,0xbd,0x0c,0xbe,0x7e,0x9a,0x8a,0xb6,  /* (0xc0-0xc3) */
	0x92,0x8f,0x93,0x8e,0x81,0x86,0x82,0x87,  /* (0xc4-0xc7) */
	0x8a,0x9a,0x8d,0x7e,0x88,0x84,0x7e,0x78,  /* (0xc8-0xcb) */
	0x77,0x07,0x77,0x98,0x89,0xb2,0x05,0x9b,  /* (0xcc-0xcf) */
	0x78,0x84,0x07,0x88,0x8a,0x7e,0x05,0xa6,  /* (0xd0-0xd3) */
	0x06,0xa7,0x04,0xa8,0x0c,0xa9,0x03,0xaa,  /* (0xd4-0xd7) */
	0x0b,0xc1,0x83,0xc0,0x0a,0xad,0x01,0xae,  /* (0xd8-0xdb) */
	0x09,0xaf,0x78,0xb0,0x07,0xb1,0x1a,0x61,  /* (0xdc-0xdf) */
	0x3b,0x69,0x42,0x72,0x4b,0x7a,0x3c,0x6b,  /* (0xe0-0xe3) */
	0x43,0x73,0x44,0x74,0x3d,0x6c,0x3e,0x75,  /* (0xe4-0xe7) */
	0x46,0x7d,0x3a,0x70,0x49,0x71,0x4a,0x94,  /* (0xe8-0xeb) */
	0x4c,0x79,0x4c,0x7c,0x45,0x7c,0x45,0x79,  /* (0xec-0xef) */
	0x4d,0x7b,0x5a,0x95,0x4c,0x7b,0x45,0x7b,  /* (0xf0-0xf3) */
	0x4d,0x79,0x4d,0x7c,0x4e,0x7b,0x54,0x95,  /* (0xf4-0xf7) */
	0x52,0x7c,0x45,0x94,0x4a,0x79,0xb3,0x95,  /* (0xf8-0xfb) */
	0xb4,0x7b,0xb5,0x7c,0x00,0x00,0x55,0x79,  /* (0xfc-0xff) */
};

#define KBD_DATA	0x60
#define KBD_SC		0x64

#define   KBD_IBF	(1 << 1) /* 1: input buffer full (data ready for ec) */
#define   KBD_OBF	(1 << 0) /* 1: output buffer full (data ready for host) */

static int send_kbd_command(u8 command)
{
	int timeout;

	timeout = 0x7ff;
	while ((inb(KBD_SC) & KBD_IBF) && --timeout) {
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "Timeout while sending command 0x%02x to EC!\n",
				command);
	}

	outb(command, KBD_SC);
	return 0;
}

static int send_kbd_data(u8 data)
{
	int timeout;

	timeout = 0x7ff;
	while ((inb(KBD_SC) & KBD_IBF) && --timeout) { /* wait for IBF = 0 */
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "Timeout while sending data 0x%02x to EC!\n",
				data);
	}

	outb(data, KBD_DATA);

	return 0;
}

static u8 recv_kbd_data(void)
{
	int timeout;
	u8 data;

	timeout = 0x7fff;
	while (--timeout) { /* Wait for OBF = 1 */
		if (inb(KBD_SC) & KBD_OBF) {
			break;
		}
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "\nTimeout while receiving data from EC!\n");
	}

	data = inb(KBD_DATA);

	return data;
}

static u8 m3885_get_variable(u8 index)
{
	u8 ret;

	send_kbd_command(0xb8);
	send_kbd_data(index);
	send_kbd_command(0xbc);
	send_kbd_command(0xff);
	ret = recv_kbd_data();
	printk(BIOS_SPEW, "m3885: get variable %02x = %02x\n", index, ret);
	return ret;
}

static void m3885_set_variable(u8 index, u8 data)
{
	printk(BIOS_SPEW, "m3885: set variable %02x = %02x\n", index, data);
	send_kbd_command(0xb8);
	send_kbd_data(index);
	send_kbd_command(0xbd);
	send_kbd_data(data);
}

static void m3885_set_proc_ram(u8 index, u8 data)
{
	printk(BIOS_SPEW, "m3885: set procram %02x = %02x\n", index, data);
	send_kbd_command(0xb8);
	send_kbd_data(index);
	send_kbd_command(0xbb);
	send_kbd_data(data);
}

static u8 m3885_get_proc_ram(u8 index)
{
	u8 ret;

	send_kbd_command(0xb8);
	send_kbd_data(index);
	send_kbd_command(0xba);
	ret = recv_kbd_data();
	printk(BIOS_SPEW, "m3885: get procram %02x = %02x\n", index, ret);
	return ret;
}

static u8 m3885_read_port(void)
{
	u8 reg8;

	reg8 = m3885_get_variable(0x0c);
	reg8 &= ~(7 << 4);
	reg8 |= (4 << 4);	/* bank 4 */
	m3885_set_variable(0x0c, reg8);

	/* P6YSTATE */
	return m3885_get_proc_ram(0xf8);
}

void m3885_configure_multikey(void)
{
	int i;
	u8 reg8;
	u8 kstate5_flags, offs, maxvars;

	/* RAM bank 0 */
	kstate5_flags = m3885_get_variable(0x0c);
	m3885_set_variable(0x0c, kstate5_flags & ~(7 << 4));

	/* Write Matrix to bank 0 */
	for (i = 0; i < ARRAY_SIZE(matrix); i++) {
		m3885_set_proc_ram(i + 0x80, matrix[i]);
	}


	/* RAM bank 2 */
	m3885_set_variable(0x0c, (kstate5_flags & (~(7 << 4))) | (2 << 4));

	/* Get the number of variables */
	maxvars = m3885_get_variable(0x00);
	printk(BIOS_DEBUG, "M388x has %d variables in bank 2.\n", maxvars);
	if (maxvars >= 35) {
		offs = m3885_get_variable(0x23);
		if ((offs > 0xc0) || (offs < 0x80)) {
			printk(BIOS_DEBUG, "M388x does not have a valid RAM offset (0x%x)\n", offs);
		} else {
			printk(BIOS_DEBUG, "Writing Fn-Table to M388x RAM offset 0x%x\n", offs);
			for (i = 0; i < ARRAY_SIZE(function_ram); i++) {
				m3885_set_proc_ram(i + offs, function_ram[i]);
			}
		}
	} else {
		printk(BIOS_DEBUG, "Could not load Function-RAM (%d).\n", maxvars);
	}

	/* restore original bank */
	m3885_set_variable(0x0c, kstate5_flags);
	maxvars = m3885_get_variable(0x00);
	printk(BIOS_DEBUG, "M388x has %d variables in original bank.\n", maxvars);
	for (i = 0; i < ARRAY_SIZE(variables); i+=3) {
		if (variables[i + 0] > maxvars)
			continue;
		reg8 = m3885_get_variable(variables[i + 0]);
		reg8 &= ~(variables[i + 1]);
		reg8 |= variables[i + 2];
		m3885_set_variable(variables[i + 0], reg8);
	}

	/* OEM Init */

	/* Set Bank 1 */
	m3885_set_variable(0x0c, (kstate5_flags & ~(7 << 4)) | (1 << 4));

	/* Set SMI# at P5.1 */
	/* SMI Control -> p5.1 = EXTSMI# */
	m3885_set_proc_ram(0xff, 0xc1);

	/* Set Fn-Key Task 0 -> SMI#/SCI */
	m3885_set_proc_ram(0x6d, 0x81);

	/* Set Fn-Key Task 1 -> SCI */
	m3885_set_proc_ram(0x6c, 0x80);

	/* Number of Thermal Sensors */
	m3885_set_proc_ram(0xf2, 0x02);

	/* Critical Task */
	m3885_set_proc_ram(0xf3, 0x5d);

	/* Thermal Polling Period */
	m3885_set_proc_ram(0xf9, 0x0a);

	/* ReadPort */

	/* AC PRESN# */
	if (m3885_read_port() & (1 << 0))
		reg8 = 0x8a;
	else
		reg8 = 0x9a;
	m3885_set_proc_ram(0xd0, reg8); /* P60SPEC */

	/* SENSE1# */
	if (m3885_read_port() & (1 << 2))
		reg8 = 0x8a;
	else
		reg8 = 0x9a;
	m3885_set_proc_ram(0xd2, reg8); /* P62SPEC */

	/* SENSE2# */
	if (m3885_read_port() & (1 << 3))
		reg8 = 0x8a;
	else
		reg8 = 0x9a;
	m3885_set_proc_ram(0xd3, reg8); /* P63SPEC */

	/* Low Active Port */
	m3885_set_proc_ram(0xd1, 0x88); /* P61SPEC */
	m3885_set_proc_ram(0xd6, 0x88); /* P66SPEC */
	m3885_set_proc_ram(0xd7, 0x88); /* P67SPEC */

	/* High Active Port */
	m3885_set_proc_ram(0xd4, 0x98); /* P64SPEC */
	m3885_set_proc_ram(0xd5, 0x98); /* P65SPEC */

	/* Set P60TASK-P67TASK */
	/* SCI */
	m3885_set_proc_ram(0xda, 0x80); /* P62TASK SENSE1# */
	m3885_set_proc_ram(0xdb, 0x80); /* P63TASK SENSE2# */
	m3885_set_proc_ram(0xdd, 0x80); /* P65TASK PROCHOT */
	m3885_set_proc_ram(0xde, 0x80); /* P65TASK THERMTRIP# */
	m3885_set_proc_ram(0xdf, 0x80); /* P65TASK PME# */
	/* SMI/SCI */
	m3885_set_proc_ram(0xd8, 0x81); /* P60TASK, AC_PRESN# */
	m3885_set_proc_ram(0xd9, 0x81); /* P61TASK, LID# */
	m3885_set_proc_ram(0xdc, 0x81); /* P64TASK, FDD/LPT# */

	/* Thermal */
	/* Bank 5 */
	m3885_set_variable(0x0c, (kstate5_flags & ~(7 << 4)) | (5 << 4));

	/* Thermal 0: Active cooling, Speed Step Down */
	m3885_set_proc_ram(0x81, 0x9c);    /* THRM0 */
	m3885_set_proc_ram(0x82, 0x01);    /* THRM0 CMD */
	m3885_set_proc_ram(0x84, TH0LOW);  /* THRM0 LOW */
	m3885_set_proc_ram(0x85, TH0HIGH); /* THRM0 HIGH */
	m3885_set_proc_ram(0x86, 0x81);    /* Set Task SMI#/SCI */
	m3885_set_proc_ram(0x87, TH0CRIT); /* THRM0 CRIT */

	/* Thermal 1: Passive cooling, Fan On */
	m3885_set_proc_ram(0x89, 0x9c);    /* THRM1 */
	m3885_set_proc_ram(0x8a, 0x01);    /* THRM1 CMD */
	m3885_set_proc_ram(0x8c, TH1LOW);  /* THRM1 LOW */
	m3885_set_proc_ram(0x8d, TH1HIGH); /* THRM1 HIGH */
	m3885_set_proc_ram(0x8e, 0x81);    /* Set Task SMI#/SCI */

	/* Switch Task to SMI */
	udelay(100 * 1000); /* 100ms */
	outb(KBD_SC, 0xca);
	udelay(100 * 1000); /* 100ms */
	outb(KBD_DATA, 0x17);

	/* Set P22 to high level, keyboard backlight default off */
	udelay(100 * 1000); /* 100ms */
	outb(KBD_SC, 0xc5);
	udelay(100 * 1000); /* 100ms */
	outb(KBD_DATA, 0x4a);
}

u8 m3885_gpio(u8 value)
{
	/* First write data */
	ec_write(M3885_CMDAT1, value);

	/* Issue command: ACCESS GPIO */
	ec_write(M3885_CMCMD, 0xc5);
	return 0;
}
