$(eval $(call asl_template,ssdt2))
$(eval $(call asl_template,ssdt3))
$(eval $(call asl_template,ssdt4))
$(eval $(call asl_template,ssdt5))

romstage-y += resourcemap.c

ramstage-y += get_bus_conf.c
