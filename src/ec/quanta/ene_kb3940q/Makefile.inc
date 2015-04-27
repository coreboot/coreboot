ifeq ($(CONFIG_EC_QUANTA_ENE_KB3940Q),y)

ramstage-y += ec.c
smm-y += ec.c
smm-y += ../../../lib/delay.c

endif
