/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <ec/acpi/ec.h>
#include <string.h>
#include <smbios.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <types.h>

#include "h8.h"
#include "chip.h"

void h8_trackpoint_enable(int on)
{
	ec_write(H8_TRACKPOINT_CTRL,
		 on ? H8_TRACKPOINT_ON : H8_TRACKPOINT_OFF);
}

/* Controls radio-off pin in WLAN MiniPCIe slot.  */
void h8_wlan_enable(int on)
{
	if (on)
		ec_set_bit(0x3a, 5);
	else
		ec_clr_bit(0x3a, 5);
}

/* Controls radio-off pin in UWB MiniPCIe slot.  */
static void h8_uwb_enable(int on)
{
	if (on)
		ec_set_bit(0x31, 2);
	else
		ec_clr_bit(0x31, 2);
}

static void h8_fn_ctrl_swap(int on)
{
	if (on)
		ec_set_bit(0xce, 4);
	else
		ec_clr_bit(0xce, 4);
}

enum battery {
	SECONDARY_BATTERY = 0,
	PRIMARY_BATTERY = 1,
};

/* h8 charge priority. Defines if primary or secondary
 * battery is charged first.
 * Because NVRAM is complete the otherway around as this register,
 * it's inverted by if
 */
static void h8_charge_priority(enum battery battery)
{
	if (battery == PRIMARY_BATTERY)
		ec_clr_bit(0x0, 4);
	else
		ec_set_bit(0x0, 4);
}

static void h8_sticky_fn(int on)
{
	if (on)
		ec_set_bit(0x0, 3);
	else
		ec_clr_bit(0x0, 3);
}

static void f1_to_f12_as_primary(int on)
{
	if (on)
		ec_set_bit(0x3b, 3);
	else
		ec_clr_bit(0x3b, 3);
}

static void h8_log_ec_version(void)
{
	char ecfw[17];
	u8 len;
	u16 fwvh, fwvl;

	len = h8_build_id_and_function_spec_version(ecfw, sizeof ecfw - 1);
	ecfw[len] = 0;

	fwvh = ec_read(0xe9);
	fwvl = ec_read(0xe8);

	printk(BIOS_INFO, "H8: EC Firmware ID %s, Version %d.%d%d%c\n", ecfw,
	       fwvh >> 4, fwvh & 0x0f, fwvl >> 4, 0x41 + (fwvl & 0xf));
}

void h8_set_audio_mute(int mute)
{
	if (mute)
		ec_set_bit(0x3a, 0);
	else
		ec_clr_bit(0x3a, 0);
}

void h8_enable_event(int event)
{
	if (event < 0 || event > 127)
		return;

	ec_set_bit(0x10 + (event >> 3), event & 7);
}

void h8_disable_event(int event)
{
	if (event < 0 || event > 127)
		return;

	ec_clr_bit(0x10 + (event >> 3), event & 7);
}

void h8_usb_always_on_enable(enum usb_always_on on)
{
	u8 val;

	switch (on) {
	case UAO_OFF:
		val = ec_read(H8_USB_ALWAYS_ON);
		// Clear bits 0,2,3
		val &= ~(H8_USB_ALWAYS_ON_ENABLE | H8_USB_ALWAYS_ON_AC_ONLY);
		ec_write(H8_USB_ALWAYS_ON, val);
		break;

	case UAO_AC_AND_BATTERY:
		val = ec_read(H8_USB_ALWAYS_ON);
		val |= H8_USB_ALWAYS_ON_ENABLE; // Set bit 0
		val &= ~H8_USB_ALWAYS_ON_AC_ONLY; // Clear bits 2 and 3
		ec_write(H8_USB_ALWAYS_ON, val);
		break;

	case UAO_AC_ONLY:
		val = ec_read(H8_USB_ALWAYS_ON);
		// Set bits 0,2,3
		val |= (H8_USB_ALWAYS_ON_ENABLE | H8_USB_ALWAYS_ON_AC_ONLY);
		ec_write(H8_USB_ALWAYS_ON, val);
		break;
	}
}

void h8_usb_power_enable(int onoff)
{
	if (onoff)
		ec_set_bit(0x3b, 4);
	else
		ec_clr_bit(0x3b, 4);
}

int h8_ultrabay_device_present(void)
{
	return ec_read(H8_STATUS1) & 0x5 ? 0 : 1;
}

u8 h8_build_id_and_function_spec_version(char *buf, u8 buf_len)
{
	u8 i, c;
	char str[16 + 1]; /* 16 ASCII chars + \0 */

	/* Build ID */
	for (i = 0; i < 8; i++) {
		c = ec_read(0xf0 + i);
		if (c < 0x20 || c > 0x7f) {
			i = snprintf(str, sizeof(str), "*INVALID");
			break;
		}
		str[i] = c;
	}

	/* EC firmware function specification version */
	i += snprintf(str + i, sizeof(str) - i, "-%u.%u", ec_read(0xef), ec_read(0xeb));

	i = MIN(buf_len, i);
	memcpy(buf, str, i);

	return i;
}

