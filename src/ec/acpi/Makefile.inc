ifeq ($(CONFIG_EC_ACPI),y)

ramstage-y += ec.c
romstage-y += ec.c
smm-y += ec.c

endif
