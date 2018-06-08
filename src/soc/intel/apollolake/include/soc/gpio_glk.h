/*
 *
 * Placed in a separate file since some of these definitions can be used from
 * assembly code
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_GLK_GPIO_H_
#define _SOC_GLK_GPIO_H_

/* North West community pads */
/* For DFx GPIO, Display, USB, I2C, UART, and Thermal GPIO*/
#define NW_OFFSET			0
#define GPIO_0				(NW_OFFSET + 0)
#define GPIO_1				(NW_OFFSET + 1)
#define GPIO_2				(NW_OFFSET + 2)
#define GPIO_3				(NW_OFFSET + 3)
#define GPIO_4				(NW_OFFSET + 4)
#define GPIO_5				(NW_OFFSET + 5)
#define GPIO_6				(NW_OFFSET + 6)
#define GPIO_7				(NW_OFFSET + 7)
#define GPIO_8				(NW_OFFSET + 8)
#define GPIO_9				(NW_OFFSET + 9)
#define GPIO_10				(NW_OFFSET + 10)
#define GPIO_11				(NW_OFFSET + 11)
#define GPIO_12				(NW_OFFSET + 12)
#define GPIO_13				(NW_OFFSET + 13)
#define GPIO_14				(NW_OFFSET + 14)
#define GPIO_15				(NW_OFFSET + 15)
#define GPIO_16				(NW_OFFSET + 16)
#define GPIO_17				(NW_OFFSET + 17)
#define GPIO_18				(NW_OFFSET + 18)
#define GPIO_19				(NW_OFFSET + 19)
#define GPIO_20				(NW_OFFSET + 20)
#define GPIO_21				(NW_OFFSET + 21)
#define GPIO_22				(NW_OFFSET + 22)
#define GPIO_23				(NW_OFFSET + 23)
#define GPIO_24				(NW_OFFSET + 24)
#define GPIO_25				(NW_OFFSET + 25)
#define GPIO_26				(NW_OFFSET + 26)
#define GPIO_27				(NW_OFFSET + 27)
#define GPIO_28				(NW_OFFSET + 28)
#define GPIO_29				(NW_OFFSET + 29)
#define GPIO_30				(NW_OFFSET + 30)
#define GPIO_31				(NW_OFFSET + 31)
#define GPIO_32				(NW_OFFSET + 32)
#define GPIO_33				(NW_OFFSET + 33)
#define GPIO_34				(NW_OFFSET + 34)
#define GPIO_35				(NW_OFFSET + 35)
#define GPIO_36				(NW_OFFSET + 36)
#define GPIO_37				(NW_OFFSET + 37)
#define GPIO_38				(NW_OFFSET + 38)
#define GPIO_39				(NW_OFFSET + 39)
#define GPIO_40				(NW_OFFSET + 40)
#define GPIO_41				(NW_OFFSET + 41)
#define GPIO_42				(NW_OFFSET + 42)
#define GPIO_43				(NW_OFFSET + 43)
#define GPIO_44				(NW_OFFSET + 44)
#define GPIO_45				(NW_OFFSET + 45)
#define GPIO_46				(NW_OFFSET + 46)
#define GPIO_47				(NW_OFFSET + 47)
#define GPIO_48				(NW_OFFSET + 48)
#define GPIO_49				(NW_OFFSET + 49)
#define GPIO_50				(NW_OFFSET + 50)
#define GPIO_51				(NW_OFFSET + 51)
#define GPIO_52				(NW_OFFSET + 52)
#define GPIO_53				(NW_OFFSET + 53)
#define GPIO_54				(NW_OFFSET + 54)
#define GPIO_55				(NW_OFFSET + 55)
#define GPIO_56				(NW_OFFSET + 56)
#define GPIO_57				(NW_OFFSET + 57)
#define GPIO_58				(NW_OFFSET + 58)
#define GPIO_59				(NW_OFFSET + 59)
#define GPIO_60				(NW_OFFSET + 60)
#define GPIO_61				(NW_OFFSET + 61)
#define GPIO_62				(NW_OFFSET + 62)
#define GPIO_63				(NW_OFFSET + 63)
#define GPIO_64				(NW_OFFSET + 64)
#define GPIO_65				(NW_OFFSET + 65)
#define GPIO_66				(NW_OFFSET + 66)
#define GPIO_67				(NW_OFFSET + 67)
#define GPIO_68				(NW_OFFSET + 68)
#define GPIO_69				(NW_OFFSET + 69)
#define GPIO_70				(NW_OFFSET + 70)
#define GPIO_71				(NW_OFFSET + 71)
#define GPIO_72				(NW_OFFSET + 72)
#define GPIO_73				(NW_OFFSET + 73)
#define GPIO_74				(NW_OFFSET + 74)
#define GPIO_75				(NW_OFFSET + 75)
#define GPIO_211			(NW_OFFSET + 76)
#define GPIO_212			(NW_OFFSET + 77)
#define GPIO_213			(NW_OFFSET + 78)
#define GPIO_214			(NW_OFFSET + 79)
#define TOTAL_NW_PADS			80

