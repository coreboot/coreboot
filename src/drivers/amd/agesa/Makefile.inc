# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_DRIVERS_AMD_PI),y)

romstage-y += romstage.c
romstage-y += mtrr_fixme.c
romstage-y += state_machine.c

ramstage-y += state_machine.c

bootblock-y += bootblock.c
bootblock-y += cache_as_ram.S

postcar-y += exit_car.S

romstage-y += def_callouts.c
romstage-y += eventlog.c

ramstage-y += def_callouts.c
ramstage-y += eventlog.c
ramstage-y += heapmanager.c
ramstage-y += acpi_tables.c

romstage-$(CONFIG_CPU_AMD_AGESA) += oem_s3.c
ramstage-$(CONFIG_CPU_AMD_AGESA) += oem_s3.c s3_mtrr.c

endif
