/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_PANTHERLAKE_ROMSTAGE_UX_H_
#define _SOC_INTEL_PANTHERLAKE_ROMSTAGE_UX_H_

bool ux_inform_user_of_update_operation(const char *name, FSPM_UPD *mupd);
bool ux_inform_user_of_poweroff_operation(const char *name, FSPM_UPD *mupd);

#endif /* _SOC_INTEL_PANTHERLAKE_ROMSTAGE_UX_H_ */
