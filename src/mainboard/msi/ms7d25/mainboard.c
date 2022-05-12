/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/device.h>
#include <soc/ramstage.h>
#include <smbios.h>
#include <string.h>

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_DESKTOP;
	fadt->iapc_boot_arch |= ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;
}

static void mainboard_init(void *chip_info)
{

}

u8 smbios_mainboard_feature_flags(void)
{
	return SMBIOS_FEATURE_FLAGS_HOSTING_BOARD | SMBIOS_FEATURE_FLAGS_REPLACEABLE;
}

smbios_wakeup_type smbios_system_wakeup_type(void)
{
	return SMBIOS_WAKEUP_TYPE_POWER_SWITCH;
}

const char *smbios_system_product_name(void)
{
	return "MS-7D25";
}

/* Only baseboard serial number is populated */
const char *smbios_system_serial_number(void)
{
	return "Default string";
}

const char *smbios_system_sku(void)
{
	return "Default string";
}

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	memset(params->PcieRpEnableCpm, 0, sizeof(params->PcieRpEnableCpm));
	memset(params->CpuPcieRpEnableCpm, 0, sizeof(params->CpuPcieRpEnableCpm));
	memset(params->CpuPcieClockGating, 0, sizeof(params->CpuPcieClockGating));
	memset(params->CpuPciePowerGating, 0, sizeof(params->CpuPciePowerGating));

	params->UsbPdoProgramming = 1;

	params->CpuPcieFiaProgramming = 1;

	params->PcieRpFunctionSwap = 0;
	params->CpuPcieRpFunctionSwap = 0;

	params->CpuPcieRpPmSci[0] = 1; // M2_1
	params->CpuPcieRpPmSci[1] = 1; // PCI_E1
	params->PcieRpPmSci[0]    = 1; // PCI_E2
	params->PcieRpPmSci[1]    = 1; // PCI_E4
	params->PcieRpPmSci[2]    = 1; // Ethernet
	params->PcieRpPmSci[4]    = 1; // PCI_E3
	params->PcieRpPmSci[8]    = 1; // M2_3
	params->PcieRpPmSci[20]   = 1; // M2_4
	params->PcieRpPmSci[24]   = 1; // M2_2

	params->PcieRpMaxPayload[0]    = 1; // PCI_E2
	params->PcieRpMaxPayload[1]    = 1; // PCI_E4
	params->PcieRpMaxPayload[2]    = 1; // Ethernet
	params->PcieRpMaxPayload[4]    = 1; // PCI_E3
	params->PcieRpMaxPayload[8]    = 1; // M2_3
	params->PcieRpMaxPayload[20]   = 1; // M2_4
	params->PcieRpMaxPayload[24]   = 1; // M2_2

	params->CpuPcieRpTransmitterHalfSwing[0] = 1; // M2_1
	params->CpuPcieRpTransmitterHalfSwing[1] = 1; // PCI_E1
	params->PcieRpTransmitterHalfSwing[0]    = 1; // PCI_E2
	params->PcieRpTransmitterHalfSwing[1]    = 1; // PCI_E4
	params->PcieRpTransmitterHalfSwing[2]    = 1; // Ethernet
	params->PcieRpTransmitterHalfSwing[4]    = 1; // PCI_E3
	params->PcieRpTransmitterHalfSwing[8]    = 1; // M2_3
	params->PcieRpTransmitterHalfSwing[20]   = 1; // M2_4
	params->PcieRpTransmitterHalfSwing[24]   = 1; // M2_2

	params->PcieRpEnableCpm[0]  = 1; // PCI_E2
	params->PcieRpEnableCpm[1]  = 1; // PCI_E4
	params->PcieRpEnableCpm[4]  = 1; // PCI_E3
	params->PcieRpEnableCpm[8]  = 1; // M2_3
	params->PcieRpEnableCpm[20] = 1; // M2_4
	params->PcieRpEnableCpm[24] = 1; // M2_2

	params->PcieRpAcsEnabled[0]  = 1; // PCI_E2
	params->PcieRpAcsEnabled[1]  = 1; // PCI_E4
	params->PcieRpAcsEnabled[2]  = 1; // Ethernet
	params->PcieRpAcsEnabled[4]  = 1; // PCI_E3
	params->PcieRpAcsEnabled[8]  = 1; // M2_3
	params->PcieRpAcsEnabled[20] = 1; // M2_4
	params->PcieRpAcsEnabled[24] = 1; // M2_2

	params->CpuPcieRpEnableCpm[0] = 1; // M2_1
	params->CpuPcieClockGating[0] = 1;
	params->CpuPciePowerGating[0] = 1;
	params->CpuPcieRpMultiVcEnabled[0] = 1;
	params->CpuPcieRpPeerToPeerMode[0] = 1;
	params->CpuPcieRpMaxPayload[0] = 2; // 512B
	params->CpuPcieRpAcsEnabled[0] = 1;

	params->CpuPcieRpEnableCpm[1] = 1; // PCI_E1
	params->CpuPcieClockGating[1] = 1;
	params->CpuPciePowerGating[1] = 1;
	params->CpuPcieRpPeerToPeerMode[1] = 1;
	params->CpuPcieRpMaxPayload[1] = 2; // 512B
	params->CpuPcieRpAcsEnabled[1] = 1;

	params->SataPortsSolidStateDrive[6] = 1; // M2_3
	params->SataPortsSolidStateDrive[7] = 1; // M2_4
	params->SataLedEnable = 1;
}

