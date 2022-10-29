## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_PI),y)

romstage-y += agesawrapper.c
romstage-y += def_callouts.c
romstage-y += heapmanager.c
romstage-y += image.c
romstage-y += refcode_loader.c
romstage-y += s3_resume.c

ramstage-y += agesawrapper.c
ramstage-y += amd_late_init.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += amd_resume_final.c
ramstage-y += def_callouts.c
ramstage-y += heapmanager.c
ramstage-y += image.c
ramstage-y += refcode_loader.c
ramstage-y += s3_resume.c

CFLAGS_x86_32 += -Wno-pragma-pack
CFLAGS_x86_64 += -Wno-pragma-pack

endif # CONFIG_SOC_AMD_PI
