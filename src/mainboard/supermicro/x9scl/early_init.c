/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/pci_ops.h>
#include <device/pnp_def.h>
#include <device/pnp_ops.h>
#include <bootblock_common.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>
#include <superio/nuvoton/wpcm450/wpcm450.h>

#include "x9scl.h"

#define SERIAL_DEV PNP_DEV(X9SCL_NCT6776_PNP_BASE, NCT6776_SP1)
#define KCS_DEV PNP_DEV(X9SCL_WPCM450_PNP_BASE, 0x11)

#define SUPERIO_INITVAL(reg, data) {(reg), (data)}
#define SUPERIO_BANK(x) SUPERIO_INITVAL(0x07, (x))

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },	/* ? USB0  1d.0 port 1 */
	{ 1, 0, 0 },	/* ? USB1  1d.0 port 2 */
	{ 1, 0, 1 },	/* ? USB2  1d.0 port 3 */
	{ 1, 0, 1 },	/* ? USB3  1d.0 port 4 */
	{ 1, 0, 2 },	/* ? USB4  1d.0 port 5 */
	{ 1, 0, 2 },	/* ? USB5  1d.0 port 6 */
	{ 1, 0, 3 },	/* ? ???   1a.0 port 1 */
	{ 1, 0, 3 },	/* ? BMC   1a.0 port 2 */
	{ 1, 0, 4 },	/* ? ???   1a.0 port 3 */
	{ 1, 0, 4 },	/* ? USB11 1a.0 port 4 */
	{ 1, 0, 6 },	/* ? USB12 1a.0 port 5 */
	{ 1, 0, 5 },	/* ? USB13 1a.0 port 6 */
	{ 1, 0, 5 },
	{ 1, 0, 6 },
};

static const uint8_t superio_initvals[][2] = {
	/* Global config registers */
	SUPERIO_INITVAL(0x1a, 0xc8),
	SUPERIO_INITVAL(0x1b, 0x68),
	SUPERIO_INITVAL(0x1c, 0x83),
	SUPERIO_INITVAL(0x24, 0x24),
	SUPERIO_INITVAL(0x2a, 0x00),
	SUPERIO_INITVAL(0x2b, 0x42),
	SUPERIO_INITVAL(0x2c, 0x80),

	SUPERIO_BANK(0x9), /* GPIO[2345] */
	SUPERIO_INITVAL(0x30, 0x0c),
	SUPERIO_INITVAL(0xe0, 0xcf),
	SUPERIO_INITVAL(0xe4, 0xbd),
	SUPERIO_INITVAL(0xe5, 0x42),
	SUPERIO_INITVAL(0xe9, 0x10),
	SUPERIO_INITVAL(0xea, 0x40),
	SUPERIO_INITVAL(0xf0, 0xff),
	SUPERIO_INITVAL(0xf1, 0x02),

	SUPERIO_BANK(0xb), /* HWM & LED */
	SUPERIO_INITVAL(0xf7, 0x07),
	SUPERIO_INITVAL(0xf8, 0x40),
	SUPERIO_INITVAL(0x30, 0x01),
	SUPERIO_INITVAL(0x60, X9SCL_NCT6776_HWM_BASE >> 8),
	SUPERIO_INITVAL(0x61, X9SCL_NCT6776_HWM_BASE & 0xff),

	SUPERIO_BANK(0x5), /* KBC */
	SUPERIO_INITVAL(0xf0, 0x83),
	SUPERIO_INITVAL(0x30, 0x01),

	SUPERIO_BANK(0x0), /* FDC */
	SUPERIO_INITVAL(0x30, 0x80),
};

static void superio_init(void)
{
	const pnp_devfn_t dev = PNP_DEV(X9SCL_NCT6776_PNP_BASE, 0);

	nuvoton_pnp_enter_conf_state(dev);
	for (size_t i = 0; i < ARRAY_SIZE(superio_initvals); i++)
		pnp_write_config(dev, superio_initvals[i][0], superio_initvals[i][1]);
	nuvoton_pnp_exit_conf_state(dev);
}

static void bmc_init(void)
{
	pnp_devfn_t dev = KCS_DEV;

	pnp_write_config(dev, 0x21, 0x11);

	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, X9SCL_WPCM450_KCS_BASE + 0);
	pnp_set_iobase(dev, PNP_IDX_IO1, X9SCL_WPCM450_KCS_BASE + 1);
	pnp_set_iobase(dev, PNP_IDX_IRQ0, 0);
	pnp_set_enable(dev, 1);
}

void bootblock_mainboard_early_init(void)
{
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	superio_init();
	bmc_init();
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[1], 0x51, id_only);
	read_spd(&spd[2], 0x52, id_only);
	read_spd(&spd[3], 0x53, id_only);
}

void mainboard_early_init(int s3resume)
{
	/* Disable IGD VGA decode, no GTT or GFX stolen */
	pci_write_config16(PCI_DEV(0, 0, 0), GGC, 2);
}