#if CONFIG(GENERATE_SMBIOS_TABLES)
static const struct port_information smbios_type8_info[] = {
	{
		.internal_reference_designator = "PS2_USB1",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "Keyboard",
		.external_connector_type       = CONN_PS_2,
		.port_type                     = TYPE_KEYBOARD_PORT
	},
	{
		.internal_reference_designator = "PS2_USB1",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "PS2Mouse",
		.external_connector_type       = CONN_PS_2,
		.port_type                     = TYPE_MOUSE_PORT
	},
	{
		.internal_reference_designator = "PS2_USB1",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "USB 2.0 Type-A",
		.external_connector_type       = CONN_ACCESS_BUS_USB,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "PS2_USB1",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "USB 2.0 Type-A (Flash BIOS)",
		.external_connector_type       = CONN_ACCESS_BUS_USB,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "JTPM1 - TPM HDR",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "LAN_USB1",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "Ethernet",
		.external_connector_type       = CONN_RJ_45,
		.port_type                     = TYPE_NETWORK_PORT
	},
	{
		.internal_reference_designator = "LAN_USB1",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "USB 3.2 Gen2x2 Type-C",
		.external_connector_type       = CONN_USB_TYPE_C,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "LAN_USB1",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "USB 3.2 Gen2 Type-A",
		.external_connector_type       = CONN_ACCESS_BUS_USB,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "SATA1",
		.internal_connector_type       = CONN_SAS_SATA,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_SATA
	},
	{
		.internal_reference_designator = "SATA2",
		.internal_connector_type       = CONN_SAS_SATA,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_SATA
	},
	{
		.internal_reference_designator = "SATA3",
		.internal_connector_type       = CONN_SAS_SATA,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_SATA
	},
	{
		.internal_reference_designator = "SATA4",
		.internal_connector_type       = CONN_SAS_SATA,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_SATA
	},
	{
		.internal_reference_designator = "SATA5",
		.internal_connector_type       = CONN_SAS_SATA,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_SATA
	},
	{
		.internal_reference_designator = "SATA6",
		.internal_connector_type       = CONN_SAS_SATA,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_SATA
	},
	{
		.internal_reference_designator = "JTBT1",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_THUNDERBOLT
	},
	{
		.internal_reference_designator = "JC1 - CHASSIS INTRUSION",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "JAUD1 - FRONT AUDIO",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_AUDIO_PORT
	},
	{
		.internal_reference_designator = "AUDIO1 - REAR AUDIO",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "Audio",
		.external_connector_type       = CONN_OTHER,
		.port_type                     = TYPE_AUDIO_PORT
	},
	{
		.internal_reference_designator = "JFP1 - FRONT PANEL",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "JFP2 - PC SPEAKER",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "JBAT1 - CLEAR CMOS",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "JDASH1 - TUNING CONTROLLER",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "JRGB1 - 5050 RGB LED",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "JRAINBOW1 - WS2812B RGB LED",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "JRAINBOW2 - WS2812B RGB LED",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "CPU_FAN1",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "PUMP_FAN1",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "SYS_FAN1",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "SYS_FAN2",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "SYS_FAN3",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "SYS_FAN4",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "SYS_FAN5",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "SYS_FAN6",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "DP_HDMI1",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "HDMI",
		.external_connector_type       = CONN_OTHER,
		.port_type                     = TYPE_VIDEO_PORT
	},
	{
		.internal_reference_designator = "DP_HDMI1",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "Display Port",
		.external_connector_type       = CONN_OTHER,
		.port_type                     = TYPE_VIDEO_PORT
	},
	{
		.internal_reference_designator = "USB2",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "USB 2.0 Type-A (Upper)",
		.external_connector_type       = CONN_ACCESS_BUS_USB,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "USB2",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "USB 2.0 Type-A (Lower)",
		.external_connector_type       = CONN_ACCESS_BUS_USB,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "USB2",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "USB 3.2 Gen1 Type-A (Upper)",
		.external_connector_type       = CONN_ACCESS_BUS_USB,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "USB2",
		.internal_connector_type       = CONN_NONE,
		.external_reference_designator = "USB 3.2 Gen1 Type-A (Lower)",
		.external_connector_type       = CONN_ACCESS_BUS_USB,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "JUSB1 - USB 2.0 ",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "JUSB2 - USB 2.0",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "JUSB3 - USB 3.2 GEN 1",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "JUSB4 - USB 3.2 GEN 1",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "JUSB5 - USB-C",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_USB
	},
	{
		.internal_reference_designator = "ATX_PWR1",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "CPU_PWR1",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
	{
		.internal_reference_designator = "CPU_PWR2",
		.internal_connector_type       = CONN_OTHER,
		.external_reference_designator = "",
		.external_connector_type       = CONN_NONE,
		.port_type                     = TYPE_OTHER_PORT
	},
};

static int mainboard_smbios_data(struct device *dev, int *handle, unsigned long *current)
{
	int len = 0;

	// add port information
	len += smbios_write_type8(
		current, handle,
		smbios_type8_info,
		ARRAY_SIZE(smbios_type8_info)
		);

	return len;
}
#endif

static void mainboard_enable(struct device *dev)
{
#if CONFIG(GENERATE_SMBIOS_TABLES)
	dev->ops->get_smbios_data = mainboard_smbios_data;
#endif
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
