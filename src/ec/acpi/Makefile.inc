ifeq ($(CONFIG_EC_ACPI),y)

ramstage-y += ec.c
romstage-y += ec.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += ec.c

endif
