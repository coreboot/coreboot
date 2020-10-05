/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <elog.h>
#include <sar.h>
#include <smbios.h>
#include <string.h>
#include <wrdd.h>
#include "chip.h"

/* WRDS Spec Revision */
#define WRDS_REVISION 0x0

/* EWRD Spec Revision */
#define EWRD_REVISION 0x0

/* WRDS Domain type */
#define WRDS_DOMAIN_TYPE_WIFI 0x7

/* EWRD Domain type */
#define EWRD_DOMAIN_TYPE_WIFI 0x7

/* WGDS Domain type */
#define WGDS_DOMAIN_TYPE_WIFI 0x7

/*
 * WIFI ACPI NAME = "WF" + hex value of last 8 bits of dev_path_encode + '\0'
 * The above representation returns unique and consistent name every time
 * generate_wifi_acpi_name is invoked. The last 8 bits of dev_path_encode is
 * chosen since it contains the bus address of the device.
 */
#define WIFI_ACPI_NAME_MAX_LEN 5

#if CONFIG(HAVE_ACPI_TABLES)
__weak
int get_wifi_sar_limits(struct wifi_sar_limits *sar_limits)
{
	return -1;
}

static void emit_sar_acpi_structures(void)
{
	int i, j, package_size;
	struct wifi_sar_limits sar_limits;
	struct wifi_sar_delta_table *wgds;

	/* Retrieve the sar limits data */
	if (get_wifi_sar_limits(&sar_limits) < 0) {
		printk(BIOS_ERR, "Error: failed from getting SAR limits!\n");
		return;
	}

	/*
	 * Name ("WRDS", Package () {
	 *   Revision,
	 *   Package () {
	 *     Domain Type,	// 0x7:WiFi
	 *     WiFi SAR BIOS,	// BIOS SAR Enable/disable
	 *     SAR Table Set	// Set#1 of SAR Table (10 bytes)
	 *   }
	 * })
	 */
	acpigen_write_name("WRDS");
	acpigen_write_package(2);
	acpigen_write_dword(WRDS_REVISION);
	/* Emit 'Domain Type' + 'WiFi SAR BIOS' + 10 bytes for Set#1 */
	package_size = 1 + 1 + BYTES_PER_SAR_LIMIT;
	acpigen_write_package(package_size);
	acpigen_write_dword(WRDS_DOMAIN_TYPE_WIFI);
	acpigen_write_dword(CONFIG(SAR_ENABLE));
	for (i = 0; i < BYTES_PER_SAR_LIMIT; i++)
		acpigen_write_byte(sar_limits.sar_limit[0][i]);
	acpigen_pop_len();
	acpigen_pop_len();

	/*
	 * Name ("EWRD", Package () {
	 *   Revision,
	 *   Package () {
	 *     Domain Type,		// 0x7:WiFi
	 *     Dynamic SAR Enable,	// Dynamic SAR Enable/disable
	 *     Extended SAR sets,	// Number of optional SAR table sets
	 *     SAR Table Set,		// Set#2 of SAR Table (10 bytes)
	 *     SAR Table Set,		// Set#3 of SAR Table (10 bytes)
	 *     SAR Table Set		// Set#4 of SAR Table (10 bytes)
	 *   }
	 * })
	 */
	acpigen_write_name("EWRD");
	acpigen_write_package(2);
	acpigen_write_dword(EWRD_REVISION);
	/*
	 * Emit 'Domain Type' + "Dynamic SAR Enable' + 'Extended SAR sets'
	 * + number of bytes for Set#2 & 3 & 4
	 */
	package_size = 1 + 1 + 1 + (NUM_SAR_LIMITS - 1) * BYTES_PER_SAR_LIMIT;
	acpigen_write_package(package_size);
	acpigen_write_dword(EWRD_DOMAIN_TYPE_WIFI);
	acpigen_write_dword(CONFIG(DSAR_ENABLE));
	acpigen_write_dword(CONFIG_DSAR_SET_NUM);
	for (i = 1; i < NUM_SAR_LIMITS; i++)
		for (j = 0; j < BYTES_PER_SAR_LIMIT; j++)
			acpigen_write_byte(sar_limits.sar_limit[i][j]);
	acpigen_pop_len();
	acpigen_pop_len();

	if (!CONFIG(GEO_SAR_ENABLE))
		return;

	/*
	 * Name ("WGDS", Package() {
	 *  Revision,
	 *  Package() {
	 *   DomainType,                         // 0x7:WiFi
	 *   WgdsWiFiSarDeltaGroup1PowerMax1,    // Group 1 FCC 2400 Max
	 *   WgdsWiFiSarDeltaGroup1PowerChainA1, // Group 1 FCC 2400 A Offset
	 *   WgdsWiFiSarDeltaGroup1PowerChainB1, // Group 1 FCC 2400 B Offset
	 *   WgdsWiFiSarDeltaGroup1PowerMax2,    // Group 1 FCC 5200 Max
	 *   WgdsWiFiSarDeltaGroup1PowerChainA2, // Group 1 FCC 5200 A Offset
	 *   WgdsWiFiSarDeltaGroup1PowerChainB2, // Group 1 FCC 5200 B Offset
	 *   WgdsWiFiSarDeltaGroup2PowerMax1,    // Group 2 EC Jap 2400 Max
	 *   WgdsWiFiSarDeltaGroup2PowerChainA1, // Group 2 EC Jap 2400 A Offset
	 *   WgdsWiFiSarDeltaGroup2PowerChainB1, // Group 2 EC Jap 2400 B Offset
	 *   WgdsWiFiSarDeltaGroup2PowerMax2,    // Group 2 EC Jap 5200 Max
	 *   WgdsWiFiSarDeltaGroup2PowerChainA2, // Group 2 EC Jap 5200 A Offset
	 *   WgdsWiFiSarDeltaGroup2PowerChainB2, // Group 2 EC Jap 5200 B Offset
	 *   WgdsWiFiSarDeltaGroup3PowerMax1,    // Group 3 ROW 2400 Max
	 *   WgdsWiFiSarDeltaGroup3PowerChainA1, // Group 3 ROW 2400 A Offset
	 *   WgdsWiFiSarDeltaGroup3PowerChainB1, // Group 3 ROW 2400 B Offset
	 *   WgdsWiFiSarDeltaGroup3PowerMax2,    // Group 3 ROW 5200 Max
	 *   WgdsWiFiSarDeltaGroup3PowerChainA2, // Group 3 ROW 5200 A Offset
	 *   WgdsWiFiSarDeltaGroup3PowerChainB2, // Group 3 ROW 5200 B Offset
	 *  }
	 * })
	 */

	wgds = &sar_limits.wgds;
	acpigen_write_name("WGDS");
	acpigen_write_package(2);
	acpigen_write_dword(wgds->version);
	/* Emit 'Domain Type' +
	 * Group specific delta of power (6 bytes * NUM_WGDS_SAR_GROUPS)
	 */
	package_size = sizeof(sar_limits.wgds.group) + 1;
	acpigen_write_package(package_size);
	acpigen_write_dword(WGDS_DOMAIN_TYPE_WIFI);
	for (i = 0; i < SAR_NUM_WGDS_GROUPS; i++) {
		acpigen_write_byte(wgds->group[i].power_max_2400mhz);
		acpigen_write_byte(wgds->group[i].power_chain_a_2400mhz);
		acpigen_write_byte(wgds->group[i].power_chain_b_2400mhz);
		acpigen_write_byte(wgds->group[i].power_max_5200mhz);
		acpigen_write_byte(wgds->group[i].power_chain_a_5200mhz);
		acpigen_write_byte(wgds->group[i].power_chain_b_5200mhz);
	}

	acpigen_pop_len();
	acpigen_pop_len();
}

