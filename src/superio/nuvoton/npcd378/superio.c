/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <superio/conf_mode.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <superio/common/ssdt.h>
#include <stdlib.h>

#include "npcd378.h"

uint8_t npcd378_hwm_read(const uint16_t iobase, const uint16_t reg)
{
	outb((reg >> 8) & 0xf, iobase + 0xff);
	uint8_t reg8 = inb(iobase + (reg & 0xff));
	if (reg8 == 0xff)
		reg8 = inb(iobase + (reg & 0xff));

	outb(0, iobase + 0xff);
	return reg8;
}

void npcd378_hwm_write(const uint16_t iobase, const uint16_t reg, const uint8_t val)
{
	outb((reg >> 8) & 0xf, iobase + 0xff);
	outb(val, iobase + (reg & 0xff));

	outb(0, iobase + 0xff);
}

void npcd378_hwm_write_start(const uint16_t iobase)
{
	u8 reg8 = npcd378_hwm_read(iobase, NPCD837_HWM_WRITE_LOCK_CTRL);
	reg8 &= ~NPCD837_HWM_WRITE_LOCK_BIT;
	npcd378_hwm_write(iobase, NPCD837_HWM_WRITE_LOCK_CTRL, reg8);
}

void npcd378_hwm_write_finished(const uint16_t iobase)
{
	u8 reg8 = npcd378_hwm_read(iobase, NPCD837_HWM_WRITE_LOCK_CTRL);
	reg8 |= NPCD837_HWM_WRITE_LOCK_BIT;
	npcd378_hwm_write(iobase, NPCD837_HWM_WRITE_LOCK_CTRL, reg8);
}

static void npcd378_init(struct device *dev)
{
	struct resource *res;
	uint8_t pwm, fan_lvl;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for FDC etc. */
	case NPCD378_KBC:
		pc_keyboard_init(PROBE_AUX_DEVICE);
		break;
	case NPCD378_HWM:
		res = find_resource(dev, PNP_IDX_IO0);
		if (!res || !res->base) {
			printk(BIOS_ERR, "NPCD378: LDN%u IOBASE not set.\n", NPCD378_HWM);
			break;
		}

		npcd378_hwm_write_start(res->base);

		if (!get_option(&fan_lvl, "psu_fan_lvl") || fan_lvl > 7)
			fan_lvl = 3;

		pwm = NPCD378_HWM_PSU_FAN_MIN +
		    (NPCD378_HWM_PSU_FAN_MAX - NPCD378_HWM_PSU_FAN_MIN) *
		    fan_lvl / 7;

		/* Set PSU fan PWM lvl */
		npcd378_hwm_write(res->base, NPCD378_HWM_PSU_FAN_PWM_CTRL, pwm);
		printk(BIOS_INFO, "NPCD378: PSU fan PWM 0x%02x\n", pwm);

		npcd378_hwm_write_finished(res->base);
		break;
	}
}

#if CONFIG(HAVE_ACPI_TABLES)
/* Provide ACPI HIDs for generic Super I/O SSDT */
static const char *npcd378_acpi_hid(const struct device *dev)
{
	/* Sanity checks */
	if (dev->path.type != DEVICE_PATH_PNP)
		return NULL;
	if (dev->path.pnp.port == 0)
		return NULL;
	if ((dev->path.pnp.device & 0xff) > NPCD378_GPIOA)
		return NULL;

	switch (dev->path.pnp.device & 0xff) {
	case NPCD378_FDC:
		return ACPI_HID_FDC;
	case NPCD378_PP:
		return ACPI_HID_LPT;
	case NPCD378_SP1: /* fallthrough */
	case NPCD378_SP2:
		return ACPI_HID_COM;
	case NPCD378_AUX:
		return ACPI_HID_MOUSE;
	case NPCD378_KBC:
		return ACPI_HID_KEYBOARD;
	default:
		return ACPI_HID_PNP;
	}
}

static void npcd378_ssdt_aux(const struct device *dev)
{
	/* Scope */
	acpigen_write_scope(acpi_device_path(dev));

	acpigen_write_method("_PSW", 1);
	acpigen_write_store();
	acpigen_emit_byte(ARG0_OP);
	acpigen_emit_namestring("^^MSFG");
	acpigen_pop_len();		/* Pop Method */

	acpigen_write_PRW(8, 3);

	acpigen_pop_len();		/* Pop Scope */
}

