/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootstate.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <stdint.h>
#include <stdlib.h>

#include "commands.h"
#include "ec.h"
#include "chip.h"

static void wilco_ec_post_complete(void *unused)
{
	wilco_ec_send(KB_BIOS_PROGRESS, BIOS_PROGRESS_POST_COMPLETE);
}
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT,
		      wilco_ec_post_complete, NULL);

static void wilco_ec_post_memory_init(void *unused)
{
	wilco_ec_send(KB_BIOS_PROGRESS, BIOS_PROGRESS_MEMORY_INIT);
}
BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT,
		      wilco_ec_post_memory_init, NULL);

static void wilco_ec_post_video_init(void *unused)
{
	wilco_ec_send(KB_BIOS_PROGRESS, BIOS_PROGRESS_VIDEO_INIT);
}
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT,
		      wilco_ec_post_video_init, NULL);

static void wilco_ec_resume(void *unused)
{
	wilco_ec_send_noargs(KB_RESTORE);
}
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, wilco_ec_resume, NULL);

static void wilco_ec_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	/* Print EC firmware information */
	wilco_ec_print_all_info();

	/* Initialize keyboard, ignore emulated PS/2 mouse */
	pc_keyboard_init(NO_AUX_DEVICE);

	/* Direct power button to the host for processing */
	wilco_ec_send(KB_POWER_BUTTON_TO_HOST, 1);

	/* Unmute speakers */
	wilco_ec_send(KB_HW_MUTE_CONTROL, AUDIO_UNMUTE_125MS);

	/* Enable WiFi radio */
	wilco_ec_radio_control(RADIO_WIFI, 1);

	/* Turn on camera power */
	wilco_ec_send(KB_CAMERA, CAMERA_ON);
}

static void wilco_ec_resource(struct device *dev, int index,
			      size_t base, size_t size)
{
	struct resource *res = new_resource(dev, index);
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	res->base = base;
	res->size = size;
}

static void wilco_ec_read_resources(struct device *dev)
{
	/* ACPI command and data regions */
	wilco_ec_resource(dev, 0, CONFIG_EC_BASE_ACPI_DATA, 8);

	/* Host command and data regions */
	wilco_ec_resource(dev, 1, CONFIG_EC_BASE_HOST_DATA, 8);

	/* Packet region */
	wilco_ec_resource(dev, 2, CONFIG_EC_BASE_PACKET, 16);
}

static struct device_operations ops = {
	.init			= wilco_ec_init,
	.read_resources		= wilco_ec_read_resources,
	.enable_resources	= DEVICE_NOOP,
	.set_resources		= DEVICE_NOOP,
};

static struct pnp_info info[] = {
	{ NULL, 0, 0, 0, }
};

static void wilco_ec_enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(info), info);
}

struct chip_operations ec_google_wilco_ops = {
	CHIP_NAME("Google Wilco EC")
	.enable_dev = wilco_ec_enable_dev,
};
