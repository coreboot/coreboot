bootblock-y += gpio.c
bootblock-y += helpers.c

romstage-y += helpers.c
romstage-y += tpm_tis.c
romstage-y += gpio.c

ramstage-y += gpio.c
ramstage-y += helpers.c
ramstage-y += tpm_tis.c

verstage-y += gpio.c
verstage-y += tpm_tis.c

smm-y += gpio.c