#if CONFIG(GENERATE_SMBIOS_TABLES)
static void h8_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	char tpec[] = "IBM ThinkPad Embedded Controller -[                 ]-";

	h8_build_id_and_function_spec_version(tpec + 35, 17);

	t->count = smbios_add_string(t->eos, tpec);
}
#endif

static void h8_init(struct device *dev)
{
	pc_keyboard_init(NO_AUX_DEVICE);
}

#if CONFIG(HAVE_ACPI_TABLES)
static const char *h8_acpi_name(const struct device *dev)
{
	return "EC";
}
#endif

struct device_operations h8_dev_ops = {
#if CONFIG(GENERATE_SMBIOS_TABLES)
	.get_smbios_strings = h8_smbios_strings,
#endif
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt = h8_ssdt_generator,
	.acpi_name = h8_acpi_name,
#endif
	.init = h8_init,
};

void __weak h8_mb_init(void){ /* NOOP */ }

static void h8_enable(struct device *dev)
{
	struct ec_lenovo_h8_config *conf = dev->chip_info;
	u8 val;
	u8 beepmask0, beepmask1, reg8;

	dev->ops = &h8_dev_ops;

	ec_clear_out_queue();
	h8_log_ec_version();

	/* Always enable I/O range 0x1600-0x160f and thermal management */
	reg8 = conf->config0;
	reg8 |= H8_CONFIG0_SMM_H8_ENABLE;
	reg8 |= H8_CONFIG0_TC_ENABLE;
	ec_write(H8_CONFIG0, reg8);

	reg8 = conf->config1;
	if (conf->has_keyboard_backlight) {
		/* Default to both backlights */
		reg8 = (reg8 & 0xf3) | ((get_uint_option("backlight", 0) & 0x3) << 2);
	}
	ec_write(H8_CONFIG1, reg8);
	ec_write(H8_CONFIG2, conf->config2);
	ec_write(H8_CONFIG3, conf->config3);

	beepmask0 = conf->beepmask0;
	beepmask1 = conf->beepmask1;

	if (conf->has_power_management_beeps) {
		if (get_uint_option("power_management_beeps", 1) == 0) {
			beepmask0 = 0x00;
			beepmask1 = 0x00;
		}
	}

	if (conf->has_power_management_beeps) {
		if (get_uint_option("low_battery_beep", 1))
			beepmask0 |= 2;
		else
			beepmask0 &= ~2;
	}

	ec_write(H8_SOUND_ENABLE0, beepmask0);
	ec_write(H8_SOUND_ENABLE1, beepmask1);

	/* silence sounds in queue */
	ec_write(H8_SOUND_REPEAT, 0x00);
	ec_write(H8_SOUND_REG, 0x00);

	ec_write(0x10, conf->event0_enable);
	ec_write(0x11, conf->event1_enable);
	ec_write(0x12, conf->event2_enable);
	ec_write(0x13, conf->event3_enable);
	ec_write(0x14, conf->event4_enable);
	ec_write(0x15, conf->event5_enable);
	ec_write(0x16, conf->event6_enable);
	ec_write(0x17, conf->event7_enable);
	ec_write(0x18, conf->event8_enable);
	ec_write(0x19, conf->event9_enable);
	ec_write(0x1a, conf->eventa_enable);
	ec_write(0x1b, conf->eventb_enable);
	ec_write(0x1c, conf->eventc_enable);
	ec_write(0x1d, conf->eventd_enable);
	ec_write(0x1e, conf->evente_enable);
	ec_write(0x1f, conf->eventf_enable);

	ec_write(H8_FAN_CONTROL, H8_FAN_CONTROL_AUTO);

	h8_usb_always_on_enable(get_uint_option("usb_always_on", 0));

	h8_wlan_enable(get_uint_option("wlan", 1));

	h8_trackpoint_enable(1);
	h8_usb_power_enable(1);

	unsigned int volume = get_uint_option("volume", ~0);
	if (volume <= 0xff && !acpi_is_wakeup_s3())
		ec_write(H8_VOLUME_CONTROL, volume);

	val = (CONFIG(H8_SUPPORT_BT_ON_WIFI) || h8_has_bdc(dev)) &&
		h8_bluetooth_nv_enable();
	h8_bluetooth_enable(val);

	val = h8_has_wwan(dev) && h8_wwan_nv_enable();
	h8_wwan_enable(val);

	if (conf->has_uwb)
		h8_uwb_enable(get_uint_option("uwb", 1));

	h8_fn_ctrl_swap(get_uint_option("fn_ctrl_swap", CONFIG(H8_FN_CTRL_SWAP)));

	h8_sticky_fn(get_uint_option("sticky_fn", 0));

	if (CONFIG(H8_HAS_PRIMARY_FN_KEYS))
		f1_to_f12_as_primary(get_uint_option("f1_to_f12_as_primary", 1));

	h8_charge_priority(get_uint_option("first_battery", PRIMARY_BATTERY));

	h8_set_audio_mute(0);
	h8_mb_init();
}

struct chip_operations ec_lenovo_h8_ops = {
	CHIP_NAME("Lenovo H8 EC")
	.enable_dev = h8_enable,
};
