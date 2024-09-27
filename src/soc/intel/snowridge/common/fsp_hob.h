/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_FSP_HOB_H_
#define _SOC_SNOWRIDGE_FSP_HOB_H_

#include <fsp/soc_binding.h>
#include <stddef.h>
#include <stdint.h>
#include <uuid.h>

extern const guid_t fsp_hob_fia_override_status_guid;
extern const guid_t fsp_hob_iio_uds_data_guid;
extern const guid_t fsp_hob_kti_cache_guid;
extern const guid_t fsp_hob_smbios_memory_info_guid;

const BL_IIO_UDS *fsp_hob_get_iio_uds_data(void);
const void *fsp_hob_get_kti_cache(size_t *hob_size);
const FSP_SMBIOS_MEMORY_INFO *fsp_hob_get_memory_info(void);

#endif // _SOC_SNOWRIDGE_FSP_HOB_H_
