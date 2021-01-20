/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <stdint.h>
#include <arch/io.h>
#include <superio/smsc/sio1007/sio1007.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>

#define SIO_PORT 0x164e

void bootblock_mainboard_early_init(void)
{
	const u16 port = SIO_PORT;
	const u16 runtime_port = 0x180;

	sio1007_enable_uart_at(port);

	/* Turn on configuration mode. */
	outb(0x55, port);

	/* Set the GPIO direction, polarity, and type. */
	sio1007_setreg(port, 0x31, 1 << 0, 1 << 0);
	sio1007_setreg(port, 0x32, 0 << 0, 1 << 0);
	sio1007_setreg(port, 0x33, 0 << 0, 1 << 0);

	/* Set the base address for the runtime register block. */
	sio1007_setreg(port, 0x30, runtime_port >> 4, 0xff);
	sio1007_setreg(port, 0x21, runtime_port >> 12, 0xff);

	/* Turn on address decoding for it. */
	sio1007_setreg(port, 0x3a, 1 << 1, 1 << 1);

	/* Set the value of GPIO 10 by changing GP1, bit 0. */
	u8 byte;
	byte = inb(runtime_port + 0xc);
	byte |= (1 << 0);
	outb(byte, runtime_port + 0xc);

	/* Turn off address decoding for it. */
	sio1007_setreg(port, 0x3a, 0 << 1, 1 << 1);

	/* Turn off configuration mode. */
	outb(0xaa, port);
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	struct pei_data pei_data_template = {
		.pei_version = PEI_VERSION,
		.mchbar = CONFIG_FIXED_MCHBAR_MMIO_BASE,
		.dmibar = CONFIG_FIXED_DMIBAR_MMIO_BASE,
		.epbar = CONFIG_FIXED_EPBAR_MMIO_BASE,
		.pciexbar = CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar = CONFIG_FIXED_SMBUS_IO_BASE,
		.wdbbar = 0x4000000,
		.wdbsize = 0x1000,
		.hpet_address = CONFIG_HPET_ADDRESS,
		.rcba = (uintptr_t)DEFAULT_RCBA,
		.pmbase = DEFAULT_PMBASE,
		.gpiobase = DEFAULT_GPIOBASE,
		.thermalbase = 0xfed08000,
		.system_type = 0, // 0 Mobile, 1 Desktop/Server
		.tseg_size = CONFIG_SMM_TSEG_SIZE,
		.spd_addresses = { 0xa0, 0x00, 0xa4, 0x00 },
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 0,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		.dimm_channel0_disabled = 2,
		.dimm_channel1_disabled = 2,
		.max_ddr3_freq = 1600,
		.usb_port_config = {
			{ 1, 0, 0x0040 }, /* P0: Front port  (OC0) */
			{ 1, 1, 0x0040 }, /* P1: Back port   (OC1) */
			{ 1, 0, 0x0040 }, /* P2: MINIPCIE1   (no OC) */
			{ 1, 0, 0x0040 }, /* P3: MMC         (no OC) */
			{ 1, 2, 0x0040 }, /* P4: Front port  (OC2) */
			{ 0, 0, 0x0000 }, /* P5: Empty */
			{ 0, 0, 0x0000 }, /* P6: Empty */
			{ 0, 0, 0x0000 }, /* P7: Empty */
			{ 1, 4, 0x0040 }, /* P8: Back port   (OC4) */
			{ 1, 4, 0x0040 }, /* P9: MINIPCIE3   (no OC) */
			{ 1, 4, 0x0040 }, /* P10: BLUETOOTH  (no OC) */
			{ 0, 4, 0x0000 }, /* P11: Empty */
			{ 1, 6, 0x0040 }, /* P12: Back port  (OC6) */
			{ 1, 5, 0x0040 }, /* P13: Back port  (OC5) */
		},
	};
	*pei_data = pei_data_template;
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* enabled power  USB oc pin  */
	{ 1, 0, 0 }, /* P0: Front port  (OC0) */
	{ 1, 0, 1 }, /* P1: Back port   (OC1) */
	{ 1, 0, -1 }, /* P2: MINIPCIE1   (no OC) */
	{ 1, 0, -1 }, /* P3: MMC         (no OC) */
	{ 1, 0, 2 }, /* P4: Front port  (OC2) */
	{ 0, 0, -1 }, /* P5: Empty */
	{ 0, 0, -1 }, /* P6: Empty */
	{ 0, 0, -1 }, /* P7: Empty */
	{ 1, 0, 4 }, /* P8: Back port   (OC4) */
	{ 1, 0, -1 }, /* P9: MINIPCIE3   (no OC) */
	{ 1, 0, -1 }, /* P10: BLUETOOTH  (no OC) */
	{ 0, 0, -1 }, /* P11: Empty */
	{ 1, 0, 6 }, /* P12: Back port  (OC6) */
	{ 1, 0, 5 }, /* P13: Back port  (OC5) */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}

int mainboard_should_reset_usb(int s3resume)
{
	return !s3resume;
}
