/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PICASSO_PLATFORM_DESCRIPTORS_H
#define AMD_PICASSO_PLATFORM_DESCRIPTORS_H

#include <types.h>
#include <vendorcode/amd/fsp/picasso/platform_descriptors.h>

/* These temporary macros apply to emmc0_mode field in FSP_S_CONFIG.
 * TODO: Remove when official definitions arrive. */
#define SD_DISABLE		0
#define SD_LOW_SPEED		1
#define SD_HIGH_SPEED		2
#define SD_UHS_I_SDR_50		3
#define SD_UHS_I_DDR_50		4
#define SD_UHS_I_SDR_104	5
#define EMMC_SDR_26		6
#define EMMC_SDR_52		7
#define EMMC_DDR_104		8
#define EMMC_HS200		9
#define EMMC_HS400		10
#define EMMC_HS300		11

struct picasso_smu_telemetry {
	uint32_t vdd_slope;
	int32_t vdd_offset;
	uint32_t soc_slope;
	int32_t soc_offset;
};

/* Devicetree telemetry calibration with the mainboard adjustments applied */
struct picasso_smu_telemetry picasso_get_smu_telemetry(void);

/* Allow the mainboard to adjust the SMU telemetry calibration from the devicetree */
void mainboard_update_picasso_smu_telemetry(struct picasso_smu_telemetry *telemetry);

/* Mainboard callback to obtain the DXIO/PCIe and DDI descriptors */
void mainboard_get_dxio_ddi_descriptors(
		const dxio_descriptor **dxio_descs, size_t *dxio_num,
		const ddi_descriptor **ddi_descs, size_t *ddi_num);

void mb_pre_fspm(void);

#endif /* AMD_PICASSO_PLATFORM_DESCRIPTORS_H */
