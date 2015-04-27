ifeq ($(CONFIG_EC_LENOVO_PMH7),y)

ramstage-y += pmh7.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += pmh7.c
romstage-y += pmh7.c

endif