/* North Community Pads */
/* For power management GPIO, I2C, Display, LPC/eSPI, SPI */
#define N_OFFSET			(NW_OFFSET + 80)
#define GPIO_76				(N_OFFSET + 0)
#define GPIO_77				(N_OFFSET + 1)
#define GPIO_78				(N_OFFSET + 2)
#define GPIO_79				(N_OFFSET + 3)
#define GPIO_80				(N_OFFSET + 4)
#define GPIO_81				(N_OFFSET + 5)
#define GPIO_82				(N_OFFSET + 6)
#define GPIO_83				(N_OFFSET + 7)
#define GPIO_84				(N_OFFSET + 8)
#define GPIO_85				(N_OFFSET + 9)
#define GPIO_86				(N_OFFSET + 10)
#define GPIO_87				(N_OFFSET + 11)
#define GPIO_88				(N_OFFSET + 12)
#define GPIO_89				(N_OFFSET + 13)
#define GPIO_90				(N_OFFSET + 14)
#define GPIO_91				(N_OFFSET + 15)
#define GPIO_92				(N_OFFSET + 16)
#define GPIO_93				(N_OFFSET + 17)
#define GPIO_94				(N_OFFSET + 18)
#define GPIO_95				(N_OFFSET + 19)
#define GPIO_96				(N_OFFSET + 20)
#define GPIO_97				(N_OFFSET + 21)
#define GPIO_98				(N_OFFSET + 22)
#define GPIO_99				(N_OFFSET + 23)
#define GPIO_100			(N_OFFSET + 24)
#define GPIO_101			(N_OFFSET + 25)
#define GPIO_102			(N_OFFSET + 26)
#define GPIO_103			(N_OFFSET + 27)
#define GPIO_104			(N_OFFSET + 28)
#define GPIO_105			(N_OFFSET + 29)
#define GPIO_106			(N_OFFSET + 30)
#define GPIO_107			(N_OFFSET + 31)
#define GPIO_108			(N_OFFSET + 32)
#define GPIO_109			(N_OFFSET + 33)
#define GPIO_110			(N_OFFSET + 34)
#define GPIO_111			(N_OFFSET + 35)
#define GPIO_112			(N_OFFSET + 36)
#define GPIO_113			(N_OFFSET + 37)
#define GPIO_114			(N_OFFSET + 38)
#define GPIO_115			(N_OFFSET + 39)
#define GPIO_116			(N_OFFSET + 40)
#define GPIO_117			(N_OFFSET + 41)
#define GPIO_118			(N_OFFSET + 42)
#define GPIO_119			(N_OFFSET + 43)
#define GPIO_120			(N_OFFSET + 44)
#define GPIO_121			(N_OFFSET + 45)
#define GPIO_122			(N_OFFSET + 46)
#define GPIO_123			(N_OFFSET + 47)
#define GPIO_124			(N_OFFSET + 48)
#define GPIO_125			(N_OFFSET + 49)
#define GPIO_126			(N_OFFSET + 50)
#define GPIO_127			(N_OFFSET + 51)
#define GPIO_128			(N_OFFSET + 52)
#define GPIO_129			(N_OFFSET + 53)
#define GPIO_130			(N_OFFSET + 54)
#define GPIO_131			(N_OFFSET + 55)
#define GPIO_132			(N_OFFSET + 56)
#define GPIO_133			(N_OFFSET + 57)
#define GPIO_134			(N_OFFSET + 58)
#define GPIO_135			(N_OFFSET + 59)
#define GPIO_136			(N_OFFSET + 60)
#define GPIO_137			(N_OFFSET + 61)
#define GPIO_138			(N_OFFSET + 62)
#define GPIO_139			(N_OFFSET + 63)
#define GPIO_140			(N_OFFSET + 64)
#define GPIO_141			(N_OFFSET + 65)
#define GPIO_142			(N_OFFSET + 66)
#define GPIO_143			(N_OFFSET + 67)
#define GPIO_144			(N_OFFSET + 68)
#define GPIO_145			(N_OFFSET + 69)
#define GPIO_146			(N_OFFSET + 70)
#define GPIO_147			(N_OFFSET + 71)
#define GPIO_148			(N_OFFSET + 72)
#define GPIO_149			(N_OFFSET + 73)
#define GPIO_150			(N_OFFSET + 74)
#define GPIO_151			(N_OFFSET + 75)
#define GPIO_152			(N_OFFSET + 76)
#define GPIO_153			(N_OFFSET + 77)
#define GPIO_154			(N_OFFSET + 78)
#define GPIO_155			(N_OFFSET + 79)
#define TOTAL_N_PADS			80

