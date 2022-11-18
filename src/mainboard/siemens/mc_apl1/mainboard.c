/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <hwilib.h>
#include <i210.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/systemagent.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <string.h>
#include <timer.h>
#include <timestamp.h>
#include <baseboard/variants.h>
#include <types.h>

#define MAX_PATH_DEPTH		12
#define MAX_NUM_MAPPINGS	10

#define BIOS_MAILBOX_WAIT_MAX_MS	1000
#define BIOS_MAILBOX_DATA		0x7080
#define BIOS_MAILBOX_INTERFACE		0x7084
#define  RUN_BUSY_STS			(1 << 31)

#define SD_CAP_BYP		0x810
#define  SD_CAP_BYP_EN		0x5A
#define SD_CAP_BYP_REG1		0x814

/** \brief This function can decide if a given MAC address is valid or not.
 *         Currently, addresses filled with 0xff or 0x00 are not valid.
 * @param  mac  Buffer to the MAC address to check
 * @return 0    if address is not valid, otherwise 1
 */
static uint8_t is_mac_adr_valid(uint8_t mac[MAC_ADDR_LEN])
{
	for (size_t i = 0; i < MAC_ADDR_LEN; i++) {
		if (mac[i] != 0x00 && mac[i] != 0xff)
			return 1;
		if (mac[i] != mac[0])
			return 1;
	}
	return 0;
}

/** \brief This function will search for a MAC address which can be assigned
 *         to a MACPHY.
 * @param  dev     pointer to PCI device
 * @param  mac     buffer where to store the MAC address
 * @return cb_err  CB_ERR or CB_SUCCESS
 */
enum cb_err mainboard_get_mac_address(struct device *dev, uint8_t mac[MAC_ADDR_LEN])
{
	struct bus *parent = dev->bus;
	uint8_t buf[16], mapping[16], i = 0, chain_len = 0;

	memset(buf, 0, sizeof(buf));
	memset(mapping, 0, sizeof(mapping));

	/* The first entry in the tree is the device itself. */
	buf[0] = dev->path.pci.devfn;
	chain_len = 1;
	for (i = 1; i < MAX_PATH_DEPTH && parent->dev->bus->subordinate; i++) {
		buf[i] = parent->dev->path.pci.devfn;
		chain_len++;
		parent = parent->dev->bus;
	}
	if (i == MAX_PATH_DEPTH) {
		/* The path is deeper than MAX_PATH_DEPTH devices, error. */
		printk(BIOS_ERR, "Too many bridges for %s\n", dev_path(dev));
		return CB_ERR;
	}
	/*
	 * Now construct the mapping based on the device chain starting from
	 * root bridge device to the device itself.
	 */
	mapping[0] = 1;
	mapping[1] = chain_len;
	for (i = 0; i < chain_len; i++)
		mapping[i + 4] = buf[chain_len - i - 1];

	/* Open main hwinfo block */
	if (hwilib_find_blocks("hwinfo.hex") != CB_SUCCESS)
		return CB_ERR;
	/* Now try to find a valid MAC address in hwinfo for this mapping. */
	for (i = 0; i < MAX_NUM_MAPPINGS; i++) {
		if (hwilib_get_field(XMac1Mapping + i, buf, 16) != 16)
			continue;
		if (memcmp(buf, mapping, chain_len + 4))
			continue;
		/* There is a matching mapping available, get MAC address. */
		if (hwilib_get_field(XMac1 + i, mac, MAC_ADDR_LEN) == MAC_ADDR_LEN) {
			if (is_mac_adr_valid(mac))
				return CB_SUCCESS;
		}
		return CB_ERR;
	}
	/* No MAC address found for */
	return CB_ERR;
}

/** \brief This function fixes an accuracy issue with IDT PMIC.
 *         The current reported system power consumption is higher than the
 *         actual consumption. With a correction of slope and offset for Vcc
 *         and Vnn, the issue is solved.
 */
