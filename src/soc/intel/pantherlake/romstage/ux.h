/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_PANTHERLAKE_ROMSTAGE_UX_H_
#define _SOC_INTEL_PANTHERLAKE_ROMSTAGE_UX_H_

#include <types.h>

bool ux_inform_user_of_update_operation(const char *name, FSPM_UPD *mupd);
bool ux_inform_user_of_poweroff_operation(const char *name, FSPM_UPD *mupd);

/* VGA initialization configuration */
#define VGA_INIT_CONTROL_ENABLE		BIT(0)
#define VGA_INIT_CONTROL_MODE12		BIT(1)
#define VGA_INIT_DISABLE_ANIMATION	BIT(4)

#endif /* _SOC_INTEL_PANTHERLAKE_ROMSTAGE_UX_H_ */
