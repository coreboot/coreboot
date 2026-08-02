/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/option/cfr_frontend.h>
#include <southbridge/intel/bd82x6x/cfr.h>
#include <string.h>

/* Intel ME State */
static const struct sm_object me_state = SM_DECLARE_ENUM({
	.opt_name	= "me_state",
	.ui_name	= "Boot State",
	.ui_helptext	= "Enable or disable the Intel Management Engine",
	.default_value	= CMOS_ME_STATE_NORMAL,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",		CMOS_ME_STATE_DISABLED	},
				{ "Enabled",		CMOS_ME_STATE_NORMAL	},
				SM_ENUM_VALUE_END				},
});

/* Intel ME State on previous boot */
static const struct sm_object me_state_prev = SM_DECLARE_NUMBER({
	.opt_name	= "me_state_prev",
	.ui_name	= "ME State Previous Boot",
	.flags		= CFR_OPTFLAG_SUPPRESS,
	.default_value	= CMOS_ME_STATE_NORMAL,
});

static char version[32] = "Unknown";
static struct sm_object me_version = SM_DECLARE_VARCHAR({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "me_version",
	.ui_name	= "Version",
	.ui_helptext	= "The current version of the Intel Management Engine firmware",
	.default_value	= version,
});

static struct sm_object me_operational_mode = SM_DECLARE_VARCHAR({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "me_operational_mode",
	.ui_name	= "Current Operation Mode",
	.ui_helptext	= "The current operational mode of the Intel Management Engine",
	.default_value	= "Unknown",
});

static struct sm_object me_operational_state = SM_DECLARE_VARCHAR({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "me_operational_state",
	.ui_name	= "Current Operation State",
	.ui_helptext	= "The current operational state of the Intel Management Engine",
	.default_value	= "Unknown",
});

static struct sm_object me_working_state = SM_DECLARE_VARCHAR({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "me_working_state",
	.ui_name	= "Current Working State",
	.default_value	= "Unknown",
});

static struct sm_object me_progress_phase = SM_DECLARE_VARCHAR({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "me_progress_phase",
	.ui_name	= "ME Progress Phase",
	.default_value	= "Unknown",
});

void sb_cfr_set_string(enum sb_cfr_strings e, const char *value)
{
	switch (e) {
	case ME_STRING_VERSION:
		strncpy(version, value, sizeof(version) - 1);
		version[sizeof(version) - 1] = '\0';
		break;
	case ME_STRING_OPERATIONAL_STATE:
		me_operational_state.sm_varchar.default_value = value;
		break;
	case ME_STRING_OPERATIONAL_MODE:
		me_operational_mode.sm_varchar.default_value = value;
		break;
	case ME_STRING_WORKING_STATE:
		me_working_state.sm_varchar.default_value = value;
		break;
	case ME_STRING_PROGRESS_PHASE:
		me_progress_phase.sm_varchar.default_value = value;
		break;
	default:
		return;
	}
}

__cfr_form static struct sm_obj_form cfr_sm_management_engine = {
	.ui_name = "Intel Management Engine",
	.obj_list = (const struct sm_object *[]) {
		&me_state,
		&me_state_prev,
		&me_version,
		&me_operational_mode,
		&me_operational_state,
		&me_working_state,
		&me_progress_phase,
		NULL,
	},
};
