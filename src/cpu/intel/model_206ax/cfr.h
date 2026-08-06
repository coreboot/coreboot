/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CPU_INTEL_MODEL_206AX_CFR_H_
#define _CPU_INTEL_MODEL_206AX_CFR_H_

#include <drivers/option/cfr_frontend.h>

/* Hyper-Threading */
static const struct sm_object hyper_threading = SM_DECLARE_BOOL({
	.opt_name	= "hyper_threading",
	.ui_name	= "Hyper-Threading",
	.ui_helptext	= "Enable or disable Hyper-Threading",
	.default_value	= true,
});

#endif /* _CPU_INTEL_MODEL_206AX_CFR_H_ */
