/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <soc/gpio.h>
#include <stdint.h>
#include <stdio.h>
#include <hwilib.h>
#include <i210.h>
#include <soc/ramstage.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <cpu/x86/msr.h>
#include <soc/msr.h>

#define MAX_PATH_DEPTH		12
#define MAX_NUM_MAPPINGS	10

/** \brief This function can decide if a given MAC address is valid or not.
*	  Currently, addresses filled with 0xff or 0x00 are not valid.
 * @param  mac	Buffer to the MAC address to check
 * @return 0	if address is not valid, otherwise 1
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
 *	   to a MACPHY.
 * @param  dev	   pointer to PCI device
 * @param  mac	   buffer where to store the MAC address
 * @return cb_err  CB_ERR or CB_SUCCESS
 */

enum cb_err mainboard_get_mac_address(struct device *dev, uint8_t mac[MAC_ADDR_LEN])
{
	struct bus *parent = dev->upstream;
	uint8_t buf[16], mapping[16], i = 0, chain_len = 0;

	memset(buf, 0, sizeof(buf));
	memset(mapping, 0, sizeof(mapping));

	/* The first entry in the tree is the device itself. */
	buf[0] = dev->path.pci.devfn;
	chain_len = 1;
	for (i = 1; i < MAX_PATH_DEPTH && parent->dev->upstream->subordinate; i++) {
		buf[i] = parent->dev->path.pci.devfn;
		chain_len++;
		parent = parent->dev->upstream;
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

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	params->TurboMode = 0;
}

static void disable_c1e(void)
{
	msr_t msr;

	msr = rdmsr(MSR_POWER_CTL);
	msr.lo &= ~(1 << 1);
	wrmsr(MSR_POWER_CTL, msr);
}

static void mainboard_final(void *chip_info)
{
	struct device *dev;

	if (CONFIG(PCI_ALLOW_BUS_MASTER_ANY_DEVICE)) {
		/* Enable bus mastering - target OS requires firmware initialization */
		dev = dev_find_device(PCI_VID_SIEMENS, 0x403f, 0);
		if (dev)
			pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);
	}

	disable_c1e();
}

static void mainboard_init(void *chip_info)
{
	variant_configure_gpio_pads();

}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.final = mainboard_final,
};
