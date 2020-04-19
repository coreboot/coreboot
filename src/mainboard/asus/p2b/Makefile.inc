bootblock-y += bootblock.c
romstage-$(CONFIG_BOARD_ASUS_P3B_F) += variants/$(VARIANT_DIR)/romstage.c

ramstage-$(CONFIG_GENERATE_PIRQ_TABLE) += variants/$(VARIANT_DIR)/irq_tables.c
ramstage-$(CONFIG_GENERATE_MP_TABLE) += variants/$(VARIANT_DIR)/mptable.c
