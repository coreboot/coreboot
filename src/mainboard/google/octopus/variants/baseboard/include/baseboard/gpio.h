/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef BASEBOARD_GPIO_H
#define BASEBOARD_GPIO_H

#include <soc/gpio.h>

/*
 * GPIO_11 for SCI is routed to GPE0_DW1 and maps to group GPIO_GPE_N_31_0
 * which is North community
 */
#define EC_SCI_GPI	GPE0A_ESPI_SCI_STS

/* EC SMI */
#define EC_SMI_GPI	GPIO_41

#define GPE_EC_WAKE	GPE0_DW2_01

#define GPIO_EC_IN_RW	GPIO_189

#define GPIO_PCH_WP	GPIO_190

/*  Memory SKU GPIOs. */
#define MEM_CONFIG0	GPIO_68
#define MEM_CONFIG1	GPIO_69
#define MEM_CONFIG2	GPIO_70
#define MEM_CONFIG3	GPIO_71

/* EC sync irq is GPP_134_IRQ */
#define EC_SYNC_IRQ	GPIO_134_IRQ

#endif /* BASEBOARD_GPIO_H */