/* Audio Community Pads */
#define AUDIO_OFFSET			(N_OFFSET + 80)
#define GPIO_156			(AUDIO_OFFSET + 0)
#define GPIO_157			(AUDIO_OFFSET + 1)
#define GPIO_158			(AUDIO_OFFSET + 2)
#define GPIO_159			(AUDIO_OFFSET + 3)
#define GPIO_160			(AUDIO_OFFSET + 4)
#define GPIO_161			(AUDIO_OFFSET + 5)
#define GPIO_162			(AUDIO_OFFSET + 6)
#define GPIO_163			(AUDIO_OFFSET + 7)
#define GPIO_164			(AUDIO_OFFSET + 8)
#define GPIO_165			(AUDIO_OFFSET + 9)
#define GPIO_166			(AUDIO_OFFSET + 10)
#define GPIO_167			(AUDIO_OFFSET + 11)
#define GPIO_168			(AUDIO_OFFSET + 12)
#define GPIO_169			(AUDIO_OFFSET + 13)
#define GPIO_170			(AUDIO_OFFSET + 14)
#define GPIO_171			(AUDIO_OFFSET + 15)
#define GPIO_172			(AUDIO_OFFSET + 16)
#define GPIO_173			(AUDIO_OFFSET + 17)
#define GPIO_174			(AUDIO_OFFSET + 18)
#define GPIO_175			(AUDIO_OFFSET + 19)
#define TOTAL_AUDIO_PADS		20


