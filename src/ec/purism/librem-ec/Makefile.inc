ifeq ($(CONFIG_EC_LIBREM_EC),y)

all-y += librem_ec.c
smm-$(CONFIG_DEBUG_SMI) += librem_ec.c

endif
