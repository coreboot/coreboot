/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <sar.h>
#include <stdlib.h>
#include <wrdd.h>

#include "chip.h"
#include "wifi_private.h"

/* WIFI Domain type */
#define DOMAIN_TYPE_WIFI 0x7

/*
 * WIFI ACPI NAME = "WF" + hex value of last 8 bits of dev_path_encode + '\0'
 * The above representation returns unique and consistent name every time
 * generate_wifi_acpi_name is invoked. The last 8 bits of dev_path_encode is
 * chosen since it contains the bus address of the device.
 */
#define WIFI_ACPI_NAME_MAX_LEN 5

__weak int get_wifi_sar_limits(union wifi_sar_limits *sar_limits)
{
	return -1;
}

static const uint8_t *sar_fetch_set(const struct sar_profile *sar, size_t set_num)
{
	const uint8_t *sar_table = &sar->sar_table[0];

	return sar_table + (sar->chains_count * sar->subbands_count * set_num);
}

static const uint8_t *wgds_fetch_set(struct geo_profile *wgds, size_t set_num)
{
	const uint8_t *wgds_table = &wgds->wgds_table[0];

	return wgds_table + (wgds->bands_count * set_num);
}

static const uint8_t *ppag_fetch_set(struct gain_profile *ppag, size_t set_num)
{
	const uint8_t *ppag_table = &ppag->ppag_table[0];

	return ppag_table + (ppag->bands_count * set_num);
}

static void sar_emit_wrds(const struct sar_profile *sar)
{
	int i;
	size_t package_size, table_size;
	const uint8_t *set;

	if (sar == NULL)
		return;

	/*
	 * Name ("WRDS", Package () {
	 *   Revision,
	 *   Package () {
	 *     Domain Type,	// 0x7:WiFi
	 *     WiFi SAR BIOS,	// BIOS SAR Enable/disable
	 *     SAR Table Set	// Set#1 of SAR Table
	 *   }
	 * })
	 */
	if (sar->revision > MAX_SAR_REVISION) {
		printk(BIOS_ERR, "ERROR: Invalid SAR table revision: %d\n", sar->revision);
		return;
	}

	acpigen_write_name("WRDS");
	acpigen_write_package(2);
	acpigen_write_dword(sar->revision);

	table_size = sar->chains_count * sar->subbands_count;
	/* Emit 'Domain Type' + 'WiFi SAR Enable' + Set#1 */
	package_size = 1 + 1 + table_size;
	acpigen_write_package(package_size);
	acpigen_write_dword(DOMAIN_TYPE_WIFI);
	acpigen_write_dword(1);

	set = sar_fetch_set(sar, 0);
	for (i = 0; i < table_size; i++)
		acpigen_write_byte(set[i]);

	acpigen_write_package_end();
	acpigen_write_package_end();
}

static void sar_emit_ewrd(const struct sar_profile *sar)
{
	int i;
	size_t package_size, set_num, table_size;
	const uint8_t *set;

	if (sar == NULL)
		return;

	/*
	 * Name ("EWRD", Package () {
	 *   Revision,
	 *   Package () {
	 *     Domain Type,		// 0x7:WiFi
	 *     Dynamic SAR Enable,	// Dynamic SAR Enable/disable
	 *     Extended SAR sets,	// Number of optional SAR table sets
	 *     SAR Table Set,		// Set#2 of SAR Table
	 *     SAR Table Set,		// Set#3 of SAR Table
	 *     SAR Table Set		// Set#4 of SAR Table
	 *   }
	 * })
	 */
	if (sar->revision > MAX_SAR_REVISION) {
		printk(BIOS_ERR, "ERROR: Invalid SAR table revision: %d\n", sar->revision);
		return;
	}

	if (sar->dsar_set_count == 0) {
		printk(BIOS_WARNING, "WARNING: DSAR set count is 0\n");
		return;
	}

	acpigen_write_name("EWRD");
	acpigen_write_package(2);
	acpigen_write_dword(sar->revision);

	table_size = sar->chains_count * sar->subbands_count;
	/*
	 * Emit 'Domain Type' + 'Dynamic SAR Enable' + 'Extended SAR sets count'
	 * + number of bytes for Set#2 & 3 & 4
	 */
	package_size = 1 + 1 + 1 + table_size * sar->dsar_set_count;
	acpigen_write_package(package_size);
	acpigen_write_dword(DOMAIN_TYPE_WIFI);
	acpigen_write_dword(1);
	acpigen_write_dword(sar->dsar_set_count);

	for (set_num = 1; set_num <= sar->dsar_set_count; set_num++) {
		set = sar_fetch_set(sar, set_num);
		for (i = 0; i < table_size; i++)
			acpigen_write_byte(set[i]);
	}

	/* wifi driver always expects 3 DSAR sets */
	for (i = 0; i < (table_size * (MAX_DSAR_SET_COUNT - sar->dsar_set_count)); i++)
		acpigen_write_byte(0);

	acpigen_write_package_end();
	acpigen_write_package_end();
}

