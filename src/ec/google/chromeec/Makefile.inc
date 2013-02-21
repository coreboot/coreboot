ramstage-y += ec.c
smm-y += ec.c
romstage-y += ec.c

CFLAGS += -I $(call strip_quotes,$(CONFIG_EC_GOOGLE_API_ROOT))