static void config_pmic_imon(void)
{
	struct stopwatch sw;
	uint32_t power_max;

	printk(BIOS_DEBUG, "PMIC: Configure PMIC IMON - Start\n");

	/* Calculate CPU TDP in mW */
	power_max = cpu_get_power_max();
	printk(BIOS_INFO, "PMIC: CPU TDP %d mW.\n", power_max);

	/*
	 * Fix Vnn slope and offset value.
	 * slope  = 0x4a4  # 2.32
	 * offset = 0xfa0d # -2.975
	 */
	stopwatch_init_msecs_expire(&sw, BIOS_MAILBOX_WAIT_MAX_MS);
	/* Read P_CR_BIOS_MAILBOX_INTERFACE_0_0_0_MCHBAR and check RUN_BUSY. */
	while ((MCHBAR32(BIOS_MAILBOX_INTERFACE) & RUN_BUSY_STS)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "PMIC: Power consumption measurement "
					"setup fails for Vnn.\n");
			return;
		}
	}
	/* Set Vnn values into P_CR_BIOS_MAILBOX_DATA_0_0_0_MCHBAR. */
	MCHBAR32(BIOS_MAILBOX_DATA) = 0xfa0d04a4;
	/* Set command, address and busy bit. */
	MCHBAR32(BIOS_MAILBOX_INTERFACE) = 0x8000011d;
	printk(BIOS_DEBUG, "PMIC: Fix Vnn slope and offset value.\n");

	/*
	 * Fix Vcc slope and offset value.
	 * Premium and High SKU:
	 * slope  = 0x466  # 2.2
	 * offset = 0xe833 # -11.9
	 * Low and Intermediate SKU:
	 * slope  = 0x3b3  # 1.85
	 * offset = 0xed33 # -9.4
	 */
	stopwatch_init_msecs_expire(&sw, BIOS_MAILBOX_WAIT_MAX_MS);
	while ((MCHBAR32(BIOS_MAILBOX_INTERFACE) & RUN_BUSY_STS)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "PMIC: Power consumption measurement "
					"setup fails for Vcc.\n");
			return;
		}
	}

	/*
	 * CPU TDP limit between Premium/High and Low/Intermediate SKU
	 * is 9010 mW.
	 */
	if (power_max > 9010) {
		MCHBAR32(BIOS_MAILBOX_DATA) = 0xe8330466;
		MCHBAR32(BIOS_MAILBOX_INTERFACE) = 0x8000001d;
		printk(BIOS_INFO, "PMIC: Fix Vcc for Premium SKU.\n");
	} else {
		MCHBAR32(BIOS_MAILBOX_DATA) = 0xed3303b3;
		MCHBAR32(BIOS_MAILBOX_INTERFACE) = 0x8000001d;
		printk(BIOS_INFO, "PMIC: Fix Vcc for Low SKU.\n");
	}

	printk(BIOS_DEBUG, "PMIC: Configure PMIC IMON - End\n");
}

void mainboard_silicon_init_params(FSP_S_CONFIG *silconfig)
{
	printk(BIOS_DEBUG, "MAINBOARD: %s/%s called\n", __FILE__, __func__);

	/* Disable CPU power states (C-states) */
	silconfig->EnableCx = 0;

	/* Set max Pkg Cstate to PkgC0C1 */
	silconfig->PkgCStateLimit = 0;

	/* Disable PCIe Transmitter Half Swing for all RPs */
	memset(silconfig->PcieRpTransmitterHalfSwing, 0,
			sizeof(silconfig->PcieRpTransmitterHalfSwing));

	/* Disable PCI Express Active State Power Management for all RPs */
	memset(silconfig->PcieRpAspm, 0,
			sizeof(silconfig->PcieRpAspm));

	/* Disable PCI Express L1 Substate for all RPs */
	memset(silconfig->PcieRpL1Substates, 0,
			sizeof(silconfig->PcieRpL1Substates));
}

static void mainboard_init(void *chip_info)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);

	config_pmic_imon();
}

static void mainboard_final(void *chip_info)
{
	struct device *dev = NULL;

	/* Do board specific things */
	variant_mainboard_final();

	/* Set Master Enable for on-board PCI device if allowed. */
	if (CONFIG(PCI_ALLOW_BUS_MASTER_ANY_DEVICE)) {
		dev = dev_find_device(PCI_VID_SIEMENS, 0x403f, 0);
		if (dev) {
			pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);
		}
	}
	/* Set up SPI OPCODE menu before the controller is locked. */
	fast_spi_set_opcode_menu();

	/* Set SD-Card speed to HS mode only. */
	dev = pcidev_path_on_root(PCH_DEVFN_SDCARD);
	if (dev) {
		uint32_t reg;
		struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);
		if (!res)
			return;

		write32(res2mmio(res, SD_CAP_BYP, 0), SD_CAP_BYP_EN);
		reg = read32(res2mmio(res, SD_CAP_BYP_REG1, 0));
		/* Disable all UHS-I SD-Card speed modes, keep only HS mode. */
		reg &= ~0x2000f800;
		write32(res2mmio(res, SD_CAP_BYP_REG1, 0), reg);
	}
}

/* The following function performs board specific things. */
void __weak variant_mainboard_final(void)
{
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.final = mainboard_final,
};

static void wait_for_legacy_dev(void *unused)
{
	uint32_t legacy_delay, us_since_boot;
	struct stopwatch sw;

	if (CONFIG(BOARD_SIEMENS_MC_APL4) || CONFIG(BOARD_SIEMENS_MC_APL7))
		return;

	/* Open main hwinfo block. */
	if (hwilib_find_blocks("hwinfo.hex") != CB_SUCCESS)
		return;

	/* Get legacy delay parameter from hwinfo. */
	if (hwilib_get_field(LegacyDelay, (uint8_t *)&legacy_delay,
			      sizeof(legacy_delay)) != sizeof(legacy_delay))
		return;

	us_since_boot = get_us_since_boot();
	/* No need to wait if the time since boot is already long enough.*/
	if (us_since_boot > legacy_delay)
		return;
	stopwatch_init_msecs_expire(&sw, (legacy_delay - us_since_boot) / 1000);
	printk(BIOS_NOTICE, "Wait remaining %d of %d us for legacy devices...",
			legacy_delay - us_since_boot, legacy_delay);
	stopwatch_wait_until_expired(&sw);
	printk(BIOS_NOTICE, "done!\n");
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_ENTRY, wait_for_legacy_dev, NULL);
