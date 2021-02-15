/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_SMI_HANDLER_H
#define SOC_INTEL_COMMON_BLOCK_SMI_HANDLER_H

#include <device/device.h>
#include <stdint.h>

struct gpi_status;
struct global_nvs;

/*
 * The register value is used with get_reg and set_reg
 */
enum smm_reg {
	RAX,
	RBX,
	RCX,
	RDX,
};

struct smm_save_state_ops {
	/* return io_misc_info from SMM Save State Area */
	uint32_t (*get_io_misc_info)(void *state);

	/* return value of the requested register from
	 * SMM Save State Area
	 */
	uint64_t (*get_reg)(void *state, enum smm_reg reg);

	void (*set_reg)(void *state, enum smm_reg reg, uint64_t val);
};

typedef void (*smi_handler_t)(const struct smm_save_state_ops *save_state_ops);

/*
 * SOC SMI Handler has to provide this structure which has methods to access
 * the SOC specific SMM Save State Area
 */
const struct smm_save_state_ops *get_smm_save_state_ops(void);

/*
 * southbridge_smi should be defined inside SOC specific code and should have
 * handlers for any SMI events that need to be handled. Default handlers
 * for some SMI events are provided in soc/intel/common/block/smm/smihandler.c
 */
extern const smi_handler_t southbridge_smi[32];

/*
 * This function should be implemented in SOC specific code to handle
 * the SMI event on SLP_EN. The default functionality is provided in
 * soc/intel/common/block/smm/smihandler.c
 */
void smihandler_southbridge_sleep(
	const struct smm_save_state_ops *save_state_ops);

/*
 * This function should be implemented in SOC specific code to handle
 * SMI_APM event. The default functionality is provided in
 * soc/intel/common/block/smm/smihandler.c
 */
void smihandler_southbridge_apmc(
	const struct smm_save_state_ops *save_state_ops);

/*
 * This function should be implemented in SOC specific code to handle
 * SMI_PM1 event. The default functionality is provided in
 * soc/intel/common/block/smm/smihandler.c
 */
void smihandler_southbridge_pm1(
	const struct smm_save_state_ops *save_state_ops);

/*
 * This function should be implemented in SOC specific code to handle
 * SMI_GPE0 event. The default functionality is provided in
 * soc/intel/common/block/smm/smihandler.c
 */
void smihandler_southbridge_gpe0(
	const struct smm_save_state_ops *save_state_ops);

/*
 * This function should be implemented in SOC specific code to handle
 * MC event. The default functionality is provided in
 * soc/intel/common/block/smm/smihandler.c
 */
void smihandler_southbridge_mc(
	const struct smm_save_state_ops *save_state_ops);

/*
 * This function should be implemented in SOC specific code to handle
 * minitor event. The default functionality is provided in
 * soc/intel/common/block/smm/smihandler.c
 */
void smihandler_southbridge_monitor(
	const struct smm_save_state_ops *save_state_ops);
/*
 * This function should be implemented in SOC specific code to handle
 * SMI_TCO event. The default functionality is provided in
 * soc/intel/common/block/smm/smihandler.c
 */
void smihandler_southbridge_tco(
	const struct smm_save_state_ops *save_state_ops);

/*
 * This function should be implemented in SOC specific code to handle
 * SMI PERIODIC_STS event. The default functionality is provided in
 * soc/intel/common/block/smm/smihandler.c
 */
void smihandler_southbridge_periodic(
	const struct smm_save_state_ops *save_state_ops);

/*
 * This function should be implemented in SOC specific code to handle
 * SMI GPIO_STS event. The default functionality is provided in
 * soc/intel/common/block/smm/smihandler.c
 */
void smihandler_southbridge_gpi(
	const struct smm_save_state_ops *save_state_ops);

/*
 * This function should be implemented in SOC specific code to handle
 * SMI ESPI_STS event. The default functionality is provided in
 * soc/intel/common/block/smm/smihandler.c
 */
void smihandler_southbridge_espi(
	const struct smm_save_state_ops *save_state_ops);

/* SoC overrides. */

/* Specific SOC SMI handler during ramstage finalize phase */
void smihandler_soc_at_finalize(void);

/*
 * This function returns a 1 or 0 depending on whether disable_busmaster
 * needs to be done for the specified device on S5 entry
 */
int smihandler_soc_disable_busmaster(pci_devfn_t dev);

/* Mainboard overrides. */

/* Mainboard handler for GPI SMIs */
void mainboard_smi_gpi_handler(const struct gpi_status *sts);

/* Mainboard handler for ESPI EMIs */
void mainboard_smi_espi_handler(void);

extern const struct smm_save_state_ops em64t100_smm_ops;

extern const struct smm_save_state_ops em64t101_smm_ops;
#endif
