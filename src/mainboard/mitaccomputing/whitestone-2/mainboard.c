/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <device/device.h>
#include "smbios_onboard.h"
#include <smbios.h>

const char *e810_cam1_port_name[E810_CAM1_PORT_NUMBER] = {
	"E810 CAM1 Port1",
	"E810 CAM1 Port2",
	"E810 CAM1 Port3",
	"E810 CAM1 Port4",
};

const char *e810_cam2_port_name[E810_CAM2_PORT_NUMBER] = {
	"E810 CAM2 Port1",
	"E810 CAM2 Port2",
	"E810 CAM2 Port3",
	"E810 CAM2 Port4",
	"E810 CAM2 Port5",
	"E810 CAM2 Port6",
	"E810 CAM2 Port7",
	"E810 CAM2 Port8",
};

static int ws_2_onboard_smbios_data(struct device *dev, int *handle,
				     unsigned long *current)
{
	int data_length = 0;
	int type_instance = 0;

	for (int i = 0; i < E810_CAM1_PORT_NUMBER; i++) {
		data_length += smbios_write_type41(
						current, handle,
						e810_cam1_port_name[i],         /* name */
						++type_instance,                /* instance */
						0,                              /* segment */
						E810_CAM1_BUS_NUMBER,           /* bus */
						0,                              /* device */
						i,                              /* function */
						SMBIOS_DEVICE_TYPE_ETHERNET);   /* device type */
	}

	for (int i = 0; i < E810_CAM2_PORT_NUMBER; i++) {
		data_length += smbios_write_type41(
						current, handle,
						e810_cam2_port_name[i],         /* name */
						++type_instance,                /* instance */
						0,                              /* segment */
						E810_CAM2_BUS_NUMBER,           /* bus */
						0,                              /* device */
						i,                              /* function */
						SMBIOS_DEVICE_TYPE_ETHERNET);   /* device type */
	}
	return data_length;
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->get_smbios_data = ws_2_onboard_smbios_data;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
