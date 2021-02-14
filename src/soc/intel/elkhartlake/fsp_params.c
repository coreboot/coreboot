/* SPDX-License-Identifier: GPL-2.0-only */
#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <intelblocks/lpss.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>

/*
 * ME End of Post configuration
 * 0 - Disable EOP.
 * 1 - Send in PEI (Applicable for FSP in API mode)
 * 2 - Send in DXE (Not applicable for FSP in API mode)
 */
enum {
	EOP_DISABLE,
	EOP_PEI,
	EOP_DXE,
} EndOfPost;

static const pci_devfn_t serial_io_dev[] = {
	PCH_DEVFN_I2C0,
	PCH_DEVFN_I2C1,
	PCH_DEVFN_I2C2,
	PCH_DEVFN_I2C3,
	PCH_DEVFN_I2C4,
	PCH_DEVFN_I2C5,
	PCH_DEVFN_I2C6,
	PCH_DEVFN_I2C7,
	PCH_DEVFN_GSPI0,
	PCH_DEVFN_GSPI1,
	PCH_DEVFN_GSPI2,
	PCH_DEVFN_UART0,
	PCH_DEVFN_UART1,
	PCH_DEVFN_UART2
};

static void parse_devicetree(FSP_S_CONFIG *params)
{
	/* TODO: Update with UPD override as FSP matures */
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	FSP_S_CONFIG *params = &supd->FspsConfig;

	/* Parse device tree and fill in FSP UPDs */
	parse_devicetree(params);

	/* TODO: Update with UPD override as FSP matures */

	/* Override/Fill FSP Silicon Param for mainboard */
	mainboard_silicon_init_params(params);
}

/* Mainboard GPIO Configuration */
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Return list of SOC LPSS controllers */
const pci_devfn_t *soc_lpss_controllers_list(size_t *size)
{
	*size = ARRAY_SIZE(serial_io_dev);
	return serial_io_dev;
}