void wifi_generic_fill_ssdt(const struct device *dev,
			    const struct drivers_wifi_generic_config *config)
{
	const char *path;
	u32 address;

	if (!dev->enabled)
		return;

	path = acpi_device_path(dev->bus->dev);
	if (!path)
		return;

	/* Device */
	acpigen_write_scope(path);
	acpigen_write_device(acpi_device_name(dev));
	acpi_device_write_uid(dev);

	if (dev->chip_ops)
		acpigen_write_name_string("_DDN", dev->chip_ops->name);

	/* Address */
	address = PCI_SLOT(dev->path.pci.devfn) & 0xffff;
	address <<= 16;
	address |= PCI_FUNC(dev->path.pci.devfn) & 0xffff;
	acpigen_write_name_dword("_ADR", address);

	/* Wake capabilities */
	if (config)
		acpigen_write_PRW(config->wake, ACPI_S3);

	/* Fill regulatory domain structure */
	if (CONFIG(HAVE_REGULATORY_DOMAIN)) {
		/*
		 * Name ("WRDD", Package () {
		 *   WRDD_REVISION, // Revision
		 *   Package () {
		 *     WRDD_DOMAIN_TYPE_WIFI,   // Domain Type, 7:WiFi
		 *     wifi_regulatory_domain() // Country Identifier
		 *   }
		 * })
		 */
		acpigen_write_name("WRDD");
		acpigen_write_package(2);
		acpigen_write_integer(WRDD_REVISION);
		acpigen_write_package(2);
		acpigen_write_dword(WRDD_DOMAIN_TYPE_WIFI);
		acpigen_write_dword(wifi_regulatory_domain());
		acpigen_pop_len();
		acpigen_pop_len();
	}

	/* Fill Wifi sar related ACPI structures */
	if (CONFIG(USE_SAR))
		emit_sar_acpi_structures();

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s.%s: %s %s\n", path, acpi_device_name(dev),
	       dev->chip_ops ? dev->chip_ops->name : "", dev_path(dev));
}

