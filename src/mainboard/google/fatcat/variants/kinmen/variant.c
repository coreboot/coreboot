/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI));
}

void variant_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	/*
	 *+-------+--------+-------------+
	 *|       | FP_SPI | FP_USB      |
	 *+-------+--------+-------------+
	 *| FPMCU | GSPI0  | usb2_port6  |
	 *+-------+--------+-------------+
	 */
	if (fw_config_probe(FW_CONFIG(FPMCU, FP_USB))) {
		printk(BIOS_INFO, "Disable GSPI\n");
		config->serial_io_gspi_mode[PchSerialIoIndexGSPI0] = PchSerialIoDisabled;
		printk(BIOS_INFO, "usb2_port6 to FP\n");
		config->usb2_ports[5] = (struct usb2_port_config) USB2_PORT_MID(OC_SKIP);
	}
	/* Probe fw_config : "IO_PORT" to reconfigure port settings accordingly.
	 * proto0  : IO_PORT => "USB2A2C_HDMI:0"
	 * porot1.5: IO_PORT => "USB3C:1"
	 * +-----------------+------------------+------------------+
	 *| IO_PORT         | USB2A2C_HDMI     |  USB3C           |
	 *+-----------------+------------------+------------------+
	 *| tcss_usb3_port0 | USB4_C0 (MB-TBT) | USB4_C0 (MB-TBT) |
	 *+-----------------+------------------+------------------+
	 *| tcss_usb3_port1 | N/A              | USB3.2 C2 (DB)   |
	 *+-----------------+------------------+------------------+
	 *| tcss_usb3_port2 | USB4_C1 (MB-TBT) | N/A              |
	 *+-----------------+------------------+------------------+
	 *| tcss_usb3_port3 | Fixed HDMI       | USB4_C1 (MB-TBT) |
	 *+-----------------+------------------+------------------+
	 *| usb3_port1      | USB3_A0          | N/A              |
	 *+-----------------+------------------+------------------+
	 *| usb3_port2      | USB3_A1          | N/A              |
	 *+-----------------+------------------+------------------+
	 *| usb2_port5      | USB2_A0          | USB2_C2 (DB)     |
	 *+-----------------+------------------+------------------+
	 *| usb2_port6      | USB2_A1          | USB_FP           |
	 *+-----------------+------------------+------------------+
	 */

	if (fw_config_probe(FW_CONFIG(IO_PORT, USB3C))) {
		printk(BIOS_INFO, "Disable Type-A Port A0/A1\n");
		config->usb3_ports[0] = (struct usb3_port_config) USB3_PORT_EMPTY;
		config->usb3_ports[1] = (struct usb3_port_config) USB3_PORT_EMPTY;

		printk(BIOS_INFO, "usb2_port5 to USB2_C2(DB).\n");
		config->usb2_ports[4] = (struct usb2_port_config) USB2_PORT_TYPE_C(OC_SKIP);

		printk(BIOS_INFO, "Enable tcss_usb3_port1 to USB3_C2(DB)\n");
		config->tcss_ports[1] = (struct tcss_port_config) TCSS_PORT_DEFAULT(OC_SKIP);
		printk(BIOS_INFO, "Disable tcss_usb3_port2\n");
		config->tcss_ports[2] = (struct tcss_port_config) TCSS_PORT_EMPTY;
		printk(BIOS_INFO, "Enable tcss_usb3_port3 to USB3_C1(MB)\n");
		config->tcss_ports[3] = (struct tcss_port_config) TCSS_PORT_DEFAULT(OC_SKIP);
	}
}