static void sar_emit_wgds(struct geo_profile *wgds)
{
	int i;
	size_t package_size, set_num;
	const uint8_t *set;

	if (wgds == NULL)
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
	 *   WgdsWiFiSarDeltaGroup1PowerMax3,    // Group 1 FCC 6000-7000 Max
	 *   WgdsWiFiSarDeltaGroup1PowerChainA3, // Group 1 FCC 6000-7000 A Offset
	 *   WgdsWiFiSarDeltaGroup1PowerChainB3, // Group 1 FCC 6000-7000 B Offset
	 *   WgdsWiFiSarDeltaGroup2PowerMax1,    // Group 2 EC Jap 2400 Max
	 *   WgdsWiFiSarDeltaGroup2PowerChainA1, // Group 2 EC Jap 2400 A Offset
	 *   WgdsWiFiSarDeltaGroup2PowerChainB1, // Group 2 EC Jap 2400 B Offset
	 *   WgdsWiFiSarDeltaGroup2PowerMax2,    // Group 2 EC Jap 5200 Max
	 *   WgdsWiFiSarDeltaGroup2PowerChainA2, // Group 2 EC Jap 5200 A Offset
	 *   WgdsWiFiSarDeltaGroup2PowerChainB2, // Group 2 EC Jap 5200 B Offset
	 *   WgdsWiFiSarDeltaGroup2PowerMax3,    // Group 2 EC Jap 6000-7000 Max
	 *   WgdsWiFiSarDeltaGroup2PowerChainA3, // Group 2 EC Jap 6000-7000 A Offset
	 *   WgdsWiFiSarDeltaGroup2PowerChainB3, // Group 2 EC Jap 6000-7000 B Offset
	 *   WgdsWiFiSarDeltaGroup3PowerMax1,    // Group 3 ROW 2400 Max
	 *   WgdsWiFiSarDeltaGroup3PowerChainA1, // Group 3 ROW 2400 A Offset
	 *   WgdsWiFiSarDeltaGroup3PowerChainB1, // Group 3 ROW 2400 B Offset
	 *   WgdsWiFiSarDeltaGroup3PowerMax2,    // Group 3 ROW 5200 Max
	 *   WgdsWiFiSarDeltaGroup3PowerChainA2, // Group 3 ROW 5200 A Offset
	 *   WgdsWiFiSarDeltaGroup3PowerChainB2, // Group 3 ROW 5200 B Offset
	 *   WgdsWiFiSarDeltaGroup3PowerMax3,    // Group 3 ROW 6000-7000 Max
	 *   WgdsWiFiSarDeltaGroup3PowerChainA3, // Group 3 ROW 6000-7000 A Offset
	 *   WgdsWiFiSarDeltaGroup3PowerChainB3, // Group 3 ROW 6000-7000 B Offset
	 *  }
	 * })
	 */
	if (wgds->revision > MAX_GEO_OFFSET_REVISION) {
		printk(BIOS_ERR, "ERROR: Invalid WGDS revision: %d\n", wgds->revision);
		return;
	}

	package_size = 1 + wgds->chains_count * wgds->bands_count;

	acpigen_write_name("WGDS");
	acpigen_write_package(2);
	acpigen_write_dword(wgds->revision);
	/* Emit 'Domain Type' +
	 * Group specific delta of power (6 bytes * NUM_WGDS_SAR_GROUPS)
	 */
	acpigen_write_package(package_size);
	acpigen_write_dword(DOMAIN_TYPE_WIFI);

	for (set_num = 0; set_num < wgds->chains_count; set_num++) {
		set = wgds_fetch_set(wgds, set_num);
		for (i = 0; i < wgds->bands_count; i++)
			acpigen_write_byte(set[i]);
	}

	acpigen_write_package_end();
	acpigen_write_package_end();
}

