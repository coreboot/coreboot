/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <variants.h>
#include <types.h>

/* Early pad configuration in bootblock. */
const struct pad_config early_gpio_table[] = {
	/* GPIO_64:	UART2_TXD	*/
	PAD_CFG_NF(GPIO_64, UP_20K, DEEP, NF1),
	/* GPIO_65:	UART2_RXD	*/
	PAD_CFG_NF(GPIO_65, UP_20K, DEEP, NF1),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Pad configuration in ramstage. */
/* clang-format off */
const struct pad_config gpio_table[] = {
	/* General Purpose I/O Deep */
	PAD_CFG_NF(GPIO_99, UP_20K, DEEP, NF1),                                                 /* Power Button */
	PAD_CFG_NF(GPIO_100, NONE, DEEP, NF1),                                                  /* Sleep S0 */
	PAD_CFG_NF(GPIO_101, NONE, DEEP, NF1),                                                  /* Sleep S3 */
	PAD_CFG_NF(GPIO_102, NONE, DEEP, NF1),                                                  /* Sleep S4 */
	PAD_CFG_NF(GPIO_103, NONE, DEEP, NF1),                                                  /* Suspend Power Ack */
	PAD_CFG_NF(GPIO_106, UP_20K, DEEP, NF1),                                                /* Battery Low */
	PAD_CFG_NF(GPIO_108, NONE, DEEP, NF1),                                                  /* Suspend Clock */
	PAD_CFG_NF(GPIO_109, NONE, DEEP, NF1),                                                  /* Suspend Status */
	PAD_CFG_TERM_GPO(GPIO_142, 1, UP_20K, DEEP),                                            /* Wake */

	/* SPI Flash */
	PAD_CFG_NF(GPIO_90, NATIVE, DEEP, NF1),                                                 /* Chip Select */
	PAD_CFG_NF(GPIO_92, DN_20K, DEEP, NF1),                                                 /* MOSI */
	PAD_CFG_NF(GPIO_93, NATIVE, DEEP, NF1),                                                 /* MISO */
	PAD_CFG_NF(GPIO_94, NATIVE, DEEP, NF1),                                                 /* IO 2 */
	PAD_CFG_NF(GPIO_95, NATIVE, DEEP, NF1),                                                 /* IO 3 */
	PAD_CFG_NF(GPIO_96, NATIVE, DEEP, NF1),                                                 /* Clock */
	PAD_CFG_NF(GPIO_97, NATIVE, DEEP, NF1),                                                 /* Clock Feedback */

	/* Touchpad */
	PAD_CFG_NF(GPIO_114, NONE, DEEP, NF1),                                                  /* Data */
	PAD_CFG_NF(GPIO_115, NONE, DEEP, NF1),                                                  /* Clock */
	PAD_CFG_GPI_GPIO_DRIVER(GPIO_177, NONE, PLTRST),                                        /* Interrupt */

	/* Wireless */
	PAD_CFG_TERM_GPO(GPIO_33, 1, UP_20K, DEEP),                                             /* Bluetooth RF Kill */
	PAD_CFG_GPO(GPIO_34, 1, DEEP),                                                          /* WiFi RF Kill */
	PAD_CFG_NF(GPIO_191, NONE, DEEP, NF1),                                                  /* BRI Data */
	PAD_CFG_NF(GPIO_192, UP_20K, DEEP, NF1),                                                /* BRI Response */
	PAD_CFG_NF(GPIO_193, NONE, DEEP, NF1),                                                  /* RGI Data */
	PAD_CFG_NF(GPIO_194, UP_20K, DEEP, NF1),                                                /* RGI Response */
	PAD_CFG_NF(GPIO_195, NONE, DEEP, NF1),                                                  /* RF Reset */
	PAD_CFG_NF(GPIO_196, NONE, DEEP, NF1),                                                  /* XTAL Clock Request */

	/* Display */
	PAD_CFG_NF(GPIO_124, UP_20K, DEEP, NF1),                                                /* DDC Data */
	PAD_CFG_NF(GPIO_125, UP_20K, DEEP, NF1),                                                /* DDC Clock */
	PAD_CFG_NF(GPIO_128, NONE, DEEP, NF1),                                                  /* Panel Power */
	PAD_CFG_NF(GPIO_129, NONE, DEEP, NF1),                                                  /* Backlight Enable */
	PAD_CFG_NF(GPIO_130, NONE, DEEP, NF1),                                                  /* Backlight PWM */
	PAD_CFG_NF(GPIO_131, NONE, DEEP, NF1),                                                  /* HDMI Hot Plug */
	PAD_CFG_NF(GPIO_132, NONE, DEEP, NF1),                                                  /* DDI1 Hot Plug */
	PAD_CFG_NF(GPIO_133, NONE, DEEP, NF1),                                                  /* eDP Hot Plug */

	/* High-Definition Audio */
	PAD_CFG_NF(GPIO_166, NONE, DEEP, NF1),                                                  /* Clock */
	PAD_CFG_NF(GPIO_167, NONE, DEEP, NF1),                                                  /* Sync */
	PAD_CFG_NF(GPIO_168, NONE, DEEP, NF1),                                                  /* Data Input */
	PAD_CFG_NF(GPIO_169, NONE, DEEP, NF1),                                                  /* Data Output */
	PAD_CFG_NF(GPIO_170, NONE, DEEP, NF1),                                                  /* Reset */

	/* PCH */
	PAD_CFG_NF(GPIO_98, NONE, DEEP, NF1),                                                   /* Platform Reset */
	PAD_CFG_GPI_APIC(GPIO_19, UP_20K, DEEP, EDGE_SINGLE, NONE),                             /* Interrupt */
	PAD_CFG_NF(GPIO_74, UP_20K, DEEP, NF1),                                                 /* Thermal Trip */
	PAD_CFG_NF(GPIO_75, NONE, DEEP, NF1),                                                   /* Processor Hot */
	PAD_CFG_NF(GPIO_107, UP_20K, DEEP, NF1),                                                /* Reset Button */
	PAD_CFG_GPI_SCI(GPIO_38, UP_20K, DEEP, EDGE_SINGLE, INVERT),                            /* SCI */
	PAD_CFG_GPI_SMI(GPIO_67, UP_20K, DEEP, EDGE_SINGLE, NONE),                              /* SMI */

	/* SMBus */
	PAD_CFG_NF(GPIO_49, UP_1K, DEEP, NF1),                                                  /* Clock */
	PAD_CFG_NF(GPIO_48, UP_1K, DEEP, NF1),                                                  /* Data */

	/* Config Straps									[ Low      / High     ] */
	PAD_CFG_GPO(GPIO_27, 0, DEEP),                                                          /* eMMC		[ Disabled / Enabled  ] */
	PAD_CFG_GPO(GPIO_28, 1, DEEP),                                                          /* SPI		[ Disabled / Enabled  ] */
	PAD_CFG_GPO(GPIO_83, 0, DEEP),                                                          /* LPC		[ 3.3V     / 1.8V     ] */
	PAD_CFG_GPO(GPIO_84, 0, DEEP),                                                          /* SPI Boot	[ Enabled  / Disabled ] */
	PAD_CFG_GPO(GPIO_174, 1, DEEP),                                                         /* VDD2		[ 1.20V    / 1.24V    ] */
	PAD_CFG_GPO(GPIO_175, 0, DEEP),                                                         /* Bus		[ LPC      / eSPI     ] */

	PAD_NC(GPIO_0, DN_20K),
	PAD_NC(GPIO_1, DN_20K),
	PAD_NC(GPIO_2, DN_20K),
	PAD_NC(GPIO_3, DN_20K),
	PAD_NC(GPIO_4, DN_20K),
	PAD_NC(GPIO_5, DN_20K),
	PAD_NC(GPIO_6, DN_20K),
	PAD_NC(GPIO_7, DN_20K),
	PAD_NC(GPIO_8, DN_20K),
	PAD_NC(GPIO_9, DN_20K),
	PAD_NC(GPIO_10, DN_20K),
	PAD_NC(GPIO_11, DN_20K),
	PAD_NC(GPIO_12, DN_20K),
	PAD_NC(GPIO_13, DN_20K),
	PAD_NC(GPIO_14, DN_20K),
	PAD_NC(GPIO_15, DN_20K),
	PAD_NC(GPIO_16, NONE),
	PAD_NC(GPIO_17, DN_20K),
	PAD_NC(GPIO_18, DN_20K),
	PAD_NC(GPIO_20, DN_20K),
	PAD_NC(GPIO_21, DN_20K),
	PAD_NC(GPIO_22, DN_20K),
	PAD_NC(GPIO_23, DN_20K),
	PAD_NC(GPIO_24, DN_20K),
	PAD_NC(GPIO_25, DN_20K),
	PAD_NC(GPIO_26, DN_20K),
	PAD_NC(GPIO_29, DN_20K),
	PAD_NC(GPIO_30, DN_20K),
	PAD_NC(GPIO_31, DN_20K),
	PAD_NC(GPIO_32, DN_20K),
	PAD_NC(GPIO_35, DN_20K),
	PAD_NC(GPIO_36, DN_20K),
	PAD_NC(GPIO_37, DN_20K),
	PAD_NC(GPIO_39, DN_20K),
	PAD_NC(GPIO_40, DN_20K),
	PAD_NC(GPIO_41, DN_20K),
	PAD_NC(GPIO_42, DN_20K),
	PAD_NC(GPIO_43, DN_20K),
	PAD_NC(GPIO_44, DN_20K),
	PAD_NC(GPIO_45, DN_20K),
	PAD_NC(GPIO_46, DN_20K),
	PAD_NC(GPIO_47, DN_20K),
	PAD_NC(GPIO_50, DN_20K),
	PAD_NC(GPIO_51, DN_20K),
	PAD_NC(GPIO_52, DN_20K),
	PAD_NC(GPIO_53, DN_20K),
	PAD_NC(GPIO_54, DN_20K),
	PAD_NC(GPIO_55, DN_20K),
	PAD_NC(GPIO_56, DN_20K),
	PAD_NC(GPIO_57, DN_20K),
	PAD_NC(GPIO_58, DN_20K),
	PAD_NC(GPIO_59, DN_20K),
	PAD_NC(GPIO_60, DN_20K),
	PAD_NC(GPIO_61, DN_20K),
	PAD_NC(GPIO_62, DN_20K),
	PAD_NC(GPIO_63, DN_20K),
	PAD_NC(GPIO_66, DN_20K),
	PAD_NC(GPIO_68, DN_20K),
	PAD_NC(GPIO_69, DN_20K),
	PAD_NC(GPIO_70, DN_20K),
	PAD_NC(GPIO_71, DN_20K),
	PAD_NC(GPIO_72, DN_20K),
	PAD_NC(GPIO_73, DN_20K),
	PAD_NC(GPIO_76, DN_20K),
	PAD_NC(GPIO_77, DN_20K),
	PAD_NC(GPIO_78, DN_20K),
	PAD_NC(GPIO_79, DN_20K),
	PAD_NC(GPIO_80, DN_20K),
	PAD_NC(GPIO_81, DN_20K),
	PAD_NC(GPIO_82, DN_20K),
	PAD_NC(GPIO_85, DN_20K),
	PAD_NC(GPIO_86, DN_20K),
	PAD_NC(GPIO_87, DN_20K),
	PAD_NC(GPIO_88, DN_20K),
	PAD_NC(GPIO_89, DN_20K),
	PAD_NC(GPIO_91, DN_20K),
	PAD_NC(GPIO_104, UP_20K),
	PAD_NC(GPIO_105, UP_20K),
	PAD_NC(GPIO_110, DN_20K),
	PAD_NC(GPIO_111, DN_20K),
	PAD_NC(GPIO_112, DN_20K),
	PAD_NC(GPIO_113, DN_20K),
	PAD_NC(GPIO_116, DN_20K),
	PAD_NC(GPIO_117, DN_20K),
	PAD_NC(GPIO_118, DN_20K),
	PAD_NC(GPIO_119, DN_20K),
	PAD_NC(GPIO_120, DN_20K),
	PAD_NC(GPIO_121, DN_20K),
	PAD_NC(GPIO_122, DN_20K),
	PAD_NC(GPIO_123, DN_20K),
	PAD_NC(GPIO_126, DN_20K),
	PAD_NC(GPIO_127, DN_20K),
	PAD_NC(GPIO_134, DN_20K),
	PAD_NC(GPIO_135, DN_20K),
	PAD_NC(GPIO_136, DN_20K),
	PAD_NC(GPIO_137, DN_20K),
	PAD_NC(GPIO_138, DN_20K),
	PAD_NC(GPIO_139, DN_20K),
	PAD_NC(GPIO_140, DN_20K),
	PAD_NC(GPIO_141, DN_20K),
	PAD_NC(GPIO_143, DN_20K),
	PAD_NC(GPIO_144, DN_20K),
	PAD_NC(GPIO_145, DN_20K),
	PAD_NC(GPIO_146, DN_20K),
	/* GPIO 147 through 155 are configured by the TXE. */
	PAD_NC(GPIO_156, DN_20K),
	PAD_NC(GPIO_157, DN_20K),
	PAD_NC(GPIO_158, DN_20K),
	PAD_NC(GPIO_159, DN_20K),
	PAD_NC(GPIO_160, DN_20K),
	PAD_NC(GPIO_161, DN_20K),
	PAD_NC(GPIO_162, DN_20K),
	PAD_NC(GPIO_163, DN_20K),
	PAD_NC(GPIO_164, DN_20K),
	PAD_NC(GPIO_165, DN_20K),
	PAD_NC(GPIO_171, DN_20K),
	PAD_NC(GPIO_172, DN_20K),
	PAD_NC(GPIO_173, DN_20K),
	PAD_NC(GPIO_176, DN_20K),
	PAD_NC(GPIO_178, DN_20K),
	PAD_NC(GPIO_179, DN_20K),
	PAD_NC(GPIO_180, DN_20K),
	PAD_NC(GPIO_181, DN_20K),
	PAD_NC(GPIO_182, DN_20K),
	PAD_NC(GPIO_183, DN_20K),
	PAD_NC(GPIO_184, DN_20K),
	PAD_NC(GPIO_185, DN_20K),
	PAD_NC(GPIO_186, DN_20K),
	PAD_NC(GPIO_187, DN_20K),
	PAD_NC(GPIO_188, DN_20K),
	PAD_NC(GPIO_189, DN_20K),
	PAD_NC(GPIO_190, DN_20K),
	PAD_NC(GPIO_197, DN_20K),
	PAD_NC(GPIO_198, DN_20K),
	PAD_NC(GPIO_199, DN_20K),
	PAD_NC(GPIO_200, DN_20K),
	PAD_NC(GPIO_201, DN_20K),
	PAD_NC(GPIO_202, DN_20K),
	PAD_NC(GPIO_203, DN_20K),
	PAD_NC(GPIO_204, DN_20K),
	PAD_NC(GPIO_205, DN_20K),
	PAD_NC(GPIO_206, DN_20K),
	PAD_NC(GPIO_207, DN_20K),
	PAD_NC(GPIO_208, DN_20K),
	PAD_NC(GPIO_209, DN_20K),
	PAD_NC(GPIO_210, DN_20K),
	PAD_NC(GPIO_211, UP_20K),
	PAD_NC(GPIO_212, DN_20K),
	PAD_NC(GPIO_213, DN_20K),
	PAD_NC(GPIO_214, DN_20K),
};
/* clang-format on */

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
