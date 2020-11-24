/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <ec/acpi/ec.h>
#include <mainboard/ec.h>
#include <timer.h>

#define EC_STATUS			0x50
#define   EC_RUNNING			(1 << 1)
#define EC_DEVICE_CONTROL_1		0x80
#define   EC_DEVICE_CONTROL_1_PROGAS_ON	(1 << 0)
#define   EC_DEVICE_CONTROL_1_BOOMER_ON	(1 << 1)
#define   EC_DEVICE_CONTROL_1_BT_RF_ON	(1 << 2)
#define   EC_DEVICE_CONTROL_1_TP_ON	(1 << 3)
#define   EC_DEVICE_CONTROL_1_LAN2_RST	(1 << 6)
#define EC_DEVICE_CONTROL_2		0x81
#define   EC_DEVICE_CONTROL_2_LAN_1_ON	(1 << 0)
#define   EC_DEVICE_CONTROL_2_LAN_2_ON	(1 << 1)
#define   EC_DEVICE_CONTROL_2_WLAN_ON	(1 << 2)
#define   EC_DEVICE_CONTROL_2_USB_ON	(1 << 3)
#define   EC_DEVICE_CONTROL_2_IDE1_ON	(1 << 4)
#define   EC_DEVICE_CONTROL_2_IDE2_ON	(1 << 5)
#define   EC_DEVICE_CONTROL_2_COM1_ON	(1 << 6)
#define   EC_DEVICE_CONTROL_2_MPI_ON	(1 << 7)

#define RUNNING_TIMEOUT_MS		3333

static bool ec_running(void)
{
	struct stopwatch sw;
	uint8_t ec_status;

	stopwatch_init_msecs_expire(&sw, RUNNING_TIMEOUT_MS);
	do
		ec_status = ec_read(EC_STATUS);
	while (!(ec_status & EC_RUNNING) && !stopwatch_expired(&sw));

	if (!(ec_status & EC_RUNNING))
		printk(BIOS_WARNING, "EC not ready after %dms\n", RUNNING_TIMEOUT_MS);

	return !!(ec_status & EC_RUNNING);
}

void ec_enable_devices(bool enable_usb)
{
	uint8_t control_1, control_2;

	if (!ec_running())
		return;

	control_1 = ec_read(EC_DEVICE_CONTROL_1);
	control_2 = ec_read(EC_DEVICE_CONTROL_2);

	printk(BIOS_INFO, "EC previous EDC1: 0x%02x\n", control_1);
	printk(BIOS_INFO, "EC previous EDC2: 0x%02x\n", control_2);

	control_1 &= ~(EC_DEVICE_CONTROL_1_BT_RF_ON);
	control_1 |= EC_DEVICE_CONTROL_1_BOOMER_ON;

	control_2 &= ~(EC_DEVICE_CONTROL_2_WLAN_ON | EC_DEVICE_CONTROL_2_USB_ON);
	control_2 |= EC_DEVICE_CONTROL_2_MPI_ON;
	if (enable_usb)
		control_2 |= EC_DEVICE_CONTROL_2_USB_ON;

	ec_write(EC_DEVICE_CONTROL_1, control_1);
	ec_write(EC_DEVICE_CONTROL_2, control_2);

	printk(BIOS_INFO, "EC current  EDC1: 0x%02x\n", ec_read(EC_DEVICE_CONTROL_1));
	printk(BIOS_INFO, "EC current  EDC2: 0x%02x\n", ec_read(EC_DEVICE_CONTROL_2));
}

void mainboard_smi_sleep(const uint8_t slp_typ)
{
	uint8_t control_1, control_2;

	if (slp_typ != ACPI_S5)
		return;

	if (!ec_running())
		return;

	control_1 = ec_read(EC_DEVICE_CONTROL_1);
	control_2 = ec_read(EC_DEVICE_CONTROL_2);

	printk(BIOS_INFO, "EC previous EDC1: 0x%02x\n", control_1);
	printk(BIOS_INFO, "EC previous EDC2: 0x%02x\n", control_2);

	control_1 &= ~(EC_DEVICE_CONTROL_1_BOOMER_ON);
	control_2 &= ~(EC_DEVICE_CONTROL_2_USB_ON | EC_DEVICE_CONTROL_2_MPI_ON);

	ec_write(EC_DEVICE_CONTROL_1, control_1);
	ec_write(EC_DEVICE_CONTROL_2, control_2);

	printk(BIOS_INFO, "EC current  EDC1: 0x%02x\n", ec_read(EC_DEVICE_CONTROL_1));
	printk(BIOS_INFO, "EC current  EDC2: 0x%02x\n", ec_read(EC_DEVICE_CONTROL_2));
}