static void sar_emit_ppag(struct gain_profile *ppag)
{
	int i;
	size_t package_size, set_num;
	const uint8_t *set;

	if (ppag == NULL)
		return;

	/*
	 * Name ("PPAG", Package () {
	 *   Revision,
	 *   Package () {
	 *     Domain Type,		// 0x7:WiFi
	 *     PPAG Mode,		// Defines the mode of ANT_gain control to be used
	 *     ANT_gain Table Chain A	// Defines the ANT_gain in dBi for chain A
	 *     ANT_gain Table Chain B	// Defines the ANT_gain in dBi for chain B
	 *   }
	 * })
	 */
	if (ppag->revision > MAX_ANT_GAINS_REVISION) {
		printk(BIOS_ERR, "Invalid PPAG revision: %d\n", ppag->revision);
		return;
	}

	package_size = 1 + 1 + ppag->chains_count * ppag->bands_count;

	acpigen_write_name("PPAG");
	acpigen_write_package(2);
	acpigen_write_dword(ppag->revision);
	acpigen_write_package(package_size);
	acpigen_write_dword(DOMAIN_TYPE_WIFI);
	acpigen_write_dword(ppag->mode);

	for (set_num = 0; set_num < ppag->chains_count; set_num++) {
		set = ppag_fetch_set(ppag, set_num);
		for (i = 0; i < ppag->bands_count; i++)
			acpigen_write_byte(set[i]);
	}

	acpigen_write_package_end();
	acpigen_write_package_end();
}

static void sar_emit_wtas(struct avg_profile *wtas)
{
	int i;
	size_t package_size;

	if (wtas == NULL)
		return;

	/*
	 * Name (WTAS, Package() {
	 * {
	 *   Revision,
	 *   Package()
	 *   {
	 *     DomainType,            // 0x7:WiFi
	 *     WifiTASSelection,      // Enable/Disable the TAS feature
	 *     WifiTASListEntries,    // No. of blocked countries not approved by OEM to
	 *     BlockedListEntry1,        support this feature
	 *     BlockedListEntry2,
	 *     BlockedListEntry3,
	 *     BlockedListEntry4,
	 *     BlockedListEntry5,
	 *     BlockedListEntry6,
	 *     BlockedListEntry7,
	 *     BlockedListEntry8,
	 *     BlockedListEntry9,
	 *     BlockedListEntry10,
	 *     BlockedListEntry11,
	 *     BlockedListEntry12,
	 *     BlockedListEntry13,
	 *     BlockedListEntry14,
	 *     BlockedListEntry15,
	 *     BlockedListEntry16,
	 *   }
	 * })
	 */
	package_size = 1 + 1 + 1 + MAX_DENYLIST_ENTRY;

	acpigen_write_name("WTAS");
	acpigen_write_package(2);
	acpigen_write_dword(wtas->revision);
	acpigen_write_package(package_size);
	acpigen_write_dword(DOMAIN_TYPE_WIFI);
	acpigen_write_dword(wtas->tas_selection);
	acpigen_write_dword(wtas->tas_list_size);
	for (i = 0; i < MAX_DENYLIST_ENTRY; i++)
		acpigen_write_byte(wtas->deny_list_entry[i]);

	acpigen_write_package_end();
	acpigen_write_package_end();
}

