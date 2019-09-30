ifeq ($(CONFIG_DRIVERS_ASPEED_AST_COMMON),y)
ramstage-y += ast_dp501.c ast_main.c ast_post.c
ramstage-$(CONFIG_GENERIC_LINEAR_FRAMEBUFFER) += ast_mode.c ast_i2c.c ast_mode_corebootfb.c
endif