/* SCC community pads */
/* For SMBus, SD-Card, Clock, CNV/SDIO, eMMC */
#define SCC_OFFSET			(AUDIO_OFFSET + 20)
#define GPIO_176			(SCC_OFFSET + 0)
#define GPIO_177			(SCC_OFFSET + 1)
#define GPIO_178			(SCC_OFFSET + 2)
#define GPIO_187			(SCC_OFFSET + 3)
#define GPIO_179			(SCC_OFFSET + 4)
#define GPIO_180			(SCC_OFFSET + 5)
#define GPIO_181			(SCC_OFFSET + 6)
#define GPIO_182			(SCC_OFFSET + 7)
#define GPIO_183			(SCC_OFFSET + 8)
#define GPIO_184			(SCC_OFFSET + 9)
#define GPIO_185			(SCC_OFFSET + 10)
#define GPIO_186			(SCC_OFFSET + 11)
#define GPIO_188			(SCC_OFFSET + 12)
#define GPIO_210			(SCC_OFFSET + 13)
#define GPIO_189			(SCC_OFFSET + 14)
#define GPIO_190			(SCC_OFFSET + 15)
#define GPIO_191			(SCC_OFFSET + 16)
#define GPIO_192			(SCC_OFFSET + 17)
#define GPIO_193			(SCC_OFFSET + 18)
#define GPIO_194			(SCC_OFFSET + 19)
#define GPIO_195			(SCC_OFFSET + 20)
#define GPIO_196			(SCC_OFFSET + 21)
#define GPIO_197			(SCC_OFFSET + 22)
#define GPIO_198			(SCC_OFFSET + 23)
#define GPIO_199			(SCC_OFFSET + 24)
#define GPIO_200			(SCC_OFFSET + 25)
#define GPIO_201			(SCC_OFFSET + 26)
#define GPIO_202			(SCC_OFFSET + 27)
#define GPIO_203			(SCC_OFFSET + 28)
#define GPIO_204			(SCC_OFFSET + 29)
#define GPIO_205			(SCC_OFFSET + 30)
#define GPIO_206			(SCC_OFFSET + 31)
#define GPIO_207			(SCC_OFFSET + 32)
#define GPIO_208			(SCC_OFFSET + 33)
#define GPIO_209			(SCC_OFFSET + 34)
#define TOTAL_SCC_PADS			35
#define TOTAL_PADS			(SCC_OFFSET + 35)

/*
 * Miscellaneous Configuration register(MISCCFG).These are community specific
 * registers and are meant to house miscellaneous configuration fields per
 * community. There are 8 GPIO groups: GPP_0 -> GPP_8 (Group 3 is absent)
 */
#define GPIO_MISCCFG		0x10 /* Miscellaneous Configuration offset */

#define GPIO_GPE_NW_31_0	0
#define GPIO_GPE_NW_63_32	1
#define GPIO_GPE_NW_95_64	2
#define GPIO_GPE_N_31_0		4
#define GPIO_GPE_N_63_32	5
#define GPIO_GPE_N_95_64	6
#define GPIO_GPE_AUDIO_31_0	7
#define GPIO_GPE_SCC_31_0	8
#define GPIO_GPE_SCC_63_32	9

#define GPIO_MAX_NUM_PER_GROUP	32

/* Host Software Pad Ownership Register.
 * The pins in the community are divided into 3 groups :
 * GPIO 0 ~ 31, GPIO 32 ~ 63, GPIO 64 ~ 95
 */
#define HOSTSW_OWN_REG_0		0xB0

#define GPI_INT_EN_0			0x110

#define GPI_SMI_STS_0			0x170
#define GPI_SMI_EN_0			0x190

/* PERST_0 not defined */
#define GPIO_PRT0_UDEF			0xFF

#define NUM_NW_PADS			(TOTAL_NW_PADS)
#define NUM_N_PADS			(TOTAL_N_PADS)
#define NUM_AUDIO_PADS			(TOTAL_AUDIO_PADS)
#define NUM_SCC_PADS			(TOTAL_SCC_PADS)