const char *wifi_generic_acpi_name(const struct device *dev)
{
	static char wifi_acpi_name[WIFI_ACPI_NAME_MAX_LEN];

	/* ACPI 6.3, ASL 20.2.2: (Name Objects Encoding). */
	snprintf(wifi_acpi_name, sizeof(wifi_acpi_name), "WF%02X",
		 (dev_path_encode(dev) & 0xff));
	return wifi_acpi_name;
}

static void wifi_generic_fill_ssdt_generator(const struct device *dev)
{
	wifi_generic_fill_ssdt(dev, dev->chip_info);
}
#endif

static void wifi_pci_dev_init(struct device *dev)
{
	if (pci_dev_is_wake_source(dev))
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME_WIFI, 0);
}

#if CONFIG(GENERATE_SMBIOS_TABLES)
static int smbios_write_intel_wifi(struct device *dev, int *handle, unsigned long *current)
{
	struct smbios_type_intel_wifi {
		u8 type;
		u8 length;
		u16 handle;
		u8 str;
		u8 eos[2];
	} __packed;

	struct smbios_type_intel_wifi *t = (struct smbios_type_intel_wifi *)*current;
	int len = sizeof(struct smbios_type_intel_wifi);

	memset(t, 0, sizeof(struct smbios_type_intel_wifi));
	t->type = 0x85;
	t->length = len - 2;
	t->handle = *handle;
	/* Intel wifi driver expects this string to be in the table 0x85. */
	t->str = smbios_add_string(t->eos, "KHOIHGIUCCHHII");

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	*handle += 1;
	return len;
}

static int smbios_write_wifi(struct device *dev, int *handle, unsigned long *current)
{
	if (dev->vendor == PCI_VENDOR_ID_INTEL)
		return smbios_write_intel_wifi(dev, handle, current);

	return 0;
}
#endif

struct device_operations wifi_generic_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= wifi_pci_dev_init,
	.ops_pci		= &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= wifi_generic_acpi_name,
	.acpi_fill_ssdt		= wifi_generic_fill_ssdt_generator,
#endif
#if CONFIG(GENERATE_SMBIOS_TABLES)
	.get_smbios_data	= smbios_write_wifi,
#endif
};

static void wifi_generic_enable(struct device *dev)
{
	struct drivers_wifi_generic_config *config = dev ? dev->chip_info : NULL;

	if (!config)
		return;

	dev->ops = &wifi_generic_ops;
}

struct chip_operations drivers_wifi_generic_ops = {
	CHIP_NAME("WIFI Device")
	.enable_dev = wifi_generic_enable
};