static void npcd378_ssdt_kbc(const struct device *dev)
{
	/* Scope */
	acpigen_write_scope(acpi_device_path(dev));

	acpigen_write_method("_PSW", 1);
	acpigen_write_store();
	acpigen_emit_byte(ARG0_OP);
	acpigen_emit_namestring("^^KBFG");
	acpigen_pop_len();		/* Pop Method */

	acpigen_write_PRW(8, 3);

	acpigen_pop_len();		/* Pop Scope */
}

static void npcd378_ssdt_pwr(const struct device *dev)
{
	const char *name = acpi_device_path(dev);
	const char *scope = acpi_device_scope(dev);
	char *tmp_name;

	/* Scope */
	acpigen_write_scope(name);

	acpigen_emit_ext_op(OPREGION_OP);
	acpigen_emit_namestring("SWCR");
	acpigen_emit_byte(SYSTEMIO);
	acpigen_emit_namestring("IO0B");
	acpigen_emit_namestring("IO0S");

	struct fieldlist l1[] = {
		FIELDLIST_OFFSET(0),
		FIELDLIST_NAMESTR("LEDC", 8),
		FIELDLIST_NAMESTR("SWCC", 8),
	};

	acpigen_write_field("SWCR", l1, ARRAY_SIZE(l1), FIELD_BYTEACC |
				FIELD_NOLOCK | FIELD_PRESERVE);

	acpigen_emit_ext_op(OPREGION_OP);
	acpigen_emit_namestring("RNTR");
	acpigen_emit_byte(SYSTEMIO);
	acpigen_emit_namestring("IO1B");
	acpigen_emit_namestring("IO1S");

	struct fieldlist l2[] = {
		FIELDLIST_OFFSET(0),
		FIELDLIST_NAMESTR("GPES", 8),
		FIELDLIST_NAMESTR("GPEE", 8),
		FIELDLIST_OFFSET(8),
		FIELDLIST_NAMESTR("GPS0", 8),
		FIELDLIST_NAMESTR("GPS1", 8),
		FIELDLIST_NAMESTR("GPS2", 8),
		FIELDLIST_NAMESTR("GPS3", 8),
		FIELDLIST_NAMESTR("GPE0", 8),
		FIELDLIST_NAMESTR("GPE1", 8),
		FIELDLIST_NAMESTR("GPE2", 8),
		FIELDLIST_NAMESTR("GPE3", 8),
	};

	acpigen_write_field("RNTR", l2, ARRAY_SIZE(l2), FIELD_BYTEACC |
				FIELD_NOLOCK | FIELD_PRESERVE);

	/* Method (SIOW, 1, NotSerialized) */
	acpigen_write_method("SIOW", 1);
	acpigen_write_store();
	acpigen_emit_namestring("^GPS2");
	acpigen_emit_namestring("^^PMFG");

	acpigen_write_store();
	acpigen_emit_byte(ZERO_OP);
	acpigen_emit_namestring("^GPEE");

	acpigen_write_store();
	acpigen_emit_byte(ZERO_OP);
	acpigen_emit_namestring("^GPE0");

	acpigen_write_store();
	acpigen_emit_byte(ZERO_OP);
	acpigen_emit_namestring("^GPE1");

	acpigen_emit_byte(AND_OP);
	acpigen_emit_namestring("^LEDC");
	acpigen_write_integer(0xE0);
	acpigen_emit_byte(LOCAL0_OP);

	acpigen_emit_byte(OR_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_integer(0x1E);
	acpigen_emit_namestring("^LEDC");

	acpigen_emit_byte(AND_OP);
	acpigen_emit_namestring("^SWCC");
	acpigen_write_integer(0xBF);
	acpigen_emit_namestring("^SWCC");

	acpigen_pop_len();		/* SIOW method */

	/* Method (SIOS, 1, NotSerialized) */
	acpigen_write_method("SIOS", 1);

	acpigen_write_if();
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(ARG0_OP);
	acpigen_write_integer(5);

	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring("^^KBFG");
	acpigen_emit_byte(ONE_OP);

		acpigen_emit_byte(OR_OP);
		acpigen_emit_namestring("^GPE2");
		acpigen_write_integer(0xE8);
		acpigen_emit_namestring("^GPE2");

	acpigen_write_else();

		acpigen_emit_byte(AND_OP);
		acpigen_emit_namestring("^GPE2");
		acpigen_write_integer(0x17);
		acpigen_emit_namestring("^GPE2");

	acpigen_pop_len();		/* Pop Else */

	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring("^^MSFG");
	acpigen_emit_byte(ONE_OP);

		acpigen_emit_byte(OR_OP);
		acpigen_emit_namestring("^GPE2");
		acpigen_write_integer(0x10);
		acpigen_emit_namestring("^GPE2");

	acpigen_write_else();

		acpigen_emit_byte(AND_OP);
		acpigen_emit_namestring("^GPE2");
		acpigen_write_integer(0xEF);
		acpigen_emit_namestring("^GPE2");

	acpigen_pop_len();		/* Pop Else */

	/* Enable wake on GPE */
	acpigen_write_store();
	acpigen_emit_byte(ONE_OP);
	acpigen_emit_namestring("^GPEE");

	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(ARG0_OP);
	acpigen_write_integer(3);

		acpigen_emit_byte(AND_OP);
		acpigen_emit_namestring("^LEDC");
		acpigen_write_integer(0xE0);
		acpigen_emit_byte(LOCAL0_OP);

		acpigen_emit_byte(OR_OP);
		acpigen_emit_byte(LOCAL0_OP);
		acpigen_write_integer(0x1C);
		acpigen_emit_namestring("^LEDC");

		acpigen_emit_byte(AND_OP);
		acpigen_emit_namestring("^SWCC");
		acpigen_write_integer(0xBF);
		acpigen_emit_byte(LOCAL0_OP);

		acpigen_emit_byte(OR_OP);
		acpigen_emit_byte(LOCAL0_OP);
		acpigen_write_integer(0x40);
		acpigen_emit_namestring("^SWCC");

	acpigen_pop_len();		/* Pop If */

	acpigen_pop_len();		/* Pop If */

	acpigen_write_store();
	acpigen_write_integer(0x10);
	acpigen_emit_namestring("^GPE0");

	acpigen_write_store();
	acpigen_write_integer(0x20);
	acpigen_emit_namestring("^GPE1");

	acpigen_pop_len();		/* Pop SIOS method */

	acpigen_pop_len();		/* Pop Scope */

	/* Inject into parent: */
	if (!scope) {
		printk(BIOS_ERR, "%s: Missing ACPI path/scope\n", dev_path(dev));
		return;
	}
	acpigen_write_scope(scope);

	acpigen_write_name_integer("MSFG", 1);
	acpigen_write_name_integer("KBFG", 1);
	acpigen_write_name_integer("PMFG", 0);

	/* DSDT must call SIOW on _WAK */
	/* Method (SIOW, 1, NotSerialized) */
	acpigen_write_method("SIOW", 1);
	acpigen_emit_byte(RETURN_OP);
	tmp_name = strconcat(name, ".SIOW");
	acpigen_emit_namestring(tmp_name);
	free(tmp_name);

	acpigen_emit_byte(ARG0_OP);
	acpigen_pop_len();

	/* DSDT must call SIOS on _PTS */
	/* Method (SIOS, 1, NotSerialized) */
	acpigen_write_method("SIOS", 1);
	acpigen_emit_byte(RETURN_OP);
	tmp_name = strconcat(name, ".SIOS");
	acpigen_emit_namestring(tmp_name);
	free(tmp_name);
	acpigen_emit_byte(ARG0_OP);
	acpigen_pop_len();		/* Pop Method */

	acpigen_pop_len();		/* Scope */

	acpigen_write_scope("\\_GPE");

	/* Method (SIOH, 0, NotSerialized) */
	acpigen_write_method("_L08", 0);
	acpigen_emit_byte(AND_OP);
	tmp_name = strconcat(scope, ".PMFG");
	acpigen_emit_namestring(tmp_name);
	free(tmp_name);
	acpigen_write_integer(0xE8);
	acpigen_emit_byte(LOCAL0_OP);

	acpigen_write_if();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_emit_byte(ZERO_OP);

	acpigen_emit_byte(NOTIFY_OP);
	tmp_name = strconcat(scope, ".L060");
	acpigen_emit_namestring(tmp_name);
	free(tmp_name);
	acpigen_write_integer(2);

	acpigen_pop_len();		/* Pop If */

	acpigen_emit_byte(AND_OP);
	tmp_name = strconcat(scope, ".PMFG");
	acpigen_emit_namestring(tmp_name);
	free(tmp_name);
	acpigen_write_integer(0x10);
	acpigen_emit_byte(LOCAL0_OP);

	acpigen_write_if();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_emit_byte(ZERO_OP);

	acpigen_emit_byte(NOTIFY_OP);
	tmp_name = strconcat(scope, ".L050");
	acpigen_emit_namestring(tmp_name);
	free(tmp_name);
	acpigen_write_integer(2);
	acpigen_pop_len();		/* Pop If */

	acpigen_pop_len();		/* Pop Method */

	acpigen_pop_len();		/* Scope */
}

static void npcd378_fill_ssdt_generator(const struct device *dev)
{
	if (!dev)
		return;

	superio_common_fill_ssdt_generator(dev);

	switch (dev->path.pnp.device) {
	case NPCD378_PWR:
		npcd378_ssdt_pwr(dev);
		break;
	case NPCD378_AUX:
		npcd378_ssdt_aux(dev);
		break;
	case NPCD378_KBC:
		npcd378_ssdt_kbc(dev);
		break;
	}
}
#endif

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = npcd378_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt   = npcd378_fill_ssdt_generator,
	.acpi_name        = superio_common_ldn_acpi_name,
	.acpi_hid         = npcd378_acpi_hid,
#endif
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NPCD378_FDC, PNP_IO0|PNP_IRQ0|PNP_DRQ0, 0x0ff8, },
	{ NULL, NPCD378_PP, PNP_IO0|PNP_IRQ0|PNP_DRQ0, 0x0ff8, },
	{ NULL, NPCD378_SP1, PNP_IO0|PNP_IRQ0, 0x0ff8, },
	{ NULL, NPCD378_SP2, PNP_IO0|PNP_IRQ0, 0x0ff8, },
	{ NULL, NPCD378_PWR, PNP_IO0|PNP_IO1|PNP_IRQ0|PNP_MSC0|
		PNP_MSC1|PNP_MSC2|PNP_MSC3|PNP_MSC4|PNP_MSC5|PNP_MSC6|PNP_MSC7|
		PNP_MSC8|PNP_MSC9|PNP_MSCA|PNP_MSCB|PNP_MSCC|PNP_MSCD|PNP_MSCE,
		0x0ff8, 0x0ff0},
	{ NULL, NPCD378_AUX, PNP_IRQ0, 0x0fff, 0x0fff, },
	{ NULL, NPCD378_KBC, PNP_IO0|PNP_IO1|PNP_IRQ0,
		0x0fff, 0x0fff, },
	{ NULL, NPCD378_WDT1, PNP_IO0|PNP_MSC8|PNP_MSC9|
		PNP_MSCA|PNP_MSCB|PNP_MSCC|PNP_MSCD|PNP_MSCE, 0x0fe0},
	{ NULL, NPCD378_HWM, PNP_IO0|PNP_MSC0|PNP_MSC1|PNP_MSC2|PNP_MSC3|
		PNP_MSC4|PNP_MSC5|PNP_MSC6|PNP_MSC7|PNP_IRQ0, 0x0f00},
	{ NULL, NPCD378_GPIO_PP_OD, PNP_MSC0|PNP_MSC1|PNP_MSC2|PNP_MSC3|
		PNP_MSC4|PNP_MSC5|PNP_MSC6|PNP_MSC7|PNP_MSC8|PNP_MSC9|PNP_MSCA|
		PNP_MSCB|PNP_MSCC|PNP_MSCD|PNP_MSCE},
	{ NULL, NPCD378_I2C, PNP_IO0|PNP_IO1|PNP_IRQ0|PNP_MSC0|
		PNP_MSC1|PNP_MSC2|PNP_MSC3|PNP_MSC4|PNP_MSC5|PNP_MSC6|PNP_MSC7|
		PNP_MSC8|PNP_MSC9|PNP_MSCA|PNP_MSCB|PNP_MSCC|PNP_MSCD|PNP_MSCE,
		0x0ff0, 0x0ff0},
	{ NULL, NPCD378_SUSPEND, PNP_IO0, 0x0fe0 },
	{ NULL, NPCD378_GPIOA, PNP_IO0|PNP_MSC0|PNP_MSC1|PNP_MSC2|PNP_MSC3|
		PNP_MSC4, 0x0fe0},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_npcd378_ops = {
	CHIP_NAME("NUVOTON NPCD378 Super I/O")
	.enable_dev = enable_dev,
};