#define NUM_NW_GPI_REGS	\
	(ALIGN_UP(NUM_NW_PADS, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define NUM_N_GPI_REGS	\
	(ALIGN_UP(NUM_N_PADS, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define NUM_AUDIO_GPI_REGS	\
	(ALIGN_UP(NUM_AUDIO_PADS, GPIO_MAX_NUM_PER_GROUP) /	\
		GPIO_MAX_NUM_PER_GROUP)

#define NUM_SCC_GPI_REGS	\
	(ALIGN_UP(NUM_SCC_PADS, GPIO_MAX_NUM_PER_GROUP) /	\
		GPIO_MAX_NUM_PER_GROUP)


#define NUM_GPI_STATUS_REGS		(NUM_N_GPI_REGS + NUM_NW_GPI_REGS \
					+ NUM_AUDIO_GPI_REGS + NUM_SCC_GPI_REGS)


/* Macros for translating a global pad offset to a local offset */
#define PAD_NW(pad)			(pad - NW_OFFSET)
#define PAD_N(pad)			(pad - N_OFFSET)
#define PAD_AUDIO(pad)			(pad - AUDIO_OFFSET)
#define PAD_SCC(pad)			(pad - SCC_OFFSET)

/* Linux names of the GPIO devices. */
#define CROS_GPIO_DEVICE_NAME		"INT3453"
#define GPIO_COMM_NW_NAME		"INT3453:00"
#define GPIO_COMM_N_NAME		"INT3453:01"
#define GPIO_COMM_AUDIO_NAME		"INT3453:02"
#define GPIO_COMM_SCC_NAME		"INT3453:03"

/* Following is used in gpio asl */
#define GPIO_COMM_NAME			"INT3453"
#define GPIO_COMM_0_DESC	\
	"General Purpose Input/Output (GPIO) Controller - Northwest"
#define GPIO_COMM_1_DESC	\
	"General Purpose Input/Output (GPIO) Controller - North"
#define GPIO_COMM_2_DESC	\
	"General Purpose Input/Output (GPIO) Controller - Audio"
#define GPIO_COMM_3_DESC	\
	"General Purpose Input/Output (GPIO) Controller - SCC"

#define GPIO_COMM0_PID			PID_GPIO_NW
#define GPIO_COMM1_PID			PID_GPIO_N
#define GPIO_COMM2_PID			PID_GPIO_AUDIO
#define GPIO_COMM3_PID			PID_GPIO_SCC

#define GPIO_8_IRQ	0x32
#define GPIO_9_IRQ	0x33
#define GPIO_10_IRQ	0x34
#define GPIO_11_IRQ	0x35
#define GPIO_12_IRQ	0x36
#define GPIO_13_IRQ	0x37
#define GPIO_14_IRQ	0x38
#define GPIO_15_IRQ	0x39
#define GPIO_16_IRQ	0x3a
#define GPIO_17_IRQ	0x3b
#define GPIO_18_IRQ	0x3c
#define GPIO_19_IRQ	0x3d
#define GPIO_20_IRQ	0x3e
#define GPIO_21_IRQ	0x3f
#define GPIO_22_IRQ	0x40
#define GPIO_23_IRQ	0x41
#define GPIO_24_IRQ	0x42
#define GPIO_25_IRQ	0x43
#define GPIO_26_IRQ	0x44
#define GPIO_27_IRQ	0x45
#define GPIO_28_IRQ	0x46
#define GPIO_29_IRQ	0x47
#define GPIO_30_IRQ	0x48
#define GPIO_31_IRQ	0x49
#define GPIO_32_IRQ	0x4a
#define GPIO_33_IRQ	0x4b
#define GPIO_34_IRQ	0x4c
#define GPIO_35_IRQ	0x4d
#define GPIO_36_IRQ	0x4e
#define GPIO_37_IRQ	0x4f
#define GPIO_38_IRQ	0x50
#define GPIO_39_IRQ	0x51
#define GPIO_40_IRQ	0x52
#define GPIO_41_IRQ	0x53
#define GPIO_42_IRQ	0x54
#define GPIO_43_IRQ	0x55
#define GPIO_44_IRQ	0x56
#define GPIO_45_IRQ	0x57
#define GPIO_46_IRQ	0x58
#define GPIO_47_IRQ	0x59
#define GPIO_48_IRQ	0x5a
#define GPIO_49_IRQ	0x5b
#define GPIO_50_IRQ	0x5c
#define GPIO_51_IRQ	0x5d
#define GPIO_52_IRQ	0x5e
#define GPIO_53_IRQ	0x5f
#define GPIO_54_IRQ	0x60
#define GPIO_55_IRQ	0x61
#define GPIO_56_IRQ	0x62
#define GPIO_57_IRQ	0x63
#define GPIO_58_IRQ	0x64
#define GPIO_59_IRQ	0x65
#define GPIO_60_IRQ	0x66
#define GPIO_61_IRQ	0x67
#define GPIO_62_IRQ	0x68
#define GPIO_63_IRQ	0x69
#define GPIO_64_IRQ	0x6a
#define GPIO_65_IRQ	0x6b
#define GPIO_66_IRQ	0x6c
#define GPIO_67_IRQ	0x6d
#define GPIO_68_IRQ	0x6e
#define GPIO_69_IRQ	0x6f
#define GPIO_70_IRQ	0x70
#define GPIO_71_IRQ	0x71
#define GPIO_72_IRQ	0x72
#define GPIO_73_IRQ	0x73
#define GPIO_211_IRQ	0x74
#define GPIO_212_IRQ	0x75
#define GPIO_213_IRQ	0x76
#define GPIO_214_IRQ	0x77
#define GPIO_79_IRQ	0x32
#define GPIO_80_IRQ	0x33
#define GPIO_81_IRQ	0x34
#define GPIO_82_IRQ	0x35
#define GPIO_83_IRQ	0x36
#define GPIO_84_IRQ	0x37
#define GPIO_85_IRQ	0x38
#define GPIO_86_IRQ	0x39
#define GPIO_87_IRQ	0x3a
#define GPIO_88_IRQ	0x3b
#define GPIO_89_IRQ	0x3c
#define GPIO_90_IRQ	0x3d
#define GPIO_91_IRQ	0x3e
#define GPIO_92_IRQ	0x3f
#define GPIO_93_IRQ	0x40
#define GPIO_94_IRQ	0x41
#define GPIO_95_IRQ	0x42
#define GPIO_96_IRQ	0x43
#define GPIO_105_IRQ	0x44
#define GPIO_110_IRQ	0x45
#define GPIO_111_IRQ	0x46
#define GPIO_112_IRQ	0x47
#define GPIO_113_IRQ	0x48
#define GPIO_114_IRQ	0x49
#define GPIO_115_IRQ	0x4a
#define GPIO_116_IRQ	0x4b
#define GPIO_117_IRQ	0x4c
#define GPIO_118_IRQ	0x4d
#define GPIO_119_IRQ	0x4e
#define GPIO_120_IRQ	0x4f
#define GPIO_121_IRQ	0x50
#define GPIO_122_IRQ	0x51
#define GPIO_123_IRQ	0x52
#define GPIO_124_IRQ	0x53
#define GPIO_125_IRQ	0x54
#define GPIO_126_IRQ	0x55
#define GPIO_127_IRQ	0x56
#define GPIO_128_IRQ	0x57
#define GPIO_129_IRQ	0x58
#define GPIO_130_IRQ	0x59
#define GPIO_131_IRQ	0x5a
#define GPIO_132_IRQ	0x5b
#define GPIO_133_IRQ	0x5c
#define GPIO_134_IRQ	0x5d
#define GPIO_135_IRQ	0x5e
#define GPIO_136_IRQ	0x5f
#define GPIO_137_IRQ	0x60
#define GPIO_138_IRQ	0x61
#define GPIO_139_IRQ	0x62
#define GPIO_140_IRQ	0x63
#define GPIO_141_IRQ	0x64
#define GPIO_142_IRQ	0x65
#define GPIO_143_IRQ	0x66
#define GPIO_144_IRQ	0x67
#define GPIO_145_IRQ	0x68
#define GPIO_146_IRQ	0x69
#define GPIO_147_IRQ	0x6a
#define GPIO_148_IRQ	0x6b
#define GPIO_149_IRQ	0x6c
#define GPIO_150_IRQ	0x6d
#define GPIO_151_IRQ	0x6e
#define GPIO_152_IRQ	0x6f
#define GPIO_153_IRQ	0x70
#define GPIO_154_IRQ	0x71
#define GPIO_155_IRQ	0x72

#define PAD_CFG_BASE	0x600

#define GPIO_NUM_PAD_CFG_REGS   4

#endif /* _SOC_GLK_GPIO_H_ */