static void emit_sar_acpi_structures(const struct device *dev)
{
	union wifi_sar_limits sar_limits;

	/*
	 * If device type is PCI, ensure that the device has Intel vendor ID. CBFS SAR and SAR
	 * ACPI tables are currently used only by Intel WiFi devices.
	 */
	if (dev->path.type == DEVICE_PATH_PCI && dev->vendor != PCI_VENDOR_ID_INTEL)
		return;

	/* Retrieve the sar limits data */
	if (get_wifi_sar_limits(&sar_limits) < 0) {
		printk(BIOS_ERR, "ERROR: failed getting SAR limits!\n");
		return;
	}

	sar_emit_wrds(sar_limits.sar);
	sar_emit_ewrd(sar_limits.sar);
	sar_emit_wgds(sar_limits.wgds);
	sar_emit_ppag(sar_limits.ppag);
	sar_emit_wtas(sar_limits.wtas);

	free(sar_limits.sar);
}

static void wifi_ssdt_write_device(const struct device *dev, const char *path)
{
	/* Device */
	acpigen_write_device(path);
	acpi_device_write_uid(dev);

	if (dev->chip_ops)
		acpigen_write_name_string("_DDN", dev->chip_ops->name);

	/* Address */
	acpigen_write_ADR_pci_device(dev);

	acpigen_pop_len(); /* Device */
}

static void wifi_ssdt_write_properties(const struct device *dev, const char *scope)
{
	const struct drivers_wifi_generic_config *config = dev->chip_info;

	/* Scope */
	acpigen_write_scope(scope);

	/* Wake capabilities */
	if (config)
		acpigen_write_PRW(config->wake, ACPI_S3);

	/* Fill regulatory domain structure */
	if (CONFIG(HAVE_REGULATORY_DOMAIN)) {
		/*
		 * Name ("WRDD", Package () {
		 *   WRDD_REVISION, // Revision
		 *   Package () {
		 *     DOMAIN_TYPE_WIFI,        // Domain Type, 7:WiFi
		 *     wifi_regulatory_domain() // Country Identifier
		 *   }
		 * })
		 */
		acpigen_write_name("WRDD");
		acpigen_write_package(2);
		acpigen_write_integer(WRDD_REVISION);
		acpigen_write_package(2);
		acpigen_write_dword(DOMAIN_TYPE_WIFI);
		acpigen_write_dword(wifi_regulatory_domain());
		acpigen_pop_len();
		acpigen_pop_len();
	}

	/* Fill Wifi sar related ACPI structures */
	if (CONFIG(USE_SAR))
		emit_sar_acpi_structures(dev);

	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s %s\n", scope, dev->chip_ops ? dev->chip_ops->name : "",
	       dev_path(dev));
}

void wifi_pcie_fill_ssdt(const struct device *dev)
{
	const char *path;

	path = acpi_device_path(dev);
	if (!path)
		return;

	wifi_ssdt_write_device(dev, path);
	wifi_ssdt_write_properties(dev, path);
}

const char *wifi_pcie_acpi_name(const struct device *dev)
{
	static char wifi_acpi_name[WIFI_ACPI_NAME_MAX_LEN];

	/* ACPI 6.3, ASL 20.2.2: (Name Objects Encoding). */
	snprintf(wifi_acpi_name, sizeof(wifi_acpi_name), "WF%02X",
		 (dev_path_encode(dev) & 0xff));
	return wifi_acpi_name;
}

void wifi_cnvi_fill_ssdt(const struct device *dev)
{
	const char *path;

	path = acpi_device_path(dev->bus->dev);
	if (!path)
		return;

	wifi_ssdt_write_properties(dev, path);
}
