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

endif
