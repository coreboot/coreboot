# SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_VENDOR_STARLABS) += cfr
subdirs-$(CONFIG_VENDOR_STARLABS) += hda
subdirs-$(CONFIG_VENDOR_STARLABS) += powercap
subdirs-$(CONFIG_VENDOR_STARLABS) += pin_mux
subdirs-$(CONFIG_VENDOR_STARLABS) += smbios

ramstage-$(CONFIG_STARLABS_NVME_POWER_SEQUENCE) += nvme_seq.c

CPPFLAGS_common += -I$(src)/mainboard/starlabs/common/include

ramstage-$(CONFIG_STARLABS_ACPI_EFI_OPTION_SMI) += gnvs.c
smm-$(CONFIG_STARLABS_ACPI_EFI_OPTION_SMI) += smihandler.c
